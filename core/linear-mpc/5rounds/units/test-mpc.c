#include "witness.h"
#include "mpc-all.h"
#include "test-mpc.h"

int randombytes(unsigned char* x, unsigned long long xlen);

int __mpc_test_plain_computation(samplable_t* entropy, char* last_error, int direct) {
    instance_t* inst;
    solution_t* wit;
    // Generation instance with solution
    generate_instance_with_solution(&inst, &wit, entropy);
    uncompress_instance(inst);
    // So far, we already the correctness of the above functions

    vec_share_t* plain_input = new_share();
    vec_wit_set(get_wit(plain_input), wit);
    vec_unif_rnd(get_unif(plain_input), entropy);
    compute_correlated(get_corr(plain_input), wit, get_unif(plain_input), inst);

    uint8_t* digest = malloc(PARAM_DIGEST_SIZE);
    randombytes(digest, PARAM_DIGEST_SIZE);
    vec_challenge_t* challenge = new_challenge();
    expand_mpc_challenge_hash(&challenge, digest, 1, inst);
    free(digest);

    vec_broadcast_t* plain_br = new_br();
    if(direct)
        mpc_compute_plain_broadcast(plain_br, challenge, plain_input, inst);
    else
        mpc_compute_communications(plain_br, challenge, plain_input, plain_br, inst, 1);
    free_instance(inst);
    free_instance_solution(wit);
    free(plain_input);
    free(challenge);

    if(!is_valid_plain_broadcast(plain_br)) {
        free(plain_br);
        strcpy(last_error, "Invalid plain broadcast.");
        return -1;
    }

    free(plain_br);
    return 0;
}


int mpc_test_plain_computation(samplable_t* entropy, char* last_error) {
    return __mpc_test_plain_computation(entropy, last_error, 1);
}

int mpc_test_plain_computation_2(samplable_t* entropy, char* last_error) {
    return __mpc_test_plain_computation(entropy, last_error, 0);
}

int __mpc_test_multiparty_computation(samplable_t* entropy, char* last_error, int with_random_shares) {
    instance_t* inst;
    solution_t* wit;
    // Generation instance with solution
    generate_instance_with_solution(&inst, &wit, entropy);
    uncompress_instance(inst);
    // So far, we already the correctness of the above functions

    vec_share_t* plain_input = new_share();
    vec_wit_set(get_wit(plain_input), wit);
    vec_unif_rnd(get_unif(plain_input), entropy);
    compute_correlated(get_corr(plain_input), wit, get_unif(plain_input), inst);

    uint8_t* digest = malloc(PARAM_DIGEST_SIZE);
    randombytes(digest, PARAM_DIGEST_SIZE);
    vec_challenge_t* challenge = new_challenge();
    expand_mpc_challenge_hash(&challenge, digest, 1, inst);
    free(digest);

    vec_broadcast_t* plain_br = new_br();
    mpc_compute_plain_broadcast(plain_br, challenge, plain_input, inst);

    uint16_t nb_parties = 3;

    vec_share_t** shares = (vec_share_t**) malloc(nb_parties*sizeof(vec_share_t*));

    vec_share_t* acc = new_share();
    vec_share_setzero(acc);
    for(uint16_t i=0; i<nb_parties; i++) {
        shares[i] = new_share();
        if(i != nb_parties-1) {
            if(with_random_shares)
                vec_share_rnd(shares[i], entropy);
            else
                vec_share_setzero(shares[i]);
            vec_share_add(acc, shares[i]);
        } else {
            vec_share_set(shares[i], plain_input);
            vec_share_sub(shares[i], acc);
            vec_share_normalize(shares[i]);
        }
    }
    free(acc);

    vec_broadcast_t* broadcast = new_br();
    vec_broadcast_t* plain_br2 = new_br();
    vec_br_setzero(plain_br2);
    for(uint16_t i=0; i<nb_parties; i++) {
        mpc_compute_communications(broadcast, challenge, shares[i], plain_br, inst, (i==nb_parties-1));
        vec_br_add(plain_br2, broadcast);
    }
    vec_br_normalize(plain_br2);
    free(broadcast);

    int ret = 0;
    if(memcmp(plain_br, plain_br2, PARAM_BR_SIZE) != 0) {
        strcpy(last_error, "Recomposed plain broadcast differs from the original plain broadcast.");
        ret = -1;
    }

    for(uint16_t i=0; i<nb_parties; i++)
        free(shares[i]);  
    free(shares);  

    free(plain_input);
    free(plain_br);
    free(plain_br2);
    free(challenge);

    free_instance(inst);
    free_instance_solution(wit);
    return ret;
}

int mpc_test_multiparty_computation_with_zero(samplable_t* entropy, char* last_error) {
    return __mpc_test_multiparty_computation(entropy, last_error, 0);
}

