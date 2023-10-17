#include "mpc.h"
#include "mpc-def.h"

void set_uint32(uint8_t* ptr, uint32_t v) {
    *((uint32_t*) ptr) = v;
}

uint32_t get_uint32(const uint8_t* ptr) {
    return *((uint32_t*) ptr);
}

void compute_correlated(vec_corr_t* corr, const vec_wit_t* wit, const vec_unif_t* unif, const instance_t* inst) {
    (void) inst;
    memset(corr->c, 0, sizeof(corr->c));
    for(int i=0; i<PARAM_CODE_WEIGHT; i++) {
        for(int j=0; j<PARAM_EXT_DEGREE; j++) {
            uint32_t v = gf2to31_mul(
                get_uint32(wit->beta[i]),
                get_uint32(unif->a[i][j])
            );
            v = gf2to31_add(get_uint32(corr->c[j]), v);
            set_uint32(corr->c[j], v);
        }
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
    
    __MPC_PARAM__("n", PARAM_CODE_LENGTH);
    __MPC_PARAM__("k", PARAM_CODE_DIMENSION);
    __MPC_PARAM__("r", PARAM_CODE_WEIGHT);
    __MPC_PARAM__("eta", PARAM_EXT_DEGREE);

    __MPC_INPUT_VAR__("x_A", share->wit.x_A);
    __MPC_INPUT_VAR__("beta", share->wit.beta);
    __MPC_INPUT_VAR__("a", share->unif.a);
    __MPC_INPUT_VAR__("c", share->corr.c);

    __MPC_CHALLENGE__("gamma", mpc_challenge->gamma);
    __MPC_CHALLENGE__("eps", mpc_challenge->eps);

    __MPC_INST_VAR__("y", inst->y);
    __MPC_INST_VAR__("H", *inst->mat_H);

    // x = x_A || x_B
    uint8_t* x = (uint8_t*) aligned_alloc(32, PARAM_CODEWORD_BYTESIZE_CEIL32);
    uint8_t* x_A = x;
    uint8_t* x_B = &x[PARAM_PLAINTEXT_BYTESIZE];
    memcpy(x_A, (uint8_t*) share->wit.x_A, PARAM_PLAINTEXT_BYTESIZE);
    if(has_sharing_offset)
        memcpy(x_B, inst->y, PARAM_SYNDROME_BYTESIZE);
    else
        memset(x_B, 0, PARAM_SYNDROME_BYTESIZE);
    uint8_t* cur_x;

    // x_B = y - H_A x_A
    gf2to31_matcols_muladd(x_B, (uint8_t*) share->wit.x_A, *inst->mat_H, PARAM_PLAINTEXT_LENGTH, PARAM_SYNDROME_LENGTH);
    __MPC_INTERMEDIATE_VAR__("x_B", x_B);

    // Compute w
    memset((uint8_t*) broadcast->alpha, 0, PARAM_CODE_WEIGHT*PARAM_EXT_DEGREE*PARAM_CODE_EXT_BYTESIZE);
    for(int i=0; i<PARAM_CODE_WEIGHT; i++) {
        cur_x = x;
        for(int j=0; j<PARAM_CODE_LENGTH; j++) {
            for(int k=0; k<PARAM_EXT_DEGREE; k++) {
                uint32_t v = gf2to31_mul(
                    get_uint32(mpc_challenge->gamma[j][k]),
                    get_uint32(cur_x)
                );
                v = gf2to31_add(get_uint32(broadcast->alpha[i][k]), v);
                set_uint32(broadcast->alpha[i][k], v);
            }
            cur_x += PARAM_CODE_EXT_BYTESIZE;
        }
        gf2to31_squ_tab((uint32_t*) x, PARAM_CODE_LENGTH);
    }
    __MPC_INTERMEDIATE_VAR__("w", broadcast->alpha);

    // Compute alpha
    for(int i=0; i<PARAM_CODE_WEIGHT; i++) {
        for(int k=0; k<PARAM_EXT_DEGREE; k++) {
            set_uint32(broadcast->alpha[i][k], gf2to31_add(
                gf2to31_mul(
                    get_uint32(broadcast->alpha[i][k]),
                    get_uint32(mpc_challenge->eps[k])
                ), get_uint32(share->unif.a[i][k])
            ));
        }
    }
    __MPC_INTERMEDIATE_VAR__("alpha", broadcast->alpha);

    if(entire_computation) {
        // Compute z
        uint8_t z[PARAM_EXT_DEGREE][PARAM_CODE_EXT_BYTESIZE];
        memset((uint8_t*) z, 0, PARAM_EXT_DEGREE*PARAM_CODE_EXT_BYTESIZE);
        cur_x = x;
        for(int j=0; j<PARAM_CODE_LENGTH; j++) {
            for(int k=0; k<PARAM_EXT_DEGREE; k++) {
                uint32_t v = gf2to31_mul(
                    get_uint32(mpc_challenge->gamma[j][k]),
                    get_uint32(cur_x)
                );
                v = gf2to31_add(get_uint32(z[k]), v);
                set_uint32(z[k], v);
            }
            cur_x += PARAM_CODE_EXT_BYTESIZE;
        }
        __MPC_INTERMEDIATE_VAR__("z", z);

        // Compute v
        for(int k=0; k<PARAM_EXT_DEGREE; k++) {
            set_uint32(broadcast->v[k], gf2to31_add(
                gf2to31_mul(
                    get_uint32(z[k]),
                    get_uint32(mpc_challenge->eps[k])
                ), get_uint32(share->corr.c[k])
            ));
        }
        for(int i=0; i<PARAM_CODE_WEIGHT; i++) {
            for(int k=0; k<PARAM_EXT_DEGREE; k++) {
                set_uint32(broadcast->v[k], gf2to31_add(
                    get_uint32(broadcast->v[k]),
                    gf2to31_mul(
                        get_uint32(plain_br->alpha[i][k]),
                        get_uint32(share->wit.beta[i])
                    )
                ));
            }
        }
    } else {
        memset(broadcast->v, 0, sizeof(plain_br->v));
    }
    __MPC_INTERMEDIATE_VAR__("v", broadcast->v);

    __MPC_END__;

    free(x);
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
        vec_rnd((uint8_t*) challenges[num]->gamma, sizeof((*challenges)->gamma), &entropy);
        vec_rnd((uint8_t*) challenges[num]->eps, sizeof((*challenges)->eps), &entropy);
    }
}
