#include "mpc.h"
#include "mpc-def.h"
#include "benchmark.h"
#include "precomputed.h"

int is_valid_plain_broadcast(vec_broadcast_t const* plain_br) {
    uint8_t ret = 0;
    // Just test if "plain_br->v" is zero
    for(unsigned int i=0; i<sizeof(plain_br->v); i++)
        ret |= ((uint8_t*) plain_br->v)[i];
    return (ret == 0);
}

void compute_hint(vec_hint_t* hint, const vec_wit_t* wit, const vec_unif_t* unif, const instance_t* inst, vec_challenge_1_t* mpc_challenge_1) {
    uint8_t tmp[PARAM_eta];

    // Compute w = linA x
    uint8_t w[PARAM_n][PARAM_eta] = {{0}};
    #ifdef PARAM_PRECOMPUTE_LIN_A
    (void) inst;
    matcols_muladd_triangular((uint8_t*) w, wit->x, (uint8_t*) mpc_challenge_1->linA, PARAM_n, PARAM_eta);
    #else
    uint8_t intermediary[PARAM_n];
    for(unsigned int j=0; j<PARAM_m; j++) {
        memset(intermediary, 0, sizeof(intermediary));
        matcols_muladd_triangular(intermediary, wit->x, (*inst->A)[j], PARAM_n, 1);
        for(unsigned int i=0; i<PARAM_n; i++) {
            ext_mulsc(tmp, intermediary[i], mpc_challenge_1->gamma[j]);
            ext_add(w[i], w[i], tmp);
        }
    }
    #endif

    // Interpolation of w -> \hat{W}
    uint8_t w_hat[PARAM_n][PARAM_eta] = {{0}};
    for(unsigned int i=0; i<PARAM_eta; i++) {
        uint8_t input[PARAM_n] = {0};
        uint8_t partial[PARAM_n] = {0};
        for(unsigned int j=0; j<PARAM_n; j++)
            input[j] = w[j][i];
        for(unsigned int offset=0; offset<(PARAM_n2-1)*PARAM_n1; offset+=PARAM_n1)
            matcols_muladd(partial+offset, input+offset, interpolation_matrix_default, PARAM_n1, PARAM_n1);
        matcols_muladd(partial+(PARAM_n-PARAM_last), input+(PARAM_n-PARAM_last), interpolation_matrix_last, PARAM_last, PARAM_last);        
        for(unsigned int j=0; j<PARAM_n; j++)
            w_hat[j][i] = partial[j];
    }

    // Interpolation of x
    uint8_t x_extended[PARAM_n2][PARAM_n1] = {{0}};
    memcpy((uint8_t*) x_extended, wit->x, PARAM_n);
    uint8_t x_poly[PARAM_n2][PARAM_n1] = {{0}};
    for(unsigned int i=0; i<PARAM_n2; i++)
        matcols_muladd(x_poly[i], x_extended[i], interpolation_matrix_default, PARAM_n1, PARAM_n1);

    // Compute Q
    memset(hint->q_poly, 0, sizeof(hint->q_poly));
    for(unsigned int j=0; j<PARAM_n2; j++) {
        unsigned int offset = j*PARAM_n1;
        unsigned int size = (j == PARAM_n2-1) ? PARAM_last :PARAM_n1;

        uint8_t w_tilde[PARAM_n1+1][PARAM_eta] = {{0}};            
        // we assume here that "vanishing(0)=0"
        for(unsigned int i=1; i<PARAM_n1; i++)
            ext_mulsc(w_tilde[i], vanishing_polynomial[i], unif->a[j]);
        memcpy(w_tilde[PARAM_n1],unif->a[j],PARAM_eta);
        // \tilde{w}(X) += \hat{w}(X)
        vec_add((uint8_t*) w_tilde, (uint8_t*) (w_hat+offset), size*PARAM_eta);

        for(unsigned int i=0; i<PARAM_n1+1; i++)
            for(unsigned int k=0; k<PARAM_n1; k++) {
                if(i+k == 0) continue;
                ext_mulsc(tmp, x_poly[j][k], w_tilde[i]);
                ext_add(hint->q_poly[i+k-1], hint->q_poly[i+k-1], tmp);
            }
    }
}

