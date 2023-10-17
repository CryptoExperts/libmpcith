#include "mpc.h"
#include "mpc-def.h"
#include "benchmark.h"

void compute_correlated(vec_corr_t* corr, const vec_wit_t* wit, const vec_unif_t* unif, const instance_t* inst) {
    (void) inst;
    memset(corr->c, 0, sizeof(corr->c));
#ifndef PARAM_PURE_BEAVER_TRIPLE
    matmult_rows((point_t*) corr->c, (point_t*) wit->mat_T, (point_t*) unif->a, PARAM_n, PARAM_r, PARAM_eta);
#else
    (void) wit;
    matmult_rows((point_t*) corr->c, (point_t*) unif->a, (point_t*) unif->b, PARAM_eta, PARAM_r, PARAM_eta);
#endif
}

int is_valid_plain_broadcast(vec_broadcast_t const* plain_br) {
    point_t ret = 0;
    // Just test if "plain_br->v" is zero
    for(unsigned int i=0; i<sizeof(plain_br->v); i++)
        ret |= ((uint8_t*) plain_br->v)[i];
    return (ret == 0);
}

void compress_plain_broadcast(uint8_t* buf, const vec_broadcast_t* plain_br) {
    vec_unif_compress(buf, (const vec_unif_t*) plain_br->alpha);
}

void uncompress_plain_broadcast(vec_broadcast_t* plain_br, const uint8_t* buf) {
    vec_unif_decompress((vec_unif_t*) plain_br->alpha, buf);
    vec_setzero((point_t*) plain_br->v, sizeof(plain_br->v));
}

