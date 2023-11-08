#ifndef MPC_STRUCT_H
#define MPC_STRUCT_H

#include "parameters.h"
#include "witness.h"
#include "field.h"

typedef solution_t vec_wit_t;

typedef struct vec_unif_t {
    uint8_t u[PARAM_eta];
#ifdef PARAM_TCITH_MT_EXTENDED
    uint8_t u_dc[PARAM_eta_dc];
#endif
} vec_unif_t;

#ifdef PARAM_TCITH_MT_EXTENDED
typedef struct vec_dc_challenge_t {
    uint8_t gamma_dc[PARAM_CODE_DIMENSION+PARAM_CODE_WEIGHT+PARAM_eta][PARAM_eta_dc];
} vec_dc_challenge_t;

typedef struct vec_dc_broadcast_t {
    uint8_t alpha_dc[PARAM_eta_dc];
} vec_dc_broadcast_t;
#endif

typedef struct vec_challenge_t {
    uint8_t gamma[PARAM_CODE_LENGTH][PARAM_eta];
} vec_challenge_t;

typedef struct vec_broadcast_t {
    uint8_t alpha[PARAM_eta];
} vec_broadcast_t;

#endif /* MPC_STRUCT_H */
