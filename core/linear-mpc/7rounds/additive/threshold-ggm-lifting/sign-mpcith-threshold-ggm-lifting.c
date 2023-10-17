#include "sign-mpcith-additive-based.h"
#include "parameters-all.h"

#define DEFAULT_EDGE_MASK ((1<<PARAM_LOG_DEFAULT_EDGE_SIZE)-1)

// For Signing Algorithm
void packing_init(packing_context_t* ctx, vec_share_t* packed_shares[]) {
    ctx->packed_shares = packed_shares;
    // All the hypercube dimension, except the last one
    for(uint32_t p=0; p<PARAM_NB_PACKS-1; p++) {
        for(uint32_t k=0; k<PARAM_DEFAULT_EDGE_SIZE-1; k++) {
            ctx->accumulators[p][k] = new_share();
            vec_share_setzero(ctx->accumulators[p][k]);
        }
    }
    // Last dimension
    for(uint32_t k=0; k<PARAM_LAST_EDGE_SIZE-1; k++) {
        ctx->last_accumulators[k] = new_share();
        vec_share_setzero(ctx->last_accumulators[k]);
    }
}

void packing_update(packing_context_t* ctx, uint32_t i, vec_share_t* party_share) {
    // All the hypercube dimension, except the last one
    for(uint32_t p=0; p<PARAM_NB_PACKS-1; p++) {
        uint32_t position_on_edge = (i>>(p*PARAM_LOG_DEFAULT_EDGE_SIZE)) & DEFAULT_EDGE_MASK;
        // We omit the first position, since it is useless for the PRSSitH after
        if(position_on_edge > 0)
            vec_share_add(ctx->accumulators[p][position_on_edge-1], party_share);
    }
    // Last dimension
    uint32_t position_on_last_edge = (i>>((PARAM_NB_PACKS-1)*PARAM_LOG_DEFAULT_EDGE_SIZE)) & DEFAULT_EDGE_MASK;
    if(position_on_last_edge > 0)
        vec_share_add(ctx->last_accumulators[position_on_last_edge-1], party_share);
}

void packing_final(packing_context_t* ctx) {
    // All the hypercube dimension, except the last one
    for(uint32_t p=0; p<PARAM_NB_PACKS-1; p++) {
        vec_share_setzero(ctx->packed_shares[p]);
        for(uint32_t k=0; k<PARAM_DEFAULT_EDGE_SIZE-1; k++) {
            vec_share_normalize(ctx->accumulators[p][k]);
            vec_share_muladd(ctx->packed_shares[p], sca_inverse_tab[k+1], ctx->accumulators[p][k]);
        }
        vec_share_neg(ctx->packed_shares[p]);
    }
    // Last dimension
    vec_share_setzero(ctx->packed_shares[PARAM_NB_PACKS-1]);
    for(uint32_t k=0; k<PARAM_LAST_EDGE_SIZE-1; k++) {
        vec_share_normalize(ctx->last_accumulators[k]);
        vec_share_muladd(ctx->packed_shares[PARAM_NB_PACKS-1], sca_inverse_tab[k+1], ctx->last_accumulators[k]);
    }
    vec_share_neg(ctx->packed_shares[PARAM_NB_PACKS-1]);

    for(uint32_t p=0; p<PARAM_NB_PACKS-1; p++)
        for(uint32_t k=0; k<PARAM_DEFAULT_EDGE_SIZE-1; k++)
            free(ctx->accumulators[p][k]);
    for(uint32_t k=0; k<PARAM_LAST_EDGE_SIZE-1; k++)
        free(ctx->last_accumulators[k]);
}

// For Verification Algorithm
void share_recomputing_init(share_recomputing_context_t* ctx, uint32_t hidden_view, vec_share_t* parties_shares[]) {
    ctx->hidden_view = hidden_view;
    // All the hypercube dimension, except the last one
    ctx->packed_shares = parties_shares;
    for(uint32_t p=0; p<PARAM_NB_PACKS-1; p++) {
        for(uint32_t k=0; k<PARAM_DEFAULT_EDGE_SIZE-1; k++) {
            ctx->accumulators[p][k] = new_share();
            vec_share_setzero(ctx->accumulators[p][k]);
        }
    }
    // Last dimension
    for(uint32_t k=0; k<PARAM_LAST_EDGE_SIZE-1; k++) {
        ctx->last_accumulators[k] = new_share();
        vec_share_setzero(ctx->last_accumulators[k]);
    }
}

