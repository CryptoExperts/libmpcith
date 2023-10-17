#include "gf256.h"
#include <stdlib.h>
#include <string.h>

static int memeq_ct(const uint8_t* a, const uint8_t* b, uint16_t n) {
    int res = 1;
    for(uint16_t i=0; i<n; i++)
        res &= (a[i] == b[i]);
    return res;
}

/*************************************************/
/***********      FIELD OPERATIONS     ***********/
/*************************************************/

#define MODULUS 0x1B
#define MASK_LSB_PER_BIT ((uint64_t)0x0101010101010101)
#define MASK_MSB_PER_BIT (MASK_LSB_PER_BIT*0x80)
#define MASK_XLSB_PER_BIT (MASK_LSB_PER_BIT*0xFE)

uint8_t gf256_add(uint8_t a, uint8_t b) {
    return a^b;
}

uint8_t gf256_mul(uint8_t a, uint8_t b) {
    uint8_t r;
    r = (-(b>>7    ) & a);
    r = (-(b>>6 & 1) & a) ^ (-(r>>7) & MODULUS) ^ (r+r);
    r = (-(b>>5 & 1) & a) ^ (-(r>>7) & MODULUS) ^ (r+r);
    r = (-(b>>4 & 1) & a) ^ (-(r>>7) & MODULUS) ^ (r+r);
    r = (-(b>>3 & 1) & a) ^ (-(r>>7) & MODULUS) ^ (r+r);
    r = (-(b>>2 & 1) & a) ^ (-(r>>7) & MODULUS) ^ (r+r);
    r = (-(b>>1 & 1) & a) ^ (-(r>>7) & MODULUS) ^ (r+r);
 return (-(b    & 1) & a) ^ (-(r>>7) & MODULUS) ^ (r+r);
}

void gf256_innerproduct_batched(uint8_t* out_batched, uint8_t* a_batched, uint8_t* b, int bytes, int n_batched) {
    for(int j=0; j<n_batched; j++)
        out_batched[j] = 0;
    int ind = 0;
    for(int i=0; i<bytes; i++) {
        for(int j=0; j<n_batched; j++) {
            out_batched[j] ^= gf256_mul(a_batched[ind],b[i]);
            ind++;
        }
    }
}

// vz[] += vx[]
void gf256_add_tab(uint8_t* vz, const uint8_t* vx, int bytes) {
    for(int i=0; i<bytes; i++)
        vz[i] ^= vx[i];
}

// vz[] = vx[] * y
void gf256_mul_tab(uint8_t* vz, const uint8_t* vx, uint8_t y, int bytes) {
    for(int i=0; i<bytes; i++)
        vz[i] = gf256_mul(vx[i], y);
}

// vz[] += vx[] * y
void gf256_muladd_tab(uint8_t* vz, uint8_t y, const uint8_t* vx, int bytes) {
    for(int i=0; i<bytes; i++)
        vz[i] = vz[i] ^ gf256_mul(vx[i], y);
}

// vz[] += vx[1][] * y[1] + ... + vx[nb][] * y[nb]
void gf256_matcols_muladd(uint8_t* vz, const uint8_t* y, const uint8_t* vx, int nb, int bytes) {
    uint32_t* unreduced_vz = (uint32_t*) aligned_alloc(32, ((sizeof(uint32_t)*bytes+31)>>5)<<5);
    memset(unreduced_vz, 0, sizeof(uint32_t)*bytes);
    size_t ind=0;
    for(int j=0; j<nb; j++) {
        for(int i=0; i<bytes; i++) {
            unreduced_vz[i] ^= gf256_mul(vx[ind++], y[j]);
        }
    }
    for(int i=0; i<bytes; i++)
        vz[i] ^= unreduced_vz[i];
    free(unreduced_vz);
}

// vz[] += vx[1][] * y[1] + ... + vx[nb][] * y[nb]
void gf256_matcols_muladd_scaled(uint8_t* vz, const uint8_t* y, const uint8_t* vx, int nb, int bytes, int scale) {
    uint32_t* unreduced_vz = (uint32_t*) aligned_alloc(32, ((sizeof(uint32_t)*bytes+31)>>5)<<5);
    memset(unreduced_vz, 0, sizeof(uint32_t)*bytes);
    size_t ind=0;
    for(int j=0; j<nb; j++) {
        for(int i=0; i<bytes; i++) {
            unreduced_vz[i] ^= gf256_mul(vx[ind++], y[scale*j]);
        }
    }
    for(int i=0; i<bytes; i++)
        vz[scale*i] ^= unreduced_vz[i];
    free(unreduced_vz);
}

