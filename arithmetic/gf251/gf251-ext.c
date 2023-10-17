#include "gf251.h"
#include <stdlib.h>
#include <string.h>

#include "gf251-internal.h"

static int memeq_ct(const uint8_t* a, const uint8_t* b, uint16_t n) {
    int res = 1;
    for(uint16_t i=0; i<n; i++)
        res &= (a[i] == b[i]);
    return res;
}

/*************************************************/
/***********      FIELD EXTENSION      ***********/
/*************************************************/

#define SET_RES_REDUCE8(i,exp) {v=exp; res[i]=v-PRIME*(PRIME<=v);}
#define SET_RES_REDUCE16(i,exp) {v=exp; res[i]=_gf251_reduce16(v);}

// X2 - 2 = 0
void gf251to2_mul(uint8_t res[2], const uint8_t a[2], const uint8_t b[2]) {
    uint16_t leading = a[1]*b[1];
    uint16_t cnst = a[0]*b[0];
    uint32_t v;
    v = (uint32_t) (a[0]+a[1])*(b[0]+b[1])+2*PRIME*PRIME-leading-cnst;
    res[1] = _gf251_reduce32(v);
    v = (uint32_t) 2*leading+cnst;
    res[0] = _gf251_reduce32(v);
}

void gf251to2_add(uint8_t res[2], const uint8_t a[2], const uint8_t b[2]) {
    uint16_t v;
    SET_RES_REDUCE8(0, a[0]+b[0]);
    SET_RES_REDUCE8(1, a[1]+b[1]);
}

void gf251to2_sub(uint8_t res[2], const uint8_t a[2], const uint8_t b[2]) {
    uint16_t v;
    SET_RES_REDUCE8(0, a[0]+PRIME-b[0]);
    SET_RES_REDUCE8(1, a[1]+PRIME-b[1]);
}

int gf251to2_eq(const uint8_t a[2], const uint8_t b[2]) {
    return memeq_ct(a, b, 2);
}

void gf251to2_mul_gf251(uint8_t res[2], uint8_t a, const uint8_t b[2]) {
    uint16_t v;
    SET_RES_REDUCE16(0, a * b[0]);
    SET_RES_REDUCE16(1, a * b[1]);
}

static void gf251to2_mul_0x0101(uint8_t res[2], const uint8_t a[2]) {
    uint16_t v;
    SET_RES_REDUCE16(0, 2*a[1]+a[0]);
    SET_RES_REDUCE16(1, a[0]+a[1]);
}

// X2 - (256*1+1) = 0
void gf251to4_mul(uint8_t res[4], const uint8_t a[4], const uint8_t b[4]) {
    uint8_t leading[2], cnst[2], sum_a[2], sum_b[2];
    gf251to2_mul(leading, &a[2], &b[2]);
    gf251to2_mul(cnst, &a[0], &b[0]);
    gf251to2_add(sum_a, &a[0], &a[2]);
    gf251to2_add(sum_b, &b[0], &b[2]);
    // Compute &res[0]
    gf251to2_mul_0x0101(&res[0],leading);
    gf251to2_add(&res[0], &res[0], cnst);
    // Compute &res[2]
    gf251to2_mul(&res[2], sum_a, sum_b);
    gf251to2_sub(&res[2], &res[2], cnst);
    gf251to2_sub(&res[2], &res[2], leading);
}

/* Optimized implementation of GF(251^4)
 *   Tower implementation of GF(251^4):
 *         GF(251) -> GF(251^2): X^2 - 2 = 0
 *       GF(251^2) -> GF(251^4): Y^2 - (X+1) = 0
 * 
 *  Complexity:
 *     9 multiplications + 4 doubles
 *        + 19 additions + 8 substractions + 4 reductions (on 32 bits)
 */
