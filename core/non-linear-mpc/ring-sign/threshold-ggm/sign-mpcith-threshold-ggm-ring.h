#ifndef LIBMPCITH_SIGN_MPCITH_ADDITIVE_BASED_H
#define LIBMPCITH_SIGN_MPCITH_ADDITIVE_BASED_H

#include <stdint.h>
#include "mpc.h"
#include "parameters-all.h"
 
typedef struct packing_context_t {
    vec_share_t* packed_share;
} packing_context_t;

typedef struct share_recomputing_context_t {
    vec_share_t* packed_share;
    uint32_t hidden_view;
} share_recomputing_context_t;

// For Signing Algorithm
void packing_init(packing_context_t* ctx, vec_share_t* packed_share, sharing_info_t* info);
void packing_update(packing_context_t* ctx, uint32_t i, vec_share_t* party_share, sharing_info_t* info);
void packing_final(packing_context_t* ctx, sharing_info_t* info);

// For Verification Algorithm
void share_recomputing_init(share_recomputing_context_t* ctx, uint32_t hidden_view, vec_share_t* party_share, sharing_info_t* info);
void share_recomputing_update(share_recomputing_context_t* ctx, uint32_t i, vec_share_t* party_share, sharing_info_t* info);
void share_recomputing_final(share_recomputing_context_t* ctx, sharing_info_t* info);

char has_sharing_offset_for(uint32_t hidden_view, uint32_t p);
void recompose_broadcast(vec_broadcast_t* broadcast[PARAM_CIRCUIT_DEPTH], uint32_t hidden_view, sharing_info_t* info);

#endif /* LIBMPCITH_SIGN_MPCITH_ADDITIVE_BASED_H */
