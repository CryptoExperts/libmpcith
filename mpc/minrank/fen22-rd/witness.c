#include "witness.h"
#include "field.h"
#include <stdio.h>

void expand_extended_witness(samplable_t* entropy, point_t x[PARAM_k], point_t mat_T[PARAM_n][PARAM_r], point_t mat_R[PARAM_r][PARAM_m]) {
    // Sample T
    vec_rnd((point_t*) mat_T, PARAM_n*PARAM_r, entropy);
    // Sample R
    vec_rnd((point_t*) mat_R, PARAM_r*PARAM_m, entropy);

    // Build the vector x
    vec_rnd(x, PARAM_k, entropy);
}

void uncompress_instance(instance_t* inst) {
    if(inst->mats == NULL) {
        inst->mats = malloc(PARAM_k*PARAM_nm*sizeof(point_t));
        prg_context entropy_ctx;
        samplable_t entropy = prg_to_samplable(&entropy_ctx);
        prg_init(&entropy_ctx, inst->seed_mats, NULL);
        vec_rnd(*inst->mats, PARAM_m*PARAM_n*PARAM_k, &entropy);
    }
}

void generate_instance_with_solution(instance_t** inst, solution_t** sol, samplable_t* entropy) {
    // Allocate
    *sol = (solution_t*) malloc(sizeof(solution_t));
    *inst = (instance_t*) malloc(sizeof(instance_t));
    (*inst)->mats = NULL;
    
    // Extended Witness
    expand_extended_witness(entropy, (*sol)->x, (*sol)->mat_T, (*sol)->mat_R);

    // Sample a seed for random matrices
    byte_sample(entropy, (*inst)->seed_mats, PARAM_SEED_SIZE);

    // Build random matrices
    uncompress_instance(*inst);

    // Compute mat_e as mat_e = 0 + TR
    memset((*inst)->m0, 0, PARAM_nm*sizeof(point_t));
    matmult_rows((point_t*) (*inst)->m0, (point_t*) (*sol)->mat_T, (point_t*) (*sol)->mat_R, PARAM_n, PARAM_r, PARAM_m);

    // Build m0 := mat_e - sum_i x_i mats_i
    vec_neg((point_t*) (*inst)->m0, PARAM_nm);
    matcols_muladd((point_t*) (*inst)->m0, (*sol)->x, *(*inst)->mats, PARAM_k, PARAM_nm);
    vec_neg((point_t*) (*inst)->m0, PARAM_nm);
}

void hash_update_instance(hash_context* ctx, const instance_t* inst) {
    hash_update(ctx, inst->seed_mats, PARAM_SEED_SIZE);
    hash_update(ctx, (point_t*) inst->m0, PARAM_m*PARAM_n);
}

void serialize_instance(uint8_t* buf, const instance_t* inst) {
    memcpy(buf, inst->seed_mats, PARAM_SEED_SIZE);
    memcpy(buf + PARAM_SEED_SIZE, inst->m0, PARAM_nm*sizeof(point_t));
}

instance_t* deserialize_instance(const uint8_t* buf) {
    instance_t* inst = (instance_t*) malloc(sizeof(instance_t));
    inst->mats = NULL;
    memcpy(inst->seed_mats, buf, PARAM_SEED_SIZE);
    memcpy(inst->m0, buf + PARAM_SEED_SIZE, PARAM_nm*sizeof(point_t));
    return inst;
}

void serialize_instance_solution(uint8_t* buf, const solution_t* sol) {
    memcpy(buf, sol->x, PARAM_k*sizeof(point_t));
    buf += PARAM_k*sizeof(point_t);
    memcpy(buf, sol->mat_T, PARAM_n*PARAM_r*sizeof(point_t));
    buf += PARAM_n*PARAM_r*sizeof(point_t);
    memcpy(buf, sol->mat_R, PARAM_r*PARAM_m*sizeof(point_t));
}

solution_t* deserialize_instance_solution(const uint8_t* buf) {
    solution_t* sol = (solution_t*) malloc(sizeof(solution_t));
    memcpy(sol->x, buf, PARAM_k*sizeof(point_t));
    buf += PARAM_k*sizeof(point_t);
    memcpy(sol->mat_T, buf, PARAM_n*PARAM_r*sizeof(point_t));
    buf += PARAM_n*PARAM_r*sizeof(point_t);
    memcpy(sol->mat_R, buf, PARAM_r*PARAM_m*sizeof(point_t));
    return sol;
}

void free_instance_solution(solution_t* sol) {
    free(sol);
}

void free_instance(instance_t* inst) {
    if(inst->mats)
        free(inst->mats);
    free(inst);
}

int are_same_instances(instance_t* inst1, instance_t* inst2) {
    int ret = 0;
    ret |= memcmp(inst1->seed_mats, inst2->seed_mats, PARAM_SEED_SIZE);
    ret |= memcmp(inst1->m0, inst2->m0, PARAM_nm);
    return ((ret) ? 0 : 1);
}

int is_correct_solution(instance_t* inst, solution_t* sol) {
    int ret = 0;
    uncompress_instance(inst);

    // Recompute the low-rank matrix mat_e (method 1)
    point_t mat_e[PARAM_n][PARAM_m];
    memcpy(mat_e, inst->m0, PARAM_nm*sizeof(point_t));
    matcols_muladd((point_t*) mat_e, sol->x, *inst->mats, PARAM_k, PARAM_nm);

    // Recompute the low-rank matrix mat_e (method 2)
    point_t mat_e_v2[PARAM_n][PARAM_m];
    memset(mat_e_v2, 0, PARAM_nm*sizeof(point_t));
    matmult_rows((point_t*) mat_e_v2, (point_t*) sol->mat_T, (point_t*) sol->mat_R, PARAM_n, PARAM_r, PARAM_m);

    if(memcmp(mat_e, mat_e_v2, PARAM_nm*sizeof(point_t)) != 0) {
        printf("The two computations of E does not the same result.\n");
        ret = -1;
    }

    return (ret == 0);
}
