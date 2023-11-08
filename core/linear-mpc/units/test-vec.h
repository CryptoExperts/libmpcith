#ifndef VEC_TESTS_H
#define VEC_TESTS_H

#include "sample.h"

int test_vec_isequal(samplable_t* entropy, char* last_error);
int test_vec_rnd(samplable_t* entropy, char* last_error);
int test_vec_neg(samplable_t* entropy, char* last_error);
int test_vec_add(samplable_t* entropy, char* last_error);
int test_vec_sub(samplable_t* entropy, char* last_error);
int test_vec_mul(samplable_t* entropy, char* last_error);
int test_vec_muladd(samplable_t* entropy, char* last_error);
int test_vec_muladd2(samplable_t* entropy, char* last_error);
int test_vec_bytes(samplable_t* entropy, char* last_error);

#endif /* VEC_TESTS_H */