int mpc_test_multiparty_computation_with_randomness(samplable_t* entropy, char* last_error) {
    return __mpc_test_multiparty_computation(entropy, last_error, 1);
}

int __mpc_test_serialization_plain_broadcast(samplable_t* entropy, char* last_error, int num_test) {
    instance_t* inst;
    solution_t* wit;
    // Generation instance with solution
    generate_instance_with_solution(&inst, &wit, entropy);
    uncompress_instance(inst);

    vec_share_t* plain_input = new_share();
    vec_wit_set(get_wit(plain_input), wit);
    vec_unif_rnd(get_unif(plain_input), entropy);
    compute_correlated(get_corr(plain_input), wit, get_unif(plain_input), inst);

    uint8_t* digest = malloc(PARAM_DIGEST_SIZE);
    randombytes(digest, PARAM_DIGEST_SIZE);
    vec_challenge_t* challenge = new_challenge();
    expand_mpc_challenge_hash(&challenge, digest, 1, inst);
    free(digest);

    vec_broadcast_t* plain_br = new_br();
    mpc_compute_plain_broadcast(plain_br, challenge, plain_input, inst);
    free_instance(inst);
    free_instance_solution(wit);
    free(plain_input);
    free(challenge);
    // So far, we have a valid plain broadcast (from the previous unit tests)

    uint8_t buffer[PARAM_COMPRESSED_BR_SIZE];
    compress_plain_broadcast(buffer, plain_br);

    vec_broadcast_t* plain_br2 = new_br();
    uncompress_plain_broadcast(plain_br2, buffer);
    int ret = 0;
    if(num_test == 0)
        ret = (is_valid_plain_broadcast(plain_br2) == 0);
    else
        ret = (memcmp(plain_br, plain_br2, PARAM_BR_SIZE) != 0);
    free(plain_br);
    
    if(ret) {
        strcpy(last_error, "Failure.");
        return -1;
    }
    return 0;
}

int mpc_test_serialization_plain_broadcast_output_validity(samplable_t* entropy, char* last_error) {
    return __mpc_test_serialization_plain_broadcast(entropy, last_error, 0);
}

int mpc_test_serialization_plain_broadcast_correctness(samplable_t* entropy, char* last_error) {
    return __mpc_test_serialization_plain_broadcast(entropy, last_error, 1);
}

int mpc_test_mpc_inversion(samplable_t* entropy, char* last_error) {
    instance_t* inst;
    solution_t* wit;
    // Generation instance with solution
    generate_instance_with_solution(&inst, &wit, entropy);
    uncompress_instance(inst);
    // So far, we already the correctness of the above functions

    vec_share_t* plain_input = new_share();
    vec_wit_set(get_wit(plain_input), wit);
    vec_unif_rnd(get_unif(plain_input), entropy);
    compute_correlated(get_corr(plain_input), wit, get_unif(plain_input), inst);

    uint8_t* digest = malloc(PARAM_DIGEST_SIZE);
    randombytes(digest, PARAM_DIGEST_SIZE);
    vec_challenge_t* challenge = new_challenge();
    expand_mpc_challenge_hash(&challenge, digest, 1, inst);
    free(digest);

    vec_broadcast_t* plain_br = new_br();
    mpc_compute_plain_broadcast(plain_br, challenge, plain_input, inst);

    uint16_t nb_parties = 3;

    vec_share_t** shares = (vec_share_t**) malloc(nb_parties*sizeof(vec_share_t*));

    vec_share_t* acc = new_share();
    vec_share_setzero(acc);
    for(uint16_t i=0; i<nb_parties; i++) {
        shares[i] = new_share();
        if(i != nb_parties-1) {
            vec_share_rnd(shares[i], entropy);
            vec_share_add(acc, shares[i]);
        } else {
            vec_share_set(shares[i], plain_input);
            vec_share_sub(shares[i], acc);
            vec_share_normalize(shares[i]);
        }
    }
    free(acc);

    int ret = 0;
    vec_broadcast_t* broadcast = new_br();
    vec_share_t* recomputed_share = new_share();
    for(uint16_t i=0; i<nb_parties; i++) {
        mpc_compute_communications(broadcast, challenge, shares[i], plain_br, inst, (i==nb_parties-1));
        memcpy(&recomputed_share->wit, &shares[i]->wit, PARAM_WIT_SIZE);
        mpc_compute_communications_inverse(recomputed_share, challenge, broadcast, plain_br, inst, (i==nb_parties-1));
        if(memcmp(shares[i], recomputed_share, PARAM_SHARE_SIZE) != 0) {
            strcpy(last_error, "Failure.");
            ret = -1;
        }
    }
    free(broadcast);
    free(recomputed_share);
    free(challenge);

    for(uint16_t i=0; i<nb_parties; i++)
        free(shares[i]);  
    free(shares);  

    free(plain_input);
    free(plain_br);

    free_instance(inst);
    free_instance_solution(wit);
    return ret;
}
