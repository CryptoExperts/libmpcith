#ifndef MPC_TESTS_H
#define MPC_TESTS_H

#include "sample.h"

int mpc_test_plain_computation(samplable_t* entropy, char* last_error);
int mpc_test_plain_computation_2(samplable_t* entropy, char* last_error);
int mpc_test_multiparty_computation_with_zero(samplable_t* entropy, char* last_error);
int mpc_test_multiparty_computation_with_randomness(samplable_t* entropy, char* last_error);
int mpc_test_serialization_plain_broadcast_output_validity(samplable_t* entropy, char* last_error);
int mpc_test_serialization_plain_broadcast_correctness(samplable_t* entropy, char* last_error);

#endif /* MPC_TESTS_H */