void gf251to4_mul_alt(uint8_t res[4], const uint8_t a[4], const uint8_t b[4]) {
    uint16_t m_ab0 = a[0]*b[0]; // <= p^2
    uint16_t m_ab1 = a[1]*b[1]; // <= p^2
    uint16_t m_ab2 = a[2]*b[2]; // <= p^2
    uint16_t m_ab3 = a[3]*b[3]; // <= p^2
    uint16_t a_a01 = a[0]+a[1]; // <= 2p
    uint16_t a_a23 = a[2]+a[3]; // <= 2p
    uint16_t a_a02 = a[0]+a[2]; // <= 2p
    uint16_t a_a13 = a[1]+a[3]; // <= 2p
    uint16_t a_b01 = b[0]+b[1]; // <= 2p
    uint16_t a_b23 = b[2]+b[3]; // <= 2p
    uint16_t a_b02 = b[0]+b[2]; // <= 2p
    uint16_t a_b13 = b[1]+b[3]; // <= 2p
    
    uint32_t cnst0 = 2*m_ab1 + m_ab0; // <= 3 p^2
    uint32_t cnst1 = a_a01*a_b01 + 2*PRIME*PRIME - m_ab1 - m_ab0; // <= 6 p^2
    uint32_t leading0 = 2*m_ab3 + m_ab2; // <= 3 p^2
    uint32_t leading1 = a_a23*a_b23 + 2*PRIME*PRIME - m_ab3 - m_ab2; // <= 6 p^2
    uint32_t cnst_sum = a_a02*a_b02; // <= 4 p^2
    uint32_t leading_sum = a_a13*a_b13; // <= 4 p^2
    uint32_t a_a01234 = a_a01 + a_a23; // <= 4 p
    uint32_t a_b01234 = a_b01 + a_b23; // <= 4 p

    uint32_t a_cnstleading0 = leading0 + cnst0; // <= 6 p^2
    uint32_t a_cnstleading1 = leading1 + cnst1; // <= 12 p^2

    uint32_t res0 = 2*leading1 + a_cnstleading0; // <= 18 p^2
    uint32_t res1 = a_cnstleading1 + leading0; // <= 15 p^2
    uint32_t res2 = 2*leading_sum + cnst_sum + 6*PRIME*PRIME - a_cnstleading0; // <= 18 p^2
    uint32_t res3 = a_a01234*a_b01234 + 20*PRIME*PRIME - leading_sum - cnst_sum - a_cnstleading1; // <= 36 p^2

    res[0] = _gf251_reduce32(res0);
    res[1] = _gf251_reduce32(res1);
    res[2] = _gf251_reduce32(res2);
    res[3] = _gf251_reduce32(res3);
}

void gf251to4_add(uint8_t res[4], const uint8_t a[4], const uint8_t b[4]) {
    uint16_t v;
    SET_RES_REDUCE8(0, a[0]+b[0]);
    SET_RES_REDUCE8(1, a[1]+b[1]);
    SET_RES_REDUCE8(2, a[2]+b[2]);
    SET_RES_REDUCE8(3, a[3]+b[3]);
}

void gf251to4_sub(uint8_t res[4], const uint8_t a[4], const uint8_t b[4]) {
    uint16_t v;
    SET_RES_REDUCE8(0, a[0]+PRIME-b[0]);
    SET_RES_REDUCE8(1, a[1]+PRIME-b[1]);
    SET_RES_REDUCE8(2, a[2]+PRIME-b[2]);
    SET_RES_REDUCE8(3, a[3]+PRIME-b[3]);
}

void gf251to4_mul_gf251(uint8_t res[4], uint8_t a, const uint8_t b[4]) {
    uint16_t v;
    SET_RES_REDUCE16(0, a * b[0]);
    SET_RES_REDUCE16(1, a * b[1]);
    SET_RES_REDUCE16(2, a * b[2]);
    SET_RES_REDUCE16(3, a * b[3]);
}

int gf251to4_eq(const uint8_t a[4], const uint8_t b[4]) {
    return memeq_ct(a, b, 4);
}

// X5 - 3 = 0
void gf251to5_mul(uint8_t res[5], const uint8_t a[5], const uint8_t b[5]) {
    uint32_t inter[9] = {0};
    unsigned int i, j;
    for(i=0; i<5; i++)
        for(j=0; j<5; j++)
            inter[i+j] += a[i]*b[j];
    for(i=5; i<9; i++)
        inter[i-5] += 3*inter[i];
    res[0] = _gf251_reduce32(inter[0]);
    res[1] = _gf251_reduce32(inter[1]);
    res[2] = _gf251_reduce32(inter[2]);
    res[3] = _gf251_reduce32(inter[3]);
    res[4] = _gf251_reduce32(inter[4]);
}

