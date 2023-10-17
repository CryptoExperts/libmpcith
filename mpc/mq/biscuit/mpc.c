#include "mpc.h"
#include "mpc-def.h"

void compute_correlated(vec_corr_t* corr, const vec_wit_t* wit, const vec_unif_t* unif, const instance_t* inst) {
    (void) inst;
    for(unsigned int k=0; k<PARAM_m; k++) {
        corr->c[k] = 0;
        for(unsigned int i=0; i<PARAM_n; i++) {
            corr->c[k] = sca_add(
                corr->c[k],
                sca_mul(wit->s[i], (*inst->f)[k][3*i+2])
            );
        }
        corr->c[k] = sca_mul(
            corr->c[k],
            unif->a[k]
        );
    }
}

int is_valid_plain_broadcast(vec_broadcast_t const* plain_br) {
    uint8_t ret = 0;
    // Just test if "plain_br->v" is zero
    for(unsigned int i=0; i<sizeof(plain_br->v); i++)
        ret |= ((uint8_t*) plain_br->v)[i];
    return (ret == 0);
}

void compress_plain_broadcast(uint8_t* buf, const vec_broadcast_t* plain_br) {
    vec_to_bytes(buf, plain_br->alpha, sizeof(plain_br->alpha));
}

void uncompress_plain_broadcast(vec_broadcast_t* plain_br, const uint8_t* buf) {
    vec_from_bytes(plain_br->alpha, buf, sizeof(plain_br->alpha));
    vec_setzero(plain_br->v, sizeof(plain_br->v));
}

void run_multiparty_computation(vec_broadcast_t* broadcast, vec_challenge_t* mpc_challenge,
                                vec_share_t const* share, vec_broadcast_t const* plain_br,
                                const instance_t* inst, char has_sharing_offset, char entire_computation) {

    __MPC_BEGIN__;
    
    __MPC_PARAM__("n", PARAM_n);
    __MPC_PARAM__("m", PARAM_m);

    __MPC_INPUT_VAR__("s", share->wit.s);
    __MPC_INPUT_VAR__("a", share->unif.a);
    __MPC_INPUT_VAR__("c", share->corr.c);

    __MPC_CHALLENGE__("eps", mpc_challenge->eps);

    __MPC_INST_VAR__("t", inst->t);
    __MPC_INST_VAR__("f", *inst->f);

    uint8_t x[PARAM_m] = {0};
    uint8_t y[PARAM_m] = {0};
    uint8_t z[PARAM_m] = {0};
    for(unsigned int k=0; k<PARAM_m; k++) {
        for(unsigned int i=0; i<PARAM_n; i++) {
            x[k] = sca_add(x[k], sca_mul((*inst->f)[k][3*i+1], share->wit.s[i]));
            if(entire_computation) {
                y[k] = sca_add(y[k], sca_mul((*inst->f)[k][3*i+2], share->wit.s[i]));
                z[k] = sca_add(z[k], sca_mul((*inst->f)[k][3*i+0], share->wit.s[i]));
            }
        }
        if(entire_computation) {
            if(has_sharing_offset)
                z[k] = sca_sub(inst->t[k], z[k]);
            else
                z[k] = sca_sub(0, z[k]);
        }
            

        broadcast->alpha[k] = sca_add(
            sca_mul(x[k], mpc_challenge->eps[k]),
            share->unif.a[k]
        );

        if(entire_computation)
            broadcast->v[k] = sca_sub(sca_sub(
                sca_mul(y[k], plain_br->alpha[k]),
                sca_mul(z[k], mpc_challenge->eps[k])),
                share->corr.c[k]
            );
        else
            broadcast->v[k] = 0;
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
        vec_rnd((uint8_t*) challenges[num]->eps, sizeof((*challenges)->eps), &entropy);

}
