#ifndef FIELD_RANDOM_H
#define FIELD_RANDOM_H

#include "matrix.h"
#include "sample.h"
#include <stdlib.h>
#include <stdio.h>
#include "mpc-struct.h"

// Vector Arithmetic
typedef ff_t vec_elt_t;

static inline void vec_add(vec_elt_t* x, const vec_elt_t* y, uint32_t size) {
    matrix_add(x, y, 2*size, 1);
}
static inline void vec_sub(vec_elt_t* x, const vec_elt_t* y, uint32_t size) {
    matrix_subtract(x, y, 2*size, 1);
}
static inline void vec_neg(vec_elt_t* x, uint32_t size) {
    matrix_negate(x, 2*size, 1);
}
static inline void vec_mul(vec_elt_t* z, vec_elt_t y, const vec_elt_t* x, uint32_t size) {
    // We work on a copy of z, in case z and x are pointers to the same address.
    uint8_t* z_copy = (uint8_t*) malloc(size);
    memset(z_copy, 0, size);
    matrix_add_multiple(z_copy, y, x, 2*size, 1);
    memcpy(z, z_copy, size);
    free(z_copy);
}
static inline void vec_muladd(vec_elt_t* z, vec_elt_t y, const vec_elt_t* x, uint32_t size) {
    matrix_add_multiple(z, y, x, 2*size, 1);
}
static inline void vec_muladd2(vec_elt_t* z, vec_elt_t y, const vec_elt_t* x, uint32_t size) {
    vec_mul(z, y, z, size);
    vec_add(z, x, size);
}
static inline void vec_rnd(vec_elt_t* x, uint32_t size, samplable_t* entropy) {
    switch(size)
    {
        case sizeof(vec_wit_t):
            matrix_init_random(x, PAR_K, 1, entropy);
            x = ((uint8_t*)x) + matrix_bytes_size(PAR_K,1);
            matrix_init_random(x, PAR_R, PAR_N-PAR_R, entropy);            
            break;

        case sizeof(vec_unif_t):
            matrix_init_random(x, PAR_S, PAR_R, entropy);
            break;

        case sizeof(vec_corr_t):
            matrix_init_random(x, PAR_S, PAR_N-PAR_R, entropy);
            break;

        case sizeof(vec_wit_t)+sizeof(vec_unif_t)+sizeof(vec_corr_t):
            // vec_wit_t
            matrix_init_random(x, PAR_K, 1, entropy);
            x = ((uint8_t*)x) + matrix_bytes_size(PAR_K, 1);
            matrix_init_random(x, PAR_R, PAR_N-PAR_R, entropy);
            x = ((uint8_t*)x) + matrix_bytes_size(PAR_R, PAR_N-PAR_R);
            // vec_unif_t
            matrix_init_random(x, PAR_S, PAR_R, entropy);
            x = ((uint8_t*)x) + matrix_bytes_size(PAR_S, PAR_R);
            // vec_corr_t
            matrix_init_random(x, PAR_S, PAR_N-PAR_R, entropy);
            break;

        default:
            printf("Error: unknown type of vector.\n");
            break;
    }
}
#ifdef PARAM_RND_EXPANSION_X4
static inline void vec_rnd_x4(vec_elt_t* const* x, uint32_t size, samplable_x4_t* entropy) {
    void* x_copy[4] = {x[0], x[1], x[2], x[3]};
    switch(size)
    {
        case sizeof(vec_wit_t):
            matrix_init_random_x4((ff_t*const*) x_copy, PAR_K, 1, entropy);
            x_copy[0] = ((uint8_t*)x_copy[0]) + matrix_bytes_size(PAR_K,1);
            x_copy[1] = ((uint8_t*)x_copy[1]) + matrix_bytes_size(PAR_K,1);
            x_copy[2] = ((uint8_t*)x_copy[2]) + matrix_bytes_size(PAR_K,1);
            x_copy[3] = ((uint8_t*)x_copy[3]) + matrix_bytes_size(PAR_K,1);
            matrix_init_random_x4((ff_t*const*)x_copy, PAR_R, PAR_N-PAR_R, entropy);            
            break;

        case sizeof(vec_unif_t):
            matrix_init_random_x4((ff_t*const*)x_copy, PAR_S, PAR_R, entropy);
            break;

        case sizeof(vec_corr_t):
            matrix_init_random_x4((ff_t*const*)x_copy, PAR_S, PAR_N-PAR_R, entropy);
            break;

        case sizeof(vec_wit_t)+sizeof(vec_unif_t)+sizeof(vec_corr_t):
            // vec_wit_t
            matrix_init_random_x4((ff_t*const*)x_copy, PAR_K, 1, entropy);
            x_copy[0] = ((uint8_t*)x_copy[0]) + matrix_bytes_size(PAR_K,1);
            x_copy[1] = ((uint8_t*)x_copy[1]) + matrix_bytes_size(PAR_K,1);
            x_copy[2] = ((uint8_t*)x_copy[2]) + matrix_bytes_size(PAR_K,1);
            x_copy[3] = ((uint8_t*)x_copy[3]) + matrix_bytes_size(PAR_K,1);
            matrix_init_random_x4((ff_t*const*)x_copy, PAR_R, PAR_N-PAR_R, entropy);
            x_copy[0] = ((uint8_t*)x_copy[0]) + matrix_bytes_size(PAR_R, PAR_N-PAR_R);
            x_copy[1] = ((uint8_t*)x_copy[1]) + matrix_bytes_size(PAR_R, PAR_N-PAR_R);
            x_copy[2] = ((uint8_t*)x_copy[2]) + matrix_bytes_size(PAR_R, PAR_N-PAR_R);
            x_copy[3] = ((uint8_t*)x_copy[3]) + matrix_bytes_size(PAR_R, PAR_N-PAR_R);
            // vec_unif_t
            matrix_init_random_x4((ff_t*const*)x_copy, PAR_S, PAR_R, entropy);
            x_copy[0] = ((uint8_t*)x_copy[0]) + matrix_bytes_size(PAR_S, PAR_R);
            x_copy[1] = ((uint8_t*)x_copy[1]) + matrix_bytes_size(PAR_S, PAR_R);
            x_copy[2] = ((uint8_t*)x_copy[2]) + matrix_bytes_size(PAR_S, PAR_R);
            x_copy[3] = ((uint8_t*)x_copy[3]) + matrix_bytes_size(PAR_S, PAR_R);
            // vec_corr_t
            matrix_init_random_x4((ff_t*const*)x_copy, PAR_S, PAR_N-PAR_R, entropy);
            break;

        default:
            printf("Error: unknown type of vector.\n");
            break;
    }
}
#endif
static inline void vec_to_bytes(uint8_t* buf, const vec_elt_t* x, uint32_t size) {
    memcpy(buf, x, size);
}
static inline void vec_from_bytes(vec_elt_t* x, const uint8_t* buf, uint32_t size) {
    memcpy(x, buf, size);
}

// Arithmetic over GF(q)
#define sca_zero()   (0)
#define sca_one()    (1)
static inline uint8_t sca_add(uint8_t a, uint8_t b) {
    return a^b;
}
#define sca_sub(a,b) sca_add(a,b)
#define GF16_MODULUS 0x03
static inline uint8_t sca_mul(uint8_t a, uint8_t b) {
    uint8_t r;
    r = (-(b>>3    ) & a);
    r = (-(b>>2 & 1) & a) ^ (-(r>>3) & GF16_MODULUS) ^ ((r+r) & 0x0F);
    r = (-(b>>1 & 1) & a) ^ (-(r>>3) & GF16_MODULUS) ^ ((r+r) & 0x0F);
 return (-(b    & 1) & a) ^ (-(r>>3) & GF16_MODULUS) ^ ((r+r) & 0x0F);
}
#define sca_neg(a)   (a)
#define sca_get(a)   ((uint8_t)(a))
extern const uint8_t gf16_inverse_tab[16];
#define sca_inverse_tab gf16_inverse_tab

#endif
