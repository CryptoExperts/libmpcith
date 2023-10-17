#include "mpc.h"
#include "field.h"
#include "poly.h"
#include "mpc-def.h"
#include "benchmark.h"
#include <stdlib.h>

extern uint8_t f_poly[];

void compute_correlated(vec_corr_t* corr, const vec_wit_t* wit, const vec_unif_t* unif, const instance_t* inst) {
    (void) wit; // The correlated part does not depend on the witness
    (void) inst;
    uint8_t tmp[PARAM_EXT_DEGREE];
    for(int j=0; j<PARAM_NB_EVALS_PER_POLY; j++) {
        memset(corr->c[j], 0, PARAM_EXT_DEGREE);
        for(int npoly=0; npoly<PARAM_SPLITTING_FACTOR; npoly++) {
            ext_mul(tmp, unif->a[npoly][j], unif->b[npoly][j]);
            ext_add(corr->c[j], corr->c[j], tmp);
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
    vec_set(buf, (uint8_t*) plain_br->alpha, sizeof(plain_br->alpha));
    vec_set(buf+sizeof(plain_br->alpha), (uint8_t*) plain_br->beta, sizeof(plain_br->beta));
}

void uncompress_plain_broadcast(vec_broadcast_t* plain_br, const uint8_t* buf) {
    vec_set((uint8_t*) plain_br->alpha, buf, sizeof(plain_br->alpha));
    vec_set((uint8_t*) plain_br->beta, buf+sizeof(plain_br->alpha), sizeof(plain_br->beta));
    vec_setzero((uint8_t*) plain_br->v, sizeof(plain_br->v));
}

static inline void get_evals_in_all_points(uint8_t evals[][PARAM_EXT_DEGREE], const uint8_t* poly, uint32_t nb_coeffients, const uint8_t powers_of_ch[PARAM_CHUNK_LENGTH+1][PARAM_ALL_EVALS_BYTESIZE_CEIL16]) {
    uint8_t* evals_aligned = (uint8_t*) aligned_alloc(32, ((PARAM_ALL_EVALS_BYTESIZE_CEIL16+31)>>5)<<5);
    memset(evals_aligned, 0, PARAM_ALL_EVALS_BYTESIZE_CEIL16);
    mat16cols_muladd(evals_aligned, poly, (const uint8_t*) powers_of_ch, nb_coeffients, PARAM_ALL_EVALS_BYTESIZE_CEIL16);
    memcpy((uint8_t*) evals, evals_aligned, PARAM_NB_EVALS_PER_POLY*PARAM_EXT_DEGREE);
    free(evals_aligned);
}

static void run_multiparty_computation(vec_broadcast_t* broadcast, vec_challenge_t* mpc_challenge,
                                vec_share_t const* share, vec_broadcast_t const* plain_br,
                                const instance_t* inst, char has_sharing_offset, char entire_computation) {
    __MPC_BEGIN__;

    __MPC_PARAM__("n", PARAM_CODE_LENGTH);
    __MPC_PARAM__("k", PARAM_CODE_DIMENSION);
    __MPC_PARAM__("w", PARAM_CODE_WEIGHT);

    __MPC_INPUT_VAR__("s_A", share->wit.s_A);
    __MPC_INPUT_VAR__("q", share->wit.q_poly);
    __MPC_INPUT_VAR__("p", share->wit.q_poly);
    __MPC_INPUT_VAR__("a", share->unif.a);
    __MPC_INPUT_VAR__("b", share->unif.b);
    __MPC_INPUT_VAR__("c", share->corr.c);

    __MPC_CHALLENGE__("evals", mpc_challenge->eval);
    __MPC_CHALLENGE__("eps", mpc_challenge->eps);

    __MPC_INST_VAR__("y", *inst->y);
    __MPC_INST_VAR__("H", *inst->mat_H);

    // s = s_A || s_B
    uint8_t* s = (uint8_t*) aligned_alloc(32, PARAM_CODEWORD_LENGTH_CEIL32);
    uint8_t* s_A = s;
    uint8_t* s_B = &s[PARAM_PLAINTEXT_LENGTH];
    memcpy(s_A, share->wit.s_A, PARAM_PLAINTEXT_LENGTH);
    if(has_sharing_offset) {
        memcpy(s_B, inst->y, PARAM_SYNDROME_LENGTH);
    } else {
        memset(s_B, 0, PARAM_SYNDROME_LENGTH);
    }

    // s_B = y - H_A x_A
    vec_neg(s_B, PARAM_SYNDROME_LENGTH);
    matcols_muladd(s_B, share->wit.s_A, *inst->mat_H, PARAM_PLAINTEXT_LENGTH, PARAM_SYNDROME_LENGTH);
    vec_neg(s_B, PARAM_SYNDROME_LENGTH);
    __MPC_INTERMEDIATE_VAR__("s_B", s_B);

    memset(broadcast->alpha, 0, sizeof(broadcast->alpha));
    for(int npoly=0; npoly<PARAM_SPLITTING_FACTOR; npoly++)
        get_evals_in_all_points(broadcast->alpha[npoly], share->wit.q_poly[npoly], PARAM_CHUNK_WEIGHT, (const uint8_t(*)[PARAM_ALL_EVALS_BYTESIZE_CEIL16]) mpc_challenge->powers_of_ch);
    __MPC_INTERMEDIATE_VAR__("q_evals", broadcast->alpha);

    memset(broadcast->beta, 0, sizeof(broadcast->beta));
    for(int npoly=0; npoly<PARAM_SPLITTING_FACTOR; npoly++)
        get_evals_in_all_points(broadcast->beta[npoly], &s[npoly*PARAM_CHUNK_LENGTH], PARAM_CHUNK_LENGTH, (const uint8_t(*)[PARAM_ALL_EVALS_BYTESIZE_CEIL16]) mpc_challenge->powers_of_ch);
    __MPC_INTERMEDIATE_VAR__("s_evals", broadcast->alpha);

    uint8_t p_evals[PARAM_SPLITTING_FACTOR][PARAM_NB_EVALS_PER_POLY][PARAM_EXT_DEGREE];
    if(entire_computation) {
        memset(p_evals, 0, sizeof(p_evals));
        for(int npoly=0; npoly<PARAM_SPLITTING_FACTOR; npoly++)
            get_evals_in_all_points(p_evals[npoly], share->wit.p_poly[npoly], PARAM_CHUNK_WEIGHT, (const uint8_t(*)[PARAM_ALL_EVALS_BYTESIZE_CEIL16]) mpc_challenge->powers_of_ch);
        __MPC_INTERMEDIATE_VAR__("p_evals", p_evals);
    }

    // Compute share of \alpha and \beta
    for(int j=0; j<PARAM_NB_EVALS_PER_POLY; j++) {
        memset(broadcast->v[j], 0, PARAM_EXT_DEGREE);
        if(entire_computation)
            ext_sub(broadcast->v[j], broadcast->v[j], share->corr.c[j]);
        for(int npoly=0; npoly<PARAM_SPLITTING_FACTOR; npoly++) {
            // alpha = eps*(r^w+Q(r)) + a
            if(has_sharing_offset) {
                ext_add(broadcast->alpha[npoly][j], broadcast->alpha[npoly][j], &mpc_challenge->powers_of_ch[PARAM_CHUNK_WEIGHT][PARAM_EXT_DEGREE*j]);
            }
            ext_mul(broadcast->alpha[npoly][j], broadcast->alpha[npoly][j], mpc_challenge->eps[npoly][j]);
            ext_add(broadcast->alpha[npoly][j], broadcast->alpha[npoly][j], share->unif.a[npoly][j]);
            // beta = S(r) + b
            ext_add(broadcast->beta[npoly][j], broadcast->beta[npoly][j], share->unif.b[npoly][j]);
            if(entire_computation) {
                // v += eps*F(r)*[[P(r)]]
                // [ToDo: in the pre-computation, multiply the F evals by eps]?
                ext_mul(p_evals[npoly][j], p_evals[npoly][j], mpc_challenge->eps[npoly][j]);
                ext_mul(p_evals[npoly][j], p_evals[npoly][j], mpc_challenge->f_eval[j]);
                ext_add(broadcast->v[j], broadcast->v[j], p_evals[npoly][j]);
                // v += alpha*[[b_j]]
                ext_mul(p_evals[npoly][j], plain_br->alpha[npoly][j], share->unif.b[npoly][j]);
                ext_add(broadcast->v[j], broadcast->v[j], p_evals[npoly][j]);
                // v += beta*[[a_j]]
                ext_mul(p_evals[npoly][j], plain_br->beta[npoly][j], share->unif.a[npoly][j]);
                ext_add(broadcast->v[j], broadcast->v[j], p_evals[npoly][j]);
                // v += alpha*beta
                if(has_sharing_offset) {
                    ext_mul(p_evals[npoly][j], plain_br->alpha[npoly][j], plain_br->beta[npoly][j]);
                    ext_sub(broadcast->v[j], broadcast->v[j], p_evals[npoly][j]);
                }
            }
        }
    }
    __MPC_INTERMEDIATE_VAR__("alpha", broadcast->alpha);
    __MPC_INTERMEDIATE_VAR__("beta", broadcast->beta);
    __MPC_INTERMEDIATE_VAR__("v", broadcast->v);

    free(s);
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

    // s = s_A || s_B
    uint8_t* s = (uint8_t*) aligned_alloc(32, PARAM_CODEWORD_LENGTH_CEIL32);
    memcpy(s, share->wit.s_A, PARAM_PLAINTEXT_LENGTH);
    if(has_sharing_offset) {
        memcpy(&s[PARAM_PLAINTEXT_LENGTH], inst->y, PARAM_SYNDROME_LENGTH);
    } else {
        memset(&s[PARAM_PLAINTEXT_LENGTH], 0, PARAM_SYNDROME_LENGTH);
    }

    // s_B = y - H_A x_A
    vec_neg(&s[PARAM_PLAINTEXT_LENGTH], PARAM_SYNDROME_LENGTH);
    matcols_muladd(&s[PARAM_PLAINTEXT_LENGTH], share->wit.s_A, *inst->mat_H, PARAM_PLAINTEXT_LENGTH, PARAM_SYNDROME_LENGTH);
    vec_neg(&s[PARAM_PLAINTEXT_LENGTH], PARAM_SYNDROME_LENGTH);

    memset(share->unif.a, 0, sizeof(share->unif.a));
    for(int npoly=0; npoly<PARAM_SPLITTING_FACTOR; npoly++)
        get_evals_in_all_points(share->unif.a[npoly], share->wit.q_poly[npoly], PARAM_CHUNK_WEIGHT, (const uint8_t(*)[PARAM_ALL_EVALS_BYTESIZE_CEIL16]) mpc_challenge->powers_of_ch);

    memset(share->unif.b, 0, sizeof(share->unif.b));
    for(int npoly=0; npoly<PARAM_SPLITTING_FACTOR; npoly++)
        get_evals_in_all_points(share->unif.b[npoly], &s[npoly*PARAM_CHUNK_LENGTH], PARAM_CHUNK_LENGTH, (const uint8_t(*)[PARAM_ALL_EVALS_BYTESIZE_CEIL16]) mpc_challenge->powers_of_ch);

    uint8_t p_evals[PARAM_SPLITTING_FACTOR][PARAM_NB_EVALS_PER_POLY][PARAM_EXT_DEGREE];
    memset(p_evals, 0, sizeof(p_evals));
    for(int npoly=0; npoly<PARAM_SPLITTING_FACTOR; npoly++)
        get_evals_in_all_points(p_evals[npoly], share->wit.p_poly[npoly], PARAM_CHUNK_WEIGHT, (const uint8_t(*)[PARAM_ALL_EVALS_BYTESIZE_CEIL16]) mpc_challenge->powers_of_ch);

    // Compute share of \alpha and \beta
    for(int j=0; j<PARAM_NB_EVALS_PER_POLY; j++) {
        memset(share->corr.c[j], 0, PARAM_EXT_DEGREE);
        ext_sub(share->corr.c[j], share->corr.c[j], broadcast->v[j]);
        for(int npoly=0; npoly<PARAM_SPLITTING_FACTOR; npoly++) {
            // a = -eps*(r^w+Q(r)) + alpha
            if(has_sharing_offset) {
                ext_add(share->unif.a[npoly][j], share->unif.a[npoly][j], &mpc_challenge->powers_of_ch[PARAM_CHUNK_WEIGHT][PARAM_EXT_DEGREE*j]);
            }
            ext_mul(share->unif.a[npoly][j], share->unif.a[npoly][j], mpc_challenge->eps[npoly][j]);
            vec_neg(share->unif.a[npoly][j], PARAM_EXT_DEGREE);
            ext_add(share->unif.a[npoly][j], share->unif.a[npoly][j], broadcast->alpha[npoly][j]);
            // b = -S(r) + beta
            vec_neg(share->unif.b[npoly][j], PARAM_EXT_DEGREE);
            ext_add(share->unif.b[npoly][j], share->unif.b[npoly][j], broadcast->beta[npoly][j]);
            // v += eps*F(r)*[[P(r)]]
            ext_mul(p_evals[npoly][j], p_evals[npoly][j], mpc_challenge->eps[npoly][j]);
            ext_mul(p_evals[npoly][j], p_evals[npoly][j], mpc_challenge->f_eval[j]);
            ext_add(share->corr.c[j], share->corr.c[j], p_evals[npoly][j]);
            // v += alpha*[[b_j]]
            ext_mul(p_evals[npoly][j], plain_br->alpha[npoly][j], share->unif.b[npoly][j]);
            ext_add(share->corr.c[j], share->corr.c[j], p_evals[npoly][j]);
            // v += beta*[[a_j]]
            ext_mul(p_evals[npoly][j], plain_br->beta[npoly][j], share->unif.a[npoly][j]);
            ext_add(share->corr.c[j], share->corr.c[j], p_evals[npoly][j]);
            // v += alpha*beta
            if(has_sharing_offset) {
                ext_mul(p_evals[npoly][j], plain_br->alpha[npoly][j], plain_br->beta[npoly][j]);
                ext_sub(share->corr.c[j], share->corr.c[j], p_evals[npoly][j]);
            }
        }
    }

    free(s);
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
        // Here, we relax the contraint of [FJR22]:
        //   the evaluation points can be equal.
        vec_rnd((uint8_t*) challenges[num]->eval, sizeof(challenges[num]->eval), &entropy);
        vec_rnd((uint8_t*) challenges[num]->eps, sizeof(challenges[num]->eps), &entropy);
    }

    // Precomputation
    for(size_t num_repetition=0; num_repetition<nb; num_repetition++) {
        for(uint8_t num=0; num<PARAM_NB_EVALS_PER_POLY; num++) {
            uint8_t* point = challenges[num_repetition]->eval[num];
            // powers[0] = 1
            memset(&challenges[num_repetition]->powers_of_ch[0][num*PARAM_EXT_DEGREE], 0, PARAM_EXT_DEGREE);
            challenges[num_repetition]->powers_of_ch[0][num*PARAM_EXT_DEGREE] = 1;
            // powers[1] = point
            memcpy(&challenges[num_repetition]->powers_of_ch[1][num*PARAM_EXT_DEGREE], point, PARAM_EXT_DEGREE);
            // powers[i] = evals[i-1]*point
            for(unsigned int i=2; i<PARAM_CHUNK_LENGTH+1; i++)
                ext_mul(
                    &challenges[num_repetition]->powers_of_ch[i][num*PARAM_EXT_DEGREE],
                    &challenges[num_repetition]->powers_of_ch[i-1][num*PARAM_EXT_DEGREE],
                    point
                );
        }
        get_evals_in_all_points(challenges[num_repetition]->f_eval, f_poly, PARAM_CHUNK_LENGTH+1, (const uint8_t(*)[PARAM_ALL_EVALS_BYTESIZE_CEIL16]) challenges[num_repetition]->powers_of_ch);
    }
}
