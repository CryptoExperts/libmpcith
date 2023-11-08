#include "witness.h"
#include "test-witness.h"

int witness_test_generate_instance_with_solution(samplable_t* entropy, char* last_error) {
    instance_t* inst;
    solution_t* sol;
    // Generation instance with solution
    generate_instance_with_solution(&inst, &sol, entropy);
    int ret = 0;

    // Test "are_same_instances"
    instance_t* inst2 = inst;
    if(!are_same_instances(inst, inst2)) {
        strcpy(last_error, "The function 'are_same_instances' fails to identify same instances.");
        ret = -1;
    }

    // Test "is_correct_solution"
    if(!is_correct_solution(inst, sol)) {
        strcpy(last_error, "The function 'is_correct_solution' fails to check a correct solution.");
        ret = -1;
    }

    free_instance_solution(sol);
    free_instance(inst);
    return ret;
}

int witness_test_generate_instance_with_solution_with_uncompress(samplable_t* entropy, char* last_error) {
    instance_t* inst;
    solution_t* sol;
    // Generation instance with solution
    generate_instance_with_solution(&inst, &sol, entropy);
    // Remark: normally, this call to uncompress is useless
    //   since "are_same_instances" and "is_correct_solution"
    //   does not require that the instance is uncompressed
    //   when calling.
    // This test aims to detect if "uncompress_instance" impacts
    //    the validity of the instance
    uncompress_instance(inst);
    int ret = 0;

    // Test "are_same_instances"
    instance_t* inst2 = inst;
    if(!are_same_instances(inst, inst2)) {
        strcpy(last_error, "The function 'are_same_instances' fails to identify same instances.");
        ret = -1;
    }
    // Test "is_correct_solution"
    if(!is_correct_solution(inst, sol)) {
        strcpy(last_error, "The function 'is_correct_solution' fails to check a correct solution.");
        ret = -1;
    }

    free_instance_solution(sol);
    free_instance(inst);
    return ret;
}

int witness_test_serialization_witness(samplable_t* entropy, char* last_error) {
    instance_t* inst;
    solution_t* sol;
    // Generation instance with solution
    generate_instance_with_solution(&inst, &sol, entropy);

    uint8_t serialized_inst[PARAM_INSTANCE_SIZE];
    uint8_t serialized_sol[PARAM_SOL_SIZE];
    serialize_instance(serialized_inst, inst);
    serialize_instance_solution(serialized_sol, sol);

    instance_t* deserialized_inst = deserialize_instance(serialized_inst);
    solution_t* deserialized_sol = deserialize_instance_solution(serialized_sol);

    int ret = 0;
    if(!are_same_instances(inst, deserialized_inst)) {
        strcpy(last_error, "The function 'are_same_instances' fails to identify same instances.");
        ret = -1;
    }
    if(!is_correct_solution(deserialized_inst, deserialized_sol)) {
        strcpy(last_error, "The function 'is_correct_solution' fails to check a correct solution.");
        ret = -1;
    }

    free_instance_solution(sol);
    free_instance(inst);
    free_instance_solution(deserialized_sol);
    free_instance(deserialized_inst);

    return ret;
}
