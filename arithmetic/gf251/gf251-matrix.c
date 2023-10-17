#include "gf251.h"
#include <stdlib.h>
#include <string.h>

#include "gf251-internal.h"

/*************************************************/
/***********     MATRIX OPERATIONS    ************/
/*************************************************/

// vz[] += vx[1][] * y[1] + ... + vx[nb][] * y[nb]
void gf251_matcols_muladd(uint8_t* vz, const uint8_t* y, const uint8_t* vx, uint32_t nb, uint32_t bytes) {
    uint32_t* unreduced_vz = (uint32_t*) calloc(bytes, sizeof(uint32_t));
    memset(unreduced_vz, 0, sizeof(uint32_t)*bytes);
    size_t ind=0;
    for(uint32_t j=0; j<nb; j++)
        for(uint32_t i=0; i<bytes; i++)
            unreduced_vz[i] += vx[ind++]*y[j];
    for(uint32_t i=0; i<bytes; i++)
        vz[i] = _gf251_reduce32(unreduced_vz[i] + vz[i]);
    free(unreduced_vz);
}

// vz[] += vx[1][] * y[1] + ... + vx[nb][] * y[nb]
void gf251_matrows_muladd(uint8_t* vz, const uint8_t* y, const uint8_t* vx, uint32_t nb, uint32_t bytes) {
    uint32_t res;
    size_t ind=0;
    for(uint32_t i=0; i<bytes; i++) {
        res = vz[i];
        for(uint32_t j=0; j<nb; j++)
            res += vx[ind++]*y[j];
        vz[i] = _gf251_reduce32(res);
    }
}

void p251_vec_mat16cols_muladd_b16_avx2_ct_offset(void *vz, uint8_t const *vx,
                                                  void const *my, uint64_t m, uint64_t offset);

// vz[] += vx[1][] * y[1] + ... + vx[nb][] * y[nb]
void gf251_matcols_muladd_scaled(uint8_t* vz, const uint8_t* y, const uint8_t* vx, uint32_t nb, uint32_t bytes, uint32_t scale) {
    uint32_t* unreduced_vz = (uint32_t*) aligned_alloc(32, ((sizeof(uint32_t)*bytes+31)>>5)<<5);
    memset(unreduced_vz, 0, sizeof(uint32_t)*bytes);
    size_t ind=0;
    for(uint32_t j=0; j<nb; j++) {
        for(uint32_t i=0; i<bytes; i++) {
            unreduced_vz[i] += vx[ind++]*y[scale*j];
        }
    }
    for(uint32_t i=0; i<bytes; i++)
        vz[scale*i] = _gf251_reduce32(unreduced_vz[i] + vz[scale*i]);
    free(unreduced_vz);
}

void p251_vec_mat16cols_muladd_b16_avx2_ct(void *vz, uint8_t const *vx,
                                                  void const *my, uint64_t m, uint64_t scaling);
// vz[] += vx[1][] * y[1] + ... + vx[nb][] * y[nb]
// bytes must be multiple of 16
void gf251_mat16cols_muladd(uint8_t* vz, const uint8_t* y, const uint8_t* vx, uint32_t nb, uint32_t bytes) {
    gf251_matcols_muladd(vz, y, vx, nb, bytes);
}

void p251_vec_mat128cols_muladd_avx2_ct(void *vz, uint8_t const *vx,
                                               void const *my, uint64_t m, uint64_t scaling);

void gf251_mat128cols_muladd(uint8_t* vz, const uint8_t* y, const uint8_t* vx, uint32_t nb, uint32_t bytes) {
    uint32_t nb_packs = bytes>>7;
    for(uint32_t i=0; i<nb_packs; i++)
        p251_vec_mat128cols_muladd_avx2_ct(vz+128*i, y, vx+128*i, nb, nb_packs);
}
