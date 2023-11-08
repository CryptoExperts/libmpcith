#ifndef FIELD_RANDOM_H
#define FIELD_RANDOM_H

#include "field.h"
#include "sample.h"

// Extension of the field library
int GF_is_zero(const GF x);
void GF_rnd(GF* x, uint32_t nb, samplable_t* entropy);
void GF_rnd_x4(GF* const* x, uint32_t nb, samplable_x4_t* entropy);

// Vector Arithmetic
typedef uint8_t vec_elt_t; // The base field is GF(2)

static inline void vec_add(vec_elt_t* x, const vec_elt_t* y, uint32_t size) {
    // We assume that all received vectors will be vectors of GF
    for(uint32_t i=0; i<size/sizeof(GF); i++)
        GF_add(((GF*) x)[i], ((GF*) y)[i], ((GF*) x)[i]);
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
    // We assume that all received vectors will be vectors of GF
    GF_rnd((GF*) x, size/sizeof(GF), entropy);
}
#ifdef PARAM_RND_EXPANSION_X4
static inline void vec_rnd_x4(vec_elt_t* const* x, uint32_t size, samplable_x4_t* entropy) {
    // We assume that all received vectors will be vectors of GF
    GF_rnd_x4((GF* const*) x, size/sizeof(GF), entropy);
}
#endif
static inline void vec_to_bytes(uint8_t* buf, const vec_elt_t* x, uint32_t size) {
    // We assume that all received vectors will be vectors of GF
    for(uint32_t i=0; i<size/sizeof(GF); i++)
        GF_to_bytes(((GF*) x)[i], buf+i*(NUMBITS_FIELD>>3));
}
static inline void vec_from_bytes(vec_elt_t* x, const uint8_t* buf, uint32_t size) {
    // We assume that all received vectors will be vectors of GF
    for(uint32_t i=0; i<size/sizeof(GF); i++)
        GF_from_bytes(buf+i*(NUMBITS_FIELD>>3), ((GF*) x)[i]);
}

#define VEC_COMPRESSIBLE
#define get_serialized_size(size)   ((size/sizeof(GF))*(NUMBITS_FIELD>>3))

// Arithmetic over GF(2)
#define sca_zero()   (0)
#define sca_one()    (1)
#define sca_add(a,b) ((a)^(b))
#define sca_sub(a,b) ((a)^(b))
#define sca_mul(a,b) ((a)&(b))
#define sca_neg(a)   (a)
#define sca_get(a)   ((uint8_t)(a))

#endif