static void run_multiparty_computation(vec_broadcast_t* broadcast,
                                vec_challenge_1_t* mpc_challenge_1, vec_challenge_2_t* mpc_challenge_2,
                                vec_share_t const* share, vec_broadcast_t const* plain_br,
                                const instance_t* inst, char has_sharing_offset, char entire_computation) {

    __MPC_BEGIN__;
    
    __MPC_PARAM__("n", PARAM_n);
    __MPC_PARAM__("m", PARAM_m);
    __MPC_PARAM__("m1", PARAM_n1);
    __MPC_PARAM__("m2", PARAM_n2);
    __MPC_PARAM__("eta", PARAM_eta);
    __MPC_PARAM__("last", PARAM_last);

    __MPC_INPUT_VAR__("x", share->wit.x);
    __MPC_INPUT_VAR__("a", share->unif.a);
    __MPC_INPUT_VAR__("q_poly", share->hint.q_poly);

    __MPC_CHALLENGE__("gamma", mpc_challenge_1->gamma);
    __MPC_CHALLENGE__("r", mpc_challenge_2->r);
    __MPC_CHALLENGE__("linA", mpc_challenge_1->linA);
    __MPC_CHALLENGE__("r_powers", mpc_challenge_2->r_powers);

    __MPC_INST_VAR__("y", inst->y);
    __MPC_INST_VAR__("A", *inst->A);
    __MPC_INST_VAR__("b", *inst->b);

    uint8_t tmp[PARAM_eta];
    uint8_t acc[PARAM_eta];

    // [[w]] = linA [[x]]
    uint8_t w[PARAM_n][PARAM_eta] = {{0}};
    #ifdef PARAM_PRECOMPUTE_LIN_A
    matcols_muladd_triangular((uint8_t*) w, share->wit.x, (uint8_t*) mpc_challenge_1->linA, PARAM_n, PARAM_eta);
    #else
    uint8_t intermediary[PARAM_n];
    for(unsigned int j=0; j<PARAM_m; j++) {
        memset(intermediary, 0, sizeof(intermediary));
        matcols_muladd_triangular(intermediary, share->wit.x, (*inst->A)[j], PARAM_n, 1);
        for(unsigned int i=0; i<PARAM_n; i++) {
            ext_mulsc(tmp, intermediary[i], mpc_challenge_1->gamma[j]);
            ext_add(w[i], w[i], tmp);
        }
    }
    #endif
    __MPC_INTERMEDIATE_VAR__("w", w);
    // Interpolation of [[w]]
    uint8_t w_hat[PARAM_n][PARAM_eta] = {{0}};
    for(unsigned int i=0; i<PARAM_eta; i++) {
        uint8_t input[PARAM_n] = {0};
        uint8_t partial[PARAM_n] = {0};
        for(unsigned int j=0; j<PARAM_n; j++)
            input[j] = w[j][i];
        for(unsigned int offset=0; offset<(PARAM_n2-1)*PARAM_n1; offset+=PARAM_n1)
            matcols_muladd(partial+offset, input+offset, interpolation_matrix_default, PARAM_n1, PARAM_n1);
        matcols_muladd(partial+(PARAM_n-PARAM_last), input+(PARAM_n-PARAM_last), interpolation_matrix_last, PARAM_last, PARAM_last);        
        for(unsigned int j=0; j<PARAM_n; j++)
            w_hat[j][i] = partial[j];
    }
    __MPC_INTERMEDIATE_VAR__("w_hat", w_hat);

    // Parse [[W^]]
    // [[W~]] = [[W^]] + [[a]]*vanishing
    uint8_t w_poly[PARAM_n2][PARAM_n1+1][PARAM_eta] = {{{0}}};   
    for(unsigned int j=0; j<PARAM_n2; j++) {
        unsigned int offset = j*PARAM_n1;
        unsigned int size = (j == PARAM_n2-1) ? PARAM_last : PARAM_n1;

        // we assume here that "vanishing(0)=0"
        for(unsigned int i=1; i<PARAM_n1; i++)
            ext_mulsc(w_poly[j][i], vanishing_polynomial[i], share->unif.a[j]);
        memcpy(w_poly[j][PARAM_n1],share->unif.a[j],PARAM_eta);
        // \tilde{w}(X) += \hat{w}(X)
        vec_add((uint8_t*) w_poly[j], (uint8_t*) (w_hat+offset), size*PARAM_eta);
    }
    __MPC_INTERMEDIATE_VAR__("w_poly", w_poly);

    // [[\alpha]] = [[W~]](r)
    memset(broadcast->alpha, 0, sizeof(broadcast->alpha));
    for(unsigned int j=0; j<PARAM_n2; j++) {
        for(unsigned int i=0; i<PARAM_n1+1; i++) {
            ext_mul(tmp, mpc_challenge_2->r_powers[i], w_poly[j][i]);
            ext_add(broadcast->alpha[j], broadcast->alpha[j], tmp);
        }
    }
    __MPC_INTERMEDIATE_VAR__("alpha", broadcast->alpha);

    if(entire_computation) {
        // [[z]] = sum_{j=1}^m gamma_j * (y_j - b_j^T [[x]])
        uint8_t z[PARAM_eta] = {0};
        for(int j=0; j<PARAM_m; j++) {
            uint8_t res = sca_innerproduct((*inst->b)[j], share->wit.x, PARAM_n);
            if(has_sharing_offset)
                res = sca_sub(inst->y[j], res);
            else
                res = sca_neg(res);
            ext_mulsc(tmp, res, mpc_challenge_1->gamma[j]);
            ext_add(z, z, tmp);
        }
        __MPC_INTERMEDIATE_VAR__("z", z);

        // Interpolation of [[x]]
        uint8_t x_extended[PARAM_n2][PARAM_n1] = {{0}};
        memcpy((uint8_t*) x_extended, share->wit.x, PARAM_n);
        uint8_t x_poly[PARAM_n2][PARAM_n1] = {{0}};
        for(unsigned int i=0; i<PARAM_n2; i++)
            matcols_muladd(x_poly[i], x_extended[i], interpolation_matrix_default, PARAM_n1, PARAM_n1);
        __MPC_INTERMEDIATE_VAR__("x_poly", x_poly);

        // [[beta]] = (1/n1) * ([[z]] - sum_i i*[[Q]](i))
        uint8_t beta[PARAM_eta];
        memset(beta, 0, sizeof(beta));
        for(uint8_t i=0; i<PARAM_n1; i++) {
            uint8_t i_power = i;
            for(unsigned int k=0; k<2*PARAM_n1-1; k++) {
                ext_mulsc(tmp, i_power, share->hint.q_poly[k]);
                ext_add(beta, beta, tmp);
                i_power = sca_mul(i_power, i);
            }
        }
        ext_sub(beta, z, beta);
        ext_mulsc(beta, inv_n1, beta);
        __MPC_INTERMEDIATE_VAR__("beta", beta);

        // [[v]] = r*[[Q]](r) + beta - sum_j \alpha [[X^]](r)
        memset(broadcast->v, 0, sizeof(broadcast->v));
        for(unsigned int j=0; j<PARAM_n2; j++) {
            memset(acc, 0, sizeof(tmp));
            for(unsigned int i=0; i<PARAM_n1; i++) {
                ext_mulsc(tmp, x_poly[j][i], mpc_challenge_2->r_powers[i]);
                ext_add(acc, acc, tmp);
            }
            ext_mul(acc, acc, plain_br->alpha[j]);
            ext_add(broadcast->v, broadcast->v, acc);
        }
        vec_neg(broadcast->v, PARAM_eta);
        ext_add(broadcast->v, broadcast->v, beta);
        for(unsigned int k=0; k<2*PARAM_n1-1; k++) {
            ext_mul(tmp, share->hint.q_poly[k], mpc_challenge_2->r_powers[k+1]);
            ext_add(broadcast->v, broadcast->v, tmp);
        }
    } else {
        memset(broadcast->v, 0, sizeof(broadcast->v));
    }
    __MPC_INTERMEDIATE_VAR__("v", broadcast->v);

    __MPC_END__;
}