void gf251to5_add(uint8_t res[5], const uint8_t a[5], const uint8_t b[5]) {
    uint16_t v;
    SET_RES_REDUCE8(0, a[0]+b[0]);
    SET_RES_REDUCE8(1, a[1]+b[1]);
    SET_RES_REDUCE8(2, a[2]+b[2]);
    SET_RES_REDUCE8(3, a[3]+b[3]);
    SET_RES_REDUCE8(4, a[4]+b[4]);
}

void gf251to5_sub(uint8_t res[5], const uint8_t a[5], const uint8_t b[5]) {
    uint16_t v;
    SET_RES_REDUCE8(0, a[0]+PRIME-b[0]);
    SET_RES_REDUCE8(1, a[1]+PRIME-b[1]);
    SET_RES_REDUCE8(2, a[2]+PRIME-b[2]);
    SET_RES_REDUCE8(3, a[3]+PRIME-b[3]);
    SET_RES_REDUCE8(4, a[4]+PRIME-b[4]);
}

void gf251to5_mul_gf251(uint8_t res[5], uint8_t a, const uint8_t b[5]) {
    uint16_t v;
    SET_RES_REDUCE16(0, a * b[0]);
    SET_RES_REDUCE16(1, a * b[1]);
    SET_RES_REDUCE16(2, a * b[2]);
    SET_RES_REDUCE16(3, a * b[3]);
    SET_RES_REDUCE16(4, a * b[4]);
}

int gf251to5_eq(const uint8_t a[5], const uint8_t b[5]) {
    return memeq_ct(a, b, 5);
}

// X7 - X - 1 = 0
void gf251to7_mul(uint8_t res[7], const uint8_t a[7], const uint8_t b[7]) {
    uint32_t inter[13] = {0};
    unsigned int i, j;
    for(i=0; i<7; i++)
        for(j=0; j<7; j++)
            inter[i+j] += a[i]*b[j];
    for(i=12; i>=7; i--) {
        inter[i-7] += inter[i];
        inter[i-6] += inter[i];
    }
    res[0] = _gf251_reduce32(inter[0]);
    res[1] = _gf251_reduce32(inter[1]);
    res[2] = _gf251_reduce32(inter[2]);
    res[3] = _gf251_reduce32(inter[3]);
    res[4] = _gf251_reduce32(inter[4]);
    res[5] = _gf251_reduce32(inter[5]);
    res[6] = _gf251_reduce32(inter[6]);
}

void gf251to7_add(uint8_t res[7], const uint8_t a[7], const uint8_t b[7]) {
    uint16_t v;
    SET_RES_REDUCE8(0, a[0]+b[0]);
    SET_RES_REDUCE8(1, a[1]+b[1]);
    SET_RES_REDUCE8(2, a[2]+b[2]);
    SET_RES_REDUCE8(3, a[3]+b[3]);
    SET_RES_REDUCE8(4, a[4]+b[4]);
    SET_RES_REDUCE8(5, a[5]+b[5]);
    SET_RES_REDUCE8(6, a[6]+b[6]);
}

void gf251to7_sub(uint8_t res[7], const uint8_t a[7], const uint8_t b[7]) {
    uint16_t v;
    SET_RES_REDUCE8(0, a[0]+PRIME-b[0]);
    SET_RES_REDUCE8(1, a[1]+PRIME-b[1]);
    SET_RES_REDUCE8(2, a[2]+PRIME-b[2]);
    SET_RES_REDUCE8(3, a[3]+PRIME-b[3]);
    SET_RES_REDUCE8(4, a[4]+PRIME-b[4]);
    SET_RES_REDUCE8(5, a[5]+PRIME-b[5]);
    SET_RES_REDUCE8(6, a[6]+PRIME-b[6]);
}

void gf251to7_mul_gf251(uint8_t res[7], uint8_t a, const uint8_t b[7]) {
    uint16_t v;
    SET_RES_REDUCE16(0, a * b[0]);
    SET_RES_REDUCE16(1, a * b[1]);
    SET_RES_REDUCE16(2, a * b[2]);
    SET_RES_REDUCE16(3, a * b[3]);
    SET_RES_REDUCE16(4, a * b[4]);
    SET_RES_REDUCE16(5, a * b[5]);
    SET_RES_REDUCE16(6, a * b[6]);
}

