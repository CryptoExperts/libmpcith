#include "mpc.h"
#include "mpc-def.h"
#include "field-interface.h"

void compute_correlated(vec_corr_t* corr, const vec_wit_t* wit, const vec_unif_t* unif, const instance_t* inst) {
    (void) inst;
    GF_mul(wit->pt, unif->a, corr->c);
}

int is_valid_plain_broadcast(vec_broadcast_t const* plain_br) {
    return GF_is_zero(plain_br->v);
}

void compress_plain_broadcast(uint8_t* buf, const vec_broadcast_t* plain_br) {
    GF_to_bytes(plain_br->alpha, buf);
}

void uncompress_plain_broadcast(vec_broadcast_t* plain_br, const uint8_t* buf) {
    GF_from_bytes(buf, plain_br->alpha);
    GF_set0(plain_br->v);
}

void run_multiparty_computation(vec_broadcast_t* broadcast, vec_challenge_t* mpc_challenge,
                                vec_share_t const* share, vec_broadcast_t const* plain_br,
                                const instance_t* inst, char has_sharing_offset, char entire_computation) {

    GF temp;
    GF x[NUM_INPUT_SBOX + 1];
    GF z[NUM_INPUT_SBOX + 1];

    // Get [[x]] and [[z]] from [[pt]] and [[t]]
    aim_mpc_(
        share->wit.pt, share->wit.t,
        (GF const* const*) inst->matrix_A, *inst->vector_b, inst->ct,
        z, x, has_sharing_offset
    );

    // [[alpha]] <- [[x_0]]*eps_0 + [[a]]
    GF_mul(x[0], mpc_challenge->eps[0], temp);
    GF_add(share->unif.a, temp, broadcast->alpha);

    for(size_t ell = 1; ell < (NUM_INPUT_SBOX + 1); ell++) {
        // [[alpha]] <- [[alpha]] + [[x_ell]]*eps_ell
        GF_mul(x[ell], mpc_challenge->eps[ell], temp);
        GF_add(broadcast->alpha, temp, broadcast->alpha);
    }

    if(entire_computation) {
        // [[v]] <- alpha*[[pt]] + [[c]]
        GF_mul(plain_br->alpha, share->wit.pt, temp);
        GF_add(share->corr.c, temp, broadcast->v);
        for(size_t ell = 0; ell < (NUM_INPUT_SBOX + 1); ell++) {
            // [[v]] <- [[v]] + [[z_ell]]*eps_ell
            GF_mul(mpc_challenge->eps[ell], z[ell], temp);
            GF_add(broadcast->v, temp, broadcast->v);
        }
    } else {
        GF_set0(broadcast->v);
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

    for(size_t num=0; num<nb; num++)
        GF_rnd(challenges[num]->eps, NUM_INPUT_SBOX+1, &entropy);
}
