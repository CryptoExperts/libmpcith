#ifndef LIBMPCITH_SIGN_MPCITH_ADDITIVE_BASED_H
#define LIBMPCITH_SIGN_MPCITH_ADDITIVE_BASED_H

#include <stdint.h>
#include "mpc.h"
#include "packing-struct.h"

// For Signing Algorithm
void packing_init(packing_context_t* ctx, vec_share_t* packed_shares[]);
void packing_update(packing_context_t* ctx, uint32_t i, vec_share_t* party_share);
void packing_final(packing_context_t* ctx);

// For Verification Algorithm
void share_recomputing_init(share_recomputing_context_t* ctx, uint32_t hidden_view, vec_share_t* parties_shares[]);
void share_recomputing_update(share_recomputing_context_t* ctx, uint32_t i, vec_share_t* party_share);
void share_recomputing_final(share_recomputing_context_t* ctx);

char has_sharing_offset_for(uint32_t hidden_view, uint32_t p);
void recompose_broadcast(vec_broadcast_t* broadcast[], vec_broadcast_t* plain_broadcast, uint32_t hidden_view);

#endif /* LIBMPCITH_SIGN_MPCITH_ADDITIVE_BASED_H */
