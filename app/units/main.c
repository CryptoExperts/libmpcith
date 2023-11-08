#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "sample.h"
#include "mpc-def.h"
#include "run-all-tests.h"

#ifndef MPC_DEBUG
#define DEFAULT_NB_TESTS 1000
#else
#define DEFAULT_NB_TESTS 10
#endif

int randombytes(unsigned char* x, unsigned long long xlen) {
    for(unsigned long long j=0; j<xlen; j++)
        x[j] = rand();
    return 0;
}

int main(int argc, char *argv[]) {
    srand((unsigned int) time(NULL));

    // Get the number of experiments
    int nb_tests = DEFAULT_NB_TESTS;
    if(argc == 2) {
        if( sscanf(argv[1], "%d", &nb_tests) != 1) {
            printf("Integer awaited.\n");
            exit(EXIT_FAILURE);
        } else if( nb_tests <= 0 ) {
            printf("Need to positive integer.\n");
            exit(EXIT_FAILURE);
        }
    }

    #ifdef MPC_DEBUG
    reset_mpc_log();
    #endif

    // Build a fresh entropy source
    uint8_t master_seed[PARAM_SEED_SIZE];
    sample_seed(master_seed);
    prg_context entropy_ctx;
    prg_init(&entropy_ctx, master_seed, NULL);
    samplable_t entropy = prg_to_samplable(&entropy_ctx);

    run_all_tests(nb_tests, &entropy);    
    return 0;
}
