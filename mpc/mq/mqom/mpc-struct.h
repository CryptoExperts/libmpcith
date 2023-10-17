#ifndef MPC_STRUCT_H
#define MPC_STRUCT_H

#include "parameters.h"
#include "witness.h"

#define PARAM_PRECOMPUTE_LIN_A

typedef solution_t vec_wit_t;

typedef struct vec_unif_t {
    uint8_t a[PARAM_n2][PARAM_eta];
} vec_unif_t;

typedef struct vec_challenge_1_t {
    uint8_t gamma[PARAM_m][PARAM_eta];
    // Precomputed
    #ifdef PARAM_PRECOMPUTE_LIN_A
    uint8_t linA[PARAM_MATRIX_BYTESIZE][PARAM_eta];
    #endif
} vec_challenge_1_t;

typedef struct vec_hint_t {
    uint8_t q_poly[2*PARAM_n1-1][PARAM_eta];
} vec_hint_t;

typedef struct vec_challenge_2_t {
    uint8_t r[PARAM_eta];
    // Precomputed
    uint8_t r_powers[2*PARAM_n1][PARAM_eta];
} vec_challenge_2_t;

typedef struct vec_broadcast_t {
    uint8_t alpha[PARAM_n2][PARAM_eta];
    uint8_t v[PARAM_eta];
} vec_broadcast_t;

#include "field.h"

#include "mpc-compression.h"

#endif /* MPC_STRUCT_H */
