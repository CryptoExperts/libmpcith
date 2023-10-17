#include "witness.h"
#include <stdlib.h>
#include <stdio.h>

void uncompress_instance(instance_t* inst) {
    if(inst->Mi == NULL) {
        inst->Mi = (ff_t(*)[PAR_K][matrix_bytes_size(PAR_M, PAR_N)])malloc(PAR_K*matrix_bytes_size(PAR_M, PAR_N));
        prg_context entropy_ctx;
        samplable_t entropy = prg_to_samplable(&entropy_ctx);
        prg_init(&entropy_ctx, inst->seed_mats, NULL);
        for(uint32_t i=0; i<PAR_K; i++)
            matrix_init_random((*inst->Mi)[i], PAR_M, PAR_N, &entropy);
    }
}

void generate_instance_with_solution(instance_t** inst, solution_t** sol, samplable_t* entropy) {
    // Allocate
    *sol = (solution_t*) malloc(sizeof(solution_t));
    *inst = (instance_t*) malloc(sizeof(instance_t));
    (*inst)->Mi = NULL;

    ff_t E_R[matrix_bytes_size(PAR_M, PAR_R)];

    /* Initialize matrices 'a', 'K', 'E_R'. */
    matrix_init_random((*sol)->alpha, PAR_K, 1, entropy);
    matrix_init_random((*sol)->K, PAR_R, PAR_N - PAR_R, entropy);
    matrix_init_random(E_R, PAR_M, PAR_R, entropy);

    /* Compute the matrix 'E'. */
    ff_t T[matrix_bytes_size(PAR_M, PAR_N - PAR_R)];
    ff_t E[matrix_bytes_size(PAR_M, PAR_N)];
    matrix_product(T, E_R, (*sol)->K, PAR_M, PAR_R, PAR_N - PAR_R);
    matrix_horizontal_concatenation(E, T, E_R, PAR_M, PAR_N - PAR_R, PAR_R);

    /* Initialize matrices */
    byte_sample(entropy, (*inst)->seed_mats, PARAM_SEED_SIZE);
    uncompress_instance(*inst);

    /* Compute matrix 'M[0]'. */
    matrix_copy((*inst)->M0, E, PAR_M, PAR_N);
    for (uint32_t i = 0; i < PAR_K; i++)
    {
        ff_t scalar;
        
        scalar = matrix_get_entry((*sol)->alpha, PAR_K, i, 0);
        matrix_subtract_multiple((*inst)->M0, scalar, (*(*inst)->Mi)[i], PAR_M, PAR_N);
    }
}

void hash_update_instance(hash_context* ctx, const instance_t* inst) {
    hash_update(ctx, inst->seed_mats, PARAM_SEED_SIZE);
    uint8_t buf[matrix_bytes_packed_size(PAR_M,PAR_N)];
    uint8_t* buf_ptr = buf;
    matrix_pack(&buf_ptr, NULL, inst->M0, PAR_M, PAR_N);
    hash_update(ctx, buf, matrix_bytes_packed_size(PAR_M,PAR_N));
}

void serialize_instance(uint8_t* buf, const instance_t* inst) {
    memcpy(buf, inst->seed_mats, PARAM_SEED_SIZE);
    buf += PARAM_SEED_SIZE;
    matrix_pack(&buf, NULL, inst->M0, PAR_M, PAR_N);
}

instance_t* deserialize_instance(const uint8_t* buf) {
    instance_t* inst = (instance_t*) malloc(sizeof(instance_t));
    inst->Mi = NULL;
    memcpy(inst->seed_mats, buf, PARAM_SEED_SIZE);
    buf += PARAM_SEED_SIZE;
    matrix_unpack(inst->M0, (uint8_t**) &buf, NULL, PAR_M, PAR_N);
    return inst;
}

void serialize_instance_solution(uint8_t* buf, const solution_t* sol) {
    int bit_offset = 0;
    matrix_pack(&buf, &bit_offset, sol->alpha, PAR_K, 1);
    matrix_pack(&buf, &bit_offset, sol->K, PAR_R, PAR_N - PAR_R);
}

solution_t* deserialize_instance_solution(const uint8_t* buf) {
    solution_t* sol = (solution_t*) malloc(sizeof(solution_t));
    int bit_offset = 0;
    matrix_unpack(sol->alpha, (uint8_t**) &buf, &bit_offset, PAR_K, 1);
    matrix_unpack(sol->K, (uint8_t**) &buf, &bit_offset, PAR_R, PAR_N - PAR_R);
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
    ret |= memcmp(inst1->M0, inst2->M0, matrix_bytes_size(PAR_M, PAR_N));
    return ((ret) ? 0 : 1);
}

int is_correct_solution(instance_t* inst, solution_t* sol) {
    int ret = 0;
    uncompress_instance(inst);

    int p;

    /* Compute the matrix 'E' from linear combination */
    ff_t E[matrix_bytes_size(PAR_M, PAR_N)];
    matrix_copy(E, inst->M0, PAR_M, PAR_N);
    for (p = 0; p < PAR_K; p++)
    {
        ff_t scalar;

        scalar = matrix_get_entry(sol->alpha, PAR_K, p, 0);
        matrix_add_multiple(E, scalar, (*inst->Mi)[p], PAR_M, PAR_N);
    }

    /* Compute the matrix 'E' from multiplication */
    ff_t MaL[matrix_bytes_size(PAR_M, PAR_N - PAR_R)];
    ff_t MaR[matrix_bytes_size(PAR_M, PAR_R)];
    matrix_horizontal_split(MaL, MaR, E, PAR_M, PAR_N - PAR_R, PAR_R);
    ff_t MaL_candidate[matrix_bytes_size(PAR_M, PAR_N - PAR_R)];
    matrix_product(MaL_candidate, MaR, sol->K, PAR_M, PAR_R, PAR_N - PAR_R);

    ret = memcmp(MaL, MaL_candidate, matrix_bytes_size(PAR_M, PAR_N - PAR_R));
    return (ret == 0);
}
