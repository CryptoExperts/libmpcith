#ifndef MPC_PROTOCOL_H
#define MPC_PROTOCOL_H

#include "parameters-all.h"

void mpc_compute_communications(vec_broadcast_t* broadcast[PARAM_CIRCUIT_DEPTH],
                                vec_challenge_t* mpc_challenge,
                                vec_share_t const* plain, vec_share_t const* share_rnd,
                                const instance_t* inst);

#endif /* MPC_PROTOCOL_H */
