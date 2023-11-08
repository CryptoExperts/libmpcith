#ifndef WITNESS_TESTS_H
#define WITNESS_TESTS_H

#include "sample.h"

int witness_test_generate_instance_with_solution(samplable_t* entropy, char* last_error);
int witness_test_generate_instance_with_solution_with_uncompress(samplable_t* entropy, char* last_error);
int witness_test_serialization_witness(samplable_t* entropy, char* last_error);

#endif /* WITNESS_TESTS_H */