// vz[] += vx[1][] * y[1] + ... + vx[nb][] * y[nb]
// bytes must be multiple of 16
void gf256_mat16cols_muladd(uint8_t* vz, const uint8_t* y, const uint8_t* vx, int nb, int bytes) {
    gf256_matcols_muladd(vz, y, vx, nb, bytes);
}

// vz[] = vz[] * y + vx[]
void gf256_mul_and_add_tab(uint8_t* vz, uint8_t y, const uint8_t* vx, int bytes) {
    for(int i=0; i<bytes; i++)
        vz[i] = vx[i] ^ gf256_mul(vz[i], y);
}

// Warning, getting the inverse of a secret value using this table,
//   would lead to non-constant-time implementation. Only accessing
//   to public positions is allowed.
const uint8_t gf256_inverse_tab[256] = {0, 1, 141, 246, 203, 82, 123, 209, 232, 79, 41, 192, 176, 225, 229, 199, 116, 180, 170, 75, 153, 43, 96, 95, 88, 63, 253, 204, 255, 64, 238, 178, 58, 110, 90, 241, 85, 77, 168, 201, 193, 10, 152, 21, 48, 68, 162, 194, 44, 69, 146, 108, 243, 57, 102, 66, 242, 53, 32, 111, 119, 187, 89, 25, 29, 254, 55, 103, 45, 49, 245, 105, 167, 100, 171, 19, 84, 37, 233, 9, 237, 92, 5, 202, 76, 36, 135, 191, 24, 62, 34, 240, 81, 236, 97, 23, 22, 94, 175, 211, 73, 166, 54, 67, 244, 71, 145, 223, 51, 147, 33, 59, 121, 183, 151, 133, 16, 181, 186, 60, 182, 112, 208, 6, 161, 250, 129, 130, 131, 126, 127, 128, 150, 115, 190, 86, 155, 158, 149, 217, 247, 2, 185, 164, 222, 106, 50, 109, 216, 138, 132, 114, 42, 20, 159, 136, 249, 220, 137, 154, 251, 124, 46, 195, 143, 184, 101, 72, 38, 200, 18, 74, 206, 231, 210, 98, 12, 224, 31, 239, 17, 117, 120, 113, 165, 142, 118, 61, 189, 188, 134, 87, 11, 40, 47, 163, 218, 212, 228, 15, 169, 39, 83, 4, 27, 252, 172, 230, 122, 7, 174, 99, 197, 219, 226, 234, 148, 139, 196, 213, 157, 248, 144, 107, 177, 13, 214, 235, 198, 14, 207, 173, 8, 78, 215, 227, 93, 80, 30, 179, 91, 35, 56, 52, 104, 70, 3, 140, 221, 156, 125, 160, 205, 26, 65, 28};

/*************************************************/
/***********      FIELD EXTENSION      ***********/
/*************************************************/

// X2 + X + 32 = 0
void gf2to16_mul(uint8_t res[2], const uint8_t a[2], const uint8_t b[2]) {
    uint8_t leading = gf256_mul(a[1], b[1]);
    uint8_t cnst = gf256_mul(a[0], b[0]);
    uint8_t sum_a = gf256_add(a[0], a[1]);
    uint8_t sum_b = gf256_add(b[0], b[1]);
    res[0] = gf256_add(
        cnst,
        gf256_mul(leading, 0x20)
    );
    res[1] = gf256_add(
        gf256_mul(sum_a,sum_b),
        cnst
    );
}

void gf2to16_add(uint8_t res[2], const uint8_t a[2], const uint8_t b[2]) {
    res[0] = a[0]^b[0];
    res[1] = a[1]^b[1];
}

static void gf2to16_mul_0x2000(uint8_t res[2], const uint8_t a[2]) {
    res[0] = gf256_mul(gf256_mul(a[1], 32), 0x20);
    res[1] = gf256_mul(gf256_add(a[0], a[1]), 0x20);
}

int gf2to16_eq(const uint8_t a[2], const uint8_t b[2]) {
    return memeq_ct(a, b, 2);
}

void gf2to16_mul_gf256(uint8_t res[2], uint8_t a, const uint8_t b[2]) {
    res[0] = gf256_mul(a, b[0]);
    res[1] = gf256_mul(a, b[1]);
}

