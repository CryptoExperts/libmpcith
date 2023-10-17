#ifndef WITNESS_STRUCT_H
#define WITNESS_STRUCT_H

#include <stdint.h>
#include "parameters.h"

// Instance Definition
//   This structure represents an instance of
//   the problem on which the security of the
//   signature scheme relies. It corresponds
//   to the public key.
//   Some member can be pointers when they are
//   generated at each signing and verification
//   from the others members.
typedef struct instance_t {
    uint8_t seed_H[PARAM_SEED_SIZE];
    uint8_t y[PARAM_SYNDROME_LENGTH];
    uint8_t (*mat_H)[PARAM_PCMATRIX_BYTESIZE];
} instance_t;

// Solution Definition
//   This structure represents a solution for
//   an instance presented by "instance_t".
//   It is part of the secret key of the
//   signature scheme.
//   It corresponds to the extended solution,
//   meaning that it contains all the secret
//   values which can be deterministically
//   built from the solution itself and which
//   are inputs of the underlying MPC protocol.
typedef struct solution_t {
    uint8_t s_A[PARAM_CODE_DIMENSION];
    uint8_t q_poly[PARAM_SPLITTING_FACTOR][PARAM_CHUNK_WEIGHT];
    uint8_t p_poly[PARAM_SPLITTING_FACTOR][PARAM_CHUNK_WEIGHT];
} solution_t;

// "PARAM_INSTANCE_SIZE" corresponds to the size
//   in bytes of the structure of type "instance_t"
//   when serialized.
#define PARAM_INSTANCE_SIZE (PARAM_SEED_SIZE+PARAM_SYNDROME_LENGTH)

// "PARAM_SOL_SIZE" corresponds to the size
//   in bytes of the structure of type "solution_t"
//   when serialized.
#define PARAM_SOL_SIZE (PARAM_CODE_DIMENSION+2*PARAM_SPLITTING_FACTOR*PARAM_CHUNK_WEIGHT)

#endif /* WITNESS_STRUCT_H */
