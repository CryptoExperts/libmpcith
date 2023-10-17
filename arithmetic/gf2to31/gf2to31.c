#include "gf2to31.h"
#include <stdlib.h>
#include <string.h>

#define MODULUS 0x00000009

uint32_t gf2to31_add(uint32_t a, uint32_t b) {
    return a^b;
}

uint32_t gf2to31_mul(uint32_t a, uint32_t b) {
    uint32_t r;
    r = (-(b>>30    ) & a);
    r = (-(b>>29 & 1) & a) ^ (-(r>>30) & MODULUS) ^ ((r+r) & 0x7FFFFFFF);
    r = (-(b>>28 & 1) & a) ^ (-(r>>30) & MODULUS) ^ ((r+r) & 0x7FFFFFFF);
    r = (-(b>>27 & 1) & a) ^ (-(r>>30) & MODULUS) ^ ((r+r) & 0x7FFFFFFF);
    r = (-(b>>26 & 1) & a) ^ (-(r>>30) & MODULUS) ^ ((r+r) & 0x7FFFFFFF);
    r = (-(b>>25 & 1) & a) ^ (-(r>>30) & MODULUS) ^ ((r+r) & 0x7FFFFFFF);
    r = (-(b>>24 & 1) & a) ^ (-(r>>30) & MODULUS) ^ ((r+r) & 0x7FFFFFFF);
    r = (-(b>>23 & 1) & a) ^ (-(r>>30) & MODULUS) ^ ((r+r) & 0x7FFFFFFF);
    r = (-(b>>22 & 1) & a) ^ (-(r>>30) & MODULUS) ^ ((r+r) & 0x7FFFFFFF);
    r = (-(b>>21 & 1) & a) ^ (-(r>>30) & MODULUS) ^ ((r+r) & 0x7FFFFFFF);
    r = (-(b>>20 & 1) & a) ^ (-(r>>30) & MODULUS) ^ ((r+r) & 0x7FFFFFFF);
    r = (-(b>>19 & 1) & a) ^ (-(r>>30) & MODULUS) ^ ((r+r) & 0x7FFFFFFF);
    r = (-(b>>18 & 1) & a) ^ (-(r>>30) & MODULUS) ^ ((r+r) & 0x7FFFFFFF);
    r = (-(b>>17 & 1) & a) ^ (-(r>>30) & MODULUS) ^ ((r+r) & 0x7FFFFFFF);
    r = (-(b>>16 & 1) & a) ^ (-(r>>30) & MODULUS) ^ ((r+r) & 0x7FFFFFFF);
    r = (-(b>>15 & 1) & a) ^ (-(r>>30) & MODULUS) ^ ((r+r) & 0x7FFFFFFF);
    r = (-(b>>14 & 1) & a) ^ (-(r>>30) & MODULUS) ^ ((r+r) & 0x7FFFFFFF);
    r = (-(b>>13 & 1) & a) ^ (-(r>>30) & MODULUS) ^ ((r+r) & 0x7FFFFFFF);
    r = (-(b>>12 & 1) & a) ^ (-(r>>30) & MODULUS) ^ ((r+r) & 0x7FFFFFFF);
    r = (-(b>>11 & 1) & a) ^ (-(r>>30) & MODULUS) ^ ((r+r) & 0x7FFFFFFF);
    r = (-(b>>10 & 1) & a) ^ (-(r>>30) & MODULUS) ^ ((r+r) & 0x7FFFFFFF);
    r = (-(b>> 9 & 1) & a) ^ (-(r>>30) & MODULUS) ^ ((r+r) & 0x7FFFFFFF);
    r = (-(b>> 8 & 1) & a) ^ (-(r>>30) & MODULUS) ^ ((r+r) & 0x7FFFFFFF);
    r = (-(b>> 7 & 1) & a) ^ (-(r>>30) & MODULUS) ^ ((r+r) & 0x7FFFFFFF);
    r = (-(b>> 6 & 1) & a) ^ (-(r>>30) & MODULUS) ^ ((r+r) & 0x7FFFFFFF);
    r = (-(b>> 5 & 1) & a) ^ (-(r>>30) & MODULUS) ^ ((r+r) & 0x7FFFFFFF);
    r = (-(b>> 4 & 1) & a) ^ (-(r>>30) & MODULUS) ^ ((r+r) & 0x7FFFFFFF);
    r = (-(b>> 3 & 1) & a) ^ (-(r>>30) & MODULUS) ^ ((r+r) & 0x7FFFFFFF);
    r = (-(b>> 2 & 1) & a) ^ (-(r>>30) & MODULUS) ^ ((r+r) & 0x7FFFFFFF);
    r = (-(b>> 1 & 1) & a) ^ (-(r>>30) & MODULUS) ^ ((r+r) & 0x7FFFFFFF);
 return (-(b     & 1) & a) ^ (-(r>>30) & MODULUS) ^ ((r+r) & 0x7FFFFFFF);
}

