#ifndef FIELD_H
#define FIELD_H

#include "parameters.h"

#if RYDE_PARAM_M == 31
#include "rbc_31_elt.h"
#include "rbc_31_vec.h"
#include "rbc_31_mat.h"
#include "rbc_31_qpoly.h"
#include "rbc_31_vspace.h"

#elif  RYDE_PARAM_M == 37
#include "rbc_37_elt.h"
#include "rbc_37_vec.h"
#include "rbc_37_mat.h"
#include "rbc_37_qpoly.h"
#include "rbc_37_vspace.h"

#elif RYDE_PARAM_M == 43
#include "rbc_43_elt.h"
#include "rbc_43_vec.h"
#include "rbc_43_mat.h"
#include "rbc_43_qpoly.h"
#include "rbc_43_vspace.h"

#else 
#error "Unknown field extension"
#endif

// Operations in field extension
#define JOIN3_0(a,b,c)              a ## b ## c
#define JOIN3(a,b,c)                JOIN3_0(a, b, c)

#define rbc_elt JOIN3(rbc_,RYDE_PARAM_M,_elt)
#define rbc_vec JOIN3(rbc_,RYDE_PARAM_M,_vec)
#define rbc_mat JOIN3(rbc_,RYDE_PARAM_M,_mat)
#define rbc_qpoly JOIN3(rbc_,RYDE_PARAM_M,_qpoly)

#define rbc_vec_add JOIN3(rbc_,RYDE_PARAM_M,_vec_add)
#define rbc_vec_set_random JOIN3(rbc_,RYDE_PARAM_M,_vec_set_random)
#define rbc_vec_set_random_x4 JOIN3(rbc_,RYDE_PARAM_M,_vec_set_random_x4)
#define rbc_elt_is_zero JOIN3(rbc_,RYDE_PARAM_M,_elt_is_zero)
#define rbc_vec_to_string JOIN3(rbc_,RYDE_PARAM_M,_vec_to_string)
#define rbc_vec_from_string JOIN3(rbc_,RYDE_PARAM_M,_vec_from_string)
#define rbc_elt_set_zero JOIN3(rbc_,RYDE_PARAM_M,_elt_set_zero)
#define rbc_elt_set JOIN3(rbc_,RYDE_PARAM_M,_elt_set)
#define rbc_mat_clear JOIN3(rbc_,RYDE_PARAM_M,_mat_clear)
#define rbc_mat_vec_mul JOIN3(rbc_,RYDE_PARAM_M,_mat_vec_mul)
#define rbc_qpoly_annihilator JOIN3(rbc_,RYDE_PARAM_M,_qpoly_annihilator)
#define rbc_vec_set_random_from_support JOIN3(rbc_,RYDE_PARAM_M,_vec_set_random_from_support)
#define rbc_vspace_set_random_full_rank_with_one JOIN3(rbc_,RYDE_PARAM_M,_vspace_set_random_full_rank_with_one)
#define rbc_mat_init JOIN3(rbc_,RYDE_PARAM_M,_mat_init)
#define rbc_mat_set_random JOIN3(rbc_,RYDE_PARAM_M,_mat_set_random)
#define rbc_elt_sqr JOIN3(rbc_,RYDE_PARAM_M,_elt_sqr)
#define rbc_vec_inner_product JOIN3(rbc_,RYDE_PARAM_M,_vec_inner_product)
#define rbc_vec_scalar_mul JOIN3(rbc_,RYDE_PARAM_M,_vec_scalar_mul)
#define rbc_elt_add JOIN3(rbc_,RYDE_PARAM_M,_elt_add)
#define rbc_elt_mul JOIN3(rbc_,RYDE_PARAM_M,_elt_mul)
#define rbc_qpoly_init JOIN3(rbc_,RYDE_PARAM_M,_qpoly_init)
#define rbc_qpoly_clear JOIN3(rbc_,RYDE_PARAM_M,_qpoly_clear)
#define rbc_elt_is_equal_to JOIN3(rbc_,RYDE_PARAM_M,_elt_is_equal_to)

#endif /* FIELD_H */
