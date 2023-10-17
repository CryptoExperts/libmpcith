#ifndef LIBMPCITH_GF251_H
#define LIBMPCITH_GF251_H

#include <stdint.h>

/*************************************/
/***        Basic Operations       ***/
/*************************************/

// Return "a+b" over GF(251)
uint8_t gf251_add(uint8_t a, uint8_t b);

// Return "a-b" over GF(251)
uint8_t gf251_sub(uint8_t a, uint8_t b);

// Return "a*b" over GF(251)
uint8_t gf251_mul(uint8_t a, uint8_t b);

// Return "-a" over GF(251)
uint8_t gf251_neg(uint8_t a);

// Return x[0]*y[0]+...+x[size-1]*y[size-1] over GF(251)
uint8_t gf251_innerproduct(const uint8_t* x, const uint8_t* y, uint32_t size);

// Inversion Table
extern const uint8_t gf251_inverse_tab[251];

/*************************************/
/***       Batched Operations      ***/
/*************************************/

// vz[] += vx[] over GF(251)
void gf251_add_tab(uint8_t* vz, const uint8_t* vx, uint32_t size);

// vz[] -= vx[] over GF(251)
void gf251_sub_tab(uint8_t* vz, const uint8_t* vx, uint32_t size);

// vz[] = vx[] * y over GF(251)
void gf251_mul_tab(uint8_t* vz, const uint8_t* vx, uint8_t y, uint32_t size);

// vz[] += vx[] * y over GF(251)
void gf251_muladd_tab(uint8_t* vz, uint8_t y, const uint8_t* vx, uint32_t size);

// vz[] = vz[] * y + vx[] over GF(251)
void gf251_mul_and_add_tab(uint8_t* vz, uint8_t y, const uint8_t* vx, uint32_t size);

// vz[] = -vz[] over GF(251)
void gf251_neg_tab(uint8_t* vz, uint32_t size);

// vz[] += sum_i vx[i][] over GF(251)
void gf251_add_many_tab(uint8_t* vz, uint8_t const* const* vx, uint32_t size, uint32_t nb_tabs);

/*************************************/
/***        Matrix Operations      ***/
/*************************************/

// vz[] += vx[][1] * y[1] + ... + vx[][nb] * y[nb]
void gf251_matcols_muladd(uint8_t* vz, const uint8_t* y, const uint8_t* vx, uint32_t nb, uint32_t size);

// vz[] += vx[1][] * y[1] + ... + vx[nb][] * y[nb]
void gf251_matrows_muladd(uint8_t* vz, const uint8_t* y, const uint8_t* vx, uint32_t nb, uint32_t size);

// vz[] += vx[][1] * y[1] + ... + vx[][nb] * y[nb], scaled
void gf251_matcols_muladd_scaled(uint8_t* vz, const uint8_t* y, const uint8_t* vx, uint32_t nb, uint32_t size, uint32_t scale);

// vz[] += vx[][1] * y[1] + ... + vx[][nb] * y[nb], size must be a multiple of 16
void gf251_mat16cols_muladd(uint8_t* vz, const uint8_t* y, const uint8_t* vx, uint32_t nb, uint32_t size);

// vz[] += vx[][1] * y[1] + ... + vx[][nb] * y[nb], size must be a multiple of 128
void gf251_mat128cols_muladd(uint8_t* vz, const uint8_t* y, const uint8_t* vx, uint32_t nb, uint32_t size);

/*************************************/
/***        Field Extensions       ***/
/*************************************/

/// Extension Degree 2

// Check if a == b over GF(251^2)
int gf251to2_eq(const uint8_t a[2], const uint8_t b[2]);
// Set res as a+b over GF(251^2)
void gf251to2_add(uint8_t res[2], const uint8_t a[2], const uint8_t b[2]);
// Set res as a-b over GF(251^2)
void gf251to2_sub(uint8_t res[2], const uint8_t a[2], const uint8_t b[2]);
// Set res as a*b over GF(251^2)
void gf251to2_mul(uint8_t res[2], const uint8_t a[2], const uint8_t b[2]);
// Set res as a*b over GF(251^2), a belongs to GF(251)
void gf251to2_mul_gf251(uint8_t res[2], uint8_t a, const uint8_t b[2]);

/// Extension Degree 4

// Check if a == b over GF(251^4)
int gf251to4_eq(const uint8_t a[4], const uint8_t b[4]);
// Set res as a+b over GF(251^4)
void gf251to4_add(uint8_t res[4], const uint8_t a[4], const uint8_t b[4]);
// Set res as a-b over GF(251^4)
void gf251to4_sub(uint8_t res[4], const uint8_t a[4], const uint8_t b[4]);
// Set res as a*b over GF(251^4)
void gf251to4_mul(uint8_t res[4], const uint8_t a[4], const uint8_t b[4]);
// Set res as a*b over GF(251^4), a belongs to GF(251)
void gf251to4_mul_gf251(uint8_t res[4], uint8_t a, const uint8_t b[4]);

