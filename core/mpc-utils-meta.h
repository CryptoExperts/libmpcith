#ifndef LIBMPCITH_MPC_UTILS_META_H
#define LIBMPCITH_MPC_UTILS_META_H

// Arithmetic on Scalars
extern const vec_elt_t sca_inverse_tab[];
#define sca_setzero sca_zero
#define sca_setone  sca_one

/******************************************************
 *                  Memory Allocation                 *
 ******************************************************/

#define build_alloc_operation_dynamic(prefix,type,get_size) \
    static inline type* new_## prefix (const sharing_info_t* sh_info) {                            \
        (void) sh_info;                                                                            \
        return (type*) malloc(get_size(sh_info));                                                  \
    }
#define build_alloc_operation_static(prefix,type,get_size)                                   \
    static inline type* new_## prefix (void) {                                                     \
        return (type*) malloc(get_size(NULL));                                                     \
    }

/******************************************************
 *                      Getters                       *
 ******************************************************/

#define build_get_operation_dynamic(prefix,type,get_size_before) \
    static inline type* get_## prefix (vec_share_t* sh, const sharing_info_t* sh_info) {           \
        (void) sh_info;                                                                            \
        return (type*) (((uint8_t*)sh) + get_size_before(sh_info));                                \
    }                                                                                              \
    static inline const type* get_## prefix ##_const(const vec_share_t* sh, const sharing_info_t* sh_info) { \
        (void) sh_info;                                                                            \
        return (const type*) (((const uint8_t*)sh) + get_size_before(sh_info));                    \
    }
#define build_get_operation_static(prefix,type,get_size_before)                                    \
    static inline type* get_## prefix (vec_share_t* sh) {                                          \
        return (type*) (((uint8_t*)sh) + get_size_before(NULL));                                   \
    }                                                                                              \
    static inline const type* get_## prefix ##_const(const vec_share_t* sh) {                      \
        return (const type*) (((const uint8_t*)sh) + get_size_before(NULL));                       \
    }

/******************************************************
 *                Arithmetic on Vectors               *
 ******************************************************/

// Vector Set: dst[] <- src[]
#define build_set_operation_dynamic(prefix,type,get_size)                                          \
    static inline void vec_## prefix ##_set(type* dst, const type* src, const sharing_info_t* sh_info) { \
        (void) sh_info;                                                                            \
        memcpy(dst, src, get_size(sh_info));                                                       \
    }
#define build_set_operation_static(prefix,type,get_size)                                           \
    static inline void vec_## prefix ##_set(type* dst, const type* src) {                          \
        memcpy(dst, src, get_size(NULL));                                                          \
    }

// Vector Set Zero: x[] <- 0
#define build_setzero_operation_dynamic(prefix,type,get_size)                                      \
    static inline void vec_## prefix ##_setzero(type* x, const sharing_info_t* sh_info) {          \
        (void) sh_info;                                                                            \
        memset(x, 0, get_size(sh_info));                                                           \
}
#define build_setzero_operation_static(prefix,type,get_size)                                       \
    static inline void vec_## prefix ##_setzero(type* x) {                                         \
        memset(x, 0, get_size(NULL));                                                              \
    }

// Vector Addition: x[] <- x[] + y[]
#define build_add_operation_dynamic(prefix,type,get_size)                                          \
    static inline void vec_## prefix ##_add(type* x, const type* y, const sharing_info_t* sh_info) { \
        (void) sh_info;                                                                            \
        vec_add((vec_elt_t*) x, (const vec_elt_t*) y, get_size(sh_info)/sizeof(vec_elt_t));        \
    }
#define build_add_operation_static(prefix,type,get_size)                                           \
    static inline void vec_## prefix ##_add(type* x, const type* y) {                              \
        vec_add((vec_elt_t*) x, (const vec_elt_t*) y, get_size(NULL)/sizeof(vec_elt_t));           \
    }

