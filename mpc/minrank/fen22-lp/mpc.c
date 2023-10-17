#include "mpc.h"
#include "mpc-def.h"
#include "benchmark.h"
#include <stdio.h>

void compute_correlated(vec_corr_t* corr, const vec_wit_t* wit, const vec_unif_t* unif, const instance_t* inst) {
    (void) inst;
    point_t tmp[PARAM_m];
    memset(corr->c, 0, sizeof(corr->c));
    for(int i=0; i<PARAM_r; i++)
        for(int j=0; j<PARAM_eta; j++) {
            ext_mul(tmp, wit->beta[i], unif->a[i][j]);
            ext_add(corr->c[j], corr->c[j], tmp);
        }
    for(int j=0; j<PARAM_eta; j++)
        ext_neg(corr->c[j]);
}

int is_valid_plain_broadcast(vec_broadcast_t const* plain_br) {
    point_t ret = 0;
    // Just test if "plain_br->v" is zero
    for(unsigned int i=0; i<sizeof(plain_br->v); i++)
        ret |= ((uint8_t*) plain_br->v)[i];
    return (ret == 0);
}

void compress_plain_broadcast(uint8_t* buf, const vec_broadcast_t* plain_br) {
    #ifdef VEC_COMPRESSIBLE
    vec_unif_compress(buf, (const vec_unif_t*) plain_br->alpha);
    #else
    memcpy(buf, plain_br->alpha, sizeof(plain_br->alpha));
    #endif
}

void uncompress_plain_broadcast(vec_broadcast_t* plain_br, const uint8_t* buf) {
    #ifdef VEC_COMPRESSIBLE
    vec_unif_decompress((vec_unif_t*) plain_br->alpha, buf);
    #else
    memcpy(plain_br->alpha, buf, sizeof(plain_br->alpha));
    #endif
    vec_setzero((point_t*) plain_br->v, sizeof(plain_br->v)/sizeof(point_t));
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
    __MPC_INPUT_VAR__("beta", share->wit.beta);
    __MPC_INPUT_VAR__("a", share->unif.a);
    __MPC_INPUT_VAR__("c", share->corr.c);

    __MPC_CHALLENGE__("gamma", mpc_challenge->gamma);
    __MPC_CHALLENGE__("eps", mpc_challenge->eps);

    __MPC_INST_VAR__("m0", inst->m0);
    __MPC_INST_VAR__("mats", *inst->mats);

    point_t tmp[PARAM_m];

    // Recompute the low-rank matrice mat_e
    point_t mat_e[PARAM_n][PARAM_m];
    if(has_sharing_offset)
        memcpy(mat_e, inst->m0, sizeof(mat_e));
    else
        memset(mat_e, 0, sizeof(mat_e));
    
    matcols_muladd((point_t*) mat_e, share->wit.x, *inst->mats, PARAM_k, PARAM_nm);
    __MPC_INTERMEDIATE_VAR__("mat_e", mat_e);

    // Compute w
    memset(broadcast->alpha, 0, sizeof(broadcast->alpha));
    for(int i=0; i<PARAM_r; i++) {
        for(int j=0; j<PARAM_n; j++) {
            for(int k=0; k<PARAM_eta; k++) {
                ext_mul(tmp, mpc_challenge->gamma[j][k], mat_e[j]);
                ext_add(broadcast->alpha[i][k], broadcast->alpha[i][k], tmp);
            }
            ext_powq(mat_e[j], mat_e[j]);
        }
    }
    __MPC_INTERMEDIATE_VAR__("w", broadcast->alpha);
    
    // Compute alpha
    for(int i=0; i<PARAM_r; i++) {
        for(int k=0; k<PARAM_eta; k++) {
            ext_mul(tmp, broadcast->alpha[i][k], mpc_challenge->eps[k]);
            ext_add(broadcast->alpha[i][k], tmp, share->unif.a[i][k]);
        }
    }
    __MPC_INTERMEDIATE_VAR__("alpha", broadcast->alpha);

    if(entire_computation) {
        // Compute z
        point_t z[PARAM_eta][PARAM_m];
        memset(z, 0, sizeof(z));
        for(int j=0; j<PARAM_n; j++) {
            for(int k=0; k<PARAM_eta; k++) {
                ext_mul(tmp, mpc_challenge->gamma[j][k], mat_e[j]);
                ext_add(z[k], z[k], tmp);
            }
        }
        for(int k=0; k<PARAM_eta; k++)
            ext_neg(z[k]);
        __MPC_INTERMEDIATE_VAR__("z", z);

        // Compute v
        for(int k=0; k<PARAM_eta; k++) {
            ext_mul(tmp, z[k], mpc_challenge->eps[k]);
            ext_sub(broadcast->v[k], tmp, share->corr.c[k]);
        }
        for(int i=0; i<PARAM_r; i++) {
            for(int k=0; k<PARAM_eta; k++) {
                ext_mul(tmp, plain_br->alpha[i][k], share->wit.beta[i]);
                ext_sub(broadcast->v[k], broadcast->v[k], tmp);
            }
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
    point_t tmp[PARAM_m];

    // Recompute the low-rank matrice mat_e
    point_t mat_e[PARAM_n][PARAM_m];
    if(has_sharing_offset)
        memcpy(mat_e, inst->m0, sizeof(mat_e));
    else
        memset(mat_e, 0, sizeof(mat_e));
    matcols_muladd((point_t*) mat_e, share->wit.x, *inst->mats, PARAM_k, PARAM_nm);

    // Compute w
    memset(share->unif.a, 0, sizeof(share->unif.a));
    for(int i=0; i<PARAM_r; i++) {
        for(int j=0; j<PARAM_n; j++) {
            for(int k=0; k<PARAM_eta; k++) {
                ext_mul(tmp, mpc_challenge->gamma[j][k], mat_e[j]);
                ext_add(share->unif.a[i][k], share->unif.a[i][k], tmp);
            }
            ext_powq(mat_e[j], mat_e[j]);
        }
    }

    // Compute alpha
    for(int i=0; i<PARAM_r; i++) {
        for(int k=0; k<PARAM_eta; k++) {
            ext_mul(tmp, share->unif.a[i][k], mpc_challenge->eps[k]);
            ext_sub(share->unif.a[i][k], broadcast->alpha[i][k], tmp);
        }
    }

    // Compute z
    point_t z[PARAM_eta][PARAM_m];
    memset(z, 0, sizeof(z));
    for(int j=0; j<PARAM_n; j++) {
        for(int k=0; k<PARAM_eta; k++) {
            ext_mul(tmp, mpc_challenge->gamma[j][k], mat_e[j]);
            ext_add(z[k], z[k], tmp);
        }
    }
    for(int k=0; k<PARAM_eta; k++)
        ext_neg(z[k]);

    // Compute v
    for(int k=0; k<PARAM_eta; k++) {
        ext_mul(tmp, z[k], mpc_challenge->eps[k]);
        ext_sub(share->corr.c[k], tmp, broadcast->v[k]);
    }
    for(int i=0; i<PARAM_r; i++) {
        for(int k=0; k<PARAM_eta; k++) {
            ext_mul(tmp, plain_br->alpha[i][k], share->wit.beta[i]);
            ext_sub(share->corr.c[k], share->corr.c[k], tmp);
        }
    }

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
        vec_rnd((point_t*) challenges[num]->gamma, PARAM_n*PARAM_eta*PARAM_m, &entropy);
        vec_rnd((point_t*) challenges[num]->eps, PARAM_eta*PARAM_m, &entropy);
    } 
}