// X2 + X + (256*32) = 0
void gf2to32_mul(uint8_t res[4], const uint8_t a[4], const uint8_t b[4]) {
    uint8_t leading[2], cnst[2], sum_a[2], sum_b[2];
    gf2to16_mul(leading, &a[2], &b[2]);
    gf2to16_mul(cnst, &a[0], &b[0]);
    gf2to16_add(sum_a, &a[0], &a[2]);
    gf2to16_add(sum_b, &b[0], &b[2]);
    // Compute &res[0]
    gf2to16_mul_0x2000(&res[0], leading);
    gf2to16_add(&res[0], &res[0], cnst);
    // Compute &res[2]
    gf2to16_mul(&res[2], sum_a, sum_b);
    gf2to16_add(&res[2], &res[2], cnst);
}

void gf2to32_add(uint8_t res[4], const uint8_t a[4], const uint8_t b[4]) {
    gf2to16_add(&res[0], &a[0], &b[0]);
    gf2to16_add(&res[2], &a[2], &b[2]);
}

int gf2to32_eq(const uint8_t a[4], const uint8_t b[4]) {
    return memeq_ct(a, b, 4);
}

void gf2to32_mul_gf256(uint8_t res[4], uint8_t a, const uint8_t b[4]) {
    res[0] = gf256_mul(a, b[0]);
    res[1] = gf256_mul(a, b[1]);
    res[2] = gf256_mul(a, b[2]);
    res[3] = gf256_mul(a, b[3]);
}

// X2 + X + (..*256*32) = 0
void gf2to64_mul(uint8_t res[8], const uint8_t a[8], const uint8_t b[8]) {
    uint8_t leading[4], cnst[4], sum_a[4], sum_b[4];
    gf2to32_mul(leading, &a[4], &b[4]);
    gf2to32_mul(cnst, &a[0], &b[0]);
    gf2to32_add(sum_a, &a[0], &a[4]);
    gf2to32_add(sum_b, &b[0], &b[4]);
    // Compute &res[0]
    const uint8_t factor[4] = {0, 0, 0, 0x20};
    gf2to32_mul(&res[0], leading, factor);
    gf2to32_add(&res[0], &res[0], cnst);
    // Compute &res[4]
    gf2to32_mul(&res[4], sum_a, sum_b);
    gf2to32_add(&res[4], &res[4], cnst);
}

void gf2to64_add(uint8_t res[8], const uint8_t a[8], const uint8_t b[8]) {
    gf2to32_add(&res[0], &a[0], &b[0]);
    gf2to32_add(&res[4], &a[4], &b[4]);
}

int gf2to64_eq(const uint8_t a[8], const uint8_t b[8]) {
    return memeq_ct(a, b, 8);
}

void gf2to64_mul_gf256(uint8_t res[8], uint8_t a, const uint8_t b[8]) {
    res[0] = gf256_mul(a, b[0]);
    res[1] = gf256_mul(a, b[1]);
    res[2] = gf256_mul(a, b[2]);
    res[3] = gf256_mul(a, b[3]);
    res[4] = gf256_mul(a, b[4]);
    res[5] = gf256_mul(a, b[5]);
    res[6] = gf256_mul(a, b[6]);
    res[7] = gf256_mul(a, b[7]);
}

// X3 - ... = 0
void gf2to96_mul(uint8_t res[12], const uint8_t a[12], const uint8_t b[12]) {
    // (extended) Karatsuba algorithm
    uint8_t p0[4], p1[4], p2[4];
    gf2to32_mul(p0, &a[0], &b[0]);
    gf2to32_mul(p1, &a[4], &b[4]);
    gf2to32_mul(p2, &a[8], &b[8]);

    uint8_t a01[4], a12[4];//, a02[4];
    uint8_t b01[4], b12[4];//, b02[4];
    gf2to32_add(a01, &a[0], &a[4]);
    gf2to32_add(a12, &a[4], &a[8]);
    //gf2to32_add(a02, &a[8], &a[0]);
    gf2to32_add(b01, &b[0], &b[4]);
    gf2to32_add(b12, &b[4], &b[8]);
    //gf2to32_add(b02, &b[8], &b[0]);

    uint8_t p01[4], p12[4];//, p02[4];
    gf2to32_mul(p01, a01, b01);
    //gf2to32_mul(p02, a02, b02);
    gf2to32_mul(p12, a12, b12);

    uint8_t a012[4], b012[4], p012[4];
    gf2to32_add(a012, a01, &a[8]);
    gf2to32_add(b012, b01, &b[8]);
    gf2to32_mul(p012, a012, b012);

    const uint8_t factor[4] = {0, 0, 0, 0x10};

    gf2to32_add(&res[0], p1, p2);
    gf2to32_add(&res[0], p12, &res[0]);
    gf2to32_mul(&res[0], &res[0], factor);
    gf2to32_add(&res[0], &res[0], p0);

    gf2to32_mul(&res[4], p2, factor);
    gf2to32_add(&res[4], &res[4], p01);
    gf2to32_add(&res[4], &res[4], p0);
    gf2to32_add(&res[4], &res[4], p1);

    gf2to32_add(&res[8], p012, p01);
    gf2to32_add(&res[8], &res[8], p12);
}

