#include "gf16.h"
#include <stdlib.h>
#include <string.h>

#define MODULUS 0x3

static int memeq_ct(const uint8_t* a, const uint8_t* b, uint16_t n) {
    int res = 1;
    for(uint16_t i=0; i<n; i++)
        res &= (a[i] == b[i]);
    return res;
}

uint8_t gf16_add(uint8_t a, uint8_t b) {
    return a^b;
}

uint8_t gf16_mul(uint8_t a, uint8_t b) {
    uint8_t r;
    r = (-(b>>3    ) & a);
    r = (-(b>>2 & 1) & a) ^ (-(r>>3) & MODULUS) ^ ((r+r) & 0x0F);
    r = (-(b>>1 & 1) & a) ^ (-(r>>3) & MODULUS) ^ ((r+r) & 0x0F);
 return (-(b    & 1) & a) ^ (-(r>>3) & MODULUS) ^ ((r+r) & 0x0F);
}

// Warning, getting the inverse of a secret value using this table,
//   would lead to non-constant-time implementation. Only accessing
//   to public positions is allowed.
const uint8_t gf16_inverse_tab[16] = {
    0, 1, 9, 14, 13, 11, 7, 6, 15, 2, 12, 5, 10, 4, 3, 8
};

// vz[] += vx[]
void gf16_add_tab(uint8_t* vz, const uint8_t* vx, int bytes) {
    for(int i=0; i<bytes; i++)
        vz[i] ^= vx[i];
}

// vz[] = vx[] * y
void gf16_mul_tab(uint8_t* vz, const uint8_t* vx, uint8_t y, int bytes) {
    for(int i=0; i<bytes; i++)
        vz[i] = gf16_mul(vx[i], y);
}

// vz[] += vx[] * y
void gf16_muladd_tab(uint8_t* vz, uint8_t y, const uint8_t* vx, int bytes) {
    for(int i=0; i<bytes; i++)
        vz[i] = vz[i] ^ gf16_mul(vx[i], y);
}

// vz[] = vz[] * y + vx[]
void gf16_mul_and_add_tab(uint8_t* vz, uint8_t y, const uint8_t* vx, int bytes) {
    for(int i=0; i<bytes; i++)
        vz[i] = vx[i] ^ gf16_mul(vz[i], y);
}

// vz[] += vx[1][] * y[1] + ... + vx[nb][] * y[nb]
void gf16_matcols_muladd(uint8_t* vz, const uint8_t* y, const uint8_t* vx, int nb, int bytes) {
    uint8_t* unreduced_vz = (uint8_t*) malloc(bytes);
    memset(unreduced_vz, 0, bytes);
    size_t ind=0;
    for(int j=0; j<nb; j++) {
        for(int i=0; i<bytes; i++) {
            unreduced_vz[i] ^= gf16_mul(vx[ind++], y[j]);
        }
    }
    for(int i=0; i<bytes; i++)
        vz[i] ^= unreduced_vz[i];
    free(unreduced_vz);
}

// vz[] += vx[1][] * y[1] + ... + vx[nb][] * y[nb]
void gf16_matcols_muladd_scaled(uint8_t* vz, const uint8_t* y, const uint8_t* vx, int nb, int bytes, int scale) {
    uint8_t* unreduced_vz = (uint8_t*) malloc(bytes);
    memset(unreduced_vz, 0, bytes);
    size_t ind=0;
    for(int j=0; j<nb; j++) {
        for(int i=0; i<bytes; i++) {
            unreduced_vz[i] ^= gf16_mul(vx[ind++], y[scale*j]);
        }
    }
    for(int i=0; i<bytes; i++)
        vz[scale*i] ^= unreduced_vz[i];
    free(unreduced_vz);
}

// C += A*B
// The matrices are given row by row.
void gf16_matmult_rows(uint8_t* mat_C, const uint8_t* mat_A, const uint8_t* mat_B, uint16_t m, uint16_t n, uint16_t p) {
    const uint8_t* cur_A = mat_A;
    const uint8_t* cur_B;
    uint8_t* cur_C = mat_C;

    uint16_t i, j, k;
    for(i=0; i<m; i++) {
        cur_B = mat_B;
        for(j=0; j<n; j++) {
            uint8_t factor = *cur_A;
            for(k=0; k<p; k++)
                *(cur_C+k) ^= gf16_mul(*(cur_B+k), factor);
            cur_A++;
            cur_B += p;
        }
        cur_C += p;
    }
}

void gf16_compress_tab(uint8_t* buf, const uint8_t* v, int bytes) {
    int i;
    for(i=0; i+1<bytes; i+=2)
        buf[i>>1] = (v[i]<<4) | (v[i+1]);
    if(i != bytes)
        buf[i>>1] = v[i];
}

void gf16_decompress_tab(uint8_t* v, const uint8_t* buf, int bytes) {
    int i;
    for(i=0; i+1<bytes; i+=2) {
        uint8_t value = buf[i>>1];
        v[i] = value>>4;
        v[i+1] = value & 0x0F;
    }
    if(i != bytes)
        v[i] = buf[i>>1];
}

