#ifndef FIELD_H
#define FIELD_H

#include <stdint.h>
#include <string.h>
#include "parameters.h"

#include "gf251.h"
#include "gf251-interface.h"

// Unitary operations
#define sca_innerproduct(x,y,size) gf251_innerproduct(x,y,size)

// Batched operations
#define matcols_muladd(vz,y,vx,nb_columns,nb_rows) \
    gf251_matcols_muladd(vz,y,vx,nb_columns,nb_rows)
#define matrows_muladd(vz,y,vx,nb_columns,nb_rows) \
    gf251_matrows_muladd(vz,y,vx,nb_columns,nb_rows)
#define matcols_muladd_triangular(vz,y,vx,nb_columns,step) \
    gf251_matcols_muladd_triangular(vz,y,vx,nb_columns,step)

#endif /* FIELD_H */