// Vector Subtraction: x[] <- x[] - y[]
#define build_sub_operation_dynamic(prefix,type,get_size)                                          \
    static inline void vec_## prefix ##_sub(type* x, const type* y, const sharing_info_t* sh_info) { \
        (void) sh_info;                                                                            \
        vec_sub((vec_elt_t*) x, (const vec_elt_t*) y, get_size(sh_info)/sizeof(vec_elt_t));        \
    }
#define build_sub_operation_static(prefix,type,get_size)                                           \
    static inline void vec_## prefix ##_sub(type* x, const type* y) {                              \
        vec_sub((vec_elt_t*) x, (const vec_elt_t*) y, get_size(NULL)/sizeof(vec_elt_t));           \
    }

// Vector Negation: x[] <- -x[]
#define build_neg_operation_dynamic(prefix,type,get_size)                                          \
    static inline void vec_## prefix ##_neg(type* x, const sharing_info_t* sh_info) {              \
        (void) sh_info;                                                                            \
        vec_neg((vec_elt_t*) x, get_size(sh_info)/sizeof(vec_elt_t));                              \
    }
#define build_neg_operation_static(prefix,type,get_size)                                           \
    static inline void vec_## prefix ##_neg(type* x) {                                             \
        vec_neg((vec_elt_t*) x, get_size(NULL)/sizeof(vec_elt_t));                                 \
    }

// Vector Sampling: x[] <- $
#define build_rnd_operation_dynamic(prefix,type,get_size)                                          \
    static inline void vec_## prefix ##_rnd(type* x, samplable_t* entropy, const sharing_info_t* sh_info) { \
        (void) sh_info;                                                                            \
        vec_rnd((vec_elt_t*) x, get_size(sh_info)/sizeof(vec_elt_t), entropy);                     \
    }
#define build_rnd_operation_static(prefix,type,get_size)                                           \
    static inline void vec_## prefix ##_rnd(type* x, samplable_t* entropy) {                       \
        vec_rnd((vec_elt_t*) x, get_size(NULL)/sizeof(vec_elt_t), entropy);                        \
    }

#ifdef PARAM_RND_EXPANSION_X4
// Vector Sampling x4: x[] <- $
#define build_rnd_operation_x4_dynamic(prefix,type,get_size)                                       \
    static inline void vec_## prefix ##_rnd_x4(type* const* x, samplable_x4_t* entropy, const sharing_info_t* sh_info) { \
        (void) sh_info;                                                                            \
        vec_rnd_x4((vec_elt_t* const*) x, get_size(sh_info)/sizeof(vec_elt_t), entropy);           \
    }
#define build_rnd_operation_x4_static(prefix,type,get_size)                                        \
    static inline void vec_## prefix ##_rnd_x4(type* const* x, samplable_x4_t* entropy) {          \
        vec_rnd_x4((vec_elt_t* const*) x, get_size(NULL)/sizeof(vec_elt_t), entropy);              \
    }
#else
#define build_rnd_operation_x4_dynamic(prefix,type,get_size)
#define build_rnd_operation_x4_static(prefix,type,get_size)
#endif

// Vector Multiplication: z[] <- x[] * y
#define build_mul_operation_dynamic(prefix,type,get_size)                                          \
    static inline void vec_## prefix ##_mul(type* z, uint16_t y, const type* x, const sharing_info_t* sh_info) { \
        (void) sh_info;                                                                            \
        vec_mul(                                                                                   \
            (vec_elt_t*) z,                                                                        \
            sca_get(y), (const vec_elt_t*) x,                                                      \
            get_size(sh_info)/sizeof(vec_elt_t)                                                    \
        );                                                                                         \
    }
#define build_mul_operation_static(prefix,type,get_size)                                           \
    static inline void vec_## prefix ##_mul(type* z, uint16_t y, const type* x) {                  \
        vec_mul(                                                                                   \
            (vec_elt_t*) z,                                                                        \
            sca_get(y), (const vec_elt_t*) x,                                                      \
            get_size(NULL)/sizeof(vec_elt_t)                                                       \
        );                                                                                         \
    }   

