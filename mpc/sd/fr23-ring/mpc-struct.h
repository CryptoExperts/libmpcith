#ifndef MPC_STRUCT_H
#define MPC_STRUCT_H

#include "parameters.h"
#include "witness.h"
#include "field.h"

typedef struct sharing_info_t {
    uint32_t ring_size;
    uint32_t selector_side_size[PARAM_SELECTOR_DEGREE];
    uint32_t selector_bitsize;
    uint32_t share_bitsize;
    uint32_t nb_additional_equations;
} sharing_info_t;

typedef solution_t vec_wit_t;

typedef struct vec_unif_t {
    uint8_t u[PARAM_CIRCUIT_DEPTH-1][PARAM_eta];
} vec_unif_t;

typedef struct vec_challenge_t {
    uint8_t gamma[PARAM_CODE_LENGTH][PARAM_eta];
    //dynamic: uint8_t gamma_[][PARAM_eta];
} vec_challenge_t;

typedef struct vec_broadcast_t {
    uint8_t alpha[PARAM_eta];
} vec_broadcast_t;

#define PARAM_COMPRESSED_BR_SIZE (PARAM_eta)

#endif /* MPC_STRUCT_H */
