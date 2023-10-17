#include "sign-mpcith-additive-based.h"
#include "parameters-all.h"

// For Signing Algorithm
void packing_init(packing_context_t* ctx, vec_share_t* packed_shares[]) {
    ctx->packed_shares = packed_shares;
    vec_share_setzero(ctx->packed_shares[0]);
}

void packing_update(packing_context_t* ctx, uint32_t i, vec_share_t* party_share) {
    if(i>0)
        vec_share_muladd(ctx->packed_shares[0], sca_inverse_tab[i], party_share);
}

void packing_final(packing_context_t* ctx) {
    vec_share_neg(ctx->packed_shares[0]);
}

// For Verification Algorithm
void share_recomputing_init(share_recomputing_context_t* ctx, uint32_t hidden_view, vec_share_t* parties_shares[]) {
    ctx->hidden_view = hidden_view;
    ctx->packed_shares = parties_shares;
    vec_share_setzero(ctx->packed_shares[0]);
}

void share_recomputing_update(share_recomputing_context_t* ctx, uint32_t i, vec_share_t* party_share) {
    uint32_t factor;
    if(ctx->hidden_view == 0) {
        factor = (i==0) ? sca_setzero() : sca_sub(sca_setzero(), sca_inverse_tab[i]);
    } else {
        factor = (i==0) ? sca_setone() : sca_sub(
            sca_setone(),
            sca_mul(ctx->hidden_view, sca_inverse_tab[i])
        );
    }
    vec_share_muladd(ctx->packed_shares[0], factor, party_share);
}

void share_recomputing_final(share_recomputing_context_t* ctx) {
    (void) ctx;
}

char has_sharing_offset_for(uint32_t hidden_view, uint32_t p) {
    (void) p;
    return (hidden_view!=0);
}

void recompose_broadcast(vec_broadcast_t* broadcast[], vec_broadcast_t* plain_broadcast, uint32_t hidden_view) {
    // Get broef_coeff from the party's broadcast messages (stored in "broef_coeff" variable)
    if(hidden_view != 0) {
        // broad_coef <- broad_coef - broad_plain
        vec_br_sub(broadcast[0], plain_broadcast);
        // broad_coef <- (1/i^*) * broad_coef
        vec_br_mul(broadcast[0], sca_inverse_tab[hidden_view], broadcast[0]);
    }
}
