#include "run-all-tests.h"
#include "test-witness.h"
#include "test-vec.h"
#include "test-mpc.h"

void run_all_tests(int nb_tests, samplable_t* entropy) {
    char last_error[100];

    NEW_TEST_SECTION("Witness");
    ADD_TEST("Generation 1", witness_test_generate_instance_with_solution);
    ADD_TEST("Generation 2", witness_test_generate_instance_with_solution_with_uncompress);
    ADD_TEST("Serialization", witness_test_serialization_witness);

    NEW_TEST_SECTION("Vec Functions");
    ADD_TEST("ISEQUAL", test_vec_isequal);
    ADD_TEST("RND", test_vec_rnd);
    ADD_TEST("NEG", test_vec_neg);
    ADD_TEST("ADD", test_vec_add);
    ADD_TEST("SUB", test_vec_sub);
    ADD_TEST("MUL", test_vec_mul);
    ADD_TEST("MULADD", test_vec_muladd);
    ADD_TEST("MULADD2", test_vec_muladd2);
    ADD_TEST("BYTES", test_vec_bytes);

    NEW_TEST_SECTION("Multiparty Computation");
    ADD_TEST("Plain Computation (Direct)", mpc_test_plain_computation);
    ADD_TEST("Plain Computation (Indirect)", mpc_test_plain_computation_2);
    ADD_TEST("Multiparty computation (with zero)", mpc_test_multiparty_computation_with_zero);
    ADD_TEST("Multiparty computation (with rnd)", mpc_test_multiparty_computation_with_randomness);
    ADD_TEST("Serialization Plain Broadcast (output validity)", mpc_test_serialization_plain_broadcast_output_validity);
    ADD_TEST("Serialization Plain Broadcast (correctness)", mpc_test_serialization_plain_broadcast_correctness);
}