void run_multiparty_computation(vec_broadcast_t* broadcast, vec_challenge_t* mpc_challenge,
                                vec_share_t const* share, vec_broadcast_t const* plain_br,
                                const instance_t* inst, char has_sharing_offset, char entire_computation) {

    __MPC_BEGIN__;
    
    __MPC_PARAM__("n", PARAM_n);
    __MPC_PARAM__("m", PARAM_m);
    __MPC_PARAM__("k", PARAM_k);
    __MPC_PARAM__("r", PARAM_r);
    __MPC_PARAM__("eta", PARAM_eta);

    __MPC_INPUT_VAR__("x", share->wit.x);
    __MPC_INPUT_VAR__("mat_T", share->wit.mat_T);
    __MPC_INPUT_VAR__("mat_R", share->wit.mat_R);
    __MPC_INPUT_VAR__("a", share->unif.a);
#ifdef PARAM_PURE_BEAVER_TRIPLE
    __MPC_INPUT_VAR__("b", share->unif.b);
#endif
    __MPC_INPUT_VAR__("c", share->corr.c);

#ifndef PARAM_PURE_BEAVER_TRIPLE
    __MPC_CHALLENGE__("eps", mpc_challenge->eps);
#else
    __MPC_CHALLENGE__("eps_a", mpc_challenge->eps_a);
    __MPC_CHALLENGE__("eps_b", mpc_challenge->eps_b);
#endif

    __MPC_INST_VAR__("m0", inst->m0);
    __MPC_INST_VAR__("mats", *inst->mats);

#ifndef PARAM_PURE_BEAVER_TRIPLE
    // Compute alpha
    memcpy(broadcast->alpha, share->unif.a, sizeof(broadcast->alpha));
    matmult_rows((point_t*) broadcast->alpha, (point_t*) share->wit.mat_R, (point_t*) mpc_challenge->eps, PARAM_r, PARAM_m, PARAM_eta);
    __MPC_INTERMEDIATE_VAR__("alpha", broadcast->alpha);

    if(entire_computation) {
        // Compute mat_e = m0 + sum_i x_i mats_i
        point_t mat_e[PARAM_n][PARAM_m];
        if(has_sharing_offset)
            memcpy(mat_e, inst->m0, PARAM_nm*sizeof(point_t));
        else
            memset(mat_e, 0, PARAM_nm*sizeof(point_t));
        matcols_muladd((point_t*) mat_e, share->wit.x, *inst->mats, PARAM_k, PARAM_nm);
        __MPC_INTERMEDIATE_VAR__("mat_e", mat_e);

        // Compute v
        memcpy(broadcast->v, share->corr.c, sizeof(broadcast->v));
        matmult_rows((point_t*) broadcast->v, (point_t*) mat_e, (point_t*) mpc_challenge->eps, PARAM_n, PARAM_m, PARAM_eta);
        vec_neg(broadcast->v, sizeof(broadcast->v));
        matmult_rows((point_t*) broadcast->v, (point_t*) share->wit.mat_T, (point_t*) plain_br->alpha, PARAM_n, PARAM_r, PARAM_eta);
        __MPC_INTERMEDIATE_VAR__("v", broadcast->v);
    } else {
        memset(broadcast->v, 0, sizeof(broadcast->v));
    }
#else
    // Compute alpha
    memcpy(broadcast->alpha, share->unif.a, sizeof(broadcast->alpha));
    matmult_rows((point_t*) broadcast->alpha, (point_t*) mpc_challenge->eps_a, (point_t*) share->wit.mat_T, PARAM_eta, PARAM_n, PARAM_r);
    __MPC_INTERMEDIATE_VAR__("alpha", broadcast->alpha);
    
    // Compute beta
    memcpy(broadcast->beta, share->unif.b, sizeof(broadcast->beta));
    matmult_rows((point_t*) broadcast->beta, (point_t*) share->wit.mat_R, (point_t*) mpc_challenge->eps_b, PARAM_r, PARAM_m, PARAM_eta);
    __MPC_INTERMEDIATE_VAR__("beta", broadcast->beta);
    
    if(entire_computation) {
        // Compute mat_e = m0 + sum_i x_i mats_i
        point_t mat_e[PARAM_n][PARAM_m];
        if(has_sharing_offset)
            memcpy(mat_e, inst->m0, PARAM_nm*sizeof(point_t));
        else
            memset(mat_e, 0, PARAM_nm*sizeof(point_t));
        matcols_muladd((point_t*) mat_e, share->wit.x, *inst->mats, PARAM_k, PARAM_nm);
        __MPC_INTERMEDIATE_VAR__("mat_e", mat_e);

        // Compute v
        memcpy(broadcast->v, share->corr.c, sizeof(broadcast->v));
        if(has_sharing_offset)
            matmult_rows((point_t*) broadcast->v, (point_t*) plain_br->alpha, (point_t*) plain_br->beta, PARAM_eta, PARAM_r, PARAM_eta);
        vec_neg(broadcast->v, sizeof(broadcast->v));
        matmult_rows((point_t*) broadcast->v, (point_t*) plain_br->alpha, (point_t*) share->unif.b, PARAM_eta, PARAM_r, PARAM_eta);
        matmult_rows((point_t*) broadcast->v, (point_t*) share->unif.a, (point_t*) plain_br->beta, PARAM_eta, PARAM_r, PARAM_eta);
        point_t tmp[PARAM_n][PARAM_eta];
        memset(tmp, 0, sizeof(tmp));
        matmult_rows((point_t*) tmp, (point_t*) mat_e, (point_t*) mpc_challenge->eps_b, PARAM_n, PARAM_m, PARAM_eta);
        matmult_rows((point_t*) broadcast->v, (point_t*) mpc_challenge->eps_a, (point_t*) tmp, PARAM_eta, PARAM_n, PARAM_eta);
        __MPC_INTERMEDIATE_VAR__("v", broadcast->v);
    } else {
        memset(broadcast->v, 0, sizeof(broadcast->v));
    }
#endif

    __MPC_END__;
}

void mpc_compute_plain_broadcast(vec_broadcast_t* plain_br, vec_challenge_t* mpc_challenge,
                                vec_share_t const* share, const instance_t* inst) {
    run_multiparty_computation(plain_br, mpc_challenge, share, plain_br, inst, 1, 0);
}

void mpc_compute_communications(vec_broadcast_t* broadcast, vec_challenge_t* mpc_challenge,
                                vec_share_t const* share, vec_broadcast_t const* plain_br,
                                const instance_t* inst, char has_sharing_offset) {
    run_multiparty_computation(broadcast, mpc_challenge, share, plain_br, inst, has_sharing_offset, 1);
}