// Vector Multiplication then Addition: z[] <- z[] + x[] * y
#define build_muladd_operation_dynamic(prefix,type,get_size)                                       \
    static inline void vec_## prefix ##_muladd(type* z, uint16_t y, const type* x, const sharing_info_t* sh_info) { \
        (void) sh_info;                                                                            \
        vec_muladd(                                                                                \
            (vec_elt_t*) z,                                                                        \
            sca_get(y), (const vec_elt_t*) x,                                                      \
            get_size(sh_info)/sizeof(vec_elt_t)                                                    \
        );                                                                                         \
    }
#define build_muladd_operation_static(prefix,type,get_size)                                        \
    static inline void vec_## prefix ##_muladd(type* z, uint16_t y, const type* x) {               \
        vec_muladd(                                                                                \
            (vec_elt_t*) z,                                                                        \
            sca_get(y), (const vec_elt_t*) x,                                                      \
            get_size(NULL)/sizeof(vec_elt_t)                                                       \
        );                                                                                         \
    }

// Vector Multiplication then Addition (v2): z[] <- z[] * y + x[]
#define build_muladd2_operation_dynamic(prefix,type,get_size)                                      \
    static inline void vec_## prefix ##_muladd2(type* z, uint16_t y, const type* x, const sharing_info_t* sh_info) { \
        (void) sh_info;                                                                            \
        vec_muladd2(                                                                               \
            (vec_elt_t*) z,                                                                        \
            sca_get(y), (const vec_elt_t*) x,                                                      \
            get_size(sh_info)/sizeof(vec_elt_t)                                                    \
        );                                                                                         \
    }
#define build_muladd2_operation_static(prefix,type,get_size)                                       \
    static inline void vec_## prefix ##_muladd2(type* z, uint16_t y, const type* x) {              \
        vec_muladd2(                                                                               \
            (vec_elt_t*) z,                                                                        \
            sca_get(y), (const vec_elt_t*) x,                                                      \
            get_size(NULL)/sizeof(vec_elt_t)                                                       \
        );                                                                                         \
    }

// Vector Normalize: z[i] <- normalize(z[i]), for all i
#ifdef VEC_REQUIRE_NORMALIZATION
#define build_normalize_operation_dynamic(prefix,type,get_size)                                    \
    static inline void vec_## prefix ##_normalize(type* x, const sharing_info_t* sh_info) {        \
        vec_normalize((vec_elt_t*) x, get_size(sh_info)/sizeof(vec_elt_t));                        \
    }
#define build_normalize_operation_static(prefix,type,get_size)                                     \
    static inline void vec_## prefix ##_normalize(type* x) {                                       \
        vec_normalize((vec_elt_t*) x, get_size(NULL)/sizeof(vec_elt_t));                           \
    }
#else
#define build_normalize_operation_dynamic(prefix,type,get_size)                                    \
    static inline void vec_## prefix ##_normalize(type* x, const sharing_info_t* sh_info) {        \
        (void) sh_info;                                                                            \
        (void) x;                                                                                  \
    }
#define build_normalize_operation_static(prefix,type,get_size)                                     \
    static inline void vec_## prefix ##_normalize(type* x) {                                       \
        (void) x;                                                                                  \
    }
#endif

