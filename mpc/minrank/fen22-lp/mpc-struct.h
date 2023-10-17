#ifndef MPC_STRUCT_H
#define MPC_STRUCT_H

#include "parameters.h"
#include "witness.h"

typedef solution_t vec_wit_t;

typedef struct vec_unif_t {
    point_t a[PARAM_r][PARAM_eta][PARAM_m];
} vec_unif_t;

typedef struct vec_corr_t {
    point_t c[PARAM_eta][PARAM_m];
} vec_corr_t;

typedef struct vec_challenge_t {
    point_t gamma[PARAM_n][PARAM_eta][PARAM_m];
    point_t eps[PARAM_eta][PARAM_m];
} vec_challenge_t;

typedef struct vec_broadcast_t {
    point_t alpha[PARAM_r][PARAM_eta][PARAM_m];
    point_t v[PARAM_eta][PARAM_m];
} vec_broadcast_t;

#include "field.h"
#ifdef VEC_COMPRESSIBLE
    #define PARAM_WIT_SHORT_SIZE (get_serialized_size(sizeof(vec_wit_t)/sizeof(vec_elt_t)))
    #define PARAM_UNIF_SHORT_SIZE (get_serialized_size(sizeof(vec_unif_t)/sizeof(vec_elt_t)))
    #define PARAM_CORR_SHORT_SIZE (get_serialized_size(sizeof(vec_corr_t)/sizeof(vec_elt_t)))
    #define PARAM_BR_SHORT_SIZE (get_serialized_size(sizeof(vec_broadcast_t)/sizeof(vec_elt_t)))
    #define PARAM_COMPRESSED_BR_SIZE (get_serialized_size(sizeof(vec_unif_t)/sizeof(vec_elt_t)))
#else
    #define PARAM_COMPRESSED_BR_SIZE (sizeof(vec_unif_t))
#endif

#endif /* MPC_STRUCT_H */
