#ifndef FIELD_H
#define FIELD_H

#include <stdlib.h>
#include "finite_fields.h"

typedef gf16 gfq;
typedef gf16_mat gfq_mat;

#define gfq_add gf16_add
#define gfq_sub gf16_sub
#define gfq_mul gf16_mul
#define gfq_mat_add gf16_mat_add
#define gfq_mat_init gf16_mat_init
#define gfq_mat_set gf16_mat_set
#define gfq_mat_set_zero gf16_mat_set_zero
#define gfq_mat_scalar_mul gf16_mat_scalar_mul
#define gfq_mat_clear gf16_mat_clear
#define gfq_mat_set_random gf16_mat_set_random
#define gfq_mat_mul gf16_mat_mul
#define gfq_mat_scalar_muladd gf16_mat_scalar_muladd
#define gfq_mat_to_string gf16_mat_to_string
#define gfq_mat_from_string gf16_mat_from_string
#define gfqm_powq gfqm_pow16

extern const uint8_t gf16_inverse_tab[16];

static inline void gfq_rnd(gfq* o, uint32_t size, samplable_t* entropy) {
    uint32_t nbytes = (size+1)>>1;
    uint8_t* buffer = (uint8_t*)malloc(nbytes);
    byte_sample(entropy, buffer, nbytes);
    uint8_t* cur = buffer;
    uint32_t i;
    for(i=0; i+1<size; i+=2) {
        o[i] = (*cur) & 0x0F;
        o[i+1] = (*cur) >> 4;
        cur++;
    }
    if(i != size)
        o[i] = (*cur) & 0x0F;
    free(buffer);
}

static inline void gfq_rnd_x4(gfq* const* o, uint32_t size, samplable_x4_t* entropy) {
    uint32_t nbytes = (size+1)>>1;
    uint8_t* buffer = (uint8_t*)malloc(4*nbytes);
    uint8_t* ptr_buffer[4] = {
        &buffer[0], &buffer[nbytes], &buffer[2*nbytes], &buffer[3*nbytes]
    };
    byte_sample_x4(entropy, ptr_buffer, nbytes);
    for(unsigned int j=0; j<4; j++) {
        uint8_t* cur = ptr_buffer[j];
        uint32_t i;
        for(i=0; i+1<size; i+=2) {
            o[j][i] = (*cur) & 0x0F;
            o[j][i+1] = (*cur) >> 4;
            cur++;
        }
        if(i != size)
            o[j][i] = (*cur) & 0x0F;
    }
    free(buffer);
}

static inline void gfqm_rnd(gfqm* o, uint32_t size, samplable_t* entropy) {
    for(uint32_t i=0; i<size; i++)
        gfq_rnd(o[i], MIRA_PARAM_M, entropy);
}

static inline void gf16_mat_scalar_muladd(gf16_mat o, gf16_mat a, gf16 e, int lines, int columns) {
	for(int l=0 ; l<lines ; l++) {
		for(int c=0 ; c<columns ; c++) {
			o[l*columns + c] ^= gf16_mul(a[l*columns + c], e);
		}
	}
}

static inline int gfqm_is_zero(const gfqm e) {
    gf16 ret = 0;
    for(unsigned int i=0; i<MIRA_PARAM_M; i++)
        ret |= e[i];
    return (ret==0);
}

static inline int gfq_mat_is_equal(const gf16_mat a, const gf16_mat b, int lines, int columns) {
    gf16 ret = 0;
    for(int i=0; i<lines; i++)
        for(int j=0; j<columns; j++)
            ret |= a[i*columns+j] ^ b[i*columns+j];
    return (ret==0);
}

#endif /* FIELD_H */
