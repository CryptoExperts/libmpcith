#ifndef LIBMPCITH_MPC_UTILS_H
#define LIBMPCITH_MPC_UTILS_H

#include "mpc-struct.h"
#include <stdlib.h>
#include "mpc-utils-meta.h"

// No pointer, to have a way to have concatenate
typedef struct vec_share_t {
    vec_wit_t wit;
    vec_unif_t unif;
    vec_hint_t hint;
} vec_share_t;
typedef void sharing_info_t;

// Size of all vectors
#define PARAM_WIT_SIZE (sizeof(vec_wit_t))
#define PARAM_UNIF_SIZE (sizeof(vec_unif_t))
#define PARAM_HINT_SIZE (sizeof(vec_hint_t))
#define PARAM_SHARE_SIZE (PARAM_WIT_SIZE+PARAM_UNIF_SIZE+PARAM_HINT_SIZE)
#define PARAM_BR_SIZE (sizeof(vec_broadcast_t))

#define get_size_of_share(sh_info) (PARAM_SHARE_SIZE)
#define get_size_of_wit(sh_info) (PARAM_WIT_SIZE)
#define get_size_of_unif(sh_info) (PARAM_UNIF_SIZE)
#define get_size_of_hint(sh_info) (PARAM_HINT_SIZE)
#define get_size_of_broadcast(sh_info) (PARAM_BR_SIZE)
#define get_size_of_challenge_1(sh_info) (sizeof(vec_challenge_1_t))
#define get_size_of_challenge_2(sh_info) (sizeof(vec_challenge_2_t))

// Vector Operation (malloc, arithmetic, compression)
build_all_vector_operations_static(share, vec_share_t, get_size_of_share)
build_all_vector_operations_static(wit, vec_wit_t, get_size_of_wit)
build_all_vector_operations_static(unif, vec_unif_t, get_size_of_unif)
build_all_vector_operations_static(hint, vec_hint_t, get_size_of_hint)
build_all_vector_operations_static(br, vec_broadcast_t, get_size_of_broadcast)

// Getters
#define get_size_before_wit(sh_info) (0)
#define get_size_before_unif(sh_info) (PARAM_WIT_SIZE)
#define get_size_before_hint(sh_info) (PARAM_WIT_SIZE+PARAM_UNIF_SIZE)
build_get_operation_static(wit, vec_wit_t, get_size_before_wit)
build_get_operation_static(unif, vec_unif_t, get_size_before_unif)
build_get_operation_static(hint, vec_hint_t, get_size_before_hint)

// Challenge
build_alloc_operation_static(challenge_1, vec_challenge_1_t, get_size_of_challenge_1)
build_alloc_operation_static(challenge_2, vec_challenge_2_t, get_size_of_challenge_2)

#endif /* LIBMPCITH_MPC_UTILS_H */
