#include "mpc-protocol.h"
#include "mpc.h"

#if PARAM_CIRCUIT_DEPTH > 2
extern uint16_t broadcast_interpolation_array[PARAM_CIRCUIT_DEPTH-1][PARAM_CIRCUIT_DEPTH];
#endif

void mpc_compute_communications(vec_broadcast_t* broadcast[PARAM_CIRCUIT_DEPTH],
                                vec_challenge_t* mpc_challenge,
                                vec_share_t const* plain, vec_share_t const* share_rnd,
                                const instance_t* tmpl, const instance_t* ring,
                                sharing_info_t* info) {

    // Coefficient of degree PARAM_CIRCUIT_DEPTH
    mpc_emulate_party(
        broadcast[PARAM_CIRCUIT_DEPTH-1],
        mpc_challenge,
        share_rnd,
        0, tmpl, ring, info
    );

    vec_share_t* party_share = new_share(info);
    #if PARAM_CIRCUIT_DEPTH == 2
    vec_share_set(party_share, plain, info);
    vec_share_add(party_share, share_rnd, info);
    mpc_emulate_party(broadcast[0], mpc_challenge, party_share, 1, tmpl, ring, info);
    vec_br_sub(broadcast[0], broadcast[1], info);
    #elif PARAM_CIRCUIT_DEPTH > 2
    vec_broadcast_t* broadcast_share[PARAM_CIRCUIT_DEPTH-1];
    for(uint32_t i=0; i<PARAM_CIRCUIT_DEPTH-1; i++)
        broadcast_share[i] = new_br(NULL);
    for(uint32_t num_party=1; num_party<PARAM_CIRCUIT_DEPTH; num_party++) {
        vec_share_set(party_share, plain, info);
        vec_share_muladd(party_share, num_party, share_rnd, info);
        mpc_emulate_party(
            broadcast_share[num_party-1],
            mpc_challenge,
            party_share,
            num_party, tmpl, ring, info
        );
    }
    for(uint32_t i=0; i<PARAM_CIRCUIT_DEPTH-1; i++) {
        vec_br_setzero(broadcast[i], info);
        for(uint32_t j=0; j<PARAM_CIRCUIT_DEPTH-1; j++) {
            vec_br_muladd(
                broadcast[i],
                broadcast_interpolation_array[i][j],
                broadcast_share[j],
                info
            );
        }
        vec_br_muladd(
            broadcast[i],
            broadcast_interpolation_array[i][PARAM_CIRCUIT_DEPTH-1],
            broadcast[PARAM_CIRCUIT_DEPTH-1],
            info
        );
    }
    for(uint32_t i=0; i<PARAM_CIRCUIT_DEPTH-1; i++)
        free(broadcast_share[i]);
    #endif
    free(party_share);
}
