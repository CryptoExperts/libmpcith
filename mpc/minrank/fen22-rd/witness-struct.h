#ifndef WITNESS_STRUCT_H
#define WITNESS_STRUCT_H

#include <stdint.h>
#include "parameters.h"
#include "field.h"

// Instance Definition
//   This structure represents an instance of
//   the problem on which the security of the
//   signature scheme relies. It corresponds
//   to the public key.
//   Some member can be pointers when they are
//   generated at each signing and verification
//   from the others members. 
typedef struct instance_t {
    uint8_t seed_mats[PARAM_SEED_SIZE];
    point_t m0[PARAM_n][PARAM_m];
    point_t (*mats)[PARAM_k*PARAM_nm];
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
    point_t x[PARAM_k];
    point_t mat_T[PARAM_n][PARAM_r];
    point_t mat_R[PARAM_r][PARAM_m];
} solution_t;

// "PARAM_INSTANCE_SIZE" corresponds to the size
//   in bytes of the structure of type "instance_t"
//   when serialized.
#define PARAM_INSTANCE_SIZE (PARAM_SEED_SIZE+PARAM_m*PARAM_n*sizeof(point_t))

// "PARAM_SOL_SIZE" corresponds to the size
//   in bytes of the structure of type "solution_t"
//   when serialized.
#define PARAM_SOL_SIZE (PARAM_k*sizeof(point_t)+PARAM_r*(PARAM_n+PARAM_m)*sizeof(point_t))

#endif /* WITNESS_STRUCT_H */
