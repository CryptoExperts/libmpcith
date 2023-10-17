#ifndef FIELD_H
#define FIELD_H

#include <stdint.h>
#include <string.h>
#include "parameters.h"
#include "rnd.h"

#if PARAM_q == 16
#include "gf16.h"
#include "gf16-interface.h"

typedef vec_elt_t point_t;

// Batched operations
#define matcols_muladd(vz,y,vx,nb_columns,nb_rows) \
    gf16_matcols_muladd(vz,y,vx,nb_columns,nb_rows)
#define matcols_muladd_scaled(vz,y,vx,nb_columns,nb_rows,scale) \
    gf16_matcols_muladd_scaled(vz,y,vx,nb_columns,nb_rows,scale)
#define matmult_rows(C,A,B,m,n,p) \
    gf16_matmult_rows(C,A,B,m,n,p)

#endif

#endif /* FIELD_H */