int gf251to7_eq(const uint8_t a[7], const uint8_t b[7]) {
    return memeq_ct(a, b, 7);
}

// X2 - ... = 0
void gf251to8_mul(uint8_t res[8], const uint8_t a[8], const uint8_t b[8]) {
    uint8_t leading[4], cnst[4], sum_a[4], sum_b[4];
    gf251to4_mul(leading, &a[4], &b[4]);
    gf251to4_mul(cnst, &a[0], &b[0]);
    gf251to4_add(sum_a, &a[0], &a[4]);
    gf251to4_add(sum_b, &b[0], &b[4]);
    // Compute &res[0]
    const uint8_t factor[4] = {1, 0, 0, 1};
    gf251to4_mul(&res[0], leading, factor);
    gf251to4_add(&res[0], &res[0], cnst);
    // Compute &res[4]
    gf251to4_mul(&res[4], sum_a, sum_b);
    gf251to4_sub(&res[4], &res[4], cnst);
    gf251to4_sub(&res[4], &res[4], leading);
}

void gf251to8_add(uint8_t res[8], const uint8_t a[8], const uint8_t b[8]) {
    gf251to4_add(&res[0], &a[0], &b[0]);
    gf251to4_add(&res[4], &a[4], &b[4]);
}

void gf251to8_sub(uint8_t res[8], const uint8_t a[8], const uint8_t b[8]) {
    gf251to4_sub(&res[0], &a[0], &b[0]);
    gf251to4_sub(&res[4], &a[4], &b[4]);
}

int gf251to8_eq(const uint8_t a[8], const uint8_t b[8]) {
    return memeq_ct(a, b, 8);
}

void gf251to8_mul_gf251(uint8_t res[8], uint8_t a, const uint8_t b[8]) {
    res[0] = gf251_mul(a, b[0]);
    res[1] = gf251_mul(a, b[1]);
    res[2] = gf251_mul(a, b[2]);
    res[3] = gf251_mul(a, b[3]);
    res[4] = gf251_mul(a, b[4]);
    res[5] = gf251_mul(a, b[5]);
    res[6] = gf251_mul(a, b[6]);
    res[7] = gf251_mul(a, b[7]);
}

// X3 - ... = 0
void gf251to12_mul(uint8_t res[12], const uint8_t a[12], const uint8_t b[12]) {
    // (extended) Karatsuba algorithm
    uint8_t p0[4], p1[4], p2[4];
    gf251to4_mul(p0, &a[0], &b[0]);
    gf251to4_mul(p1, &a[4], &b[4]);
    gf251to4_mul(p2, &a[8], &b[8]);

    uint8_t a01[4], a12[4], a02[4];
    uint8_t b01[4], b12[4], b02[4];
    gf251to4_add(a01, &a[0], &a[4]);
    gf251to4_add(a12, &a[4], &a[8]);
    gf251to4_add(a02, &a[8], &a[0]);
    gf251to4_add(b01, &b[0], &b[4]);
    gf251to4_add(b12, &b[4], &b[8]);
    gf251to4_add(b02, &b[8], &b[0]);

    uint8_t p01[4], p12[4], p02[4];
    gf251to4_mul(p01, a01, b01);
    gf251to4_mul(p02, a02, b02);
    gf251to4_mul(p12, a12, b12);

    //uint8_t a012[4], b012[4], p012[4];
    //gf251to4_add(a012, a01, &a[8]);
    //gf251to4_add(b012, b01, &b[8]);
    //gf251to4_mul(p012, a012, b012);

    const uint8_t factor[4] = {1, 0, 0, 1};

    gf251to4_add(&res[0], p1, p2);
    gf251to4_sub(&res[0], p12, &res[0]);
    gf251to4_mul(&res[0], &res[0], factor);
    gf251to4_add(&res[0], &res[0], p0);

    gf251to4_mul(&res[4], p2, factor);
    gf251to4_add(&res[4], &res[4], p01);
    gf251to4_sub(&res[4], &res[4], p0);
    gf251to4_sub(&res[4], &res[4], p1);

    gf251to4_sub(&res[8], p02, p0);
    gf251to4_add(&res[8], &res[8], p1);
    gf251to4_sub(&res[8], &res[8], p2);
}

