#ifndef LIBMPCITH_SHARE_H
#define LIBMPCITH_SHARE_H

#include "parameters-all.h"
#include "mpc-all.h"

void compute_complete_share(vec_share_t* share, const vec_share_t* plain, const vec_share_t* rnd[PARAM_NB_REVEALED], uint8_t eval);
void compute_share_wit(vec_wit_t* out, const vec_share_t* plain, const vec_share_t* rnd[PARAM_NB_REVEALED], uint8_t eval);
void compute_share_broadcast(vec_broadcast_t* out, const vec_broadcast_t* plain, const vec_broadcast_t* rnd[PARAM_NB_REVEALED], uint8_t eval);

#endif /* LIBMPCITH_SHARE_H */
