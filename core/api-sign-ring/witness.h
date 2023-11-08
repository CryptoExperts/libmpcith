#ifndef LIBMPCITH_WITNESS_H
#define LIBMPCITH_WITNESS_H

#include <stdint.h>
#include "hash.h"
#include "sample.h"
#include "witness-struct.h"

void generate_template_instance(instance_t** tmpl, samplable_t* entropy);
void complete_template_instance(instance_t** inst, solution_t** sol, const instance_t* tmpl, samplable_t* entropy);

/**
 * @brief Check if "sol" is a valid extended solution of
 *    the instance represented by "inst".
 * 
 * @param inst a problem instance
 * @param sol the tested solution
 * @return 1 if it is the case, 0 otherwise
 */
int is_correct_solution(instance_t* inst, solution_t* sol);

/**
 * @brief Check if "inst1" and "inst2" corresponds to
 *    the same problem instance.
 * 
 * @param inst1 the first tested instance
 * @param inst2 the second tested instance
 * @return 1 if it is the case, 0 otherwise
 */
int are_same_instances(instance_t* inst1, instance_t* inst2);

int has_same_template(instance_t* inst1, instance_t* inst2);

/**
 * @brief Serialize a instance structure in a byte buffer
 * 
 * @param buf the buffer (allocated array of PARAM_INSTANCE_SIZE bytes)
 * @param inst the instance to serialized
 */
void serialize_instance(uint8_t* buf, const instance_t* inst);

/**
 * @brief Deserialize the instance structure which has been
 *    serialized in a byte buffer by "serialize_instance"
 * 
 * @param buf the buffer containing the instance to deserialize
 * @return the deserialized instance
 */
instance_t* deserialize_instance(const uint8_t* buf);

void serialize_template_instance(uint8_t* buf, const instance_t* inst);
instance_t* deserialize_template_instance(const uint8_t* buf);

/**
 * @brief Update a hash context by putting in the hash input
 *      the instance structure
 * 
 * @param ctx the hash structure
 * @param inst the instance to hash
 */
void hash_update_instance(hash_context* ctx, const instance_t* inst);

/**
 * @brief Serialize a solution structure in a byte buffer
 * 
 * @param buf the buffer (allocated array of PARAM_SOL_SIZE bytes)
 * @param sol the solution to serialized
 */
void serialize_instance_solution(uint8_t* buf, const solution_t* sol);

/**
 * @brief Deserialize the solution structure which has been
 *    serialized in a byte buffer by "serialize_instance_solution"
 * 
 * @param buf the buffer containing the solution to deserialize
 * @return the deserialized solution
 */
solution_t* deserialize_instance_solution(const uint8_t* buf);

/**
 * @brief Uncompress a instance structure. If some of the members
 *    is still undefined, compute them from the other members.
 * 
 * @param inst the instance to uncompress   
 */
void uncompress_instance(instance_t* inst);

/**
 * @brief Deallocates the memory previously allocated
 *    for a solution by a call to generate_instance_with_solution
 *    or deserialize_instance_solution.
 * 
 * @param sol the pointer to a memory block previously allocated for a solution.
 */
void free_instance_solution(solution_t* sol);

/**
 * @brief Deallocates the memory previously allocated
 *    for a instance by a call to generate_instance_with_solution
 *    or deserialize_instance.
 *
 * @param inst the pointer to a memory block previously allocated for a instance.
 */
void free_instance(instance_t* inst);

#endif /* LIBMPCITH_WITNESS_H */