void gf2to96_add(uint8_t res[12], const uint8_t a[12], const uint8_t b[12]) {
    gf2to32_add(&res[0], &a[0], &b[0]);
    gf2to32_add(&res[4], &a[4], &b[4]);
    gf2to32_add(&res[8], &a[8], &b[8]);
}

const uint8_t gf2to96_pow256_tabA[16] = {
    1, 0, 0, 0,
    1, 1, 0, 0,
    0, 1, 1, 0,
    32, 0, 1, 1
};
const uint8_t gf2to96_pow256_tabB[16] = {
    66, 68, 242, 150,
    26, 66, 175, 242,
    13, 50, 237, 182,
    23, 13, 220, 237
};
const uint8_t gf2to96_pow256_tabC[16] = {
    196, 255, 127, 149,
    22, 196, 66, 127,
    51, 127, 134, 128,
    74, 51, 45, 134
};

void gf2to96_pow256(uint8_t res[12], const uint8_t a[12]) {
    uint8_t tmp[12];
    memset(tmp, 0, 12);
    gf256_matcols_muladd(&tmp[0], &a[0], gf2to96_pow256_tabA, 4, 4);
    gf256_matcols_muladd(&tmp[4], &a[4], gf2to96_pow256_tabB, 4, 4);
    gf256_matcols_muladd(&tmp[8], &a[8], gf2to96_pow256_tabC, 4, 4);
    // We work on a copy in case than "res" and "a"
    //   point to the same memory block.
    memcpy(res, tmp, 12);
}

void gf2to96_inv(uint8_t res[12], const uint8_t a[12]) {
    // Compute a_pow_254
    uint8_t a_pow_254[12];
    memcpy(a_pow_254, a, 12);
    gf2to96_mul(a_pow_254, a_pow_254, a_pow_254); // a^2
    gf2to96_mul(a_pow_254, a_pow_254, a); // a^3
    gf2to96_mul(a_pow_254, a_pow_254, a_pow_254); // a^6
    gf2to96_mul(a_pow_254, a_pow_254, a); // a^7
    gf2to96_mul(a_pow_254, a_pow_254, a_pow_254); // a^14
    gf2to96_mul(a_pow_254, a_pow_254, a); // a^15
    gf2to96_mul(a_pow_254, a_pow_254, a_pow_254); // a^30
    gf2to96_mul(a_pow_254, a_pow_254, a); // a^31
    gf2to96_mul(a_pow_254, a_pow_254, a_pow_254); // a^62
    gf2to96_mul(a_pow_254, a_pow_254, a); // a^63
    gf2to96_mul(a_pow_254, a_pow_254, a_pow_254); // a^126
    gf2to96_mul(a_pow_254, a_pow_254, a); // a^127
    gf2to96_mul(a_pow_254, a_pow_254, a_pow_254); // a^254

    // Deduce a_pow_255
    uint8_t a_pow_255[12];
    gf2to96_mul(a_pow_255, a_pow_254, a); // a^255

    // Compute the desired result: res = a^254 * (a^255)^{sum_{i>0}^11 256^i}
    memcpy(res, a_pow_255, 12);
    for(int i=0; i<10; i++) {
        gf2to96_pow256(res, res);
        gf2to96_mul(res, res, a_pow_255);
    }
    gf2to96_pow256(res, res);
    gf2to96_mul(res, res, a_pow_254);
}

int gf2to96_eq(const uint8_t a[12], const uint8_t b[12]) {
    return memeq_ct(a, b, 12);
}

// X2 + X + (..*256*32) = 0
void gf2to128_mul(uint8_t res[16], const uint8_t a[16], const uint8_t b[16]) {
    uint8_t leading[8], cnst[8], sum_a[8], sum_b[8];
    gf2to64_mul(leading, &a[8], &b[8]);
    gf2to64_mul(cnst, &a[0], &b[0]);
    gf2to64_add(sum_a, &a[0], &a[8]);
    gf2to64_add(sum_b, &b[0], &b[8]);
    // Compute &res[0]
    const uint8_t factor[8] = {0, 0, 0, 0, 0, 0, 0, 0x20};
    gf2to64_mul(&res[0], leading, factor);
    gf2to64_add(&res[0], &res[0], cnst);
    // Compute &res[8]
    gf2to64_mul(&res[8], sum_a, sum_b);
    gf2to64_add(&res[8], &res[8], cnst);
}

