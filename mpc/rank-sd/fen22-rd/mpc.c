#include "mpc.h"
#include "mpc-def.h"
#include "gf2to31.h"

void compute_correlated(vec_corr_t* corr, const vec_wit_t* wit, const vec_unif_t* unif, const instance_t* inst) {
    (void) inst;
    memset(corr->c, 0, sizeof(corr->c));
    gf2_matmult_rows((uint8_t*) corr->c, (uint8_t*) wit->mat_T, (uint8_t*) unif->a, PARAM_n, PARAM_r, PARAM_eta);
    gf2_clean_matrix_by_rows((uint8_t*) corr->c, PARAM_n, PARAM_eta);
}

int is_valid_plain_broadcast(vec_broadcast_t const* plain_br) {
    uint8_t ret = 0;
    // Just test if "plain_br->v" is zero
    for(unsigned int i=0; i<sizeof(plain_br->v); i++)
        ret |= ((uint8_t*) plain_br->v)[i];
    return (ret == 0);
}

void compress_plain_broadcast(uint8_t* buf, const vec_broadcast_t* plain_br) {
    memcpy(buf, plain_br->alpha, sizeof(plain_br->alpha));
}

void uncompress_plain_broadcast(vec_broadcast_t* plain_br, const uint8_t* buf) {
    memcpy(plain_br->alpha, buf, sizeof(plain_br->alpha));
    memset(plain_br->v, 0, sizeof(plain_br->v));
}

void run_multiparty_computation(vec_broadcast_t* broadcast, vec_challenge_t* mpc_challenge,
                                vec_share_t const* share, vec_broadcast_t const* plain_br,
                                const instance_t* inst, char has_sharing_offset, char entire_computation) {

    __MPC_BEGIN__;
    
    __MPC_PARAM__("m", PARAM_m);
    __MPC_PARAM__("n", PARAM_n);
    __MPC_PARAM__("k", PARAM_k);
    __MPC_PARAM__("r", PARAM_r);
    __MPC_PARAM__("eta", PARAM_eta);

    __MPC_INPUT_VAR__("x_A", share->wit.x_A);
    __MPC_INPUT_VAR__("mat_T", share->wit.mat_T);
    __MPC_INPUT_VAR__("mat_R", share->wit.mat_R);
    __MPC_INPUT_VAR__("a", share->unif.a);
    __MPC_INPUT_VAR__("c", share->corr.c);

    __MPC_CHALLENGE__("eps", mpc_challenge->eps);

    __MPC_INST_VAR__("y", inst->y);
    __MPC_INST_VAR__("H", *inst->mat_H);

    // Compute alpha
    memcpy(broadcast->alpha, share->unif.a, sizeof(broadcast->alpha));
    gf2_matmult_rows((uint8_t*) broadcast->alpha, (uint8_t*) share->wit.mat_R, (uint8_t*) mpc_challenge->eps, PARAM_r, PARAM_m, PARAM_eta);
    gf2_clean_matrix_by_rows((uint8_t*) broadcast->alpha, PARAM_r, PARAM_eta);
    __MPC_INTERMEDIATE_VAR__("alpha", broadcast->alpha);
    
    if(entire_computation) {
        // x = x_A || x_B
        uint8_t* x = (uint8_t*) aligned_alloc(32, PARAM_CODEWORD_BYTESIZE_CEIL32);
        uint8_t* x_A = x;
        uint8_t* x_B = &x[PARAM_PLAINTEXT_BYTESIZE];
        memcpy(x_A, (uint8_t*) share->wit.x_A, PARAM_PLAINTEXT_BYTESIZE);
        // Clean
        for(int i=0;i<PARAM_k;i++)
            ((uint32_t*) x_A)[i] &= 0x7FFFFFFF;
        if(has_sharing_offset)
            memcpy(x_B, (uint8_t*) inst->y, PARAM_SYNDROME_BYTESIZE);
        else
            memset(x_B, 0, PARAM_SYNDROME_BYTESIZE);

        // x_B = y - H_A x_A
        //neg_tab_points(&s[PARAM_PLAINTEXT_LENGTH], PARAM_SYNDROME_LENGTH);
        gf2to31_matcols_muladd(x_B, x_A, *inst->mat_H, PARAM_PLAINTEXT_LENGTH, PARAM_SYNDROME_LENGTH);
        //neg_tab_points(&s[PARAM_PLAINTEXT_LENGTH], PARAM_SYNDROME_LENGTH);
        __MPC_INTERMEDIATE_VAR__("x_B", x_B);

        // Compute v
        // Warning: there is no substraction since we are over GF(2)
        memcpy(broadcast->v, share->corr.c, sizeof(broadcast->v));
        gf2_matmult_rows((uint8_t*) broadcast->v, (uint8_t*) share->wit.mat_T, (uint8_t*) plain_br->alpha, PARAM_n, PARAM_r, PARAM_eta);
        gf2_matmult_rows((uint8_t*) broadcast->v, x, (uint8_t*) mpc_challenge->eps, PARAM_n, PARAM_m, PARAM_eta);
        gf2_clean_matrix_by_rows((uint8_t*) broadcast->v, PARAM_n, PARAM_eta);
        __MPC_INTERMEDIATE_VAR__("v", broadcast->v);

        free(x);
    } else {
        memset(broadcast->v, 0, sizeof(broadcast->v));
    }

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

    // To do
    (void) share;
    (void) mpc_challenge;
    (void) broadcast;
    (void) plain_br;
    (void) inst;
    (void) has_sharing_offset;

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
        gf2_random_matrix_by_rows((uint8_t*) challenges[num]->eps, PARAM_m, PARAM_eta, &entropy);
    }
}
