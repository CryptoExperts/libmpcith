#ifndef LIBMPCITH_MPC_H
#define LIBMPCITH_MPC_H

#include "parameters.h"
#include "mpc-struct.h"

/********************************************************
 *                Multiparty Computation                *
 ********************************************************/

int get_sharing_info(sharing_info_t* info, uint32_t ring_size);
uint32_t get_share_bytesize(const sharing_info_t* info);
uint32_t get_selector_bytesize(const sharing_info_t* info);
uint32_t get_selector_short_size(const sharing_info_t* info);
uint32_t get_challenge_bytesize(const sharing_info_t* info);

#include "mpc-utils.h"

void build_selector(vec_sel_t* sel, uint32_t position_in_ring, const sharing_info_t* info);
void sel_compress(uint8_t* buf, const vec_sel_t* sel, const sharing_info_t* info);
void sel_decompress(vec_sel_t* sel, const uint8_t* buf, const sharing_info_t* info);

// Expand a hash digest to several MPC challenges
void expand_mpc_challenge_hash(vec_challenge_t** challenges, const uint8_t* digest, size_t nb, const instance_t* tmpl, const instance_t* ring, sharing_info_t* info);

// Compute the messages broadcast by a party from its input share
void mpc_emulate_party(vec_broadcast_t* broadcast, vec_challenge_t* mpc_challenge,
                            vec_share_t const* share, uint16_t num_party, const instance_t* tmpl, const instance_t* ring, const sharing_info_t* info);

#endif /* LIBMPCITH_MPC_H */