void gf2to128_add(uint8_t res[16], const uint8_t a[16], const uint8_t b[16]) {
    gf2to64_add(&res[0], &a[0], &b[0]);
    gf2to64_add(&res[8], &a[8], &b[8]);
}

int gf2to128_eq(const uint8_t a[16], const uint8_t b[16]) {
    return memeq_ct(a, b, 16);
}

const uint8_t gf2to128_pow256_tab[256] = {
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    32, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    57, 33, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    117, 57, 32, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    32, 56, 25, 33, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    97, 32, 85, 25, 32, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 
    151, 182, 85, 24, 57, 33, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 
    166, 151, 120, 85, 117, 57, 32, 0, 1, 1, 0, 0, 0, 0, 0, 0, 
    230, 66, 239, 227, 32, 56, 25, 33, 0, 1, 1, 0, 0, 0, 0, 0, 
    126, 230, 144, 239, 97, 32, 85, 25, 32, 0, 1, 1, 0, 0, 0, 0, 
    61, 79, 107, 33, 251, 182, 117, 24, 57, 33, 0, 1, 1, 0, 0, 0, 
    30, 61, 39, 107, 202, 251, 88, 117, 117, 57, 32, 0, 1, 1, 0, 0, 
    96, 241, 26, 36, 230, 66, 163, 195, 32, 56, 25, 33, 0, 1, 1, 0, 
    105, 96, 246, 26, 126, 230, 176, 163, 97, 32, 85, 25, 32, 0, 1, 1, 
};

void gf2to128_pow256(uint8_t res[16], const uint8_t a[16]) {
    for(int i=0; i<16; i++) {
        uint8_t v = 0;
        for(int j=i; j<16; j++)
            v ^= gf256_mul(a[j], gf2to128_pow256_tab[16*j+i]);
        // We set res[.] only after in case than "res" and "a"
        //   point to the same memory block.
        res[i] = v;
    }
}

void gf2to128_inv(uint8_t res[16], const uint8_t a[16]) {
    // Compute a_pow_254
    uint8_t a_pow_254[16];
    memcpy(a_pow_254, a, 16);
    gf2to128_mul(a_pow_254, a_pow_254, a_pow_254); // a^2
    gf2to128_mul(a_pow_254, a_pow_254, a); // a^3
    gf2to128_mul(a_pow_254, a_pow_254, a_pow_254); // a^6
    gf2to128_mul(a_pow_254, a_pow_254, a); // a^7
    gf2to128_mul(a_pow_254, a_pow_254, a_pow_254); // a^14
    gf2to128_mul(a_pow_254, a_pow_254, a); // a^15
    gf2to128_mul(a_pow_254, a_pow_254, a_pow_254); // a^30
    gf2to128_mul(a_pow_254, a_pow_254, a); // a^31
    gf2to128_mul(a_pow_254, a_pow_254, a_pow_254); // a^62
    gf2to128_mul(a_pow_254, a_pow_254, a); // a^63
    gf2to128_mul(a_pow_254, a_pow_254, a_pow_254); // a^126
    gf2to128_mul(a_pow_254, a_pow_254, a); // a^127
    gf2to128_mul(a_pow_254, a_pow_254, a_pow_254); // a^254

    // Deduce a_pow_255
    uint8_t a_pow_255[16];
    gf2to128_mul(a_pow_255, a_pow_254, a); // a^255

    // Compute the desired result: res = a^254 * (a^255)^{sum_{i>0}^15 256^i}
    memcpy(res, a_pow_255, 16);
    for(int i=0; i<14; i++) {
        gf2to128_pow256(res, res);
        gf2to128_mul(res, res, a_pow_255);
    }
    gf2to128_pow256(res, res);
    gf2to128_mul(res, res, a_pow_254);
}

#ifndef NO_RND
/*************************************************/
/************        RANDOMNESS       ************/
/*************************************************/

#include "sample.h"

void gf256_random_elements(uint8_t points[], uint32_t nb_points, samplable_t* entropy) {
    byte_sample(entropy, points, nb_points);
}
void gf256_random_elements_x4(uint8_t* const* points, uint32_t nb_points, samplable_x4_t* entropy) {
    byte_sample_x4(entropy, points, nb_points);
}

#endif
