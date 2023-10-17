#ifndef FIELD_H
#define FIELD_H

#include <stdint.h>
#include <string.h>
#include "parameters.h"

#include "gf31.h"
#include "gf31-interface.h"

// Unitary operations
#define sca_innerproduct(x,y,size) gf31_innerproduct(x,y,size)

// Batched operations
#define matcols_muladd(vz,y,vx,nb_columns,nb_rows) \
    gf31_matcols_muladd(vz,y,vx,nb_columns,nb_rows)
#define matrows_muladd(vz,y,vx,nb_columns,nb_rows) \
    gf31_matrows_muladd(vz,y,vx,nb_columns,nb_rows)
#define mat128cols_muladd(vz,y,vx,nb_columns,nb_rows) \
    gf31_mat128cols_muladd(vz,y,vx,nb_columns,nb_rows)
#define matcols_muladd_triangular(vz,y,vx,nb_columns,step) \
    gf31_matcols_muladd_triangular(vz,y,vx,nb_columns,step)

// Operations in field extension
#define JOIN3_0(a,b,c)              a ## b ## c
#define JOIN3(a,b,c)                JOIN3_0(a, b, c)

#define ext_mul(r,a,b)       JOIN3(gf31to,PARAM_EXT_DEGREE,_mul)(r,a,b)
#define ext_add(r,a,b)       JOIN3(gf31to,PARAM_EXT_DEGREE,_add)(r,a,b)
#define ext_sub(r,a,b)       JOIN3(gf31to,PARAM_EXT_DEGREE,_sub)(r,a,b)
#define ext_mulsc(r,a,b)     JOIN3(gf31to,PARAM_EXT_DEGREE,_mul_gf31)(r,a,b)
#define ext_eq(a,b)          JOIN3(gf31to,PARAM_EXT_DEGREE,_eq)(a,b)

#endif /* FIELD_H */
