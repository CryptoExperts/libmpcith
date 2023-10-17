#ifndef LIBMPCITH_MPC_UTILS_H
#define LIBMPCITH_MPC_UTILS_H

#include "mpc-struct.h"
#include <stdlib.h>

// No pointer, to have a way to have concatenate
typedef struct vec_share_t {
    vec_wit_t wit;
    vec_unif_t unif;
    vec_hint_t hint;
} vec_share_t;

#define PARAM_WIT_SIZE (sizeof(vec_wit_t))
#define PARAM_UNIF_SIZE (sizeof(vec_unif_t))
#define PARAM_HINT_SIZE (sizeof(vec_hint_t))
#define PARAM_SHARE_SIZE (PARAM_WIT_SIZE+PARAM_UNIF_SIZE+PARAM_HINT_SIZE)
#define PARAM_BR_SIZE (sizeof(vec_broadcast_t))

/******************************************************
 *                  Memory Allocation                 *
 ******************************************************/

static inline vec_share_t* new_share(void) {
    return (vec_share_t*) malloc(PARAM_SHARE_SIZE);
}

static inline vec_unif_t* new_unif(void) {
    return (vec_unif_t*) malloc(PARAM_UNIF_SIZE);
}

static inline vec_hint_t* new_hint(void) {
    return (vec_hint_t*) malloc(PARAM_HINT_SIZE);
}

static inline vec_broadcast_t* new_br(void) {
    return (vec_broadcast_t*) malloc(PARAM_BR_SIZE);
}

static inline vec_challenge_1_t* new_challenge_1(void) {
    return (vec_challenge_1_t*) malloc(sizeof(vec_challenge_1_t));
}

static inline vec_challenge_2_t* new_challenge_2(void) {
    return (vec_challenge_2_t*) malloc(sizeof(vec_challenge_2_t));
}

/******************************************************
 *                      Getters                       *
 ******************************************************/

static inline vec_wit_t* get_wit(vec_share_t* sh) {
    return (vec_wit_t*) sh;
}

static inline vec_unif_t* get_unif(vec_share_t* sh) {
    return (vec_unif_t*) (((uint8_t*)sh) + PARAM_WIT_SIZE);
}

static inline vec_hint_t* get_hint(vec_share_t* sh) {
    return (vec_hint_t*) (((uint8_t*)sh) + PARAM_WIT_SIZE + PARAM_UNIF_SIZE);
}

static inline const vec_wit_t* get_wit_const(const vec_share_t* sh) {
    return (const vec_wit_t*) sh;
}

static inline const vec_unif_t* get_unif_const(const vec_share_t* sh) {
    return (const vec_unif_t*) (((uint8_t*)sh) + PARAM_WIT_SIZE);
}

static inline const vec_hint_t* get_corr_const(const vec_share_t* sh) {
    return (const vec_hint_t*) (((uint8_t*)sh) + PARAM_WIT_SIZE + PARAM_UNIF_SIZE);
}

/******************************************************
 *                Arithmetic on Scalars               *
 ******************************************************/

extern const vec_elt_t sca_inverse_tab[];
#define sca_setzero sca_zero
#define sca_setone  sca_one

/******************************************************
 *                Arithmetic on Vectors               *
 ******************************************************/

// Vector Set: dst[] <- src[]
#define build_set_operation(prefix,type)                                               \
    static inline void vec_## prefix ##_set(type* dst, const type* src) {              \
        memcpy(dst, src, sizeof(type));                                                \
    }

// Vector Set Zero: x[] <- 0
#define build_setzero_operation(prefix,type)                                          \
    static inline void vec_## prefix ##_setzero(type* x) {                            \
        memset(x, 0, sizeof(type));                                                    \
    }

// Vector Addition: x[] <- x[] + y[]
#define build_add_operation(prefix,type)                                               \
    static inline void vec_## prefix ##_add(type* x, const type* y) {                  \
        vec_add((vec_elt_t*) x, (const vec_elt_t*) y, sizeof(type)/sizeof(vec_elt_t)); \
    }

// Vector Subtraction: x[] <- x[] - y[]
#define build_sub_operation(prefix,type)                                               \
    static inline void vec_## prefix ##_sub(type* x, const type* y) {                  \
        vec_sub((vec_elt_t*) x, (const vec_elt_t*) y, sizeof(type)/sizeof(vec_elt_t)); \
    }

// Vector Negation: x[] <- -x[]
#define build_neg_operation(prefix,type)                                               \
    static inline void vec_## prefix ##_neg(type* x) {                                 \
        vec_neg((vec_elt_t*) x, sizeof(type)/sizeof(vec_elt_t));                       \
    }

