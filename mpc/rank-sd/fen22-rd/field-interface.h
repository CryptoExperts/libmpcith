#ifndef FIELD_RANDOM_H
#define FIELD_RANDOM_H

#include "gf2.h"
#include "sample.h"

// Vector Arithmetic
typedef uint8_t vec_elt_t; // The base field is GF(2)

static inline void vec_add(vec_elt_t* x, const vec_elt_t* y, uint32_t size) {
    gf2_add(x,y,size<<3);
}
#define vec_sub(x,y,size) vec_add(x, y, size)
#define vec_neg(x,size) {(void) (x); (void) (size);}
static inline void vec_mul(vec_elt_t* z, vec_elt_t y, const vec_elt_t* x, uint32_t size) {
    uint8_t mask = y*0xFF;
    for(uint32_t i=0; i<size; i++)
        z[i] = mask & x[i];
}
static inline void vec_muladd(vec_elt_t* z, vec_elt_t y, const vec_elt_t* x, uint32_t size) {
    uint8_t mask = y*0xFF;
    for(uint32_t i=0; i<size; i++)
        z[i] |= mask & x[i];
}
static inline void vec_muladd2(vec_elt_t* z, vec_elt_t y, const vec_elt_t* x, uint32_t size) {
    uint8_t mask = y*0xFF;
    for(uint32_t i=0; i<size; i++)
        z[i] = (mask & z[i]) | x[i];
}
static inline void vec_rnd(vec_elt_t* x, uint32_t size, samplable_t* entropy) {
    gf2_random(x,size<<3,entropy);
}
#ifdef PARAM_RND_EXPANSION_X4
static inline void vec_rnd_x4(vec_elt_t* const* x, uint32_t size, samplable_x4_t* entropy) {
    gf2_random_x4((uint8_t* const*) x,size<<3,entropy);
}
#endif
static inline void vec_to_bytes(uint8_t* buf, const vec_elt_t* x, uint32_t size) {
    memcpy(buf, (const uint8_t*) x, size);
}
static inline void vec_from_bytes(vec_elt_t* x, const uint8_t* buf, uint32_t size) {
    memcpy((uint8_t*) x, buf, size);
}

// Arithmetic over GF(2)
#define sca_zero()   (0)
#define sca_one()    (1)
#define sca_add(a,b) ((a)^(b))
#define sca_sub(a,b) ((a)^(b))
#define sca_mul(a,b) ((a)&(b))
#define sca_neg(a)   (a)
#define sca_get(a)   ((uint8_t)(a))

#endif