void share_recomputing_update(share_recomputing_context_t* ctx, uint32_t i, vec_share_t* party_share) {
    // All the hypercube dimension, except the last one
    for(uint32_t p=0; p<PARAM_NB_PACKS-1; p++) {
        uint32_t position_on_edge = (i>>(p*PARAM_LOG_DEFAULT_EDGE_SIZE)) & DEFAULT_EDGE_MASK;
        uint32_t hidden_position_on_edge = (ctx->hidden_view>>(p*PARAM_LOG_DEFAULT_EDGE_SIZE)) & DEFAULT_EDGE_MASK;
        if(position_on_edge < hidden_position_on_edge)
            vec_share_add(ctx->accumulators[p][position_on_edge], party_share);
        else if(position_on_edge > hidden_position_on_edge)
            vec_share_add(ctx->accumulators[p][position_on_edge-1], party_share);
    }
    // Last dimension
    uint32_t position_on_last_edge = (i>>((PARAM_NB_PACKS-1)*PARAM_LOG_DEFAULT_EDGE_SIZE)) & DEFAULT_EDGE_MASK;
    uint32_t hidden_position_on_last_edge = (ctx->hidden_view>>((PARAM_NB_PACKS-1)*PARAM_LOG_DEFAULT_EDGE_SIZE)) & DEFAULT_EDGE_MASK;
    if(position_on_last_edge < hidden_position_on_last_edge)
        vec_share_add(ctx->last_accumulators[position_on_last_edge], party_share);
    else if(position_on_last_edge > hidden_position_on_last_edge)
        vec_share_add(ctx->last_accumulators[position_on_last_edge-1], party_share);
}

void share_recomputing_final(share_recomputing_context_t* ctx) {
    // All the hypercube dimension, except the last one
    for(uint32_t p=0; p<PARAM_NB_PACKS-1; p++) {
        vec_share_setzero(ctx->packed_shares[p]);

        uint32_t hidden_position_on_edge = (ctx->hidden_view>>(p*PARAM_LOG_DEFAULT_EDGE_SIZE)) & DEFAULT_EDGE_MASK;
        for(uint32_t k=0; k<PARAM_DEFAULT_EDGE_SIZE-1; k++) {
            uint32_t factor;
            if(hidden_position_on_edge == 0) {
                factor = sca_sub(sca_setzero(), sca_inverse_tab[k+1]);
            } else {
                if(k == 0)
                    factor = sca_setone();
                else if(k < hidden_position_on_edge)
                    factor = sca_sub(sca_setone(), sca_mul(hidden_position_on_edge, sca_inverse_tab[k]));
                else
                    factor = sca_sub(sca_setone(), sca_mul(hidden_position_on_edge, sca_inverse_tab[k+1]));
            }
            vec_share_muladd(ctx->packed_shares[p], factor, ctx->accumulators[p][k]);
        }
    }
    // Last dimension
    vec_share_setzero(ctx->packed_shares[PARAM_NB_PACKS-1]);
    uint32_t hidden_position_on_last_edge = (ctx->hidden_view>>((PARAM_NB_PACKS-1)*PARAM_LOG_DEFAULT_EDGE_SIZE)) & DEFAULT_EDGE_MASK;
    for(uint32_t k=0; k<PARAM_LAST_EDGE_SIZE-1; k++) {
        uint32_t factor;
        if(hidden_position_on_last_edge == 0) {
            factor = sca_sub(sca_setzero(), sca_inverse_tab[k+1]);
        } else {
            if(k == 0)
                factor = sca_setone();
            else if(k < hidden_position_on_last_edge)
                factor = sca_sub(sca_setone(), sca_mul(hidden_position_on_last_edge, sca_inverse_tab[k]));
            else
                factor = sca_sub(sca_setone(), sca_mul(hidden_position_on_last_edge, sca_inverse_tab[k+1]));
        }
        vec_share_muladd(ctx->packed_shares[PARAM_NB_PACKS-1], factor, ctx->last_accumulators[k]);
    }

    for(uint32_t p=0; p<PARAM_NB_PACKS-1; p++)
        for(uint32_t k=0; k<PARAM_DEFAULT_EDGE_SIZE-1; k++)
            free(ctx->accumulators[p][k]);
    for(uint32_t k=0; k<PARAM_LAST_EDGE_SIZE-1; k++)
        free(ctx->last_accumulators[k]);
}

char has_sharing_offset_for(uint32_t hidden_view, uint32_t p) {
    uint32_t hidden_position_on_edge = (hidden_view>>(p*PARAM_LOG_DEFAULT_EDGE_SIZE)) & DEFAULT_EDGE_MASK;
    return (hidden_position_on_edge!=0);
}

void recompose_broadcast(vec_broadcast_t* broadcast[], vec_broadcast_t* plain_broadcast, uint32_t hidden_view) {
    for(uint32_t p=0; p<PARAM_NB_PACKS; p++) {
        uint32_t hidden_position_on_edge = (hidden_view>>(p*PARAM_LOG_DEFAULT_EDGE_SIZE)) & DEFAULT_EDGE_MASK;
        // Get broef_coeff from the party's broadcast messages (stored in "broef_coeff" variable)
        if(hidden_position_on_edge != 0) {
            // broad_coef <- broad_coef - broad_plain
            vec_br_sub(broadcast[p], plain_broadcast);
            // broad_coef <- (1/i^*) * broad_coef
            vec_br_mul(broadcast[p], sca_inverse_tab[hidden_position_on_edge], broadcast[p]);
        }
    }
}
