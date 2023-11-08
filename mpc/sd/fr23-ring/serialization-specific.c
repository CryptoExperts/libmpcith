#include "witness.h"
#include "mpc.h"

void hash_update_instance(hash_context* ctx, const instance_t* inst) {
    hash_update(ctx, inst->seed_H, PARAM_SEED_SIZE);
    hash_update(ctx, inst->y, PARAM_SYNDROME_LENGTH);
}

void serialize_template_instance(uint8_t* buf, const instance_t* inst) {
    memcpy(buf, inst->seed_H, PARAM_SEED_SIZE);
}

instance_t* deserialize_template_instance(const uint8_t* buf) {
    instance_t* inst = (instance_t*) malloc(sizeof(instance_t));
    inst->mat_H = NULL;
    memcpy(inst->seed_H, buf, PARAM_SEED_SIZE);
    return inst;
}

void serialize_instance(uint8_t* buf, const instance_t* inst) {
    memcpy(buf, inst->seed_H, PARAM_SEED_SIZE);
    memcpy(buf + PARAM_SEED_SIZE, inst->y, PARAM_SYNDROME_LENGTH);
}

instance_t* deserialize_instance(const uint8_t* buf) {
    instance_t* inst = (instance_t*) malloc(sizeof(instance_t));
    inst->mat_H = NULL;
    memcpy(inst->seed_H, buf, PARAM_SEED_SIZE);
    memcpy(inst->y, buf + PARAM_SEED_SIZE, PARAM_SYNDROME_LENGTH);
    return inst;
}

void serialize_instance_solution(uint8_t* buf, const solution_t* sol) {
    memcpy(buf, sol->x_A, PARAM_PLAINTEXT_LENGTH);
    buf += PARAM_PLAINTEXT_LENGTH;
    for(size_t nchunk=0; nchunk<PARAM_SPLITTING_FACTOR; nchunk++) {
        memcpy(buf, sol->q_poly[nchunk], PARAM_CHUNK_WEIGHT);
        buf += PARAM_CHUNK_WEIGHT;
    }
}

solution_t* deserialize_instance_solution(const uint8_t* buf) {
    solution_t* sol = (solution_t*) malloc(sizeof(solution_t));
    memcpy(sol->x_A, buf, PARAM_PLAINTEXT_LENGTH);
    buf += PARAM_PLAINTEXT_LENGTH;
    for(size_t nchunk=0; nchunk<PARAM_SPLITTING_FACTOR; nchunk++) {
        memcpy(sol->q_poly[nchunk], buf, PARAM_CHUNK_WEIGHT);
        buf += PARAM_CHUNK_WEIGHT;
    }
    return sol;
}