void mpc_compute_plain_broadcast(vec_broadcast_t* plain_br,
                                vec_challenge_1_t* mpc_challenge_1, vec_challenge_2_t* mpc_challenge_2,
                                vec_share_t const* share, const instance_t* inst) {
    run_multiparty_computation(plain_br, mpc_challenge_1, mpc_challenge_2, share, plain_br, inst, 1, 0);
}

void mpc_compute_communications(vec_broadcast_t* broadcast,
                                vec_challenge_1_t* mpc_challenge_1, vec_challenge_2_t* mpc_challenge_2,
                                vec_share_t const* share, vec_broadcast_t const* plain_br,
                                const instance_t* inst, char has_sharing_offset) {
    run_multiparty_computation(broadcast, mpc_challenge_1, mpc_challenge_2, share, plain_br, inst, has_sharing_offset, 1);
}

void expand_mpc_challenge_hash_1(vec_challenge_1_t** challenges, const uint8_t* digest, size_t nb, instance_t* inst) {

    xof_context entropy_ctx;
    xof_init(&entropy_ctx);
    xof_update(&entropy_ctx, digest, PARAM_DIGEST_SIZE);
    samplable_t entropy = xof_to_samplable(&entropy_ctx);

    for(size_t num=0; num<nb; num++) {
        vec_rnd((uint8_t*) challenges[num]->gamma, sizeof((*challenges)->gamma), &entropy);

        #ifdef PARAM_PRECOMPUTE_LIN_A
            uint8_t gamma_[PARAM_eta][PARAM_m];
            uint8_t linA_[PARAM_eta][PARAM_MATRIX_BYTESIZE] = {{0}};
            for(unsigned int i=0; i<PARAM_eta; i++) {
                for(unsigned int j=0; j<PARAM_m; j++)
                    gamma_[i][j] = challenges[num]->gamma[j][i];

                mat128cols_muladd(linA_[i], gamma_[i], (uint8_t*)(*inst->A), PARAM_m, PARAM_MATRIX_BYTESIZE);

                for(unsigned int j=0; j<PARAM_MATRIX_BYTESIZE; j++)
                        challenges[num]->linA[j][i] = linA_[i][j];
            }
        #else
        (void) inst;
        #endif
    }
}

