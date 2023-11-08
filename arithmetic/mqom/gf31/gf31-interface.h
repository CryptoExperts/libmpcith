#ifndef LIBMPCITH_GF31_INTERFACE_H
#define LIBMPCITH_GF31_INTERFACE_H

#include "gf31.h"

typedef uint8_t vec_elt_t;

static inline void vec_setzero(vec_elt_t* vz, int size) { memset(vz, 0, size*sizeof(vec_elt_t)); }
static inline void vec_set(vec_elt_t* vz, const vec_elt_t* vx, int size) { memcpy(vz, vx, size*sizeof(vec_elt_t)); }
static inline int vec_isequal(const vec_elt_t* vx, const vec_elt_t* vy, int size) { return (memcmp(vx, vy, size*sizeof(vec_elt_t)) == 0); }

#define vec_add(x,y,size)           gf31_add_tab(x,y,size)
#define vec_sub(x,y,size)           gf31_sub_tab(x,y,size)
#define vec_neg(x,size)             gf31_neg_tab(x,size)
#define vec_rnd(x,size,ent)         gf31_random_elements(x,size,ent)
#define vec_rnd_x4(x,size,ent)      gf31_random_elements_x4(x,size,ent)
#define vec_mul(z,y,x,size)         gf31_mul_tab(z,x,y,size)
#define vec_muladd(z,y,x,size)      gf31_muladd_tab(z,y,x,size)
#define vec_muladd2(z,y,x,size)     gf31_mul_and_add_tab(z,y,x,size)
#define vec_to_bytes(buf,x,size)    gf31_compress_tab(buf,x,size)
#define vec_from_bytes(x,buf,size)  gf31_decompress_tab(x,buf,size)

#define VEC_COMPRESSIBLE
#define get_serialized_size(size)   (((size)*5+7)>>3)

#define sca_zero()   (0)
#define sca_one()    (1)
#define sca_get(a)   ((uint8_t)(a))
#define sca_add(a,b) gf31_add(a,b)
#define sca_sub(a,b) gf31_sub(a,b)
#define sca_mul(a,b) gf31_mul(a,b)
#define sca_neg(a)   gf31_neg(a)
#define sca_inverse_tab gf31_inverse_tab

#endif /* LIBMPCITH_GF31_INTERFACE_H */
