#include "mpc.h"
#include "mpc-def.h"
#include "benchmark.h"
#include "precomputed.h"
#include "parameters-all.h"
#include "selector.h"

#if !(PARAM_SELECTOR_DEGREE == 4 && PARAM_CIRCUIT_DEPTH == 5)
#error "Incompatible PARAM_SELECTOR_DEGREE or PARAM_CIRCUIT_DEPTH"
#endif

const uint16_t broadcast_interpolation_array[PARAM_CIRCUIT_DEPTH-1][PARAM_CIRCUIT_DEPTH] = {
    {4, 248, 85, 188, 24},
    {163, 193, 165, 241, 201},
    {127, 249, 43, 188, 35},
    {209, 63, 209, 136, 241},
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

    uint8_t y[PARAM_SYNDROME_LENGTH] = {0};
    for(uint32_t i=0; i<info->ring_size; i++)
        for(uint32_t j=0; j<PARAM_SYNDROME_LENGTH; j++) // Not optimal
            y[j] = gf251_add(y[j], gf251_mul(selector[i], ring[i].y[j]));

    // x = x_A || x_B
    uint8_t* x = (uint8_t*) aligned_alloc(32, PARAM_CODEWORD_LENGTH_CEIL32);
    uint8_t* x_A = x;
    uint8_t* x_B = &x[PARAM_PLAINTEXT_LENGTH];
    memcpy(x_A, get_wit_const(share, info)->x_A, PARAM_PLAINTEXT_LENGTH);
    memcpy(x_B, y, PARAM_SYNDROME_LENGTH);

    // x_B = y - H_A x_A
    if(num_party != 0) {
        vec_neg(x_B, PARAM_SYNDROME_LENGTH);
        matcols_muladd(x_B, get_wit_const(share, info)->x_A, *tmpl->mat_H, PARAM_PLAINTEXT_LENGTH, PARAM_SYNDROME_LENGTH);
        vec_neg(x_B, PARAM_SYNDROME_LENGTH);
    }
    uint8_t (*x_chunks)[PARAM_CHUNK_LENGTH] = (uint8_t(*)[PARAM_CHUNK_LENGTH]) x;

    uint8_t scalar;
    uint8_t (*gamma_)[PARAM_eta] = (uint8_t (*)[PARAM_eta]) (((uint8_t*) mpc_challenge) + PARAM_CODE_LENGTH*PARAM_eta);

    if(num_party != 0) {
        memset(broadcast->alpha, 0, PARAM_eta);
        scalar = num_party;
        for(uint32_t i=0; i<PARAM_CIRCUIT_DEPTH-1; i++){
            gf251_muladd_tab(broadcast->alpha, scalar, get_unif_const(share, info)->u[i], PARAM_eta);
            scalar = gf251_mul(scalar, num_party);
        }

        for(int nchunk=0; nchunk<PARAM_SPLITTING_FACTOR; nchunk++) {
            for(uint32_t j=0; j<PARAM_CHUNK_LENGTH; j++) {
                uint8_t point = (uint8_t)(j+PARAM_CHUNK_LENGTH*nchunk);
                const uint8_t* powers = &powers_points[(PARAM_CHUNK_WEIGHT+1)*point];
                uint8_t q_eval = gf251_innerproduct(get_wit_const(share, info)->q_poly[nchunk], powers, PARAM_CHUNK_WEIGHT);
                q_eval = gf251_add(q_eval, powers[PARAM_CHUNK_WEIGHT]);

                scalar = gf251_mul(q_eval, x_chunks[nchunk][j]);
                gf251_muladd_tab(broadcast->alpha, scalar, mpc_challenge->gamma[point], PARAM_eta);
            }
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

        for(int nchunk=0; nchunk<PARAM_SPLITTING_FACTOR; nchunk++) {
            for(uint32_t j=0; j<PARAM_CHUNK_LENGTH; j++) {
                uint8_t point = (uint8_t)(j+PARAM_CHUNK_LENGTH*nchunk);
                if(point < PARAM_PLAINTEXT_LENGTH)
                    continue;

                const uint8_t* powers = &powers_points[(PARAM_CHUNK_WEIGHT+1)*point];
                uint8_t q_eval = gf251_innerproduct(get_wit_const(share, info)->q_poly[nchunk], powers, PARAM_CHUNK_WEIGHT);

                scalar = gf251_mul(q_eval, x_chunks[nchunk][j]);
                gf251_muladd_tab(broadcast->alpha, scalar, mpc_challenge->gamma[point], PARAM_eta);
            }
        }
    }

    free(x);
    free(selector);
}
