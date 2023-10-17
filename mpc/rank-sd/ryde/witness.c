#include "witness.h"
#include "field.h"
#include <stdlib.h>
#include <stdio.h>

void uncompress_instance(instance_t* inst) {
    if(inst->mat_H == NULL) {
        rbc_mat_init(&inst->mat_H, RYDE_PARAM_N-RYDE_PARAM_K, RYDE_PARAM_K);
        prg_context entropy_ctx;
        samplable_t entropy = prg_to_samplable(&entropy_ctx);
        prg_init(&entropy_ctx, inst->seed_H, NULL);
        rbc_mat_set_random(&entropy, inst->mat_H, RYDE_PARAM_N-RYDE_PARAM_K, RYDE_PARAM_K);
    }
}

void generate_instance_with_solution(instance_t** inst, solution_t** sol, samplable_t* entropy) {
    // Allocate
    *sol = (solution_t*) malloc(sizeof(solution_t));
    *inst = (instance_t*) malloc(sizeof(instance_t));
    (*inst)->mat_H = NULL;

    rbc_elt support[RYDE_PARAM_W];
    rbc_elt x[RYDE_PARAM_N];
    rbc_elt x1[RYDE_PARAM_N - RYDE_PARAM_K];
    rbc_vspace_set_random_full_rank_with_one(entropy, support, RYDE_PARAM_W);
    rbc_vec_set_random_from_support(entropy, x, RYDE_PARAM_N, support, RYDE_PARAM_W, 1);
    rbc_qpoly polyL;
    rbc_qpoly_init(&polyL, RYDE_PARAM_W);
    rbc_qpoly_annihilator(polyL, support, RYDE_PARAM_W);
    memcpy((*sol)->beta, polyL->values+1, sizeof((*sol)->beta));
    rbc_qpoly_clear(polyL);

    // Split x as (x1 | x2)
    for(uint32_t i = 0; i < RYDE_PARAM_N - RYDE_PARAM_K; i++)
        rbc_elt_set(x1[i], x[i]);
    for(uint32_t i = 0; i < RYDE_PARAM_K; i++)
        rbc_elt_set((*sol)->x2[i], x[i + RYDE_PARAM_N - RYDE_PARAM_K]);

    // Sample a seed for matrix H
    byte_sample(entropy, (*inst)->seed_H, PARAM_SEED_SIZE);

    // Build H
    uncompress_instance(*inst);

    // Build y
    rbc_mat_vec_mul((*inst)->y, (*inst)->mat_H, (*sol)->x2, RYDE_PARAM_N - RYDE_PARAM_K, RYDE_PARAM_K);
    rbc_vec_add((*inst)->y, (*inst)->y, x1, RYDE_PARAM_N - RYDE_PARAM_K);
}

void hash_update_instance(hash_context* ctx, const instance_t* inst) {
    hash_update(ctx, inst->seed_H, PARAM_SEED_SIZE);
    uint8_t buf[RYDE_VEC_BYTES(RYDE_PARAM_N-RYDE_PARAM_K)];
    rbc_vec_to_string(buf, (const rbc_vec) inst->y, RYDE_PARAM_N-RYDE_PARAM_K);
    hash_update(ctx, buf, RYDE_VEC_BYTES(RYDE_PARAM_N-RYDE_PARAM_K));
}

void serialize_instance(uint8_t* buf, const instance_t* inst) {
    memcpy(buf, inst->seed_H, PARAM_SEED_SIZE);
    rbc_vec_to_string(buf + PARAM_SEED_SIZE, (const rbc_vec) inst->y, RYDE_PARAM_N-RYDE_PARAM_K);
}

instance_t* deserialize_instance(const uint8_t* buf) {
    instance_t* inst = (instance_t*) malloc(sizeof(instance_t));
    inst->mat_H = NULL;
    memcpy(inst->seed_H, buf, PARAM_SEED_SIZE);
    rbc_vec_from_string(inst->y, RYDE_PARAM_N-RYDE_PARAM_K, buf + PARAM_SEED_SIZE);
    return inst;
}

void serialize_instance_solution(uint8_t* buf, const solution_t* sol) {
    rbc_vec_to_string(buf, (const rbc_vec) sol->x2, RYDE_PARAM_K);
    buf += RYDE_VEC_BYTES(RYDE_PARAM_K);
    rbc_vec_to_string(buf, (const rbc_vec) sol->beta, RYDE_PARAM_W);
}

solution_t* deserialize_instance_solution(const uint8_t* buf) {
    solution_t* sol = (solution_t*) malloc(sizeof(solution_t));
    rbc_vec_from_string(sol->x2, RYDE_PARAM_K, buf);
    buf += RYDE_VEC_BYTES(RYDE_PARAM_K);
    rbc_vec_from_string(sol->beta, RYDE_PARAM_W, buf);
    return sol;
}

void free_instance_solution(solution_t* sol) {
    free(sol);
}

void free_instance(instance_t* inst) {
    if(inst->mat_H)
        rbc_mat_clear(inst->mat_H);
    free(inst);
}

int are_same_instances(instance_t* inst1, instance_t* inst2) {
    int ret = 0;
    ret |= memcmp(inst1->seed_H, inst2->seed_H, PARAM_SEED_SIZE);
    for(uint32_t i=0; i<RYDE_PARAM_N-RYDE_PARAM_K; i++)
        ret |= !rbc_elt_is_equal_to(inst1->y[i], inst2->y[i]);
    return ((ret) ? 0 : 1);
}

int is_correct_solution(instance_t* inst, solution_t* sol) {
    int ret = 0;
    uncompress_instance(inst);

    /// We recompute the vector x as (x_A||x_B),
    ///      where s_B := y - H' s_A
    rbc_elt x1[RYDE_PARAM_N - RYDE_PARAM_K];
    rbc_mat_vec_mul(x1, inst->mat_H, sol->x2, RYDE_PARAM_N-RYDE_PARAM_K, RYDE_PARAM_K);
    rbc_vec_add(x1, x1, inst->y, RYDE_PARAM_N-RYDE_PARAM_K);

    // x = x_A || x_B
    rbc_elt x[RYDE_PARAM_N];
    for(uint32_t i = 0; i < RYDE_PARAM_N - RYDE_PARAM_K; i++)
        rbc_elt_set(x[i], x1[i]);
    for(uint32_t i = 0; i < RYDE_PARAM_K; i++)
        rbc_elt_set(x[i + RYDE_PARAM_N - RYDE_PARAM_K], sol->x2[i]);
    
    rbc_elt xi_qj[RYDE_PARAM_W];
    for(int i=0; i<RYDE_PARAM_N; i++) {
        rbc_elt_sqr(xi_qj[0], x[i]);
        for(uint32_t j=1; j<RYDE_PARAM_W; j++)
            rbc_elt_sqr(xi_qj[j], xi_qj[j-1]);

        rbc_elt diff[RYDE_PARAM_W];
        for(uint32_t j=0; j<RYDE_PARAM_W; j++)
            rbc_elt_add(diff[j], xi_qj[j], x[i]);

        rbc_elt res;
        rbc_vec_inner_product(res, sol->beta, diff, RYDE_PARAM_W-1);
        rbc_elt_add(res, res, diff[RYDE_PARAM_W-1]);

        if(!rbc_elt_is_zero(res)) {
            printf("Error: Coordinate %d is not a root of the q-polynomial.\n", i);
            ret = -1;
        }
    }

    return (ret == 0);
}
