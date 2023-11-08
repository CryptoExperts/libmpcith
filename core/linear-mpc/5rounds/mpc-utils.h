#ifndef LIBMPCITH_MPC_UTILS_H
#define LIBMPCITH_MPC_UTILS_H

#include "mpc-struct.h"
#include <stdlib.h>
#include "mpc-utils-meta.h"

// No pointer, to have a way to have concatenate
typedef struct vec_share_t {
    vec_wit_t wit;
    vec_unif_t unif;
    vec_corr_t corr;
} vec_share_t;
typedef void sharing_info_t;

// Size of all vectors
#define PARAM_WIT_SIZE (sizeof(vec_wit_t))
#define PARAM_UNIF_SIZE (sizeof(vec_unif_t))
#define PARAM_CORR_SIZE (sizeof(vec_corr_t))
#define PARAM_SHARE_SIZE (PARAM_WIT_SIZE+PARAM_UNIF_SIZE+PARAM_CORR_SIZE)
#define PARAM_BR_SIZE (sizeof(vec_broadcast_t))

#define get_size_of_share(sh_info) (PARAM_SHARE_SIZE)
#define get_size_of_wit(sh_info) (PARAM_WIT_SIZE)
#define get_size_of_unif(sh_info) (PARAM_UNIF_SIZE)
#define get_size_of_corr(sh_info) (PARAM_CORR_SIZE)
#define get_size_of_broadcast(sh_info) (PARAM_BR_SIZE)
#define get_size_of_challenge(sh_info) (sizeof(vec_challenge_t))

// Vector Operation (malloc, arithmetic, compression)
build_all_vector_operations_static(share, vec_share_t, get_size_of_share)
build_all_vector_operations_static(wit, vec_wit_t, get_size_of_wit)
build_all_vector_operations_static(unif, vec_unif_t, get_size_of_unif)
build_all_vector_operations_static(corr, vec_corr_t, get_size_of_corr)
build_all_vector_operations_static(br, vec_broadcast_t, get_size_of_broadcast)

// Getters
#define get_size_before_wit(sh_info) (0)
#define get_size_before_unif(sh_info) (PARAM_WIT_SIZE)
#define get_size_before_corr(sh_info) (PARAM_WIT_SIZE+PARAM_UNIF_SIZE)
build_get_operation_static(wit, vec_wit_t, get_size_before_wit)
build_get_operation_static(unif, vec_unif_t, get_size_before_unif)
build_get_operation_static(corr, vec_corr_t, get_size_before_corr)

// Challenge
build_alloc_operation_static(challenge, vec_challenge_t, get_size_of_challenge)

#endif /* LIBMPCITH_MPC_UTILS_H */
