#ifndef MPC_STRUCT_H
#define MPC_STRUCT_H

#include "parameters.h"
#include "witness.h"
#include "field.h"

typedef solution_t vec_wit_t;

typedef struct vec_unif_t {
    uint8_t a[PARAM_SPLITTING_FACTOR][PARAM_NB_EVALS_PER_POLY][PARAM_EXT_DEGREE];
    uint8_t b[PARAM_SPLITTING_FACTOR][PARAM_NB_EVALS_PER_POLY][PARAM_EXT_DEGREE];
} vec_unif_t;

typedef struct vec_corr_t {
    uint8_t c[PARAM_NB_EVALS_PER_POLY][PARAM_EXT_DEGREE];
} vec_corr_t;

typedef struct vec_challenge_t {
    uint8_t eval[PARAM_NB_EVALS_PER_POLY][PARAM_EXT_DEGREE];
    uint8_t eps[PARAM_SPLITTING_FACTOR][PARAM_NB_EVALS_PER_POLY][PARAM_EXT_DEGREE];
    uint8_t powers_of_ch[PARAM_CHUNK_LENGTH+1][PARAM_ALL_EVALS_BYTESIZE_CEIL16]; // Pre-computed
    uint8_t f_eval[PARAM_NB_EVALS_PER_POLY][PARAM_EXT_DEGREE]; // Pre-computed
} vec_challenge_t;

typedef struct vec_broadcast_t {
    uint8_t alpha[PARAM_SPLITTING_FACTOR][PARAM_NB_EVALS_PER_POLY][PARAM_EXT_DEGREE];
    uint8_t beta[PARAM_SPLITTING_FACTOR][PARAM_NB_EVALS_PER_POLY][PARAM_EXT_DEGREE];
    uint8_t v[PARAM_NB_EVALS_PER_POLY][PARAM_EXT_DEGREE];
} vec_broadcast_t;

#define PARAM_COMPRESSED_BR_SIZE (sizeof(vec_unif_t))

#endif /* MPC_STRUCT_H */
