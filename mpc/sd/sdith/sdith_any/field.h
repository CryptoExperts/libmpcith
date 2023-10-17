#ifndef FIELD_H
#define FIELD_H

#include <stdint.h>
#include <string.h>
#include "parameters.h"
#include "rnd.h"

#if PARAM_FIELD_SIZE == 251
#include "gf251.h"
#include "gf251-interface.h"

// Batched operations
#define matcols_muladd(vz,y,vx,nb_columns,nb_rows) \
    gf251_matcols_muladd(vz,y,vx,nb_columns,nb_rows)
#define mat16cols_muladd(vz,y,vx,nb_columns,nb_rows) \
    gf251_mat16cols_muladd(vz,y,vx,nb_columns,nb_rows)

// Operations in field extension
#define JOIN3_0(a,b,c)              a ## b ## c
#define JOIN3(a,b,c)                JOIN3_0(a, b, c)

#define ext_eq(a,b)          JOIN3(gf251to,PARAM_EXT_DEGREE,_eq)(a,b)
#define ext_add(r,a,b)       JOIN3(gf251to,PARAM_EXT_DEGREE,_add)(r,a,b)
#define ext_sub(r,a,b)       JOIN3(gf251to,PARAM_EXT_DEGREE,_sub)(r,a,b)
#define ext_mul(r,a,b)       JOIN3(gf251to,PARAM_EXT_DEGREE,_mul)(r,a,b)
#define ext_mulsc(r,a,b)     JOIN3(gf251to,PARAM_EXT_DEGREE,_mul_gf251)(r,a,b)
#define ext_set(dst, src)    memcpy(dst, src, PARAM_EXT_DEGREE*sizeof(uint8_t))
#define ext_setzero(a)       memset(a, 0, PARAM_EXT_DEGREE*sizeof(uint8_t))
#define ext_neg(a)           gf251_neg_tab(a, PARAM_EXT_DEGREE)

#elif PARAM_FIELD_SIZE == 256
#include "gf256.h"
#include "gf256-interface.h"

// Batched operations
#define matcols_muladd(vz,y,vx,nb_columns,nb_rows) \
    gf256_matcols_muladd(vz,y,vx,nb_columns,nb_rows)
#define mat16cols_muladd(vz,y,vx,nb_columns,nb_rows) \
    gf256_mat16cols_muladd(vz,y,vx,nb_columns,nb_rows)

// Operations in field extension
#define JOIN3_0(a,b,c)              a ## b ## c
#define JOIN3(a,b,c)                JOIN3_0(a, b, c)

#define ext_eq(a,b)          JOIN3(gf256to,PARAM_EXT_DEGREE,_eq)(a,b)
#define ext_add(r,a,b)       JOIN3(gf256to,PARAM_EXT_DEGREE,_add)(r,a,b)
#define ext_sub(r,a,b)       JOIN3(gf256to,PARAM_EXT_DEGREE,_add)(r,a,b)
#define ext_mul(r,a,b)       JOIN3(gf256to,PARAM_EXT_DEGREE,_mul)(r,a,b)
#define ext_mulsc(r,a,b)     JOIN3(gf256to,PARAM_EXT_DEGREE,_mul_gf256)(r,a,b)
#define ext_set(dst, src)    memcpy(dst, src, PARAM_EXT_DEGREE*sizeof(uint8_t))
#define ext_setzero(a)       memset(a, 0, PARAM_EXT_DEGREE*sizeof(uint8_t))
#define ext_neg(a)           {(void) (a);}

#endif

#endif /* FIELD_H */
