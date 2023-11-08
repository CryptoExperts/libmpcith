#ifndef WITNESS_STRUCT_H
#define WITNESS_STRUCT_H

#include <stdint.h>
#include "parameters.h"

// Instance Definition
typedef struct instance_t {
    uint8_t seed_H[PARAM_SEED_SIZE];
    uint8_t y[PARAM_SYNDROME_LENGTH];
    uint8_t (*mat_H)[PARAM_PCMATRIX_BYTESIZE];
} instance_t;

// Solution Definition
typedef struct solution_t {
    uint8_t x_A[PARAM_CODE_DIMENSION];
    uint8_t q_poly[PARAM_SPLITTING_FACTOR][PARAM_CHUNK_WEIGHT];
} solution_t;

#include "witness-serialized.h"

#endif /* WITNESS_STRUCT_H */
