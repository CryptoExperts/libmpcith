#include "sign-mpcith-threshold-ggm-extended.h"
#include "parameters-all.h"

// For Signing Algorithm
void packing_init(packing_context_t* ctx, vec_share_t* packed_share) {
    ctx->packed_share = packed_share;
    vec_share_setzero(ctx->packed_share);
}

void packing_update(packing_context_t* ctx, uint32_t i, vec_share_t* party_share) {
    if(i>0)
        vec_share_muladd(ctx->packed_share, sca_inverse_tab[i], party_share);
}

void packing_final(packing_context_t* ctx) {
    vec_share_neg(ctx->packed_share);
}

// For Verification Algorithm
void share_recomputing_init(share_recomputing_context_t* ctx, uint32_t hidden_view, vec_share_t* party_share) {
    ctx->hidden_view = hidden_view;
    ctx->packed_share = party_share;
    vec_share_setzero(ctx->packed_share);
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
    vec_share_muladd(ctx->packed_share, factor, party_share);
}

void share_recomputing_final(share_recomputing_context_t* ctx) {
    (void) ctx;
}

char has_sharing_offset_for(uint32_t hidden_view, uint32_t p) {
    (void) p;
    return (hidden_view!=0);
}

void recompose_broadcast(vec_broadcast_t* broadcast[PARAM_CIRCUIT_DEPTH], uint32_t hidden_view) {
    // Get broef_coeff from the party's broadcast messages (stored in "broef_coeff" variable)
    if(hidden_view != 0) {
        // broad_coef <- broad_coef - e_i * broad[0] - e_i^2*broad[1] - ... - e_i^(d-1)*broad[d-2]
        uint32_t scalar = hidden_view;
        vec_br_neg(broadcast[PARAM_CIRCUIT_DEPTH-1]);
        for(uint32_t j=0; j<PARAM_CIRCUIT_DEPTH-1; j++) { 
            vec_br_muladd(broadcast[PARAM_CIRCUIT_DEPTH-1], scalar, broadcast[j]);
            scalar = sca_mul(scalar, hidden_view);
        }
        vec_br_neg(broadcast[PARAM_CIRCUIT_DEPTH-1]);
        // broad_coef <- (1/i^*)**d * broad_coef
        vec_br_mul(broadcast[PARAM_CIRCUIT_DEPTH-1], sca_inverse_tab[scalar], broadcast[PARAM_CIRCUIT_DEPTH-1]);
    }
}
