#include "witness.h"
#include "gf2.h"
#include "gf2to31.h"
#include <stdio.h>

void expand_extended_witness(samplable_t* entropy, uint8_t x[PARAM_n][PARAM_m_div8], uint8_t mat_T[PARAM_n][PARAM_r_div8], uint8_t mat_R[PARAM_r][PARAM_m_div8]) {
    // Sample T
    gf2_random_matrix_by_rows((uint8_t*) mat_T, PARAM_n, PARAM_r, entropy);
    // Sample R
    gf2_random_matrix_by_rows((uint8_t*) mat_R, PARAM_r, PARAM_m_div8<<3, entropy);
    for(int i=0;i<PARAM_r;i++)
        ((uint32_t*) mat_R)[i] &= 0x7FFFFFFF;

    // Compute X as X = 0 + TR
    memset(x, 0, PARAM_n*PARAM_m_div8);
    gf2_matmult_rows((uint8_t*) x, (uint8_t*) mat_T, (uint8_t*) mat_R, PARAM_n, PARAM_r, PARAM_m);
}

void uncompress_instance(instance_t* inst) {
    if(inst->mat_H == NULL) {
        inst->mat_H = malloc(PARAM_PCMATRIX_BYTESIZE);
        prg_context entropy_ctx;
        samplable_t entropy = prg_to_samplable(&entropy_ctx);
        prg_init(&entropy_ctx, inst->seed_H, NULL);
        gf2to31_random_elements(*inst->mat_H, PARAM_PCMATRIX_BYTESIZE>>2, &entropy);
    }
}

void generate_instance_with_solution(instance_t** inst, solution_t** sol, samplable_t* entropy) {
    // Allocate
    *sol = (solution_t*) malloc(sizeof(solution_t));
    *inst = (instance_t*) malloc(sizeof(instance_t));
    (*inst)->mat_H = NULL;

    // Build the polynomial S, Q and P
    uint8_t x[PARAM_n][PARAM_m_div8];
    expand_extended_witness(entropy, x, (*sol)->mat_T, (*sol)->mat_R);

    // Split x as (x_A | x_B)
    uint8_t* x_b = (uint8_t*) &x[PARAM_PLAINTEXT_LENGTH];
    memcpy((uint8_t*) (*sol)->x_A, x, PARAM_k*PARAM_m_div8);

    // Sample a seed for matrix H
    byte_sample(entropy, (*inst)->seed_H, PARAM_SEED_SIZE);

    // Build H
    uncompress_instance(*inst);

    // Build y
    memcpy((uint8_t*) (*inst)->y, x_b, PARAM_SYNDROME_LENGTH*PARAM_m_div8);
    gf2to31_matcols_muladd((uint8_t*) (*inst)->y, (uint8_t*) (*sol)->x_A, *(*inst)->mat_H, PARAM_PLAINTEXT_LENGTH, PARAM_SYNDROME_LENGTH);
}

void hash_update_instance(hash_context* ctx, const instance_t* inst) {
    hash_update(ctx, inst->seed_H, PARAM_SEED_SIZE);
    hash_update(ctx, (uint8_t*) inst->y, PARAM_SYNDROME_BYTESIZE);
}

void serialize_instance(uint8_t* buf, const instance_t* inst) {
    memcpy(buf, inst->seed_H, PARAM_SEED_SIZE);
    memcpy(buf + PARAM_SEED_SIZE, inst->y, PARAM_SYNDROME_BYTESIZE);
}

instance_t* deserialize_instance(const uint8_t* buf) {
    instance_t* inst = (instance_t*) malloc(sizeof(instance_t));
    inst->mat_H = NULL;
    memcpy(inst->seed_H, buf, PARAM_SEED_SIZE);
    memcpy(inst->y, buf + PARAM_SEED_SIZE, PARAM_SYNDROME_BYTESIZE);
    return inst;
}

void serialize_instance_solution(uint8_t* buf, const solution_t* sol) {
    memcpy(buf, sol->x_A, PARAM_k*PARAM_m_div8);
    buf += PARAM_k*PARAM_m_div8;
    memcpy(buf, sol->mat_T, PARAM_n*PARAM_r_div8);
    buf += PARAM_n*PARAM_r_div8;
    memcpy(buf, sol->mat_R, PARAM_r*PARAM_m_div8);
}

solution_t* deserialize_instance_solution(const uint8_t* buf) {
    solution_t* sol = (solution_t*) malloc(sizeof(solution_t));
    memcpy(sol->x_A, buf, PARAM_k*PARAM_m_div8);
    buf += PARAM_k*PARAM_m_div8;
    memcpy(sol->mat_T, buf, PARAM_n*PARAM_r_div8);
    buf += PARAM_n*PARAM_r_div8;
    memcpy(sol->mat_R, buf, PARAM_r*PARAM_m_div8);
    return sol;
}

void free_instance_solution(solution_t* sol) {
    free(sol);
}

void free_instance(instance_t* inst) {
    if(inst->mat_H)
        free(inst->mat_H);
    free(inst);
}

int are_same_instances(instance_t* inst1, instance_t* inst2) {
    int ret = 0;
    ret |= memcmp(inst1->seed_H, inst2->seed_H, PARAM_SEED_SIZE);
    ret |= memcmp(inst1->y, inst2->y, PARAM_SYNDROME_BYTESIZE);
    return ((ret) ? 0 : 1);
}

int is_correct_solution(instance_t* inst, solution_t* sol) {
    int ret = 0;
    uncompress_instance(inst);

    /// We recompute the vector x as (x_A||x_B),
    ///      where s_B := y - H' s_A

    // x = x_A || x_B
    uint8_t* x = (uint8_t*) aligned_alloc(32, PARAM_CODEWORD_BYTESIZE_CEIL32);
    if(x == NULL) {
        printf("Error: Aligned alloc returns NULL.\n");
        return 0;
    }
    memcpy(x, (uint8_t*) sol->x_A, PARAM_PLAINTEXT_BYTESIZE);
    memcpy(&x[PARAM_PLAINTEXT_BYTESIZE], (uint8_t*) inst->y, PARAM_SYNDROME_BYTESIZE);

    // x_B = y - H_A x_A
    //neg_tab_points(&s[PARAM_PLAINTEXT_LENGTH], PARAM_SYNDROME_LENGTH);
    gf2to31_matcols_muladd(&x[PARAM_PLAINTEXT_BYTESIZE], (uint8_t*) sol->x_A, *inst->mat_H, PARAM_PLAINTEXT_LENGTH, PARAM_SYNDROME_LENGTH);
    //neg_tab_points(&s[PARAM_PLAINTEXT_LENGTH], PARAM_SYNDROME_LENGTH);



    uint8_t x2[PARAM_n][PARAM_m_div8];
    memset(x2, 0, sizeof(x2));
    gf2_matmult_rows((uint8_t*) x2, (uint8_t*) sol->mat_T, (uint8_t*) sol->mat_R, PARAM_n, PARAM_r, PARAM_m);
    if(memcmp(x, x2, PARAM_CODEWORD_BYTESIZE) != 0) {
        printf("The two computations of x does not the same result.\n");
        ret = -1;
    }

    free(x);
    return (ret == 0);
}