/// Extension Degree 5

// Check if a == b over GF(251^5)
int gf251to5_eq(const uint8_t a[5], const uint8_t b[5]);
// Set res as a+b over GF(251^5)
void gf251to5_add(uint8_t res[5], const uint8_t a[5], const uint8_t b[5]);
// Set res as a-b over GF(251^5)
void gf251to5_sub(uint8_t res[5], const uint8_t a[5], const uint8_t b[5]);
// Set res as a*b over GF(251^5)
void gf251to5_mul(uint8_t res[5], const uint8_t a[5], const uint8_t b[5]);
// Set res as a*b over GF(251^5), a belongs to GF(251)
void gf251to5_mul_gf251(uint8_t res[5], uint8_t a, const uint8_t b[5]);

/// Extension Degree 7

// Check if a == b over GF(251^7)
int gf251to7_eq(const uint8_t a[7], const uint8_t b[7]);
// Set res as a+b over GF(251^7)
void gf251to7_add(uint8_t res[7], const uint8_t a[7], const uint8_t b[7]);
// Set res as a-b over GF(251^7)
void gf251to7_sub(uint8_t res[7], const uint8_t a[7], const uint8_t b[7]);
// Set res as a*b over GF(251^7)
void gf251to7_mul(uint8_t res[7], const uint8_t a[7], const uint8_t b[7]);
// Set res as a*b over GF(251^7), a belongs to GF(251)
void gf251to7_mul_gf251(uint8_t res[7], uint8_t a, const uint8_t b[7]);

/// Extension Degree 8

// Check if a == b over GF(251^8)
int gf251to8_eq(const uint8_t a[8], const uint8_t b[8]);
// Set res as a+b over GF(251^8)
void gf251to8_add(uint8_t res[8], const uint8_t a[8], const uint8_t b[8]);
// Set res as a-b over GF(251^8)
void gf251to8_sub(uint8_t res[8], const uint8_t a[8], const uint8_t b[8]);
// Set res as a*b over GF(251^8)
void gf251to8_mul(uint8_t res[8], const uint8_t a[8], const uint8_t b[8]);
// Set res as a*b over GF(251^8), a belongs to GF(251)
void gf251to8_mul_gf251(uint8_t res[8], uint8_t a, const uint8_t b[8]);

/// Extension Degree 12

// Check if a == b over GF(251^12)
int gf251to12_eq(const uint8_t a[12], const uint8_t b[12]);
// Set res as a+b over GF(251^12)
void gf251to12_add(uint8_t res[12], const uint8_t a[12], const uint8_t b[12]);
// Set res as a-b over GF(251^12)
void gf251to12_sub(uint8_t res[12], const uint8_t a[12], const uint8_t b[12]);
// Set res as a*b over GF(251^12)
void gf251to12_mul(uint8_t res[12], const uint8_t a[12], const uint8_t b[12]);
// Set res as a^251 over GF(251^12)
void gf251to12_pow251(uint8_t res[12], const uint8_t a[12]);
// Set res as 1/a over GF(251^12)
void gf251to12_inv(uint8_t res[12], const uint8_t a[12]);

/// Extension Degree 16

// Check if a == b over GF(251^16)
int gf251to16_eq(const uint8_t a[16], const uint8_t b[16]);
// Set res as a+b over GF(251^16)
void gf251to16_add(uint8_t res[16], const uint8_t a[16], const uint8_t b[16]);
// Set res as a-b over GF(251^16)
void gf251to16_sub(uint8_t res[16], const uint8_t a[16], const uint8_t b[16]);
// Set res as a*b over GF(251^16)
void gf251to16_mul(uint8_t res[16], const uint8_t a[16], const uint8_t b[16]);
// Set res as a^251 over GF(251^16)
void gf251to16_pow251(uint8_t res[16], const uint8_t a[16]);
// Set res as 1/a over GF(251^16)
void gf251to16_inv(uint8_t res[16], const uint8_t a[16]);

/*************************************/
/***           Randomness          ***/
/*************************************/

#ifndef NO_RND
#include "sample.h"
// Fill the array "points" with random GF(251) elements
void gf251_random_elements(uint8_t points[], uint32_t nb_points, samplable_t* entropy);
void gf251_random_elements_x4(uint8_t* const* points, uint32_t nb_points, samplable_x4_t* entropy);
#endif

#endif /* LIBMPCITH_GF251_H */
