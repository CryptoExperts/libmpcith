#include "gf2.h"

void gf2_add(uint8_t* z, const uint8_t* x, uint16_t nbits) {
    uint16_t nbytes = (nbits+7)>>3;
    for(uint16_t k=0; k<nbytes; k++)
        z[k] ^= x[k];
}

void gf2_setif(uint8_t* z, const uint8_t* x, uint16_t nbits, uint8_t bit) {
    uint16_t nbytes = (nbits+7)>>3;
    for(uint16_t k=0; k<nbytes; k++)
        z[k] ^= x[k]*bit;
}

// C += A*B
// The matrices are given column by column.
void gf2_matmult_cols(uint8_t* mat_C, const uint8_t* mat_A, const uint8_t* mat_B, uint16_t m, uint16_t n, uint16_t p) {
    uint8_t mask = (1<<7);
    const uint8_t* cur_A;
    const uint8_t* cur_B = mat_B;
    uint8_t* cur_C;

    uint16_t m_div8 = (m+7)>>3;

    uint16_t i, j, k;
    for(i=0; i<p; i++) {
        for(j=0; j<n; j++) {
            cur_A = mat_A + j*m_div8;
            cur_C = mat_C + i*m_div8;
            uint8_t bit = ((*cur_B & mask) != 0);
            for(k=0; k<m_div8; k++)
                *(cur_C+k) ^= (*(cur_A+k))*bit;
            mask >>= 1;
            if(mask == 0) {
                mask = (1<<7);
                cur_B++;
            }
        }
        if(mask != (1<<7)) {
            mask = (1<<7);
            cur_B++;
        }
    }
}

// C += A*B
// The matrices are given row by row.
void gf2_matmult_rows(uint8_t* mat_C, const uint8_t* mat_A, const uint8_t* mat_B, uint16_t m, uint16_t n, uint16_t p) {
    uint8_t mask = (1<<7);
    const uint8_t* cur_A = mat_A;
    const uint8_t* cur_B;
    uint8_t* cur_C;

    uint16_t p_div8 = (p+7)>>3;

    uint16_t i, j, k;
    for(i=0; i<m; i++) {
        for(j=0; j<n; j++) {
            cur_B = mat_B + j*p_div8;
            cur_C = mat_C + i*p_div8;
            uint8_t bit = ((*cur_A & mask) != 0);
            for(k=0; k<p_div8; k++)
                *(cur_C+k) ^= (*(cur_B+k))*bit;
            mask >>= 1;
            if(mask == 0) {
                mask = (1<<7);
                cur_A++;
            }
        }
        if(mask != (1<<7)) {
            mask = (1<<7);
            cur_A++;
        }
    }
}

void gf2_clean_matrix_by_cols(uint8_t* mat, uint16_t nrows, uint16_t ncols) {
    uint8_t colsize = ((nrows+7)>>3);
    uint8_t n_added_bits = (colsize<<3) - nrows;
    uint8_t mask = 0xFF<<n_added_bits;
    for(int i=0; i<ncols; i++)
        mat[colsize*(i+1)-1] &= mask;
}

void gf2_clean_matrix_by_rows(uint8_t* mat, uint16_t nrows, uint16_t ncols) {
    uint8_t rowsize = ((ncols+7)>>3);
    uint8_t n_added_bits = (rowsize<<3) - ncols;
    uint8_t mask = 0xFF<<n_added_bits;
    for(int i=0; i<nrows; i++)
        mat[rowsize*(i+1)-1] &= mask;
}

#ifndef NO_RND
void gf2_random(uint8_t* x, uint32_t nbits, samplable_t* entropy) {
    uint32_t nbytes = (nbits+7)>>3;
    byte_sample(entropy, x, nbytes);
    uint8_t n_added_bits = (nbytes<<3) - nbits;
    uint8_t mask = 0xFF<<n_added_bits;
    x[nbytes-1] &= mask;
}

void gf2_random_matrix_by_cols(uint8_t* mat, uint32_t nrows, uint32_t ncols, samplable_t* entropy) {
    uint32_t colsize = ((nrows+7)>>3);
    byte_sample(entropy, mat, colsize*ncols);
    uint8_t n_added_bits = (colsize<<3) - nrows;
    uint8_t mask = 0xFF<<n_added_bits;
    for(unsigned int i=0; i<ncols; i++)
        mat[colsize*(i+1)-1] &= mask;
}

void gf2_random_matrix_by_rows(uint8_t* mat, uint32_t nrows, uint32_t ncols, samplable_t* entropy) {
    uint32_t rowsize = ((ncols+7)>>3);
    byte_sample(entropy, mat, nrows*rowsize);
    uint8_t n_added_bits = (rowsize<<3) - ncols;
    uint8_t mask = 0xFF<<n_added_bits;
    for(unsigned int i=0; i<nrows; i++)
        mat[rowsize*(i+1)-1] &= mask;
}

void gf2_random_x4(uint8_t* const* x, uint32_t nbits, samplable_x4_t* entropy) {
    uint32_t nbytes = (nbits+7)>>3;
    byte_sample_x4(entropy, x, nbytes);
    uint8_t n_added_bits = (nbytes<<3) - nbits;
    uint8_t mask = 0xFF<<n_added_bits;
    for(unsigned int j=0;j<4; j++)
        x[j][nbytes-1] &= mask;
}

void gf2_random_matrix_by_cols_x4(uint8_t* const* mat, uint32_t nrows, uint32_t ncols, samplable_x4_t* entropy) {
    uint32_t colsize = ((nrows+7)>>3);
    byte_sample_x4(entropy, mat, colsize*ncols);
    uint8_t n_added_bits = (colsize<<3) - nrows;
    uint8_t mask = 0xFF<<n_added_bits;
    for(unsigned int j=0;j<4; j++)
        for(unsigned int i=0; i<ncols; i++)
            mat[j][colsize*(i+1)-1] &= mask;
}

void gf2_random_matrix_by_rows_x4(uint8_t* const* mat, uint32_t nrows, uint32_t ncols, samplable_x4_t* entropy) {
    uint32_t rowsize = ((ncols+7)>>3);
    byte_sample_x4(entropy, mat, nrows*rowsize);
    uint8_t n_added_bits = (rowsize<<3) - ncols;
    uint8_t mask = 0xFF<<n_added_bits;
    for(unsigned int j=0;j<4; j++)
        for(unsigned int i=0; i<nrows; i++)
            mat[j][rowsize*(i+1)-1] &= mask;
}
#endif
