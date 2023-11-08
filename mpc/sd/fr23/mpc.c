#include "mpc.h"
#include "mpc-def.h"
#include "benchmark.h"
#include "precomputed.h"
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
    matcols_muladd(dc_broadcast->alpha_dc, (uint8_t*) share, (uint8_t*) dc_challenge->gamma_dc, PARAM_CODE_DIMENSION+PARAM_CODE_WEIGHT+PARAM_eta, PARAM_eta_dc);
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

    // x = x_A || x_B
    uint8_t* x = (uint8_t*) aligned_alloc(32, PARAM_CODEWORD_LENGTH_CEIL32);
    uint8_t* x_A = x;
    uint8_t* x_B = &x[PARAM_PLAINTEXT_LENGTH];
    memcpy(x_A, share->wit.x_A, PARAM_PLAINTEXT_LENGTH);
    if(num_party != 0) {
        memcpy(x_B, inst->y, PARAM_SYNDROME_LENGTH);
    } else {
        memset(x_B, 0, PARAM_SYNDROME_LENGTH);
    }

    // x_B = y - H_A x_A
    vec_neg(x_B, PARAM_SYNDROME_LENGTH);
    matcols_muladd(x_B, share->wit.x_A, *inst->mat_H, PARAM_PLAINTEXT_LENGTH, PARAM_SYNDROME_LENGTH);
    vec_neg(x_B, PARAM_SYNDROME_LENGTH);
    uint8_t (*x_chunks)[PARAM_CHUNK_LENGTH] = (uint8_t(*)[PARAM_CHUNK_LENGTH]) x;

    if(num_party != 0) {
        memset(broadcast->alpha, 0, PARAM_eta);
        vec_muladd(broadcast->alpha, num_party, share->unif.u, PARAM_eta);
    } else {
        memcpy(broadcast->alpha, share->unif.u, PARAM_eta);
    }

    for(int nchunk=0; nchunk<PARAM_SPLITTING_FACTOR; nchunk++) {
        for(uint32_t j=0; j<PARAM_CHUNK_LENGTH; j++) {
            uint8_t point = (uint8_t)(j);
            const uint8_t* powers = &powers_points[(PARAM_CHUNK_WEIGHT+1)*point];
            uint8_t q_eval = sca_innerproduct(share->wit.q_poly[nchunk], powers, PARAM_CHUNK_WEIGHT);
            if(num_party != 0)
                q_eval = sca_add(q_eval, powers[PARAM_CHUNK_WEIGHT]);

            uint8_t scalar = sca_mul(q_eval, x_chunks[nchunk][j]);
            vec_muladd(broadcast->alpha, scalar, mpc_challenge->gamma[j], PARAM_eta);
        }
    }

    free(x);
}
