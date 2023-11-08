#include "witness.h"
#include "mpc.h"

void hash_update_instance(hash_context* ctx, const instance_t* inst) {
    hash_update(ctx, inst->seed, PARAM_SEED_SIZE);
    hash_update(ctx, inst->y, PARAM_m);
}

void serialize_template_instance(uint8_t* buf, const instance_t* inst) {
    memcpy(buf, inst->seed, PARAM_SEED_SIZE);
}

instance_t* deserialize_template_instance(const uint8_t* buf) {
    instance_t* inst = (instance_t*) malloc(sizeof(instance_t));
    inst->A = NULL;
    inst->b = NULL;
    memcpy(inst->seed, buf, PARAM_SEED_SIZE);
    return inst;
}

void serialize_instance(uint8_t* buf, const instance_t* inst) {
    memcpy(buf, inst->seed, PARAM_SEED_SIZE);
    memcpy(buf + PARAM_SEED_SIZE, inst->y, PARAM_m);
}

instance_t* deserialize_instance(const uint8_t* buf) {
    instance_t* inst = (instance_t*) malloc(sizeof(instance_t));
    inst->A = NULL;
    inst->b = NULL;
    memcpy(inst->seed, buf, PARAM_SEED_SIZE);
    memcpy(inst->y, buf + PARAM_SEED_SIZE, PARAM_m);
    return inst;
}

void serialize_instance_solution(uint8_t* buf, const solution_t* sol) {
    memcpy(buf, sol->x, PARAM_n);
}

solution_t* deserialize_instance_solution(const uint8_t* buf) {
    solution_t* sol = (solution_t*) malloc(sizeof(solution_t));
    memcpy(sol->x, buf, PARAM_n);
    return sol;
}
