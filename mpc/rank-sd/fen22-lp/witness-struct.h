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
    uint8_t y[PARAM_SYNDROME_LENGTH][PARAM_CODE_EXT_BYTESIZE];
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
    uint8_t x_A[PARAM_CODE_DIMENSION][PARAM_CODE_EXT_BYTESIZE];
    uint8_t beta[PARAM_CODE_WEIGHT][PARAM_CODE_EXT_BYTESIZE];
} solution_t;

// "PARAM_INSTANCE_SIZE" corresponds to the size
//   in bytes of the structure of type "instance_t"
//   when serialized.
#define PARAM_INSTANCE_SIZE (PARAM_SEED_SIZE+PARAM_SYNDROME_LENGTH*PARAM_CODE_EXT_BYTESIZE)

// "PARAM_SOL_SIZE" corresponds to the size
//   in bytes of the structure of type "solution_t"
//   when serialized.
#define PARAM_SOL_SIZE ((PARAM_CODE_DIMENSION+PARAM_CODE_WEIGHT)*PARAM_CODE_EXT_BYTESIZE)

#endif /* WITNESS_STRUCT_H */
