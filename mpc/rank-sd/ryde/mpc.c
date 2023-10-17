#include "mpc.h"
#include <stdio.h>
#include "mpc-def.h"

void compute_correlated(vec_corr_t* corr, const vec_wit_t* wit, const vec_unif_t* unif, const instance_t* inst) {
    (void) inst;
    rbc_vec_inner_product(corr->c, (const rbc_vec) wit->beta, (const rbc_vec) unif->a, RYDE_PARAM_W-1);
}

int is_valid_plain_broadcast(vec_broadcast_t const* plain_br) {
    return rbc_elt_is_zero(plain_br->v);
}

void compress_plain_broadcast(uint8_t* buf, const vec_broadcast_t* plain_br) {
    rbc_vec_to_string(buf, (const rbc_vec) plain_br->alpha, RYDE_PARAM_W-1);
}

void uncompress_plain_broadcast(vec_broadcast_t* plain_br, const uint8_t* buf) {
    rbc_vec_from_string(plain_br->alpha, RYDE_PARAM_W-1, buf);
    rbc_elt_set_zero(plain_br->v);
}

void run_multiparty_computation(vec_broadcast_t* broadcast, vec_challenge_t* mpc_challenge,
                                vec_share_t const* share, vec_broadcast_t const* plain_br,
                                const instance_t* inst, char has_sharing_offset, char entire_computation) {

    // Compute share [x1] = y - H [x2] 
    rbc_elt x1[RYDE_PARAM_N - RYDE_PARAM_K];
    rbc_mat_vec_mul(x1, inst->mat_H, (const rbc_vec) share->wit.x2, RYDE_PARAM_N-RYDE_PARAM_K, RYDE_PARAM_K);
    if(has_sharing_offset)
        rbc_vec_add(x1, x1, (const rbc_vec) inst->y, RYDE_PARAM_N-RYDE_PARAM_K);

    // Compute share [x] = ([x1]|[x2])
    rbc_elt x[RYDE_PARAM_N];
    for(uint32_t i = 0; i < RYDE_PARAM_N - RYDE_PARAM_K; i++)
        rbc_elt_set(x[i], x1[i]);
    for(uint32_t i = 0; i < RYDE_PARAM_K; i++)
        rbc_elt_set(x[i + RYDE_PARAM_N - RYDE_PARAM_K], share->wit.x2[i]);

    rbc_elt x_qj[RYDE_PARAM_N];
    for(uint32_t i = 0; i < RYDE_PARAM_N; i++)
        rbc_elt_set(x_qj[i], x[i]);
    rbc_elt diff[RYDE_PARAM_N];

    // Compute share [w]
    rbc_elt w[RYDE_PARAM_W - 1];
    for(size_t k = 0; k < (RYDE_PARAM_W - 1); k++) {
        for(size_t j = 0; j < RYDE_PARAM_N; j++) {
            rbc_elt_sqr(x_qj[j], x_qj[j]);
        }
        rbc_vec_add(diff, x_qj, x, RYDE_PARAM_N);
        rbc_vec_inner_product(w[k], mpc_challenge->gamma, diff, RYDE_PARAM_N);
    }

    // Compute [alpha]
    rbc_vec_scalar_mul(broadcast->alpha, w, mpc_challenge->eps, RYDE_PARAM_W - 1);
    rbc_vec_add(broadcast->alpha, broadcast->alpha, (const rbc_vec) share->unif.a, RYDE_PARAM_W - 1);

    if(entire_computation) {
        // Compute [z]
        rbc_elt z;
        for(size_t j = 0; j < RYDE_PARAM_N; j++) {
            rbc_elt_sqr(x_qj[j], x_qj[j]);
        }
        rbc_vec_add(diff, x_qj, x, RYDE_PARAM_N);
        rbc_vec_inner_product(z, mpc_challenge->gamma, diff, RYDE_PARAM_N);

        // Compute [v]
        rbc_elt tmp;
        rbc_elt_mul(broadcast->v, mpc_challenge->eps, z);
        rbc_vec_inner_product(tmp, (const rbc_vec) plain_br->alpha, (const rbc_vec) share->wit.beta, RYDE_PARAM_W - 1);
        rbc_elt_add(broadcast->v, broadcast->v, tmp);
        rbc_elt_add(broadcast->v, broadcast->v, share->corr.c);
    } else {
        rbc_elt_set_zero(broadcast->v);
    }
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
    (void) share;
    (void) mpc_challenge;
    (void) broadcast;
    (void) plain_br;
    (void) inst;
    (void) has_sharing_offset;
}

void expand_mpc_challenge_hash(vec_challenge_t** challenges, const uint8_t* digest, size_t nb, instance_t* inst) {
    (void) inst;

    xof_context entropy_ctx;
    xof_init(&entropy_ctx);
    xof_update(&entropy_ctx, digest, PARAM_DIGEST_SIZE);
    xof_final(&entropy_ctx);
    samplable_t entropy = xof_to_samplable(&entropy_ctx);

    for(size_t num=0; num<nb; num++) {
        rbc_vec_set_random(&entropy, challenges[num]->gamma, RYDE_PARAM_N);
        rbc_vec_set_random(&entropy, &challenges[num]->eps, 1);
    }
}
