#ifndef LIBMPCITH_MPC_H
#define LIBMPCITH_MPC_H

#include "parameters.h"
#include "mpc-struct.h"
#include "mpc-utils.h"

/********************************************************
 *                Multiparty Computation                *
 ********************************************************/

// Expand a hash digest to several DC challenges
void expand_dc_challenge_hash(vec_dc_challenge_t** challenges, const uint8_t* digest, size_t nb);

void mpc_run_degree_checking(vec_dc_broadcast_t* dc_broadcast, vec_dc_challenge_t* dc_challenge, vec_share_t const* share, uint16_t num_party);

// Compute the messages broadcast by a party from its input share
void mpc_emulate_party(vec_broadcast_t* broadcast, vec_challenge_t* mpc_challenge,
                            vec_share_t const* share, uint16_t num_party, const instance_t* inst);

// Expand a hash digest to several MPC challenges
void expand_mpc_challenge_hash(vec_challenge_t** challenges, const uint8_t* digest, size_t nb, instance_t* inst);

// Compute the messages broadcast by a party from its input share
void mpc_emulate_party(vec_broadcast_t* broadcast, vec_challenge_t* mpc_challenge,
                            vec_share_t const* share, uint16_t num_party, const instance_t* inst);

#endif /* LIBMPCITH_MPC_H */
