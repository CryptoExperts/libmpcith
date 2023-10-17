#ifndef WITNESS_STRUCT_H
#define WITNESS_STRUCT_H

#include <stdint.h>
#include "parameters.h"
#include "matrix.h"

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
    ff_t M0[matrix_bytes_size(PAR_M, PAR_N)];
    ff_t (*Mi)[PAR_K][matrix_bytes_size(PAR_M, PAR_N)];
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
    ff_t alpha[matrix_bytes_size(PAR_K,1)];
    ff_t K[matrix_bytes_size(PAR_R,PAR_N-PAR_R)];
} solution_t;

// "PARAM_INSTANCE_SIZE" corresponds to the size
//   in bytes of the structure of type "instance_t"
//   when serialized.
#define PARAM_INSTANCE_SIZE (PARAM_SEED_SIZE+matrix_bytes_packed_size(PAR_M,PAR_N))

// "PARAM_SOL_SIZE" corresponds to the size
//   in bytes of the structure of type "solution_t"
//   when serialized.
#define PARAM_SOL_SIZE (matrix_bytes_packed_size(PAR_K,1)+matrix_bytes_packed_size(PAR_R,PAR_N-PAR_R))

#endif /* WITNESS_STRUCT_H */