// Vector Sampling: x[] <- $
#define build_rnd_operation(prefix,type)                                               \
    static inline void vec_## prefix ##_rnd(type* x, samplable_t* entropy) {           \
        vec_rnd((vec_elt_t*) x, sizeof(type)/sizeof(vec_elt_t), entropy);              \
    }

#ifdef PARAM_RND_EXPANSION_X4
// Vector Sampling x4: x[] <- $
#define build_rnd_operation_x4(prefix,type)                                            \
    static inline void vec_## prefix ##_rnd_x4(type* const* x, samplable_x4_t* entropy) { \
        vec_rnd_x4((vec_elt_t* const*) x, sizeof(type)/sizeof(vec_elt_t), entropy);    \
    }
#else
#define build_rnd_operation_x4(prefix,type)
#endif

// Vector Multiplication: z[] <- x[] * y
#define build_mul_operation(prefix,type)                                               \
    static inline void vec_## prefix ##_mul(type* z, uint16_t y, const type* x) {      \
        vec_mul(                                                                       \
            (vec_elt_t*) z,                                                            \
            sca_get(y), (const vec_elt_t*) x,                                       \
            sizeof(type)/sizeof(vec_elt_t)                                             \
        );                                                                             \
    }

// Vector Multiplication then Addition: z[] <- z[] + x[] * y
#define build_muladd_operation(prefix,type)                                            \
    static inline void vec_## prefix ##_muladd(type* z, uint16_t y, const type* x) {   \
        vec_muladd(                                                                    \
            (vec_elt_t*) z,                                                            \
            sca_get(y), (const vec_elt_t*) x,                                       \
            sizeof(type)/sizeof(vec_elt_t)                                             \
        );                                                                             \
    }

// Vector Multiplication then Addition (v2): z[] <- z[] * y + x[]
#define build_muladd2_operation(prefix,type)                                           \
    static inline void vec_## prefix ##_muladd2(type* z, uint16_t y, const type* x) {  \
        vec_muladd2(                                                                   \
            (vec_elt_t*) z,                                                            \
            sca_get(y), (const vec_elt_t*) x,                                       \
            sizeof(type)/sizeof(vec_elt_t)                                             \
        );                                                                             \
    }

// Vector Normalize: z[i] <- normalize(z[i]), for all i
#ifdef VEC_REQUIRE_NORMALIZATION
#define build_normalize_operation(prefix,type)                                         \
    static inline void vec_## prefix ##_normalize(type* x) {                           \
        vec_normalize((vec_elt_t*) x, sizeof(type)/sizeof(vec_elt_t));                 \
    }
#else
#define build_normalize_operation(prefix,type)                                         \
    static inline void vec_## prefix ##_normalize(type* x) { (void) x; }
#endif

#define build_all_arithmetic_operations(prefix,type)                                   \
    build_set_operation(prefix,type)                                                   \
    build_setzero_operation(prefix,type)                                              \
    build_add_operation(prefix,type)                                                   \
    build_sub_operation(prefix,type)                                                   \
    build_neg_operation(prefix,type)                                                   \
    build_rnd_operation(prefix,type)                                                   \
    build_rnd_operation_x4(prefix,type)                                                \
    build_mul_operation(prefix,type)                                                   \
    build_muladd_operation(prefix,type)                                                \
    build_muladd2_operation(prefix,type)                                               \
    build_normalize_operation(prefix,type)

build_all_arithmetic_operations(share, vec_share_t)
build_all_arithmetic_operations(wit, vec_wit_t)
build_all_arithmetic_operations(unif, vec_unif_t)
build_all_arithmetic_operations(hint, vec_hint_t)
build_all_arithmetic_operations(br, vec_broadcast_t)

/******************************************************
 *                  Vector Compression                *
 ******************************************************/

// Vector Compression
#define build_compress_operation(prefix,type)                                         \
    static inline void vec_## prefix ##_compress(uint8_t* buf, const type* x) {       \
        vec_to_bytes(buf, (const vec_elt_t*) x, sizeof(type)/sizeof(vec_elt_t));      \
    }

// Vector Decompression
#define build_decompress_operation(prefix,type)                                       \
    static inline void vec_## prefix ##_decompress(type* x, const uint8_t* buf) {     \
        vec_from_bytes((vec_elt_t*) x, buf, sizeof(type)/sizeof(vec_elt_t));          \
    }

#define build_all_compression_operations(prefix,type)                                  \
    build_compress_operation(prefix,type)                                              \
    build_decompress_operation(prefix,type)

build_all_compression_operations(share, vec_share_t)
build_all_compression_operations(wit, vec_wit_t)
build_all_compression_operations(unif, vec_unif_t)
build_all_compression_operations(hint, vec_hint_t)
build_all_compression_operations(br, vec_broadcast_t)

#endif /* LIBMPCITH_MPC_UTILS_H */
