#ifndef LIBMPCITH_GF2_H
#define LIBMPCITH_GF2_H

#include <stdint.h>

// z += x
void gf2_add(uint8_t* z, const uint8_t* x, uint16_t nbits);
// z += x*bit
void gf2_setif(uint8_t* z, const uint8_t* x, uint16_t nbits, uint8_t bit);

// C += A*B (matrices stored by cols)
void gf2_matmult_cols(uint8_t* mat_C, const uint8_t* mat_A, const uint8_t* mat_B, uint16_t m, uint16_t n, uint16_t p);
// C += A*B (matrices stored by rows)
void gf2_matmult_rows(uint8_t* mat_C, const uint8_t* mat_A, const uint8_t* mat_B, uint16_t m, uint16_t n, uint16_t p);

void gf2_clean_matrix_by_cols(uint8_t* mat, uint16_t nrows, uint16_t ncols);
void gf2_clean_matrix_by_rows(uint8_t* mat, uint16_t nrows, uint16_t ncols);


#ifndef NO_RND
#include "sample.h"
void gf2_random(uint8_t* x, uint32_t nbits, samplable_t* entropy);
void gf2_random_matrix_by_cols(uint8_t* mat, uint32_t nrows, uint32_t ncols, samplable_t* entropy);
void gf2_random_matrix_by_rows(uint8_t* mat, uint32_t nrows, uint32_t ncols, samplable_t* entropy);

void gf2_random_x4(uint8_t* const* x, uint32_t nbits, samplable_x4_t* entropy);
void gf2_random_matrix_by_cols_x4(uint8_t* const* mat, uint32_t nrows, uint32_t ncols, samplable_x4_t* entropy);
void gf2_random_matrix_by_rows_x4(uint8_t* const* mat, uint32_t nrows, uint32_t ncols, samplable_x4_t* entropy);
#endif

#endif /* LIBMPCITH_GF2_H */
