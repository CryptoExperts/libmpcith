#ifndef MPC_STRUCT_H
#define MPC_STRUCT_H

#include "parameters.h"
#include "witness.h"
#include "field.h"

typedef solution_t vec_wit_t;

typedef struct vec_unif_t {
    point_t a[PARAM_n][PARAM_eta];
} vec_unif_t;

typedef struct vec_corr_t {
    point_t c[PARAM_eta];
} vec_corr_t;

typedef struct vec_challenge_t {
    point_t gamma[PARAM_m][PARAM_eta];
    point_t eps[PARAM_eta];
    // Precomputed
    point_t linA[PARAM_n][PARAM_n][PARAM_eta];
} vec_challenge_t;

typedef struct vec_broadcast_t {
    point_t alpha[PARAM_n][PARAM_eta];
    point_t v[PARAM_eta];
} vec_broadcast_t;

#define PARAM_COMPRESSED_BR_SIZE (sizeof(vec_unif_t))

#endif /* MPC_STRUCT_H */
