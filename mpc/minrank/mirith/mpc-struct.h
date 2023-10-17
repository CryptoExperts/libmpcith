#ifndef MPC_STRUCT_H
#define MPC_STRUCT_H

#include "parameters.h"
#include "witness.h"

typedef solution_t vec_wit_t;

typedef struct vec_unif_t {
    ff_t A[matrix_bytes_size(PAR_S, PAR_R)];
} vec_unif_t;

typedef struct vec_corr_t {
    ff_t C[matrix_bytes_size(PAR_S, PAR_N-PAR_R)];
} vec_corr_t;

typedef struct vec_challenge_t {
    ff_t R[matrix_bytes_size(PAR_S, PAR_M)];
} vec_challenge_t;

typedef struct vec_broadcast_t {
    ff_t S[matrix_bytes_size(PAR_S, PAR_R)];
    ff_t V[matrix_bytes_size(PAR_S, PAR_N-PAR_R)];
} vec_broadcast_t;

#include "field-interface.h"

#define PARAM_COMPRESSED_BR_SIZE (sizeof(vec_unif_t))

#endif /* MPC_STRUCT_H */
