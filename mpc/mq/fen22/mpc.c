#include "mpc.h"
#include <stdio.h>
#include "mpc-def.h"

void compute_correlated(vec_corr_t* corr, const vec_wit_t* wit, const vec_unif_t* unif, const instance_t* inst) {
    (void) inst;
    vec_setzero(corr->c, sizeof(corr->c));
    point_t tmp[PARAM_eta];
    for(int i=0; i<PARAM_n; i++) {
        ext_mulsc(tmp, wit->x[i], unif->a[i]);
        ext_add(corr->c, corr->c, tmp);
    }
    vec_neg(corr->c, sizeof(corr->c));
}

int is_valid_plain_broadcast(vec_broadcast_t const* plain_br) {
    uint8_t ret = 0;
    // Just test if "plain_br->v" is zero
    for(unsigned int i=0; i<sizeof(plain_br->v); i++)
        ret |= ((uint8_t*) plain_br->v)[i];
    return (ret == 0);
}

void compress_plain_broadcast(uint8_t* buf, const vec_broadcast_t* plain_br) {
    vec_to_bytes(buf, (const point_t*) plain_br->alpha, sizeof(plain_br->alpha));
}

void uncompress_plain_broadcast(vec_broadcast_t* plain_br, const uint8_t* buf) {
    vec_from_bytes((point_t*) plain_br->alpha, buf, sizeof(plain_br->alpha));
    vec_setzero(plain_br->v, sizeof(plain_br->v));
}