void expand_mpc_challenge_hash_2(vec_challenge_2_t** challenges, const uint8_t* digest, size_t nb, instance_t* inst) {

    xof_context entropy_ctx;
    xof_init(&entropy_ctx);
    xof_update(&entropy_ctx, digest, PARAM_DIGEST_SIZE);
    xof_final(&entropy_ctx);
    samplable_t entropy = xof_to_samplable(&entropy_ctx);

    (void) inst;
    for(size_t num=0; num<nb; num++) {
        unsigned int valid_challenge = 0;
        while(!valid_challenge) {
            vec_rnd((uint8_t*) challenges[num]->r, sizeof((*challenges)->r), &entropy);

            uint8_t val = 0;
            for(unsigned int i=1; i<PARAM_eta; i++)
                val |= challenges[num]->r[i];
            valid_challenge = !((val == 0) && (challenges[num]->r[0] < PARAM_n1));
        }
    }

    for(size_t num=0; num<nb; num++) {
        uint8_t* point = challenges[num]->r;
        // powers[0] = 1
        memset(&challenges[num]->r_powers[0], 0, PARAM_eta);
        challenges[num]->r_powers[0][0] = 1;
        // powers[1] = point
        memcpy(&challenges[num]->r_powers[1], point, PARAM_eta);
        // powers[i] = evals[i-1]*point
        for(unsigned int i=2; i<2*PARAM_n1; i++)
            ext_mul(
                challenges[num]->r_powers[i],
                challenges[num]->r_powers[i-1],
                point
            );
    }
}
