#ifndef LIBMPCITH_GF256_INTERFACE_H
#define LIBMPCITH_GF256_INTERFACE_H

#include "gf256.h"

typedef uint8_t vec_elt_t;

static inline void vec_setzero(vec_elt_t* vz, int size) { memset(vz, 0, size*sizeof(vec_elt_t)); }
static inline void vec_set(vec_elt_t* vz, const vec_elt_t* vx, int size) { memcpy(vz, vx, size*sizeof(vec_elt_t)); }
static inline int vec_isequal(const vec_elt_t* vx, const vec_elt_t* vy, int size) { return (memcmp(vx, vy, size*sizeof(vec_elt_t)) == 0); }

#define vec_add(x,y,size)           gf256_add_tab(x,y,size)
#define vec_sub(x,y,size)           gf256_add_tab(x,y,size)
#define vec_neg(x,size)             {(void) (x); (void) (size);}
#define vec_rnd(x,size,ent)         gf256_random_elements(x,size,ent)
#define vec_rnd_x4(x,size,ent)      gf256_random_elements_x4(x,size,ent)
#define vec_mul(z,y,x,size)         gf256_mul_tab(z,x,y,size)
#define vec_muladd(z,y,x,size)      gf256_muladd_tab(z,y,x,size)
#define vec_muladd2(z,y,x,size)     gf256_mul_and_add_tab(z,y,x,size)
#define vec_to_bytes(buf,x,size)    memcpy(buf,x,size)
#define vec_from_bytes(x,buf,size)  memcpy(x,buf,size)

#define sca_zero()   (0)
#define sca_one()    (1)
#define sca_get(a)   ((uint8_t)(a))
#define sca_add(a,b) gf256_add(a,b)
#define sca_sub(a,b) gf256_add(a,b)
#define sca_mul(a,b) gf256_mul(a,b)
#define sca_neg(a)   (a)
#define sca_inverse_tab gf256_inverse_tab

#endif /* LIBMPCITH_GF256_INTERFACE_H */