void mpc_compute_communications_inverse(vec_share_t* share, vec_challenge_t* mpc_challenge,
                                const vec_broadcast_t* broadcast, vec_broadcast_t const* plain_br,
                                const instance_t* inst, char has_sharing_offset) {
    __MPC_BEGIN__;

#ifndef PARAM_PURE_BEAVER_TRIPLE
    // Compute alpha
    memcpy(share->unif.a, broadcast->alpha, sizeof(share->unif.a));
    vec_neg(share->unif.a, sizeof(share->unif.a));
    matmult_rows((point_t*) share->unif.a, (point_t*) share->wit.mat_R, (point_t*) mpc_challenge->eps, PARAM_r, PARAM_m, PARAM_eta);
    vec_neg(share->unif.a, sizeof(share->unif.a));

    // Compute mat_e = m0 + sum_i x_i mats_i
    point_t mat_e[PARAM_n][PARAM_m];
    if(has_sharing_offset)
        memcpy(mat_e, inst->m0, PARAM_nm*sizeof(point_t));
    else
        memset(mat_e, 0, PARAM_nm*sizeof(point_t));
    matcols_muladd((point_t*) mat_e, share->wit.x, *inst->mats, PARAM_k, PARAM_nm);

    // Compute v
    memcpy(share->corr.c, broadcast->v, sizeof(share->corr.c));
    matmult_rows((point_t*) share->corr.c, (point_t*) mat_e, (point_t*) mpc_challenge->eps, PARAM_n, PARAM_m, PARAM_eta);
    vec_neg(share->corr.c, sizeof(share->corr.c));
    matmult_rows((point_t*) share->corr.c, (point_t*) share->wit.mat_T, (point_t*) plain_br->alpha, PARAM_n, PARAM_r, PARAM_eta);
#else
    // Compute alpha
    memcpy(share->unif.a, broadcast->alpha, sizeof(share->unif.a));
    vec_neg(share->unif.a, sizeof(share->unif.a));
    matmult_rows((point_t*) share->unif.a, (point_t*) mpc_challenge->eps_a, (point_t*) share->wit.mat_T, PARAM_eta, PARAM_n, PARAM_r);
    vec_neg(share->unif.a, sizeof(share->unif.a));
    
    // Compute beta
    memcpy(share->unif.b, broadcast->beta, sizeof(share->unif.b));
    vec_neg(share->unif.b, sizeof(share->unif.b));
    matmult_rows((point_t*) share->unif.b, (point_t*) share->wit.mat_R, (point_t*) mpc_challenge->eps_b, PARAM_r, PARAM_m, PARAM_eta);
    vec_neg(share->unif.b, sizeof(share->unif.b));
    
    // Compute mat_e = m0 + sum_i x_i mats_i
    point_t mat_e[PARAM_n][PARAM_m];
    if(has_sharing_offset)
        memcpy(mat_e, inst->m0, PARAM_nm*sizeof(point_t));
    else
        memset(mat_e, 0, PARAM_nm*sizeof(point_t));
    matcols_muladd((point_t*) mat_e, share->wit.x, *inst->mats, PARAM_k, PARAM_nm);

    // Compute v
    memcpy(share->corr.c, broadcast->v, sizeof(share->corr.c));
    matmult_rows((point_t*) share->corr.c, (point_t*) plain_br->alpha, (point_t*) plain_br->beta, PARAM_eta, PARAM_r, PARAM_eta);
    vec_neg(share->corr.c, sizeof(share->corr.c));
    matmult_rows((point_t*) share->corr.c, (point_t*) plain_br->alpha, (point_t*) share->unif.b, PARAM_eta, PARAM_r, PARAM_eta);
    matmult_rows((point_t*) share->corr.c, (point_t*) share->unif.a, (point_t*) plain_br->beta, PARAM_eta, PARAM_r, PARAM_eta);
    point_t tmp[PARAM_n][PARAM_eta];
    memset(tmp, 0, sizeof(tmp));
    matmult_rows((point_t*) tmp, (point_t*) mat_e, (point_t*) mpc_challenge->eps_b, PARAM_n, PARAM_m, PARAM_eta);
    matmult_rows((point_t*) share->corr.c, (point_t*) mpc_challenge->eps_a, (point_t*) tmp, PARAM_eta, PARAM_n, PARAM_eta);
#endif

    __MPC_END__;
}

void expand_mpc_challenge_hash(vec_challenge_t** challenges, const uint8_t* digest, size_t nb, instance_t* inst) {

    xof_context entropy_ctx;
    xof_init(&entropy_ctx);
    xof_update(&entropy_ctx, digest, PARAM_DIGEST_SIZE);
    xof_final(&entropy_ctx);
    samplable_t entropy = xof_to_samplable(&entropy_ctx);

    (void) inst;
    for(size_t num=0; num<nb; num++) {
#ifndef PARAM_PURE_BEAVER_TRIPLE
        vec_rnd((point_t*) challenges[num]->eps, PARAM_m*PARAM_eta, &entropy);
#else
        vec_rnd((point_t*) challenges[num]->eps_a, PARAM_eta*PARAM_n, &entropy);
        vec_rnd((point_t*) challenges[num]->eps_b, PARAM_m*PARAM_eta, &entropy);
#endif
    }
}