uint32_t gf2to31_squ(uint32_t a) {
    uint32_t r = 0;
    r ^= ((-(a>> 0 & 1)) & 0x1);
    r ^= ((-(a>> 1 & 1)) & 0x4);
    r ^= ((-(a>> 2 & 1)) & 0x10);
    r ^= ((-(a>> 3 & 1)) & 0x40);
    r ^= ((-(a>> 4 & 1)) & 0x100);
    r ^= ((-(a>> 5 & 1)) & 0x400);
    r ^= ((-(a>> 6 & 1)) & 0x1000);
    r ^= ((-(a>> 7 & 1)) & 0x4000);
    r ^= ((-(a>> 8 & 1)) & 0x10000);
    r ^= ((-(a>> 9 & 1)) & 0x40000);
    r ^= ((-(a>>10 & 1)) & 0x100000);
    r ^= ((-(a>>11 & 1)) & 0x400000);
    r ^= ((-(a>>12 & 1)) & 0x1000000);
    r ^= ((-(a>>13 & 1)) & 0x4000000);
    r ^= ((-(a>>14 & 1)) & 0x10000000);
    r ^= ((-(a>>15 & 1)) & 0x40000000);
    r ^= ((-(a>>16 & 1)) & 0x12);
    r ^= ((-(a>>17 & 1)) & 0x48);
    r ^= ((-(a>>18 & 1)) & 0x120);
    r ^= ((-(a>>19 & 1)) & 0x480);
    r ^= ((-(a>>20 & 1)) & 0x1200);
    r ^= ((-(a>>21 & 1)) & 0x4800);
    r ^= ((-(a>>22 & 1)) & 0x12000);
    r ^= ((-(a>>23 & 1)) & 0x48000);
    r ^= ((-(a>>24 & 1)) & 0x120000);
    r ^= ((-(a>>25 & 1)) & 0x480000);
    r ^= ((-(a>>26 & 1)) & 0x1200000);
    r ^= ((-(a>>27 & 1)) & 0x4800000);
    r ^= ((-(a>>28 & 1)) & 0x12000000);
    r ^= ((-(a>>29 & 1)) & 0x48000000);
    r ^= ((-(a>>30 & 1)) & 0x20000012);
    return r;
}

uint32_t gf2to31_inv(uint32_t a) {
    uint32_t r = a;
    for(int i=0; i<29; i++)
        r = gf2to31_mul(gf2to31_mul(r, r), a);
    return gf2to31_mul(r, r);
}

// vz[] += vx[1][] * y[1] + ... + vx[nb][] * y[nb]
void gf2to31_matcols_muladd(uint8_t* vz, const uint8_t* y, const uint8_t* vx, int nb, int bytes) {
    uint32_t* unreduced_vz = (uint32_t*) aligned_alloc(32, ((sizeof(uint32_t)*bytes+31)>>5)<<5);
    memset(unreduced_vz, 0, sizeof(uint32_t)*bytes);
    uint32_t ind=0;
    for(int j=0; j<nb; j++) {
        for(int i=0; i<bytes; i++) {
            unreduced_vz[i] ^= gf2to31_mul(((uint32_t*)vx)[ind++], ((uint32_t*)y)[j]);
        }
        //ind = ((ind+31)>>5)<<5;
    }
    for(int i=0; i<bytes; i++)
        ((uint32_t*)vz)[i] ^= unreduced_vz[i];
    free(unreduced_vz);
}

void gf2to31_innerproduct_batched(uint32_t* out_batched, uint32_t* a_batched, uint32_t* b, int bytes, int n_batched) {
    for(int j=0; j<n_batched; j++)
        out_batched[j] = 0;
    int ind = 0;
    for(int i=0; i<bytes; i++) {
        for(int j=0; j<n_batched; j++) {
            out_batched[j] ^= gf2to31_mul(a_batched[ind],b[i]);
            ind++;
        }
    }
}

// vz[] += vx[]
void gf2to31_add_tab(uint32_t* vz, const uint32_t* vx, int bytes) {
    for(int i=0; i<bytes; i++)
        vz[i] ^= vx[i];
}

// vz[] = vx[] * y
void gf2to31_mul_tab(uint32_t* vz, const uint32_t* vx, uint32_t y, int bytes) {
    for(int i=0; i<bytes; i++)
        vz[i] = gf2to31_mul(vx[i], y);
}

// vz[] = vz[]^2
void gf2to31_squ_tab(uint32_t* vz, int bytes) {
    for(int i=0; i<bytes; i++)
        vz[i] = gf2to31_squ(vz[i]);
}

// vz[] += vx[] * y
void gf2to31_muladd_tab(uint32_t* vz, uint32_t y, const uint32_t* vx, int bytes) {
    for(int i=0; i<bytes; i++)
        vz[i] = vz[i] ^ gf2to31_mul(vx[i], y);
}

// vz[] = vz[] * y + vx[]
void gf2to31_mul_and_add_tab(uint32_t* vz, uint32_t y, const uint32_t* vx, int bytes) {
    for(int i=0; i<bytes; i++)
        vz[i] = vx[i] ^ gf2to31_mul(vz[i], y);
}

#ifndef NO_RND
/*************************************************/
/************        RANDOMNESS       ************/
/*************************************************/

#include "sample.h"

void gf2to31_random_elements(uint8_t points[], uint32_t nb_points, samplable_t* entropy) {
    byte_sample(entropy, points, nb_points*sizeof(uint32_t));
    for(unsigned int i=0; i<nb_points; i++)
        ((uint32_t*) points)[i] &= 0x7FFFFFFF;
}
void gf2to31_random_elements_x4(uint8_t* const* points, uint32_t nb_points, samplable_x4_t* entropy) {
    byte_sample_x4(entropy, points, nb_points*sizeof(uint32_t));
    for(unsigned int j=0; j<4; j++)
        for(unsigned int i=0; i<nb_points; i++)
            ((uint32_t*) points[j])[i] &= 0x7FFFFFFF;
}
#endif
