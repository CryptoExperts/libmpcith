#include "witness.h"
#include <stdio.h>
#include <stdlib.h>
#include "aim.h"
#include "field-interface.h"

void uncompress_instance(instance_t* inst) {
    if(inst->vector_b == NULL) {
        // We assume here that
        //   inst->vector_b == NULL iff inst->matrix_A == NULL
        inst->vector_b = (void*) malloc(sizeof(GF));
        inst->matrix_A = malloc(NUM_INPUT_SBOX * sizeof(GF*));
        generate_matrix_LU(inst->iv, inst->matrix_A, *inst->vector_b);
    }
}

void generate_instance_with_solution(instance_t** inst, solution_t** sol, samplable_t* entropy) {
    // Allocate
    *sol = (solution_t*) malloc(sizeof(solution_t));
    *inst = (instance_t*) malloc(sizeof(instance_t));
    (*inst)->vector_b = NULL;
    (*inst)->matrix_A = NULL;

    // Extended Witness
    uint8_t pt_[PARAM_BLOCK_SIZE] = {0,};
    byte_sample(entropy, pt_, PARAM_BLOCK_SIZE);
    GF_from_bytes(pt_, (*sol)->pt);

    // Sample a seed
    byte_sample(entropy, (*inst)->iv, PARAM_SEED_SIZE);

    // Compute the ciphertext by AIM
    uint8_t ct_[PARAM_BLOCK_SIZE];
    aim(pt_, (*inst)->iv, ct_);
    GF_from_bytes(ct_, (*inst)->ct);
    compute_sbox_outputs(pt_, (*sol)->t);
}

void hash_update_instance(hash_context* ctx, const instance_t* inst) {
    hash_update(ctx, inst->iv, PARAM_SEED_SIZE);
    uint8_t ct_[PARAM_BLOCK_SIZE];
    GF_to_bytes(inst->ct, ct_);
    hash_update(ctx, ct_, PARAM_BLOCK_SIZE);
}

void serialize_instance(uint8_t* buf, const instance_t* inst) {
    memcpy(buf, inst->iv, PARAM_SEED_SIZE);
    GF_to_bytes(inst->ct, buf + PARAM_SEED_SIZE);
}

instance_t* deserialize_instance(const uint8_t* buf) {
    instance_t* inst = (instance_t*) malloc(sizeof(instance_t));
    inst->vector_b = NULL;
    inst->matrix_A = NULL;
    memcpy(inst->iv, buf, PARAM_SEED_SIZE);
    GF_from_bytes(buf + PARAM_SEED_SIZE, inst->ct);
    return inst;
}

void serialize_instance_solution(uint8_t* buf, const solution_t* sol) {
    GF_to_bytes(sol->pt, buf);
    for(uint32_t i=0; i<NUM_INPUT_SBOX; i++)
        GF_to_bytes(sol->t[i], buf + (i+1)*PARAM_BLOCK_SIZE);
}

solution_t* deserialize_instance_solution(const uint8_t* buf) {
    solution_t* sol = (solution_t*) malloc(sizeof(solution_t));
    GF_from_bytes(buf, sol->pt);
    for(uint32_t i=0; i<NUM_INPUT_SBOX; i++)
        GF_from_bytes(buf + (i+1)*PARAM_BLOCK_SIZE, sol->t[i]);
    return sol;
}

void free_instance_solution(solution_t* sol) {
    free(sol);
}

void free_instance(instance_t* inst) {
    if(inst->vector_b) {
        for (size_t i = 0; i < NUM_INPUT_SBOX; i++)
            free(inst->matrix_A[i]);
        free(inst->matrix_A);

        free(inst->vector_b);
        inst->vector_b = NULL;
    }
    free(inst);
}

int are_same_instances(instance_t* inst1, instance_t* inst2) {
    int ret = 0;
    ret |= memcmp(inst1->iv, inst2->iv, PARAM_SEED_SIZE);
    uint8_t ct1_[PARAM_BLOCK_SIZE];
    uint8_t ct2_[PARAM_BLOCK_SIZE];
    GF_to_bytes(inst1->ct, ct1_);
    GF_to_bytes(inst2->ct, ct2_);
    ret |= memcmp(ct1_, ct2_, PARAM_BLOCK_SIZE);
    return ((ret) ? 0 : 1);
}

int is_correct_solution(instance_t* inst, solution_t* sol) {
    int ret = 0;
    uncompress_instance(inst);

    GF x[NUM_INPUT_SBOX + 1];
    GF z[NUM_INPUT_SBOX + 1];
    aim_mpc_(
        sol->pt, (const GF*) sol->t,
        (GF const* const*) inst->matrix_A, *inst->vector_b, inst->ct,
        z, x, 1
    );
    GF res;
    for(uint32_t i=0; i<NUM_INPUT_SBOX + 1; i++) {
        GF_mul(x[i], sol->pt, res);
        GF_add(res, z[i], res);
        ret |= (! GF_is_zero(res));
    }

    return (ret == 0);
}
