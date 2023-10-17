#include "mpc.h"
#include "mpc-def.h"
#include "benchmark.h"

void compute_correlated(vec_corr_t* corr, const vec_wit_t* wit, const vec_unif_t* unif, const instance_t* inst) {
    (void) inst;
    matrix_product(corr->C, unif->A, wit->K, PAR_S, PAR_R, PAR_N - PAR_R);
}

int is_valid_plain_broadcast(vec_broadcast_t const* plain_br) {
    uint8_t ret = 0;
    // Just test if "plain_br->V" is zero
    for(unsigned int i=0; i<sizeof(plain_br->V); i++)
        ret |= ((uint8_t*) plain_br->V)[i];
    return (ret == 0);
}

void compress_plain_broadcast(uint8_t* buf, const vec_broadcast_t* plain_br) {
    matrix_pack(&buf, NULL, plain_br->S, PAR_S, PAR_R);
}

void uncompress_plain_broadcast(vec_broadcast_t* plain_br, const uint8_t* buf) {
    matrix_unpack(plain_br->S, (uint8_t**) &buf, NULL, PAR_S, PAR_R);
    matrix_init_zero(plain_br->V, PAR_S, PAR_N-PAR_R);
}

void run_multiparty_computation(vec_broadcast_t* broadcast, vec_challenge_t* mpc_challenge,
                                vec_share_t const* share, vec_broadcast_t const* plain_br,
                                const instance_t* inst, char has_sharing_offset, char entire_computation) {
    int p;

    /* Set E_main_shr_k_j = (M[0] if i = 0 else 0) + sum_{p = 0}^{PAR_K-1} a_shr[i] * M[p + 1]. */
    ff_t E[matrix_bytes_size(PAR_M, PAR_N)];
    if (has_sharing_offset)
    {
        matrix_copy(E, inst->M0, PAR_M, PAR_N);
    }
    else
    {
        matrix_init_zero(E, PAR_M, PAR_N);
    }

    for (p = 0; p < PAR_K; p++)
    {
        ff_t scalar;

        scalar = matrix_get_entry(share->wit.alpha, PAR_K, p, 0);
        matrix_add_multiple(E, scalar, (*inst->Mi)[p], PAR_M, PAR_N);
    }

    /* Compute 'MaL_main_shr_k_j' and 'MaR_main_shr_k_j' using the fact
        * that E_main_shr_k_j = [MaL_main_shr_k_j | MaR_main_shr_k_j]. */
    ff_t MaL[matrix_bytes_size(PAR_M, PAR_N - PAR_R)];
    ff_t MaR[matrix_bytes_size(PAR_M, PAR_R)];
    matrix_horizontal_split(MaL, MaR, E, PAR_M, PAR_N - PAR_R, PAR_R);

    /* Overwrite 'A_main_shr[k][j]' with 'S_main_shr[k][j] = R * MaR_main_shr_k_j + A_main_shr[k][j]'. */
    matrix_product(broadcast->S, mpc_challenge->R, MaR, PAR_S, PAR_M, PAR_R);
    matrix_add(broadcast->S, share->unif.A, PAR_S, PAR_R);

    if(entire_computation) {
        /* Compute 'R * MaL_main_shr_k_j'. */
        ff_t RxMaL[matrix_bytes_size(PAR_S, PAR_N - PAR_R)];
        matrix_product(RxMaL, mpc_challenge->R, MaL, PAR_S, PAR_M, PAR_N - PAR_R);

        /* Set V_main_shr_k_j = S * K_main_shr[k][j] - R * MaL_main_shr_k_j - C_main_shr_k_j. */
        matrix_product(broadcast->V, plain_br->S, share->wit.K, PAR_S, PAR_R, PAR_N - PAR_R);
        matrix_subtract(broadcast->V, RxMaL, PAR_S, PAR_N - PAR_R);
        matrix_subtract(broadcast->V, share->corr.C, PAR_S, PAR_N - PAR_R);
    } else {
        matrix_init_zero(broadcast->V, PAR_S, PAR_N - PAR_R);
    }
}

void mpc_compute_plain_broadcast(vec_broadcast_t* plain_br, vec_challenge_t* mpc_challenge,
                                vec_share_t const* share, const instance_t* inst) {
    run_multiparty_computation(plain_br, mpc_challenge, share, plain_br, inst, 1, 0);
}

void mpc_compute_communications(vec_broadcast_t* broadcast, vec_challenge_t* mpc_challenge,
                                vec_share_t const* share, vec_broadcast_t const* plain_br,
                                const instance_t* inst, char has_sharing_offset) {
    run_multiparty_computation(broadcast, mpc_challenge, share, plain_br, inst, has_sharing_offset, 1);
}

void mpc_compute_communications_inverse(vec_share_t* share, vec_challenge_t* mpc_challenge,
                                const vec_broadcast_t* broadcast, vec_broadcast_t const* plain_br,
                                const instance_t* inst, char has_sharing_offset) {

    (void) share;
    (void) mpc_challenge;
    (void) broadcast;
    (void) plain_br;
    (void) inst;
    (void) has_sharing_offset;
}

void expand_mpc_challenge_hash(vec_challenge_t** challenges, const uint8_t* digest, size_t nb, instance_t* inst) {

    xof_context entropy_ctx;
    xof_init(&entropy_ctx);
    xof_update(&entropy_ctx, digest, PARAM_DIGEST_SIZE);
    xof_final(&entropy_ctx);
    samplable_t entropy = xof_to_samplable(&entropy_ctx);

    (void) inst;
    for(size_t num=0; num<nb; num++) {
        matrix_init_random(challenges[num]->R, PAR_S, PAR_M, &entropy);
    } 
}