void gf16to2_mul(uint8_t res[2], const uint8_t a[2], const uint8_t b[2]) {
    uint8_t leading = gf16_mul(a[1], b[1]);
    uint8_t cnst = gf16_mul(a[0], b[0]);
    uint8_t sum_a = gf16_add(a[0], a[1]);
    uint8_t sum_b = gf16_add(b[0], b[1]);
    res[0] = gf16_add(
        cnst,
        gf16_mul(leading, 0x8)
    );
    res[1] = gf16_add(
        gf16_mul(sum_a,sum_b),
        cnst
    );
}

void gf16to2_add(uint8_t res[2], const uint8_t a[2], const uint8_t b[2]) {
    res[0] = a[0] ^ b[0];
    res[1] = a[1] ^ b[1];
}

static void gf16to2_mul_0x80(uint8_t res[2], const uint8_t a[2]) {
    res[0] = gf16_mul(a[1], 0xC); // 0xC = 0x8*0x8
    res[1] = gf16_mul(gf16_add(a[0], a[1]), 0x8);
}

void gf16to4_mul(uint8_t res[4], const uint8_t a[4], const uint8_t b[4]) {
    uint8_t leading[2], cnst[2], sum_a[2], sum_b[2];
    gf16to2_mul(leading, &a[2], &b[2]);
    gf16to2_mul(cnst, &a[0], &b[0]);
    gf16to2_add(sum_a, &a[0], &a[2]);
    gf16to2_add(sum_b, &b[0], &b[2]);
    // Compute &res[0]
    gf16to2_mul_0x80(&res[0], leading);
    gf16to2_add(&res[0], &res[0], cnst);
    // Compute &res[2]
    gf16to2_mul(&res[2], sum_a, sum_b);
    gf16to2_add(&res[2], &res[2], cnst);
}

void gf16to4_add(uint8_t res[4], const uint8_t a[4], const uint8_t b[4]) {
    res[0] = a[0] ^ b[0];
    res[1] = a[1] ^ b[1];
    res[2] = a[2] ^ b[2];
    res[3] = a[3] ^ b[3];
}

static void gf16to4_mul_0x8000(uint8_t res[4], const uint8_t a[4]) {
    uint8_t leading[2], sum_a[2];
    gf16to2_mul_0x80(leading, &a[2]);
    gf16to2_add(sum_a, &a[0], &a[2]);
    // Compute &res[0]
    gf16to2_mul_0x80(&res[0], leading);
    // Compute &res[2]
    gf16to2_mul_0x80(&res[2], sum_a);
}

void gf16to8_mul(uint8_t res[8], const uint8_t a[8], const uint8_t b[8]) {
    uint8_t leading[4], cnst[4], sum_a[4], sum_b[4];
    gf16to4_mul(leading, &a[4], &b[4]);
    gf16to4_mul(cnst, &a[0], &b[0]);
    gf16to4_add(sum_a, &a[0], &a[4]);
    gf16to4_add(sum_b, &b[0], &b[4]);
    // Compute &res[0]
    gf16to4_mul_0x8000(&res[0], leading);
    gf16to4_add(&res[0], &res[0], cnst);
    // Compute &res[4]
    gf16to4_mul(&res[4], sum_a, sum_b);
    gf16to4_add(&res[4], &res[4], cnst);
}

void gf16to8_add(uint8_t res[8], const uint8_t a[8], const uint8_t b[8]) {
    res[0] = a[0] ^ b[0];
    res[1] = a[1] ^ b[1];
    res[2] = a[2] ^ b[2];
    res[3] = a[3] ^ b[3];
    res[4] = a[4] ^ b[4];
    res[5] = a[5] ^ b[5];
    res[6] = a[6] ^ b[6];
    res[7] = a[7] ^ b[7];
}

static void gf16to8_mul_0x80000000(uint8_t res[8], const uint8_t a[8]) {
    uint8_t leading[4], sum_a[4];
    gf16to4_mul_0x8000(leading, &a[4]);
    gf16to4_add(sum_a, &a[0], &a[4]);
    // Compute &res[0]
    gf16to4_mul_0x8000(&res[0], leading);
    // Compute &res[2]
    gf16to4_mul_0x8000(&res[4], sum_a);
}

void gf16to16_mul(uint8_t res[16], const uint8_t a[16], const uint8_t b[16]) {
    uint8_t leading[8], cnst[8], sum_a[8], sum_b[8];
    gf16to8_mul(leading, &a[8], &b[8]);
    gf16to8_mul(cnst, &a[0], &b[0]);
    gf16to8_add(sum_a, &a[0], &a[8]);
    gf16to8_add(sum_b, &b[0], &b[8]);
    // Compute &res[0]
    gf16to8_mul_0x80000000(&res[0], leading);
    gf16to8_add(&res[0], &res[0], cnst);
    // Compute &res[8]
    gf16to8_mul(&res[8], sum_a, sum_b);
    gf16to8_add(&res[8], &res[8], cnst);
}

