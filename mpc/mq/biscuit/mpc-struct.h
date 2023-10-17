#ifndef MPC_STRUCT_H
#define MPC_STRUCT_H

#include "parameters.h"
#include "witness.h"
#include "field.h"

typedef solution_t vec_wit_t;

typedef struct vec_unif_t {
    uint8_t a[PARAM_m];
} vec_unif_t;

typedef struct vec_corr_t {
    uint8_t c[PARAM_m];
} vec_corr_t;

typedef struct vec_challenge_t {
    uint8_t eps[PARAM_m];
} vec_challenge_t;

typedef struct vec_broadcast_t {
    uint8_t alpha[PARAM_m];
    uint8_t v[PARAM_m];
} vec_broadcast_t;

#define PARAM_COMPRESSED_BR_SIZE (sizeof(vec_unif_t))

#endif /* MPC_STRUCT_H */
