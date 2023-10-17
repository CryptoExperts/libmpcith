#ifndef MPC_STRUCT_H
#define MPC_STRUCT_H

#include "parameters.h"
#include "witness.h"
#include "field-interface.h"

typedef solution_t vec_wit_t;

typedef struct vec_unif_t {
    uint8_t a[PARAM_r][PARAM_eta_div8];
} vec_unif_t;

typedef struct vec_corr_t {
    uint8_t c[PARAM_n][PARAM_eta_div8];
} vec_corr_t;

typedef struct vec_challenge_t {
    uint8_t eps[PARAM_m][PARAM_eta_div8];
} vec_challenge_t;

typedef struct vec_broadcast_t {
    uint8_t alpha[PARAM_r][PARAM_eta_div8];
    uint8_t v[PARAM_m][PARAM_eta_div8];
} vec_broadcast_t;

#define PARAM_COMPRESSED_BR_SIZE (sizeof(vec_unif_t))

#endif /* MPC_STRUCT_H */
