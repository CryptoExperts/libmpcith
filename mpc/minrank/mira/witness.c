#include "witness.h"
#include "field.h"
#include <stdio.h>

void expand_extended_witness(samplable_t* entropy, gfq x[MIRA_PARAM_K], gfqm beta[MIRA_PARAM_R], gfq M0[MIRA_PARAM_N*MIRA_PARAM_M], gfq Mi[MIRA_PARAM_K][MIRA_PARAM_M*MIRA_PARAM_N]) {
    //Sample PARAM_R independant elements of GF(q^m)
    gfq_mat support;
    gfq_mat_init(&support, MIRA_PARAM_M, MIRA_PARAM_R);
    //Identity matrix "trick"
    for(int i=0 ; i<MIRA_PARAM_R ; i++) {
        for(int j=0 ; j<MIRA_PARAM_R ; j++) {
        if(i==j) support[i*MIRA_PARAM_R + j] = 1;
        else support[i*MIRA_PARAM_R + j] = 0;
        }
    }

    gfq_mat_set_random(entropy, support + MIRA_PARAM_R * MIRA_PARAM_R, MIRA_PARAM_M - MIRA_PARAM_R, MIRA_PARAM_R);

    //Sample E
    gfq_mat coordinates, E;
    gfq_mat_init(&coordinates, MIRA_PARAM_R, MIRA_PARAM_N);
    gfq_mat_init(&E, MIRA_PARAM_M, MIRA_PARAM_N);

    gfq_mat_set_random(entropy, coordinates, MIRA_PARAM_R, MIRA_PARAM_N);
    gfq_mat_mul(E, support, coordinates, MIRA_PARAM_M, MIRA_PARAM_R, MIRA_PARAM_N);

    //Compute F
    gfq_mat F, tmp_mat;
    gfq_mat B, f;
    gfq_mat_init(&B, 1, MIRA_PARAM_K);
    gfq_mat_init(&f, 1, MIRA_PARAM_K);
    gfq_mat_init(&F, MIRA_PARAM_M, MIRA_PARAM_N);
    gfq_mat_init(&tmp_mat, MIRA_PARAM_M, MIRA_PARAM_N);

    gfq_mat_set_random(entropy, B, 1, MIRA_PARAM_K);

    gfq_mat_set(F, E, MIRA_PARAM_M, MIRA_PARAM_N);
    for(int i=0 ; i<MIRA_PARAM_K ; i++) {
        gfq_mat_scalar_mul(tmp_mat, Mi[i], B[i], MIRA_PARAM_M, MIRA_PARAM_N);
        gfq_mat_add(F, F, tmp_mat, MIRA_PARAM_M, MIRA_PARAM_N);
    }

    gfq_mat_set(f, F, 1, MIRA_PARAM_K);

    //Compute M0
    gfq_mat_set(M0, F, MIRA_PARAM_M, MIRA_PARAM_N);
    for(int i=0 ; i<MIRA_PARAM_K ; i++) {
        gfq_mat_scalar_mul(tmp_mat, Mi[i], f[i], MIRA_PARAM_M, MIRA_PARAM_N);
        gfq_mat_add(M0, M0, tmp_mat, MIRA_PARAM_M, MIRA_PARAM_N);
    }

    // Compute x
    gfq_mat_add(x, f, B, 1, MIRA_PARAM_K);

    //Compute A
    gfqm support_vec[MIRA_PARAM_R];
    for(int i=0 ; i<MIRA_PARAM_R ; i++) {
        for(int j=0 ; j<MIRA_PARAM_M ; j++) {
            support_vec[i][j] = support[j*MIRA_PARAM_R + i];
        }
    }

    gfqm A[MIRA_PARAM_R+1];
    gfqm_qpoly_annihilator(A, support_vec, MIRA_PARAM_R);
    memcpy(beta, A, MIRA_PARAM_R*MIRA_PARAM_M);

    gfq_mat_clear(support);
    gfq_mat_clear(coordinates);
    gfq_mat_clear(E);
    gfq_mat_clear(F);
    gfq_mat_clear(f);
    gfq_mat_clear(B);
    gfq_mat_clear(tmp_mat);
}

void uncompress_instance(instance_t* inst) {
    if(inst->Mi == NULL) {
        inst->Mi = (gfq(*)[MIRA_PARAM_K][MIRA_PARAM_M*MIRA_PARAM_N])malloc(MIRA_PARAM_K*MIRA_PARAM_M*MIRA_PARAM_N*sizeof(gfq));
        prg_context entropy_ctx;
        samplable_t entropy = prg_to_samplable(&entropy_ctx);
        prg_init(&entropy_ctx, inst->seed_mats, NULL);
        for(int i=0 ; i<MIRA_PARAM_K ; i++) {
            for(int j=0 ; j<MIRA_PARAM_K ; j++) {
                if(j==i) (*inst->Mi)[i][j] = 1;
                else (*inst->Mi)[i][j] = 0;
            }
            gfq_mat_set_random(&entropy, &((*inst->Mi)[i][MIRA_PARAM_K]), 1, MIRA_PARAM_N * MIRA_PARAM_M - MIRA_PARAM_K);
        }
    }
}

