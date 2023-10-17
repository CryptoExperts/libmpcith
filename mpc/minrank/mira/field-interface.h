#ifndef FIELD_RANDOM_H
#define FIELD_RANDOM_H

#include "field.h"
#include "sample.h"

// Vector Arithmetic
typedef gfq vec_elt_t;

static inline void vec_add(vec_elt_t* x, const vec_elt_t* y, uint32_t size) {
    gfq_mat_add(x, x, (gfq_mat) y, 1, size);
}
#define vec_sub(x,y,size) vec_add(x, y, size)
#define vec_neg(x,size) {(void) (x); (void) (size);}
static inline void vec_mul(vec_elt_t* z, vec_elt_t y, const vec_elt_t* x, uint32_t size) {
    gfq_mat_scalar_mul(z, (gfq_mat) x, y, 1, size);
}
static inline void vec_muladd(vec_elt_t* z, vec_elt_t y, const vec_elt_t* x, uint32_t size) {
    gfq_mat_scalar_muladd(z, (gfq_mat) x, y, 1, size);
}
static inline void vec_muladd2(vec_elt_t* z, vec_elt_t y, const vec_elt_t* x, uint32_t size) {
    gfq_mat_scalar_mul(z, z, y, 1, size);
    gfq_mat_add(z, z, (gfq_mat) x, 1, size);
}
static inline void vec_rnd(vec_elt_t* x, uint32_t size, samplable_t* entropy) {
    gfq_rnd(x, size, entropy);
}
#ifdef PARAM_RND_EXPANSION_X4
static inline void vec_rnd_x4(vec_elt_t* const* x, uint32_t size, samplable_x4_t* entropy) {
    gfq_rnd_x4((gfq* const*) x, size, entropy);
}
#endif
static inline void vec_to_bytes(uint8_t* buf, const vec_elt_t* x, uint32_t size) {
    gfq_mat_to_string(buf, (const gfq_mat) x, 1, size);
}
static inline void vec_from_bytes(vec_elt_t* x, const uint8_t* buf, uint32_t size) {
    gfq_mat_from_string(x, 1, size, buf);
}

#define VEC_COMPRESSIBLE
#define get_serialized_size(size)   ((size+1)>>1)

// Arithmetic over GF(q)
#define sca_zero()   (0)
#define sca_one()    (1)
#define sca_add(a,b) gfq_add(a,b)
#define sca_sub(a,b) gfq_sub(a,b)
#define sca_mul(a,b) gfq_mul(a,b)
#define sca_neg(a)   (a)
#define sca_get(a)   ((uint8_t)(a))
#define sca_inverse_tab gf16_inverse_tab

#endif
