#include "mpc.h"
#include "mpc-def.h"
#include "benchmark.h"
#include "parameters-all.h"
#include "selector.h"

#if !(PARAM_SELECTOR_DEGREE == 4 && PARAM_CIRCUIT_DEPTH == 4)
#error "Incompatible PARAM_SELECTOR_DEGREE or PARAM_CIRCUIT_DEPTH"
#endif

const uint16_t broadcast_interpolation_array[PARAM_CIRCUIT_DEPTH-1][PARAM_CIRCUIT_DEPTH] = {
    {3, 124, 84, 245}, {123, 2, 125, 11}, {126, 125, 42, 245}
};

void mpc_emulate_party(vec_broadcast_t* broadcast, vec_challenge_t* mpc_challenge,
                            vec_share_t const* share, uint16_t num_party, const instance_t* tmpl, const instance_t* ring, const sharing_info_t* info) {

    uint8_t* sel_vec[PARAM_SELECTOR_DEGREE];
    uint32_t sel_pos[PARAM_SELECTOR_DEGREE];
    parse_selector(sel_vec, sel_pos, share, info);

    uint8_t* selector = (uint8_t*) malloc(info->ring_size);
    uint32_t inter1_size = info->selector_side_size[0]*info->selector_side_size[1];
    uint32_t inter2_size = info->selector_side_size[2]*info->selector_side_size[3];
    uint8_t* inter1 = (uint8_t*) malloc(inter1_size);
    uint8_t* inter2 = (uint8_t*) malloc(inter2_size);
    for(uint32_t i=0; i<info->selector_side_size[0]; i++) {
        for(uint32_t j=0; j<info->selector_side_size[1]; j++) {
            uint32_t pos = j+i*info->selector_side_size[1];
            inter1[pos] = gf251_mul(sel_vec[0][i], sel_vec[1][j]);
        }
    }
    for(uint32_t i=0; i<info->selector_side_size[2]; i++) {
        for(uint32_t j=0; j<info->selector_side_size[3]; j++) {
            uint32_t pos = j+i*info->selector_side_size[3];
            inter2[pos] = gf251_mul(sel_vec[2][i], sel_vec[3][j]);
        }
    }
    for(uint32_t i=0; i<inter1_size; i++) {
        for(uint32_t j=0; j<inter2_size; j++) {
            uint32_t pos = (j+i*inter2_size);
            if(pos < info->ring_size)
                selector[pos] = gf251_mul(inter1[i], inter2[j]);
        }
    }
    free(inter1);
    free(inter2);

    uint32_t y[PARAM_m] = {0};
    for(uint32_t i=0; i<info->ring_size; i++)
        for(uint32_t j=0; j<PARAM_m; j++) // Not optimal
            y[j] = gf251_add(y[j], gf251_mul(selector[i], ring[i].y[j]));

    uint8_t scalar;
    uint8_t intermediary[PARAM_n];
    uint8_t (*gamma_)[PARAM_eta] = (uint8_t (*)[PARAM_eta]) (((uint8_t*) mpc_challenge) + PARAM_m*PARAM_eta);

    if(num_party != 0) {
        memset(broadcast->alpha, 0, PARAM_eta);
        scalar = num_party;
        for(uint32_t i=0; i<PARAM_CIRCUIT_DEPTH-1; i++){
            gf251_muladd_tab(broadcast->alpha, scalar, get_unif_const(share, info)->u[i], PARAM_eta);
            scalar = gf251_mul(scalar, num_party);
        }

        for(uint32_t j=0; j<PARAM_m; j++) {
            memset(intermediary, 0, sizeof(intermediary));
            matcols_muladd_triangular(intermediary, get_wit_const(share, info)->x, (*tmpl->A)[j], PARAM_n, 1);
            scalar = gf251_innerproduct(get_wit_const(share, info)->x, intermediary, PARAM_n);
            scalar = gf251_add(scalar, gf251_innerproduct(get_wit_const(share, info)->x, (*tmpl->b)[j], PARAM_n));
            scalar = gf251_sub(scalar, y[j]);
            gf251_muladd_tab(broadcast->alpha, scalar, mpc_challenge->gamma[j], PARAM_eta);
        }

        uint32_t offset = 0;
        for(uint32_t k=0; k<PARAM_SELECTOR_DEGREE; k++) {
            for(uint32_t j=0; j<info->selector_side_size[k]; j++) {
                scalar = gf251_mul(sel_vec[k][j], gf251_sub(j, sel_pos[k]));
                gf251_muladd_tab(broadcast->alpha, scalar, gamma_[offset+j], PARAM_eta);
            }
            offset += info->selector_side_size[k];
        }
        {
            scalar = 0;
            for(uint32_t i=0; i<info->ring_size; i++) // Not optimal
                scalar = gf251_add(scalar, selector[i]);
            scalar = gf251_sub(scalar, 1);
            gf251_muladd_tab(broadcast->alpha, scalar, gamma_[offset], PARAM_eta);
        }

    } else {
        memcpy(broadcast->alpha, get_unif_const(share, info)->u[PARAM_CIRCUIT_DEPTH-1-1], PARAM_eta);
        for(uint32_t j=0; j<PARAM_m; j++) {
            scalar = gf251_neg(y[j]);
            gf251_muladd_tab(broadcast->alpha, scalar, mpc_challenge->gamma[j], PARAM_eta);
        }

        uint32_t offset = 0;
        for(uint32_t k=0; k<PARAM_SELECTOR_DEGREE; k++) {
            offset += info->selector_side_size[k];
        }
        {
            scalar = 0;
            for(uint32_t i=0; i<info->ring_size; i++) // Not optimal
                scalar = gf251_add(scalar, selector[i]);
            gf251_muladd_tab(broadcast->alpha, scalar, gamma_[offset], PARAM_eta);
        }
    }

    free(selector);
}