#define build_all_arithmetic_operations_dynamic(prefix,type,get_size)                              \
    build_set_operation_dynamic(prefix,type,get_size)                                              \
    build_setzero_operation_dynamic(prefix,type,get_size)                                          \
    build_add_operation_dynamic(prefix,type,get_size)                                              \
    build_sub_operation_dynamic(prefix,type,get_size)                                              \
    build_neg_operation_dynamic(prefix,type,get_size)                                              \
    build_rnd_operation_dynamic(prefix,type,get_size)                                              \
    build_rnd_operation_x4_dynamic(prefix,type,get_size)                                           \
    build_mul_operation_dynamic(prefix,type,get_size)                                              \
    build_muladd_operation_dynamic(prefix,type,get_size)                                           \
    build_muladd2_operation_dynamic(prefix,type,get_size)                                          \
    build_normalize_operation_dynamic(prefix,type,get_size)
#define build_all_arithmetic_operations_static(prefix,type,get_size)                               \
    build_set_operation_static(prefix,type,get_size)                                               \
    build_setzero_operation_static(prefix,type,get_size)                                           \
    build_add_operation_static(prefix,type,get_size)                                               \
    build_sub_operation_static(prefix,type,get_size)                                               \
    build_neg_operation_static(prefix,type,get_size)                                               \
    build_rnd_operation_static(prefix,type,get_size)                                               \
    build_rnd_operation_x4_static(prefix,type,get_size)                                            \
    build_mul_operation_static(prefix,type,get_size)                                               \
    build_muladd_operation_static(prefix,type,get_size)                                            \
    build_muladd2_operation_static(prefix,type,get_size)                                           \
    build_normalize_operation_static(prefix,type,get_size)

/******************************************************
 *                  Vector Compression                *
 ******************************************************/

// Vector Compression
#define build_compress_operation_dynamic(prefix,type,get_size)                                     \
    static inline void vec_## prefix ##_compress(uint8_t* buf, const type* x, const sharing_info_t* sh_info) { \
        (void) sh_info;                                                                            \
        vec_to_bytes(buf, (const vec_elt_t*) x, get_size(sh_info)/sizeof(vec_elt_t));              \
    }
#define build_compress_operation_static(prefix,type,get_size)                                      \
    static inline void vec_## prefix ##_compress(uint8_t* buf, const type* x) {                    \
        vec_to_bytes(buf, (const vec_elt_t*) x, get_size(NULL)/sizeof(vec_elt_t));                 \
    }

// Vector Decompression
#define build_decompress_operation_dynamic(prefix,type,get_size)                                   \
    static inline void vec_## prefix ##_decompress(type* x, const uint8_t* buf, const sharing_info_t* sh_info) { \
        (void) sh_info;                                                                            \
        vec_from_bytes((vec_elt_t*) x, buf, get_size(sh_info)/sizeof(vec_elt_t));                  \
    }
#define build_decompress_operation_static(prefix,type,get_size)                                    \
    static inline void vec_## prefix ##_decompress(type* x, const uint8_t* buf) {                  \
        vec_from_bytes((vec_elt_t*) x, buf, get_size(NULL)/sizeof(vec_elt_t));                     \
    }

#define build_all_compression_operations_dynamic(prefix,type,get_size)                             \
    build_compress_operation_dynamic(prefix,type,get_size)                                         \
    build_decompress_operation_dynamic(prefix,type,get_size)
#define build_all_compression_operations_static(prefix,type,get_size)                              \
    build_compress_operation_static(prefix,type,get_size)                                          \
    build_decompress_operation_static(prefix,type,get_size)

/******************************************************
 *                      Misc                          *
 ******************************************************/

#define build_all_vector_operations_dynamic(prefix,type,get_size)                                  \
    build_alloc_operation_dynamic(prefix,type,get_size)                                            \
    build_all_arithmetic_operations_dynamic(prefix,type,get_size)                                  \
    build_all_compression_operations_dynamic(prefix,type,get_size)
#define build_all_vector_operations_static(prefix,type,get_size)                                   \
    build_alloc_operation_static(prefix,type,get_size)                                             \
    build_all_arithmetic_operations_static(prefix,type,get_size)                                   \
    build_all_compression_operations_static(prefix,type,get_size)

#endif /* LIBMPCITH_MPC_UTILS_META_H */
