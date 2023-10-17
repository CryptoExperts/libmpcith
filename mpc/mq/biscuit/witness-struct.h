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
    uint8_t seed[PARAM_SEED_SIZE];
    uint8_t t[PARAM_m];
    uint8_t (*f)[PARAM_m][3*PARAM_n];
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
    uint8_t s[PARAM_n];
} solution_t;

// "PARAM_INSTANCE_SIZE" corresponds to the size
//   in bytes of the structure of type "instance_t"
//   when serialized.
#define PARAM_INSTANCE_SIZE (PARAM_SEED_SIZE+PARAM_m)

// "PARAM_SOL_SIZE" corresponds to the size
//   in bytes of the structure of type "solution_t"
//   when serialized.
#define PARAM_SOL_SIZE (PARAM_n)

#endif /* WITNESS_STRUCT_H */
