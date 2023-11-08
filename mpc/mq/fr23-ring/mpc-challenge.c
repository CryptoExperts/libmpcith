#include "mpc.h"
#include "parameters-all.h"

void expand_mpc_challenge_hash(vec_challenge_t** challenges, const uint8_t* digest, size_t nb, const instance_t* tmpl, const instance_t* ring, sharing_info_t* info) {
    (void) tmpl;
    (void) ring;

    xof_context entropy_ctx;
    xof_init(&entropy_ctx);
    xof_update(&entropy_ctx, digest, PARAM_DIGEST_SIZE);
    samplable_t entropy = xof_to_samplable(&entropy_ctx);
    
    for(size_t num=0; num<nb; num++) {
        vec_rnd((uint8_t*) challenges[num]->gamma, sizeof((*challenges)->gamma), &entropy);
        uint8_t (*gamma_)[PARAM_eta] = (uint8_t (*)[PARAM_eta]) (((uint8_t*) challenges[num]) + PARAM_m*PARAM_eta);
        vec_rnd((uint8_t*) gamma_, info->nb_additional_equations*PARAM_eta, &entropy);
    }
}
