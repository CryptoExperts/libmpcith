#include "sign-mpcith-additive-based.h"
#include "parameters-all.h"

// For Signing Algorithm
void packing_init(packing_context_t* ctx, vec_share_t* packed_shares[]) {
    ctx->packed_shares = packed_shares;
    for(uint32_t p=0; p<PARAM_NB_PACKS; p++)
        vec_share_setzero(ctx->packed_shares[p]);
}

void packing_update(packing_context_t* ctx, uint32_t i, vec_share_t* party_share) {
    if(i<PARAM_NB_PARTIES-1)
        vec_share_set(ctx->packed_shares[i], party_share);
}

void packing_final(packing_context_t* ctx) {
    (void) ctx;
}

// For Verification Algorithm
void share_recomputing_init(share_recomputing_context_t* ctx, uint32_t hidden_view, vec_share_t* parties_shares[]) {
    ctx->hidden_view = hidden_view;
    ctx->packed_shares = parties_shares;
    for(uint32_t p=0; p<PARAM_NB_PACKS; p++)
        vec_share_setzero(ctx->packed_shares[p]);
}

void share_recomputing_update(share_recomputing_context_t* ctx, uint32_t i, vec_share_t* party_share) {
    uint32_t p = (i == PARAM_NB_PARTIES-1) ? ctx->hidden_view : i;
    vec_share_set(ctx->packed_shares[p], party_share);
}

void share_recomputing_final(share_recomputing_context_t* ctx) {
    (void) ctx;
}

char has_sharing_offset_for(uint32_t hidden_view, uint32_t p) {
    return (p==hidden_view);
}

void recompose_broadcast(vec_broadcast_t* broadcast[], vec_broadcast_t* plain_broadcast, uint32_t hidden_view) {
    if(hidden_view != PARAM_NB_PARTIES-1) {
        // Sum all the broacast shares in "broadcast[hidden_view]"
        for(uint32_t p=0; p<PARAM_NB_PACKS; p++)
            if(p != hidden_view)
                vec_br_add(broadcast[hidden_view], broadcast[p]);
        // Deduce the missing broadcast share
        vec_br_neg(broadcast[hidden_view]);
        vec_br_add(broadcast[hidden_view], plain_broadcast);
        vec_br_normalize(broadcast[hidden_view]);
    }
}