void gf16to16_add(uint8_t res[16], const uint8_t a[16], const uint8_t b[16]) {
    res[0] = a[0] ^ b[0];
    res[1] = a[1] ^ b[1];
    res[2] = a[2] ^ b[2];
    res[3] = a[3] ^ b[3];
    res[4] = a[4] ^ b[4];
    res[5] = a[5] ^ b[5];
    res[6] = a[6] ^ b[6];
    res[7] = a[7] ^ b[7];
    res[8] = a[8] ^ b[8];
    res[9] = a[9] ^ b[9];
    res[10] = a[10] ^ b[10];
    res[11] = a[11] ^ b[11];
    res[12] = a[12] ^ b[12];
    res[13] = a[13] ^ b[13];
    res[14] = a[14] ^ b[14];
    res[15] = a[15] ^ b[15];
}

const uint8_t gf16to16_pow16_array[16*16] = {
    1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
    9,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,
    11,8,1,1,1,0,0,0,0,0,0,0,0,0,0,0,
    7,11,9,1,1,1,0,0,0,0,0,0,0,0,0,0,
    11,11,3,8,1,1,1,0,0,0,0,0,0,0,0,0,
    12,11,15,3,9,1,1,1,0,0,0,0,0,0,0,0,
    7,6,12,3,11,8,1,1,1,0,0,0,0,0,0,0,
    2,7,7,12,7,11,9,1,1,1,0,0,0,0,0,0,
    5,6,12,9,11,11,3,8,1,1,1,0,0,0,0,0,
    0,5,8,12,12,11,15,3,9,1,1,1,0,0,0,0,
    5,10,9,15,3,14,4,3,11,8,1,1,1,0,0,0,
    10,5,8,9,10,3,15,4,7,11,9,1,1,1,0,0,
    2,0,4,12,13,14,8,9,11,11,3,8,1,1,1,0,
    2,2,14,4,4,13,12,8,12,11,15,3,9,1,1,1
};

void gf16to16_pow16(uint8_t res[16], const uint8_t a[16]) {
    for(int i=0; i<16; i++) {
        uint8_t v = 0;
        for(int j=i; j<16; j++)
            v ^= gf16_mul(a[j], gf16to16_pow16_array[16*j+i]);
        // We set res[.] only after in case than "res" and "a"
        //   point to the same memory block.
        res[i] = v;
    }
}

void gf16to16_inv(uint8_t res[16], const uint8_t a[16]) {
    // Compute a_pow_14
    uint8_t a_pow_14[16];
    memcpy(a_pow_14, a, 16);
    gf16to16_mul(a_pow_14, a_pow_14, a_pow_14); // a^2
    gf16to16_mul(a_pow_14, a_pow_14, a); // a^3
    gf16to16_mul(a_pow_14, a_pow_14, a_pow_14); // a^6
    gf16to16_mul(a_pow_14, a_pow_14, a); // a^7
    gf16to16_mul(a_pow_14, a_pow_14, a_pow_14); // a^14

    // Deduce a_pow_15
    uint8_t a_pow_15[16];
    gf16to16_mul(a_pow_15, a_pow_14, a); // a^255

    // Compute the desired result: res = a^14 * (a^15)^{sum_{i>0}^15 16^i}
    memcpy(res, a_pow_15, 16);
    for(int i=0; i<14; i++) {
        gf16to16_pow16(res, res);
        gf16to16_mul(res, res, a_pow_15);
    }
    gf16to16_pow16(res, res);
    gf16to16_mul(res, res, a_pow_14);
}

int gf16to16_eq(const uint8_t a[16], const uint8_t b[16]) {
    return memeq_ct(a, b, 16);
}

#ifndef NO_RND
void gf16_random_elements(uint8_t points[], uint32_t nb_points, samplable_t* entropy) {
    uint32_t nbytes = (nb_points+1)>>1;
    uint8_t* buffer = (uint8_t*)malloc(nbytes);
    byte_sample(entropy, buffer, nbytes);
    uint8_t* cur = buffer;
    uint32_t i;
    for(i=0; i+1<nb_points; i+=2) {
        points[i] = (*cur) & 0x0F;
        points[i+1] = (*cur) >> 4;
        cur++;
    }
    if(i != nb_points)
        points[i] = (*cur) & 0x0F;
    free(buffer);
}

void gf16_random_elements_x4(uint8_t* const* points, uint32_t nb_points, samplable_x4_t* entropy) {
    uint32_t nbytes = (nb_points+1)>>1;
    uint8_t* buffer = (uint8_t*)malloc(4*nbytes);
    uint8_t* ptr_buffer[4] = {
        &buffer[0], &buffer[nbytes], &buffer[2*nbytes], &buffer[3*nbytes]
    };
    byte_sample_x4(entropy, ptr_buffer, nbytes);
    for(unsigned int j=0; j<4; j++) {
        uint8_t* cur = ptr_buffer[j];
        uint32_t i;
        for(i=0; i+1<nb_points; i+=2) {
            points[j][i] = (*cur) & 0x0F;
            points[j][i+1] = (*cur) >> 4;
            cur++;
        }
        if(i != nb_points)
            points[j][i] = (*cur) & 0x0F;
    }
    free(buffer);
}
#endif
