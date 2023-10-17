#ifndef LIBMPCITH_GF256_H
#define LIBMPCITH_GF256_H

#include <stdint.h>

uint8_t gf256_add(uint8_t a, uint8_t b);
uint8_t gf256_mul(uint8_t a, uint8_t b);

// Inversion Table
extern const uint8_t gf256_inverse_tab[256];

void gf256_innerproduct_batched(uint8_t* out_batched, uint8_t* a_batched, uint8_t* b, int bytes, int n_batched);
// vz[] += vx[1][] * y[1] + ... + vx[nb][] * y[nb]
void gf256_matcols_muladd(uint8_t* vz, const uint8_t* y, const uint8_t* vx, int nb, int bytes);
void gf256_matcols_muladd_scaled(uint8_t* vz, const uint8_t* y, const uint8_t* vx, int nb, int bytes, int scale);
void gf256_mat16cols_muladd(uint8_t* vz, const uint8_t* y, const uint8_t* vx, int nb, int bytes);


// vz[] += vx[]
void gf256_add_tab(uint8_t* vz, const uint8_t* vx, int bytes);
// vz[] = vx[] * y
void gf256_mul_tab(uint8_t* vz, const uint8_t* vx, uint8_t y, int bytes);
// vz[] += vx[] * y
void gf256_muladd_tab(uint8_t* vz, uint8_t y, const uint8_t* vx, int bytes);
// vz[] = vz[] * y + vx[]
void gf256_mul_and_add_tab(uint8_t* vz, uint8_t y, const uint8_t* vx, int bytes);

int gf2to16_eq(const uint8_t a[2], const uint8_t b[2]);
void gf2to16_mul(uint8_t res[2], const uint8_t a[2], const uint8_t b[2]);
void gf2to16_add(uint8_t res[2], const uint8_t a[2], const uint8_t b[2]);
void gf2to16_mul_gf256(uint8_t res[2], uint8_t a, const uint8_t b[2]);
#define gf256to2_eq        gf2to16_eq
#define gf256to2_mul       gf2to16_mul
#define gf256to2_add       gf2to16_add
#define gf256to2_mul_gf256 gf2to16_mul_gf256

int gf2to32_eq(const uint8_t a[4], const uint8_t b[4]);
void gf2to32_mul(uint8_t res[4], const uint8_t a[4], const uint8_t b[4]);
void gf2to32_add(uint8_t res[4], const uint8_t a[4], const uint8_t b[4]);
void gf2to32_mul_gf256(uint8_t res[4], uint8_t a, const uint8_t b[4]);
#define gf256to4_eq        gf2to32_eq
#define gf256to4_mul       gf2to32_mul
#define gf256to4_add       gf2to32_add
#define gf256to4_mul_gf256 gf2to32_mul_gf256

int gf2to64_eq(const uint8_t a[8], const uint8_t b[8]);
void gf2to64_mul(uint8_t res[8], const uint8_t a[8], const uint8_t b[8]);
void gf2to64_add(uint8_t res[8], const uint8_t a[8], const uint8_t b[8]);
void gf2to64_mul_gf256(uint8_t res[8], uint8_t a, const uint8_t b[8]);
#define gf256to8_eq        gf2to64_eq
#define gf256to8_mul       gf2to64_mul
#define gf256to8_add       gf2to64_add
#define gf256to8_mul_gf256 gf2to64_mul_gf256

int gf2to96_eq(const uint8_t a[12], const uint8_t b[12]);
void gf2to96_mul(uint8_t res[12], const uint8_t a[12], const uint8_t b[12]);
void gf2to96_add(uint8_t res[12], const uint8_t a[12], const uint8_t b[12]);
void gf2to96_pow256(uint8_t res[12], const uint8_t a[12]);
void gf2to96_inv(uint8_t res[12], const uint8_t a[12]);
#define gf256to12_eq        gf2to96_eq
#define gf256to12_mul       gf2to96_mul
#define gf256to12_add       gf2to96_add
#define gf256to12_pow256    gf2to96_pow256
#define gf256to12_inv       gf2to96_inv

int gf2to128_eq(const uint8_t a[16], const uint8_t b[16]);
void gf2to128_mul(uint8_t res[16], const uint8_t a[16], const uint8_t b[16]);
void gf2to128_add(uint8_t res[16], const uint8_t a[16], const uint8_t b[16]);
void gf2to128_pow256(uint8_t res[16], const uint8_t a[16]);
void gf2to128_inv(uint8_t res[16], const uint8_t a[16]);
#define gf256to16_eq        gf2to128_eq
#define gf256to16_mul       gf2to128_mul
#define gf256to16_add       gf2to128_add
#define gf256to16_pow256    gf2to128_pow256
#define gf256to16_inv       gf2to128_inv

#ifndef NO_RND
#include "sample.h"
void gf256_random_elements(uint8_t points[], uint32_t nb_points, samplable_t* entropy);
void gf256_random_elements_x4(uint8_t* const* points, uint32_t nb_points, samplable_x4_t* entropy);
#endif

#endif /* LIBMPCITH_GF256_H */