void run_multiparty_computation(vec_broadcast_t* broadcast, vec_challenge_t* mpc_challenge,
                                vec_share_t const* share, vec_broadcast_t const* plain_br,
                                const instance_t* inst, char has_sharing_offset, char entire_computation) {

    __MPC_BEGIN__;
    
    __MPC_PARAM__("n", PARAM_n);
    __MPC_PARAM__("m", PARAM_m);
    __MPC_PARAM__("eta", PARAM_eta);

    __MPC_INPUT_VAR__("x", share->wit.x);
    __MPC_INPUT_VAR__("a", share->unif.a);
    __MPC_INPUT_VAR__("c", share->corr.c);

    __MPC_CHALLENGE__("gamma", mpc_challenge->gamma);
    __MPC_CHALLENGE__("eps", mpc_challenge->eps);
    __MPC_CHALLENGE__("linA", mpc_challenge->linA);

    __MPC_INST_VAR__("y", inst->y);
    __MPC_INST_VAR__("A", *inst->A);
    __MPC_INST_VAR__("b", *inst->b);

    // [[w]] = linA [[x]]
    memset(broadcast->alpha, 0, sizeof(broadcast->alpha));
    matcols_muladd((point_t*) broadcast->alpha, share->wit.x, (point_t*) mpc_challenge->linA, PARAM_n, PARAM_n*PARAM_eta);
    __MPC_INTERMEDIATE_VAR__("w", broadcast->alpha);

    // [[alpha]] = eps*[[w]] + [[a]]
    for(int i=0; i<PARAM_n; i++)
        ext_mul(broadcast->alpha[i], broadcast->alpha[i], mpc_challenge->eps);
    vec_add((point_t*) broadcast->alpha, (point_t*) share->unif.a, sizeof(share->unif.a));
    __MPC_INTERMEDIATE_VAR__("alpha", broadcast->alpha);

    if(entire_computation) {
        point_t tmp[PARAM_eta];
        // [[z]] = sum_{j=1}^m gamma_j * (y_j - b_j^T [[x]])
        memset(broadcast->v, 0, sizeof(broadcast->v));
        for(int j=0; j<PARAM_m; j++) {
            point_t v = 0;
            for(int i=0; i<PARAM_n; i++)
                v = sca_add(v, 
                    sca_mul((*inst->b)[j][i], share->wit.x[i])
                );
            if(has_sharing_offset)
                v = sca_sub(inst->y[j], v);
            else
                v = sca_neg(v);
            ext_mulsc(tmp, v, mpc_challenge->gamma[j]);
            ext_add(broadcast->v, broadcast->v, tmp);
        }
        __MPC_INTERMEDIATE_VAR__("z", broadcast->v);

        // [[v]] = eps*[[z]] - <alpha, [[x]]> - [[c]]
        ext_mul(broadcast->v, broadcast->v, mpc_challenge->eps);
        ext_sub(broadcast->v, broadcast->v, share->corr.c);
        for(int i=0; i<PARAM_n; i++) {
            ext_mulsc(tmp, share->wit.x[i], plain_br->alpha[i]);
            ext_sub(broadcast->v, broadcast->v, tmp);
        }
    } else {
        memset(broadcast->v, 0, sizeof(broadcast->v));
    }
    __MPC_INTERMEDIATE_VAR__("v", broadcast->v);

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

    // [[w]] = linA [[x]]
    memset(share->unif.a, 0, sizeof(share->unif.a));
    matcols_muladd((point_t*) share->unif.a, share->wit.x, (point_t*) mpc_challenge->linA, PARAM_n, PARAM_n*PARAM_eta);

    // [[a]] = [[alpha]] - eps*[[w]]
    for(int i=0; i<PARAM_n; i++)
        ext_mul(share->unif.a[i], share->unif.a[i], mpc_challenge->eps);
    vec_neg((point_t*) share->unif.a, sizeof(share->unif.a));
    vec_add((point_t*) share->unif.a, (point_t*) broadcast->alpha, sizeof(share->unif.a));

    point_t tmp[PARAM_eta];
    // [[z]] = sum_{j=1}^m gamma_j * (y_j - b_j^T [[x]])
    memset(share->corr.c, 0, sizeof(share->corr.c));
    for(int j=0; j<PARAM_m; j++) {
        point_t v = 0;
        for(int i=0; i<PARAM_n; i++)
            v = sca_add(v, 
                sca_mul((*inst->b)[j][i], share->wit.x[i])
            );
        if(has_sharing_offset)
            v = sca_sub(inst->y[j], v);
        else
            v = sca_neg(v);
        ext_mulsc(tmp, v, mpc_challenge->gamma[j]);
        ext_add(share->corr.c, share->corr.c, tmp);
    }

    // [[c]] = eps*[[z]] - <alpha, [[x]]> - [[v]]
    ext_mul(share->corr.c, share->corr.c, mpc_challenge->eps);
    ext_sub(share->corr.c, share->corr.c, broadcast->v);
    for(int i=0; i<PARAM_n; i++) {
        ext_mulsc(tmp, share->wit.x[i], plain_br->alpha[i]);
        ext_sub(share->corr.c, share->corr.c, tmp);
    }

    __MPC_END__;
}

void expand_mpc_challenge_hash(vec_challenge_t** challenges, const uint8_t* digest, size_t nb, instance_t* inst) {

    xof_context entropy_ctx;
    xof_init(&entropy_ctx);
    xof_update(&entropy_ctx, digest, PARAM_DIGEST_SIZE);
    xof_final(&entropy_ctx);
    samplable_t entropy = xof_to_samplable(&entropy_ctx);

    for(size_t num=0; num<nb; num++) {
        vec_rnd((point_t*) challenges[num]->gamma, sizeof((*challenges)->gamma), &entropy);
        vec_rnd((point_t*) challenges[num]->eps, sizeof((*challenges)->eps), &entropy);

        memset(challenges[num]->linA, 0, sizeof(challenges[num]->linA));
        for(int k=0; k<PARAM_eta; k++) {
            matcols_muladd_scaled(
                ((point_t*) challenges[num]->linA)+k,
                ((point_t*) challenges[num]->gamma)+k,
                (point_t*) (*inst->A),
                PARAM_m, PARAM_n*PARAM_n,
                PARAM_eta /* scale*/
            );
        }
    }
}
