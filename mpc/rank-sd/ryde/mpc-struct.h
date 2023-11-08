#ifndef MPC_STRUCT_H
#define MPC_STRUCT_H

#include "parameters.h"
#include "witness.h"
#include "field-interface.h"

typedef solution_t vec_wit_t;

typedef struct vec_unif_t {
    rbc_elt a[RYDE_PARAM_W-1];
} vec_unif_t;

typedef struct vec_corr_t {
    rbc_elt c;
} vec_corr_t;

typedef struct vec_challenge_t {
    rbc_elt gamma[RYDE_PARAM_N];
    rbc_elt eps;
} vec_challenge_t;

typedef struct vec_broadcast_t {
    rbc_elt alpha[RYDE_PARAM_W-1];
    rbc_elt v;
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