void gf251to12_add(uint8_t res[12], const uint8_t a[12], const uint8_t b[12]) {
    gf251to4_add(&res[0], &a[0], &b[0]);
    gf251to4_add(&res[4], &a[4], &b[4]);
    gf251to4_add(&res[8], &a[8], &b[8]);
}

void gf251to12_sub(uint8_t res[12], const uint8_t a[12], const uint8_t b[12]) {
    gf251to4_sub(&res[0], &a[0], &b[0]);
    gf251to4_sub(&res[4], &a[4], &b[4]);
    gf251to4_sub(&res[8], &a[8], &b[8]);
}

const uint8_t gf251to12_pow251_tabA[16] = {
    1, 0, 0, 0,
    0, 250, 0, 0,
    0, 0, 160, 171,
    0, 0, 160, 91
};
const uint8_t gf251to12_pow251_tabB[16] = {
    2, 32, 175, 187,
    187, 249, 128, 76,
    51, 195, 220, 191,
    112, 200, 120, 31
};
const uint8_t gf251to12_pow251_tabC[16] = {
    224, 154, 175, 56,
    194, 27, 139, 76,
    175, 195, 156, 194,
    112, 76, 114, 95
};
void gf251to12_pow251(uint8_t res[12], const uint8_t a[12]) {
    uint8_t tmp[12];
    memset(tmp, 0, 12);
    gf251_matcols_muladd(&tmp[0], &a[0], gf251to12_pow251_tabA, 4, 4);
    gf251_matcols_muladd(&tmp[4], &a[8], gf251to12_pow251_tabB, 4, 4);
    gf251_matcols_muladd(&tmp[8], &a[4], gf251to12_pow251_tabC, 4, 4);
    // We work on a copy in case than "res" and "a"
    //   point to the same memory block.
    memcpy(res, tmp, 12);
}

void gf251to12_inv(uint8_t res[12], const uint8_t a[12]) {
    // Compute a_pow_249
    uint8_t a_pow_249[12];
    memcpy(a_pow_249, a, 12);
    gf251to12_mul(a_pow_249, a_pow_249, a_pow_249); // a^2
    gf251to12_mul(a_pow_249, a_pow_249, a); // a^3
    gf251to12_mul(a_pow_249, a_pow_249, a_pow_249); // a^6
    gf251to12_mul(a_pow_249, a_pow_249, a); // a^7
    gf251to12_mul(a_pow_249, a_pow_249, a_pow_249); // a^14
    gf251to12_mul(a_pow_249, a_pow_249, a); // a^15
    gf251to12_mul(a_pow_249, a_pow_249, a_pow_249); // a^30
    gf251to12_mul(a_pow_249, a_pow_249, a); // a^31
    gf251to12_mul(a_pow_249, a_pow_249, a_pow_249); // a^62
    gf251to12_mul(a_pow_249, a_pow_249, a_pow_249); // a^124
    gf251to12_mul(a_pow_249, a_pow_249, a_pow_249); // a^248
    gf251to12_mul(a_pow_249, a_pow_249, a); // a^249

    // Deduce a_pow_250
    uint8_t a_pow_250[12];
    gf251to12_mul(a_pow_250, a_pow_249, a); // a^250

    // Compute the desired result: res = a^249 * (a^250)^{sum_{i>0}^11 251^i}
    memcpy(res, a_pow_250, 12);
    for(int i=0; i<10; i++) {
        gf251to12_pow251(res, res);
        gf251to12_mul(res, res, a_pow_250);
    }
    gf251to12_pow251(res, res);
    gf251to12_mul(res, res, a_pow_249);
}

int gf251to12_eq(const uint8_t a[12], const uint8_t b[12]) {
    return memeq_ct(a, b, 12);
}

// X2 - ... = 0
void gf251to16_mul(uint8_t res[16], const uint8_t a[16], const uint8_t b[16]) {
    uint8_t leading[8], cnst[8], sum_a[8], sum_b[8];
    gf251to8_mul(leading, &a[8], &b[8]);
    gf251to8_mul(cnst, &a[0], &b[0]);
    gf251to8_add(sum_a, &a[0], &a[8]);
    gf251to8_add(sum_b, &b[0], &b[8]);
    // Compute &res[0]
    const uint8_t factor[8] = {1, 0, 0, 0, 0, 0, 0, 1};
    gf251to8_mul(&res[0], leading, factor);
    gf251to8_add(&res[0], &res[0], cnst);
    // Compute &res[8]
    gf251to8_mul(&res[8], sum_a, sum_b);
    gf251to8_sub(&res[8], &res[8], cnst);
    gf251to8_sub(&res[8], &res[8], leading);
}

