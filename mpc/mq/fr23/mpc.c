#include "mpc.h"
#include "mpc-def.h"
#include "benchmark.h"
#include "parameters-all.h"

/********************************************************
 *            Degree Checking (if TCitH-MT)             *
 ********************************************************/

#ifdef PARAM_TCITH_MT_EXTENDED
void expand_dc_challenge_hash(vec_dc_challenge_t** challenges, const uint8_t* digest, size_t nb) {
    xof_context entropy_ctx;
    xof_init(&entropy_ctx);
    xof_update(&entropy_ctx, digest, PARAM_DIGEST_SIZE);
    samplable_t entropy = xof_to_samplable(&entropy_ctx);

    for(size_t num=0; num<nb; num++) {
        vec_rnd((uint8_t*) challenges[num]->gamma_dc, sizeof((*challenges)->gamma_dc), &entropy);
    }
}

void mpc_run_degree_checking(vec_dc_broadcast_t* dc_broadcast, vec_dc_challenge_t* dc_challenge, vec_share_t const* share, uint16_t num_party) {
    (void) num_party;
    memset(dc_broadcast->alpha_dc, 0, sizeof(dc_broadcast->alpha_dc));
    matcols_muladd(dc_broadcast->alpha_dc, (uint8_t*) share, (uint8_t*) dc_challenge->gamma_dc, PARAM_n+PARAM_eta, PARAM_eta_dc);
    vec_add(dc_broadcast->alpha_dc, share->unif.u_dc, PARAM_eta_dc);
}
#endif

/********************************************************
 *                     MPC Protocol                     *
 ********************************************************/

void expand_mpc_challenge_hash(vec_challenge_t** challenges, const uint8_t* digest, size_t nb, instance_t* inst) {
    (void) inst;

    xof_context entropy_ctx;
    xof_init(&entropy_ctx);
    xof_update(&entropy_ctx, digest, PARAM_DIGEST_SIZE);
    samplable_t entropy = xof_to_samplable(&entropy_ctx);

    for(size_t num=0; num<nb; num++) {
        vec_rnd((uint8_t*) challenges[num]->gamma, sizeof((*challenges)->gamma), &entropy);
    }
}

void mpc_emulate_party(vec_broadcast_t* broadcast, vec_challenge_t* mpc_challenge,
                            vec_share_t const* share, uint16_t num_party, const instance_t* inst) {

    uint8_t scalar;
    uint8_t intermediary[PARAM_n];

    if(num_party != 0) {
        memset(broadcast->alpha, 0, PARAM_eta);
        vec_muladd(broadcast->alpha, num_party, share->unif.u, PARAM_eta);

        for(uint32_t j=0; j<PARAM_m; j++) {
            memset(intermediary, 0, sizeof(intermediary));
            matcols_muladd_triangular(intermediary, share->wit.x, (*inst->A)[j], PARAM_n, 1);
            scalar = sca_innerproduct(share->wit.x, intermediary, PARAM_n);
            scalar = sca_add(scalar, sca_innerproduct(share->wit.x, (*inst->b)[j], PARAM_n));
            scalar = sca_sub(scalar, inst->y[j]);
            vec_muladd(broadcast->alpha, scalar, mpc_challenge->gamma[j], PARAM_eta);
        }
    } else {
        memcpy(broadcast->alpha, share->unif.u, PARAM_eta);
        for(uint32_t j=0; j<PARAM_m; j++) {
            memset(intermediary, 0, sizeof(intermediary));
            matcols_muladd_triangular(intermediary, share->wit.x, (*inst->A)[j], PARAM_n, 1);
            scalar = sca_innerproduct(share->wit.x, intermediary, PARAM_n);
            vec_muladd(broadcast->alpha, scalar, mpc_challenge->gamma[j], PARAM_eta);
        }
    }
}
