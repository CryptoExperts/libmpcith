#include "mpc.h"
#include "mpc-def.h"
#include "benchmark.h"

void compute_correlated(vec_corr_t* corr, const vec_wit_t* wit, const vec_unif_t* unif, const instance_t* inst) {
    (void) inst;
    gfqm_vec_inner_product(corr->c, (const gfqm_vec) unif->a, (const gfqm_vec) wit->beta, MIRA_PARAM_R);
}

int is_valid_plain_broadcast(vec_broadcast_t const* plain_br) {
    return gfqm_is_zero(plain_br->v);
}

void compress_plain_broadcast(uint8_t* buf, const vec_broadcast_t* plain_br) {
    gfqm_vec_to_string(buf, (const gfqm_vec) plain_br->alpha, MIRA_PARAM_R);
}

void uncompress_plain_broadcast(vec_broadcast_t* plain_br, const uint8_t* buf) {
    gfqm_vec_from_string(plain_br->alpha, MIRA_PARAM_R, buf);
    gfqm_set_zero(plain_br->v);
}

void run_multiparty_computation(vec_broadcast_t* broadcast, vec_challenge_t* mpc_challenge,
                                vec_share_t const* share, vec_broadcast_t const* plain_br,
                                const instance_t* inst, char has_sharing_offset, char entire_computation) {

    // Compute [E]
    gfq_mat E, tmp;
    gfq_mat_init(&E, MIRA_PARAM_M, MIRA_PARAM_N);
    gfq_mat_init(&tmp, MIRA_PARAM_M, MIRA_PARAM_N);
    if(has_sharing_offset)
        gfq_mat_set(E, (const gfq_mat) inst->M0, MIRA_PARAM_M, MIRA_PARAM_N);
    else
        gfq_mat_set_zero(E, MIRA_PARAM_M, MIRA_PARAM_N);

    for(int j=0 ; j<MIRA_PARAM_K ; j++) {
        gfq_mat_scalar_mul(tmp, (*inst->Mi)[j], share->wit.x[j], MIRA_PARAM_M, MIRA_PARAM_N);
        gfq_mat_add(E, E, tmp, MIRA_PARAM_M, MIRA_PARAM_N);
    }

    //"Transpose" into elements of Fqm
    gfqm_vec E_vec;
    gfqm_vec E_vec_pow_k;
    gfqm_vec_init(&E_vec, MIRA_PARAM_N);
    gfqm_vec_init(&E_vec_pow_k, MIRA_PARAM_N);
    for(int j=0 ; j<MIRA_PARAM_N ; j++) {
        for(int k=0 ; k<MIRA_PARAM_M ; k++) {
            E_vec[j][k] = E[k*MIRA_PARAM_N + j];
        }
        gfqm_set(E_vec_pow_k[j], E_vec[j]);
    }

    // Compute [w]
    gfqm z;
    gfqm gamma_e;
    gfqm_vec w;
    gfqm_vec_init(&w, MIRA_PARAM_R);
    for(size_t k=0 ; k < MIRA_PARAM_R ; k++) {
        gfqm_set_zero(w[k]);
        //w[k] = <gamma . (e_ji^q^k)>
        for(int j=0 ; j<MIRA_PARAM_N ; j++) {
            gfqm_mul(gamma_e, E_vec_pow_k[j], mpc_challenge->gamma[j]);
            gfqm_add(w[k], w[k], gamma_e);
        }
        for(size_t j = 0; j < MIRA_PARAM_N; j++) {
            gfqm_powq(E_vec_pow_k[j], E_vec_pow_k[j]);
        }
    }

    // Compute [alpha]
    gfqm_vec_scalar_mul(broadcast->alpha, w, mpc_challenge->eps, MIRA_PARAM_R);
    gfqm_vec_add(broadcast->alpha, broadcast->alpha, (const gfqm_vec) share->unif.a, MIRA_PARAM_R);

    if(entire_computation) {
        //Compute [z]
        gfqm_set_zero(z);
        //z = <gamma . (e_ji^q^r)>
        for(int j=0 ; j<MIRA_PARAM_N ; j++) {
            gfqm_mul(gamma_e, E_vec_pow_k[j], mpc_challenge->gamma[j]);
            gfqm_add(z, z, gamma_e);
        }

        // Compute [v]
        gfqm_mul(broadcast->v, mpc_challenge->eps, z);
        gfqm_vec_inner_product(gamma_e, (const gfqm_vec) plain_br->alpha, (const gfqm_vec) share->wit.beta, MIRA_PARAM_R); //Use gamma_e as a tmp buffer
        gfqm_add(broadcast->v, broadcast->v, gamma_e);
        gfqm_add(broadcast->v, broadcast->v, share->corr.c);
        gfqm_set_zero(gamma_e);
    } else {
        gfqm_set_zero(broadcast->v);
    }

    gfqm_vec_clear(E_vec);
    gfqm_vec_clear(E_vec_pow_k);
    gfqm_vec_clear(w);

    gfq_mat_clear(E);
    gfq_mat_clear(tmp);
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

    xof_context entropy_ctx;
    xof_init(&entropy_ctx);
    xof_update(&entropy_ctx, digest, PARAM_DIGEST_SIZE);
    xof_final(&entropy_ctx);
    samplable_t entropy = xof_to_samplable(&entropy_ctx);

    (void) inst;
    for(size_t num=0; num<nb; num++) {
        gfqm_rnd(challenges[num]->gamma, MIRA_PARAM_N, &entropy);
        gfqm_rnd(&challenges[num]->eps, 1, &entropy);
    } 
}