void generate_instance_with_solution(instance_t** inst, solution_t** sol, samplable_t* entropy) {
    // Allocate
    *sol = (solution_t*) malloc(sizeof(solution_t));
    *inst = (instance_t*) malloc(sizeof(instance_t));
    (*inst)->Mi = NULL;

    // Sample a seed for random matrices
    byte_sample(entropy, (*inst)->seed_mats, PARAM_SEED_SIZE);

    // Build random matrices
    uncompress_instance(*inst);

    // Extended Witness
    expand_extended_witness(entropy, (*sol)->x, (*sol)->beta, (*inst)->M0, *(*inst)->Mi);
}

void hash_update_instance(hash_context* ctx, const instance_t* inst) {
    hash_update(ctx, inst->seed_mats, PARAM_SEED_SIZE);
    uint8_t buf[MIRA_VEC_BYTES(MIRA_PARAM_N*MIRA_PARAM_M)];
    gf16_mat_to_string(buf, (const gfq_mat) inst->M0, MIRA_PARAM_M, MIRA_PARAM_N);
    hash_update(ctx, buf, MIRA_VEC_BYTES(MIRA_PARAM_N*MIRA_PARAM_M));
}

void serialize_instance(uint8_t* buf, const instance_t* inst) {
    memcpy(buf, inst->seed_mats, PARAM_SEED_SIZE);
    gf16_mat_to_string(buf + PARAM_SEED_SIZE, (const gf16_mat) inst->M0, MIRA_PARAM_M, MIRA_PARAM_N);
}

instance_t* deserialize_instance(const uint8_t* buf) {
    instance_t* inst = (instance_t*) malloc(sizeof(instance_t));
    inst->Mi = NULL;
    memcpy(inst->seed_mats, buf, PARAM_SEED_SIZE);
    gf16_mat_from_string(inst->M0, MIRA_PARAM_M, MIRA_PARAM_N, buf + PARAM_SEED_SIZE);
    return inst;
}

void serialize_instance_solution(uint8_t* buf, const solution_t* sol) {
    gf16_mat_to_string(buf, (const gfq_mat) sol->x, 1, MIRA_PARAM_K);
    buf += MIRA_VEC_BYTES(MIRA_PARAM_K);
    gfqm_vec_to_string(buf, (const gfqm_vec) sol->beta, MIRA_PARAM_R);
}

solution_t* deserialize_instance_solution(const uint8_t* buf) {
    solution_t* sol = (solution_t*) malloc(sizeof(solution_t));
    gf16_mat_from_string(sol->x, 1, MIRA_PARAM_K, buf);
    buf += MIRA_VEC_BYTES(MIRA_PARAM_K);
    gfqm_vec_from_string(sol->beta, MIRA_PARAM_R, buf);
    return sol;
}

void free_instance_solution(solution_t* sol) {
    free(sol);
}

void free_instance(instance_t* inst) {
    if(inst->Mi)
        free(inst->Mi);
    free(inst);
}

int are_same_instances(instance_t* inst1, instance_t* inst2) {
    int ret = 0;
    ret |= memcmp(inst1->seed_mats, inst2->seed_mats, PARAM_SEED_SIZE);
    ret |= (gfq_mat_is_equal(inst1->M0, inst2->M0, MIRA_PARAM_M, MIRA_PARAM_N) != 1);
    return ((ret) ? 0 : 1);
}

int is_correct_solution(instance_t* inst, solution_t* sol) {
    int ret = 0;
    uncompress_instance(inst);

    // Compute [E]
    gfq_mat E, tmp;
    gfq_mat_init(&E, MIRA_PARAM_M, MIRA_PARAM_N);
    gfq_mat_init(&tmp, MIRA_PARAM_M, MIRA_PARAM_N);
    gfq_mat_set(E, inst->M0, MIRA_PARAM_M, MIRA_PARAM_N);

    for(int j=0 ; j<MIRA_PARAM_K ; j++) {
        gfq_mat_scalar_mul(tmp, (*inst->Mi)[j], sol->x[j], MIRA_PARAM_M, MIRA_PARAM_N);
        gfq_mat_add(E, E, tmp, MIRA_PARAM_M, MIRA_PARAM_N);
    }

    //"Transpose" into elements of Fqm
    gfqm_vec E_vec;
    gfqm_vec_init(&E_vec, MIRA_PARAM_N);
    for(int j=0 ; j<MIRA_PARAM_N ; j++)
        for(int k=0 ; k<MIRA_PARAM_M ; k++)
            E_vec[j][k] = E[k*MIRA_PARAM_N + j];

    gfqm res;
    gfqm tmp2;
    for(int i=0; i<MIRA_PARAM_N; i++) {
        // Compute x^{q^j}, for all j>0
        gfqm_set_zero(res);
        for(int j=0; j<MIRA_PARAM_R; j++) {
            // res += beta_j * mat_e[i]^{q^j}
            gfqm_mul(tmp2, sol->beta[j], E_vec[i]);
            gfqm_add(res, res, tmp2);
            gfqm_powq(E_vec[i], E_vec[i]);
        }
        // res += mat_e[i]^{q^r}
        gfqm_add(res, res, E_vec[i]);
        // Check if res is zero
        if(!gfqm_is_zero(res)) {
            printf("Error: Coordinate %d is not a root of the q-polynomial.\n", i);
            ret = -1;
        }
    }

    gfqm_vec_clear(E_vec);
    gfq_mat_clear(E);
    gfq_mat_clear(tmp);

    return (ret == 0);
}
