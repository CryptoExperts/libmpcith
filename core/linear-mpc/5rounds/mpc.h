#ifndef LIBMPCITH_MPC_H
#define LIBMPCITH_MPC_H

#include "parameters.h"
#include "mpc-struct.h"
#include "mpc-utils.h"

/********************************************************
 *                Multiparty Computation                *
 ********************************************************/

// Expand a hash digest to several MPC challenges
void expand_mpc_challenge_hash(vec_challenge_t** challenges, const uint8_t* digest, size_t nb, instance_t* inst);

// Compute the correlated part of the plain input given the witness and the plain random component
void compute_correlated(vec_corr_t* corr, const vec_wit_t* wit, const vec_unif_t* unif, const instance_t* inst);

// Compute the plain broadcast from the plain input
void mpc_compute_plain_broadcast(vec_broadcast_t* plain_br, vec_challenge_t* mpc_challenge,
                                vec_share_t const* share, const instance_t* inst);

// Compute the messages broadcast by a party from its input share
void mpc_compute_communications(vec_broadcast_t* broadcast, vec_challenge_t* mpc_challenge,
                                vec_share_t const* share, vec_broadcast_t const* plain_br,
                                const instance_t* inst, char has_sharing_offset);

// Compute the input share of a party from its broadcast messages
void mpc_compute_communications_inverse(vec_share_t* share, vec_challenge_t* mpc_challenge,
                                const vec_broadcast_t* broadcast, vec_broadcast_t const* plain_br,
                                const instance_t* inst, char has_sharing_offset);

/********************************************************
 *          Management of the plain broadcast           *
 ********************************************************/

// Return 1 if the plain broadcast is valid, 0 otherwise
int is_valid_plain_broadcast(const vec_broadcast_t* plain_br);

// Serialize plain broadcast messages
void compress_plain_broadcast(uint8_t* buf, const vec_broadcast_t* plain_br);

// Deserialize plain broadcast messages
void uncompress_plain_broadcast(vec_broadcast_t* plain_br, const uint8_t* buf);

#endif /* LIBMPCITH_MPC_H */