void gf251to16_add(uint8_t res[16], const uint8_t a[16], const uint8_t b[16]) {
    gf251to8_add(&res[0], &a[0], &b[0]);
    gf251to8_add(&res[8], &a[8], &b[8]);
}

void gf251to16_sub(uint8_t res[16], const uint8_t a[16], const uint8_t b[16]) {
    gf251to8_sub(&res[0], &a[0], &b[0]);
    gf251to8_sub(&res[8], &a[8], &b[8]);
}

const uint8_t gf251to16_pow251_tabA[16] = {
    1, 0, 0, 0,
    0, 250, 0, 0,
    0, 0, 160, 171,
    0, 0, 160, 91
};
const uint8_t gf251to16_pow251_tabB[16] = {
    50, 16, 117, 191,
    219, 201, 120, 134,
    189, 73, 169, 66,
    105, 62, 119, 82
};
const uint8_t gf251to16_pow251_tabC[256] = {
    73, 248, 210, 210, 91, 80, 72, 164,
    6, 178, 82, 41, 91, 160, 174, 179,
    217, 234, 112, 206, 136, 238, 3, 249,
    34, 34, 90, 139, 26, 115, 4, 248,
    98, 36, 170, 188, 7, 59, 17, 93,
    179, 153, 126, 81, 133, 244, 65, 234,
    211, 46, 131, 179, 71, 105, 214, 95,
    159, 40, 144, 120, 41, 180, 61, 37
};
void gf251to16_pow251(uint8_t res[16], const uint8_t a[16]) {
    uint8_t tmp[16];
    memset(tmp, 0, 16);
    gf251_matcols_muladd(&tmp[0], &a[0], gf251to16_pow251_tabA, 4, 4);
    gf251_matcols_muladd(&tmp[4], &a[4], gf251to16_pow251_tabB, 4, 4);
    gf251_matcols_muladd(&tmp[8], &a[8], gf251to16_pow251_tabC, 8, 8);
    // We work on a copy in case than "res" and "a"
    //   point to the same memory block.
    memcpy(res, tmp, 16);
}

void gf251to16_inv(uint8_t res[16], const uint8_t a[16]) {
    // Compute a_pow_249
    uint8_t a_pow_249[16];
    memcpy(a_pow_249, a, 16);
    gf251to16_mul(a_pow_249, a_pow_249, a_pow_249); // a^2
    gf251to16_mul(a_pow_249, a_pow_249, a); // a^3
    gf251to16_mul(a_pow_249, a_pow_249, a_pow_249); // a^6
    gf251to16_mul(a_pow_249, a_pow_249, a); // a^7
    gf251to16_mul(a_pow_249, a_pow_249, a_pow_249); // a^14
    gf251to16_mul(a_pow_249, a_pow_249, a); // a^15
    gf251to16_mul(a_pow_249, a_pow_249, a_pow_249); // a^30
    gf251to16_mul(a_pow_249, a_pow_249, a); // a^31
    gf251to16_mul(a_pow_249, a_pow_249, a_pow_249); // a^62
    gf251to16_mul(a_pow_249, a_pow_249, a_pow_249); // a^124
    gf251to16_mul(a_pow_249, a_pow_249, a_pow_249); // a^248
    gf251to16_mul(a_pow_249, a_pow_249, a); // a^249

    // Deduce a_pow_250
    uint8_t a_pow_250[16];
    gf251to16_mul(a_pow_250, a_pow_249, a); // a^250

    // Compute the desired result: res = a^249 * (a^250)^{sum_{i>0}^15 251^i}
    memcpy(res, a_pow_250, 16);
    for(int i=0; i<14; i++) {
        gf251to16_pow251(res, res);
        gf251to16_mul(res, res, a_pow_250);
    }
    gf251to16_pow251(res, res);
    gf251to16_mul(res, res, a_pow_249);
}

int gf251to16_eq(const uint8_t a[16], const uint8_t b[16]) {
    return memeq_ct(a, b, 16);
}
