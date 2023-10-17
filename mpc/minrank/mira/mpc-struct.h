#ifndef MPC_STRUCT_H
#define MPC_STRUCT_H

#include "parameters.h"
#include "witness.h"
#include "field-interface.h"

typedef solution_t vec_wit_t;

typedef struct vec_unif_t {
    gfqm a[MIRA_PARAM_R];
} vec_unif_t;

typedef struct vec_corr_t {
    gfqm c;
} vec_corr_t;

typedef struct vec_challenge_t {
    gfqm gamma[MIRA_PARAM_N];
    gfqm eps;
} vec_challenge_t;

typedef struct vec_broadcast_t {
    gfqm alpha[MIRA_PARAM_R];
    gfqm v;
} vec_broadcast_t;

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
