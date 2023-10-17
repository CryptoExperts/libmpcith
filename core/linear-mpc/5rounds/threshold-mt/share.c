#include "share.h"

void compute_complete_share(vec_share_t* share, const vec_share_t* plain, const vec_share_t* rnd[PARAM_NB_REVEALED], uint8_t eval) {
    vec_share_set(share, rnd[PARAM_NB_REVEALED-1]);
    if(eval != 0) {
        for(int i=PARAM_NB_REVEALED-2; i>=0; i--)
            vec_share_muladd2(share, eval, rnd[i]);
        vec_share_muladd2(share, eval, plain);
    }
}

void compute_share_wit(vec_wit_t* out, const vec_share_t* plain, const vec_share_t* rnd[PARAM_NB_REVEALED], uint8_t eval) {
    vec_wit_set(out, get_wit_const(rnd[PARAM_NB_REVEALED-1]));
    if(eval != 0) {
        for(int i=PARAM_NB_REVEALED-2; i>=0; i--)
            vec_wit_muladd2(out, eval, get_wit_const(rnd[i]));
        vec_wit_muladd2(out, eval, get_wit_const(plain));
    }
}

void compute_share_broadcast(vec_broadcast_t* out, const vec_broadcast_t* plain, const vec_broadcast_t* rnd[PARAM_NB_REVEALED], uint8_t eval) {
    vec_br_set(out, rnd[PARAM_NB_REVEALED-1]);
    if(eval != 0) {
        for(int i=PARAM_NB_REVEALED-2; i>=0; i--)
            vec_br_muladd2(out, eval, rnd[i]);
        vec_br_muladd2(out, eval, plain);
    }
}
