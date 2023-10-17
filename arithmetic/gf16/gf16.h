#ifndef LIBMPCITH_GF16_H
#define LIBMPCITH_GF16_H

#include <stdint.h>

uint8_t gf16_add(uint8_t a, uint8_t b);
uint8_t gf16_mul(uint8_t a, uint8_t b);
extern const uint8_t gf16_inverse_tab[16];

// vz[] += vx[]
void gf16_add_tab(uint8_t* vz, const uint8_t* vx, int bytes);
// vz[] = vx[] * y
void gf16_mul_tab(uint8_t* vz, const uint8_t* vx, uint8_t y, int bytes);
// vz[] += vx[] * y
void gf16_muladd_tab(uint8_t* vz, uint8_t y, const uint8_t* vx, int bytes);
// vz[] = vz[] * y + vx[]
void gf16_mul_and_add_tab(uint8_t* vz, uint8_t y, const uint8_t* vx, int bytes);

// vz[] += vx[1][] * y[1] + ... + vx[nb][] * y[nb]
void gf16_matcols_muladd(uint8_t* vz, const uint8_t* y, const uint8_t* vx, int nb, int bytes);
void gf16_matcols_muladd_scaled(uint8_t* vz, const uint8_t* y, const uint8_t* vx, int nb, int bytes, int scale);

void gf16_matmult_rows(uint8_t* mat_C, const uint8_t* mat_A, const uint8_t* mat_B, uint16_t m, uint16_t n, uint16_t p);

void gf16_compress_tab(uint8_t* buf, const uint8_t* v, int bytes);
void gf16_decompress_tab(uint8_t* v, const uint8_t* buf, int bytes);

void gf16to2_mul(uint8_t r[2], const uint8_t a[2], const uint8_t b[2]);
void gf16to2_add(uint8_t r[2], const uint8_t a[2], const uint8_t b[2]);
void gf16to4_mul(uint8_t r[4], const uint8_t a[4], const uint8_t b[4]);
void gf16to4_add(uint8_t r[4], const uint8_t a[4], const uint8_t b[4]);
void gf16to8_mul(uint8_t r[8], const uint8_t a[8], const uint8_t b[8]);
void gf16to8_add(uint8_t r[8], const uint8_t a[8], const uint8_t b[8]);

void gf16to16_mul(uint8_t r[16], const uint8_t a[16], const uint8_t b[16]);
void gf16to16_add(uint8_t r[16], const uint8_t a[16], const uint8_t b[16]);
void gf16to16_pow16(uint8_t res[16], const uint8_t a[16]);
void gf16to16_inv(uint8_t res[16], const uint8_t a[16]);
int gf16to16_eq(const uint8_t a[16], const uint8_t b[16]);

#ifndef NO_RND
#include "sample.h"
void gf16_random_elements(uint8_t points[], uint32_t nb_points, samplable_t* entropy);
void gf16_random_elements_x4(uint8_t* const* points, uint32_t nb_points, samplable_x4_t* entropy);
#endif

#endif /* LIBMPCITH_GF16_H */
