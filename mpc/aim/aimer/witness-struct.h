#ifndef WITNESS_STRUCT_H
#define WITNESS_STRUCT_H

#include <stdint.h>
#include "parameters.h"
#include "field.h"
#include "aim.h"

typedef struct instance_t {
    uint8_t iv[PARAM_SEED_SIZE];
    GF ct;
    GF *vector_b;
    GF **matrix_A;
} instance_t;

typedef struct solution_t {
    GF pt;
    GF t[NUM_INPUT_SBOX];
} solution_t;

#define PARAM_INSTANCE_SIZE (PARAM_SEED_SIZE+PARAM_BLOCK_SIZE)
#define PARAM_SOL_SIZE (PARAM_BLOCK_SIZE*(NUM_INPUT_SBOX+1))

#endif /* WITNESS_STRUCT_H */
