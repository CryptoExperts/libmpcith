#ifndef LIBMPCITH_MPC_UTILS_H
#define LIBMPCITH_MPC_UTILS_H

#include "mpc-struct.h"
#include "mpc.h"
#include <stdlib.h>
#include "mpc-utils-meta.h"

typedef uint8_t vec_sel_t;
typedef uint8_t vec_share_t;

// Size of all vectors
#define PARAM_WIT_SIZE (sizeof(vec_wit_t))
#define PARAM_UNIF_SIZE (sizeof(vec_unif_t))
#define PARAM_BR_SIZE (sizeof(vec_broadcast_t))

#define get_size_of_share(sh_info) (get_share_bytesize(sh_info))
#define get_size_of_wit(sh_info) (sizeof(vec_wit_t))
#define get_size_of_unif(sh_info) (sizeof(vec_unif_t))
#define get_size_of_sel(sh_info) (get_selector_bytesize(sh_info))
#define get_size_of_broadcast(sh_info) (sizeof(vec_broadcast_t))
#define get_size_of_challenge(sh_info) (get_challenge_bytesize(sh_info))

// Vector Operation (malloc, arithmetic, compression)
build_all_vector_operations_dynamic(share, vec_share_t, get_size_of_share)
build_all_vector_operations_dynamic(wit, vec_wit_t, get_size_of_wit)
build_all_vector_operations_dynamic(unif, vec_unif_t, get_size_of_unif)
build_all_vector_operations_dynamic(sel, vec_sel_t, get_size_of_sel)
build_all_vector_operations_dynamic(br, vec_broadcast_t, get_size_of_broadcast)

// Getters
#define get_size_before_wit(sh_info) (0)
#define get_size_before_unif(sh_info) (PARAM_WIT_SIZE)
#define get_size_before_sel(sh_info) (PARAM_WIT_SIZE + PARAM_UNIF_SIZE)
build_get_operation_dynamic(wit, vec_wit_t, get_size_before_wit)
build_get_operation_dynamic(unif, vec_unif_t, get_size_before_unif)
build_get_operation_dynamic(sel, vec_sel_t, get_size_before_sel)

// Challenge
build_alloc_operation_dynamic(challenge, vec_challenge_t, get_size_of_challenge)

#endif /* LIBMPCITH_MPC_UTILS_H */
