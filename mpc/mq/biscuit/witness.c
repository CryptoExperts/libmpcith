#include "witness.h"
#include "field.h"
#include <stdio.h>
#include <stdlib.h>

void uncompress_instance(instance_t* inst) {
    if(inst->f == NULL) {
        inst->f = (void*) malloc(PARAM_m*PARAM_n*3);
        prg_context entropy_ctx;
        samplable_t entropy = prg_to_samplable(&entropy_ctx);
        prg_init(&entropy_ctx, inst->seed, NULL);
        vec_rnd((uint8_t*) (*inst->f), PARAM_m*PARAM_n*3, &entropy);
    }
}

void generate_instance_with_solution(instance_t** inst, solution_t** sol, samplable_t* entropy) {
    // Allocate
    *sol = (solution_t*) malloc(sizeof(solution_t));
    *inst = (instance_t*) malloc(sizeof(instance_t));
    (*inst)->f = NULL;

    // Extended Witness
    vec_rnd((*sol)->s, PARAM_n, entropy);

    // Sample a seed
    byte_sample(entropy, (*inst)->seed, PARAM_SEED_SIZE);

    // Build random matrices
    uncompress_instance(*inst);

    // Build t
    uint8_t x, y, z;
    for(unsigned int k=0; k<PARAM_m; k++) {
        x = 0;
        y = 0;
        z = 0;
        for(unsigned int i=0; i<PARAM_n; i++) {
            x = sca_add(x, sca_mul((*(*inst)->f)[k][3*i+1], (*sol)->s[i]));
            y = sca_add(y, sca_mul((*(*inst)->f)[k][3*i+2], (*sol)->s[i]));
            z = sca_add(z, sca_mul((*(*inst)->f)[k][3*i+0], (*sol)->s[i]));
        }
        (*inst)->t[k] = sca_add(z, sca_mul(x, y));
    }
}

void hash_update_instance(hash_context* ctx, const instance_t* inst) {
    hash_update(ctx, inst->seed, PARAM_SEED_SIZE);
    hash_update(ctx, inst->t, PARAM_m);
}

void serialize_instance(uint8_t* buf, const instance_t* inst) {
    memcpy(buf, inst->seed, PARAM_SEED_SIZE);
    memcpy(buf + PARAM_SEED_SIZE, inst->t, PARAM_m);
}

instance_t* deserialize_instance(const uint8_t* buf) {
    instance_t* inst = (instance_t*) malloc(sizeof(instance_t));
    inst->f = NULL;
    memcpy(inst->seed, buf, PARAM_SEED_SIZE);
    memcpy(inst->t, buf + PARAM_SEED_SIZE, PARAM_m);
    return inst;
}

void serialize_instance_solution(uint8_t* buf, const solution_t* sol) {
    memcpy(buf, sol->s, PARAM_n);
}

solution_t* deserialize_instance_solution(const uint8_t* buf) {
    solution_t* sol = (solution_t*) malloc(sizeof(solution_t));
    memcpy(sol->s, buf, PARAM_n);
    return sol;
}

void free_instance_solution(solution_t* sol) {
    free(sol);
}

void free_instance(instance_t* inst) {
    if(inst->f) {
        free(inst->f);
        inst->f = NULL;
    }
    free(inst);
}

int are_same_instances(instance_t* inst1, instance_t* inst2) {
    int ret = 0;
    ret |= memcmp(inst1->seed, inst2->seed, PARAM_SEED_SIZE);
    ret |= memcmp(inst1->t, inst2->t, PARAM_m);
    return ((ret) ? 0 : 1);
}

int is_correct_solution(instance_t* inst, solution_t* sol) {
    int ret = 0;
    uncompress_instance(inst);

    uint8_t t_candidate[PARAM_m];
    uint8_t x, y, z;
    for(unsigned int k=0; k<PARAM_m; k++) {
        x = 0;
        y = 0;
        z = 0;
        for(unsigned int i=0; i<PARAM_n; i++) {
            x = sca_add(x, sca_mul((*inst->f)[k][3*i+1], sol->s[i]));
            y = sca_add(y, sca_mul((*inst->f)[k][3*i+2], sol->s[i]));
            z = sca_add(z, sca_mul((*inst->f)[k][3*i+0], sol->s[i]));
        }
        t_candidate[k] = sca_add(z, sca_mul(x, y));
    }
    ret = memcmp(inst->t, t_candidate, PARAM_m);

    return (ret == 0);
}
