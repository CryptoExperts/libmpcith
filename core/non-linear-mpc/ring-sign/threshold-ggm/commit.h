#ifndef LIBMPCITH_COMMIT_H
#define LIBMPCITH_COMMIT_H

#include "parameters.h"
#include "mpc.h"
#include <stdint.h>

// Commitment x1
void commit_seed(uint8_t* digest, const uint8_t* seed, const uint8_t* salt, uint16_t e, uint16_t i);
void commit_seed_and_aux(uint8_t* digest, const uint8_t* seed, const vec_wit_t* wit, const vec_sel_t* sel, const uint8_t* salt, uint16_t e, uint16_t i, sharing_info_t* info);

// Commitment x4
void commit_seed_x4(uint8_t** digest, uint8_t const* const* seed, const uint8_t* salt, uint16_t e, const uint16_t* i);
void commit_seed_and_aux_x4(uint8_t** digest, uint8_t const* const* seed, vec_wit_t const* const* wit, vec_sel_t const* const* sel, const uint8_t* salt, uint16_t e, const uint16_t* i, sharing_info_t* info);

#endif /* LIBMPCITH_COMMIT_H */
