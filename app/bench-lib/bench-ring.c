#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

#include <stdio.h>
#include <math.h>
#include "timing.h"

#include "api.h"
#include "bench-ring.h"

int randombytes(unsigned char* x, unsigned long long xlen) {
    for(unsigned long long j=0; j<xlen; j++)
        x[j] = (uint8_t) rand();
    return 0;
}

#ifdef BENCHMARK
btimer_t timers[NUMBER_OF_BENCHES];
#endif

#ifndef API_SIGN_RING
#error The API for ring signatures is not found: did you select the right API?
#else
int run_benchmark(int nb_users, int nb_tests, int nb_tests_by_setup, btimer_t timers_algos[NUMBER_OF_ALGO_BENCHES], double std_timer[NUMBER_OF_ALGO_BENCHES], double* mean_of_sig_size, double* std_sig_size) {
    srand((unsigned int) time(NULL));
    int ret;

    // Initialisation
    double timer_pow2[NUMBER_OF_ALGO_BENCHES];
    for(int j=0; j<NUMBER_OF_ALGO_BENCHES; j++) {
        btimer_init(&timers_algos[j]);
        timer_pow2[j] = 0;
    }
    #ifdef BENCHMARK
    for(int num=0; num<NUMBER_OF_BENCHES; num++)
        btimer_init(&timers[num]);
    #endif

    *mean_of_sig_size = 0;
    double sig_size_pow2 = 0;

    // Execution
    int score = 0;
    uint8_t* dynamic_memory = malloc(
        2*sizeof(uint8_t*)*nb_users
            + (CRYPTO_PUBLICKEYBYTES+CRYPTO_SECRETKEYBYTES)*nb_users
    );
    #define MLEN 32
    uint32_t signature_size = crypto_rsign_get_size(nb_users);
    uint8_t* sm = (uint8_t*) malloc(MLEN + signature_size);
    for(int i=0; i<nb_tests; i++) {
        #ifdef BENCHMARK
        for(int num=0; num<NUMBER_OF_BENCHES; num++)
            btimer_count(&timers[num]);
        #endif
        // Generate the keys
        uint8_t pp[CRYPTO_PUBLICPARAMBYTES];
        uint8_t* mem_buf = dynamic_memory;
        uint8_t** all_pk = (uint8_t**) mem_buf;
        mem_buf += sizeof(uint8_t*)*nb_users;
        uint8_t** all_sk = (uint8_t**) mem_buf;
        mem_buf += sizeof(uint8_t*)*nb_users;
        for(int num_user=0; num_user<nb_users; num_user++) {
            all_pk[num_user] = mem_buf;
            mem_buf += CRYPTO_PUBLICKEYBYTES;
            all_sk[num_user] = mem_buf;
            mem_buf += CRYPTO_SECRETKEYBYTES;
        }
        btimer_start(&timers_algos[B_KEY_GENERATION]);
        ret = crypto_rsign_setup(pp);
        if(ret) {
            printf("Failure (num %d): crypto_sign_setup\n", i);
            continue;
        }
        int error = 0;
        for(int num_user=0; num_user<nb_users; num_user++) {
            ret = crypto_rsign_keypair(all_pk[num_user], all_sk[num_user], pp);
            if(ret) {
                printf("Failure (num %d): crypto_sign_setup\n", i);
                error = 1;
                break;
            }
        }
        if(error)
            continue;
        btimer_end(&timers_algos[B_KEY_GENERATION]);
        btimer_count(&timers_algos[B_KEY_GENERATION]);
        timer_pow2[B_KEY_GENERATION] += pow(btimer_diff(&timers_algos[B_KEY_GENERATION]), 2) / nb_tests;
        if(ret) {
            printf("Failure (num %d): crypto_sign_keypair\n", i);
            continue;
        }

        for(int j=0; j<nb_tests_by_setup; j++) {
            // Select the message
            uint8_t m[MLEN] = {1, 2, 3, 4};
            uint8_t m2[MLEN] = {0};
            unsigned long long m2len = 0;
            uint32_t selected_user = rand() % nb_users; // Bias distribution (not very important, it is just for testing)

            // Sign the message
            unsigned long long smlen;
            btimer_start(&timers_algos[B_SIGN_ALGO]);
            ret = crypto_rsign(sm, &smlen, m, MLEN, all_sk[selected_user], (uint8_t const* const*) all_pk, nb_users);
            btimer_end(&timers_algos[B_SIGN_ALGO]);
            btimer_count(&timers_algos[B_SIGN_ALGO]);
            timer_pow2[B_SIGN_ALGO] += pow(btimer_diff(&timers_algos[B_SIGN_ALGO]), 2) / (nb_tests*nb_tests_by_setup);
            // Update statistics
            size_t signature_len = smlen - MLEN;
            *mean_of_sig_size += (double) signature_len / (nb_tests*nb_tests_by_setup);
            sig_size_pow2 += pow(signature_len, 2) / (nb_tests*nb_tests_by_setup);
            if(ret) {
                printf("Failure (num %d): crypto_sign\n", i);
                continue;
            }

            // Verify/Open the signature
            btimer_start(&timers_algos[B_VERIFY_ALGO]);
            ret = crypto_rsign_open(m2, &m2len, sm, smlen, (uint8_t const* const*) all_pk, nb_users);
            btimer_end(&timers_algos[B_VERIFY_ALGO]); btimer_count(&timers_algos[B_VERIFY_ALGO]);
            timer_pow2[B_VERIFY_ALGO] += pow(btimer_diff(&timers_algos[B_VERIFY_ALGO]), 2) / (nb_tests*nb_tests_by_setup);
            if(ret) {
                printf("Failure (num %d): crypto_sign_open\n", i);
                continue;
            }
            
            // Test of correction of the primitives
            if(m2len != MLEN) {
                printf("Failure (num %d): message size does not match\n", i);
                continue;
            }
            for(int h=0; h<MLEN; h++)
                if(m[h] != m2[h]) {
                    printf("Failure (num %d): message does not match (char %d)\n", i, h);
                    continue;
                }

            score++;
        }
    }
    free(dynamic_memory);
    free(sm);

    // Compute some statistics
    std_timer[B_KEY_GENERATION] = sqrt(timer_pow2[B_KEY_GENERATION] - pow(btimer_get(&timers_algos[B_KEY_GENERATION]),2));
    std_timer[B_SIGN_ALGO] = sqrt(timer_pow2[B_SIGN_ALGO] - pow(btimer_get(&timers_algos[B_SIGN_ALGO]),2));
    std_timer[B_VERIFY_ALGO] = sqrt(timer_pow2[B_VERIFY_ALGO] - pow(btimer_get(&timers_algos[B_VERIFY_ALGO]),2));
    *std_sig_size = sqrt(sig_size_pow2 - pow(*mean_of_sig_size,2));
    return score;
}
#endif
