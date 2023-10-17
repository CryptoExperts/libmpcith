#include "sign-mpcith-additive-based.h"
#include "parameters-all.h"

// For Signing Algorithm
void packing_init(packing_context_t* ctx, vec_share_t* packed_shares[]) {
    ctx->packed_shares = packed_shares;
    for(uint32_t p=0; p<PARAM_NB_PACKS; p++)
        vec_share_setzero(ctx->packed_shares[p]);
}

void packing_update(packing_context_t* ctx, uint32_t i, vec_share_t* party_share) {
    for(uint32_t p=0; p<PARAM_NB_PACKS; p++)
        if(((i>>p) & 1) == 0) // If pth bit of i is zero
            vec_share_add(ctx->packed_shares[p], party_share);
}

void packing_final(packing_context_t* ctx) {
    for(uint32_t p=0; p<PARAM_NB_PACKS; p++)
        vec_share_normalize(ctx->packed_shares[p]);
}

// For Verification Algorithm
void share_recomputing_init(share_recomputing_context_t* ctx, uint32_t hidden_view, vec_share_t* parties_shares[]) {
    ctx->hidden_view = hidden_view;
    ctx->packed_shares = parties_shares;
    for(uint32_t p=0; p<PARAM_NB_PACKS; p++)
        vec_share_setzero(ctx->packed_shares[p]);
}

void share_recomputing_update(share_recomputing_context_t* ctx, uint32_t i, vec_share_t* party_share) {
    // Aggregate to get the shares of the "main parties"
    uint16_t _xor = (i ^ ctx->hidden_view);
    for(uint32_t p=0; p<PARAM_NB_PACKS; p++)
        if(((_xor>>p) & 1) == 1) // If the pth bit of i* is different of the pth bit of i
            vec_share_add(ctx->packed_shares[p], party_share);
}

void share_recomputing_final(share_recomputing_context_t* ctx) {
    for(uint32_t p=0; p<PARAM_NB_PACKS; p++)
        vec_share_normalize(ctx->packed_shares[p]);
}

char has_sharing_offset_for(uint32_t hidden_view, uint32_t p) {
    return (((hidden_view>>p) & 1) != 1);
}

void recompose_broadcast(vec_broadcast_t* broadcast[], vec_broadcast_t* plain_broadcast, uint32_t hidden_view) {
    for(uint32_t p=0; p<PARAM_NB_PACKS; p++)
        if(((hidden_view>>p) & 1) != 1) {
            vec_br_neg(broadcast[p]);
            vec_br_add(broadcast[p], plain_broadcast);
            vec_br_normalize(broadcast[p]);
        }
}
