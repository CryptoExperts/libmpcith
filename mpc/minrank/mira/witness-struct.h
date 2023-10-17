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
    gfq M0[MIRA_PARAM_M*MIRA_PARAM_N];
    gfq (*Mi)[MIRA_PARAM_K][MIRA_PARAM_M*MIRA_PARAM_N];
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
    gfq x[MIRA_PARAM_K];
    gfqm beta[MIRA_PARAM_R];
} solution_t;

// "PARAM_INSTANCE_SIZE" corresponds to the size
//   in bytes of the structure of type "instance_t"
//   when serialized.
#define PARAM_INSTANCE_SIZE (PARAM_SEED_SIZE+MIRA_VEC_BYTES(MIRA_PARAM_M*MIRA_PARAM_N))

// "PARAM_SOL_SIZE" corresponds to the size
//   in bytes of the structure of type "solution_t"
//   when serialized.
#define PARAM_SOL_SIZE (MIRA_VEC_BYTES(MIRA_PARAM_K)+MIRA_VEC_BYTES(MIRA_PARAM_R*MIRA_PARAM_M))

#endif /* WITNESS_STRUCT_H */
