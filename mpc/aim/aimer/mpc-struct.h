#ifndef MPC_STRUCT_H
#define MPC_STRUCT_H

#include "parameters.h"
#include "witness.h"
#include "field-interface.h"

typedef solution_t vec_wit_t;

typedef struct vec_unif_t {
    GF a;
} vec_unif_t;

typedef struct vec_corr_t {
    GF c;
} vec_corr_t;

typedef struct vec_challenge_t {
    GF eps[NUM_INPUT_SBOX+1];
} vec_challenge_t;

typedef struct vec_broadcast_t {
    GF alpha;
    GF v;
} vec_broadcast_t;

#define PARAM_COMPRESSED_BR_SIZE (sizeof(vec_unif_t))

#endif /* MPC_STRUCT_H */
