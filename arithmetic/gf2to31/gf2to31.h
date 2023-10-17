#ifndef LIBMPCITH_GF2TO31_H
#define LIBMPCITH_GF2TO31_H

#include <stdint.h>

uint32_t gf2to31_add(uint32_t a, uint32_t b);
uint32_t gf2to31_mul(uint32_t a, uint32_t b);
uint32_t gf2to31_squ(uint32_t a);
uint32_t gf2to31_inv(uint32_t a);

void gf2to31_matcols_muladd(uint8_t* vz, const uint8_t* y, const uint8_t* vx, int nb, int bytes);

void gf2to31_innerproduct_batched(uint32_t* out_batched, uint32_t* a_batched, uint32_t* b, int bytes, int n_batched);
void gf2to31_add_tab(uint32_t* vz, const uint32_t* vx, int bytes);
void gf2to31_mul_tab(uint32_t* vz, const uint32_t* vx, uint32_t y, int bytes);
void gf2to31_squ_tab(uint32_t* vz, int bytes);
void gf2to31_muladd_tab(uint32_t* vz, uint32_t y, const uint32_t* vx, int bytes);
void gf2to31_mul_and_add_tab(uint32_t* vz, uint32_t y, const uint32_t* vx, int bytes);

#ifndef NO_RND
#include "sample.h"
void gf2to31_random_elements(uint8_t points[], uint32_t nb_points, samplable_t* entropy);
void gf2to31_random_elements_x4(uint8_t* const* points, uint32_t nb_points, samplable_x4_t* entropy);
#endif

#endif /* LIBMPCITH_GF2TO31_H */
