#include <stdio.h>
#include "bench.h"
#include "utils.h"
#include "api.h"

#ifdef BENCHMARK_CYCLES
#define display_timer(label,num) printf(label ": %f\n", btimer_get(&timers[num])) \
printf(label "-cycles: %f\n", btimer_get_cycles(&timers[num]))
#else
#define display_timer(label,num) printf(label ": %f\n", btimer_get(&timers[num]))
#endif


int main(int argc, char *argv[]) {

    int nb_tests = get_number_of_tests(argc, argv, 1);
    if(nb_tests < 0)
        exit(EXIT_FAILURE);

    btimer_t timers_algos[NUMBER_OF_ALGO_BENCHES];
    double std_timer[NUMBER_OF_ALGO_BENCHES];
    double mean_of_sig_size;
    double std_sig_size;

    int score = run_benchmark(nb_tests, timers_algos, std_timer, &mean_of_sig_size, &std_sig_size);

    /**
     * CONFIGURATION
     */
#ifdef PARAM_LABEL
    printf("label: " PARAM_LABEL "\n");
#else
    printf("label: ?\n");
#endif
#ifdef PARAM_TRADITIONAL
    printf("variant: traditional\n");
#endif
#ifdef PARAM_HYPERCUBE
    printf("variant: hypercube\n");
#endif
#ifdef PARAM_THRESHOLD
  #ifndef PARAM_NFPR
    printf("variant: threshold\n");
  #else
    printf("variant: threshold-nfpr\n");
  #endif
#endif
    printf("security: %d\n", PARAM_SECURITY);
    printf("instruction-sets:");
#ifdef __SSE__
    printf(" sse");
#endif
#ifdef __AVX__
    printf(" avx");
#endif
    printf("\n");
#ifndef NDEBUG
    printf("debug: on\n");
#else
    printf("debug: off\n");
#endif

    /**
     * ALGOS
     */
    printf("score: %d/%d\n", score, nb_tests);
    printf("keygen: %.2f\n", btimer_get(&timers_algos[B_KEY_GENERATION]));
    printf("keygen-std: %.2f\n", std_timer[B_KEY_GENERATION]);
    printf("sign: %.2f\n", btimer_get(&timers_algos[B_SIGN_ALGO]));
    printf("sign-std: %.2f\n", std_timer[B_SIGN_ALGO]);
    printf("verify: %.2f\n", btimer_get(&timers_algos[B_VERIFY_ALGO]));
    printf("verify-std: %.2f\n", std_timer[B_VERIFY_ALGO]);
    printf("sig-size-max: %ld\n", CRYPTO_BYTES);
    printf("sig-size: %.0f\n", mean_of_sig_size);
    printf("sig-size-std: %.0f\n", std_sig_size);
    printf("pk-size: %ld\n", CRYPTO_PUBLICKEYBYTES);
    printf("sk-size: %ld\n", CRYPTO_SECRETKEYBYTES);

    #ifdef BENCHMARK
    display_timer("signing-share-preparation", BS_SHARE_PREPARATION);
    display_timer("signing-tree-expansion", BS_TREE_EXPANSION);
    display_timer("signing-rnd-expansion", BS_RND_EXPANSION);
    display_timer("signing-commit", BS_COMMITMENT);
    display_timer("signing-share-packing", BS_SHARE_PACKING);
    display_timer("signing-challenge", BS_CHALLENGE);
    display_timer("signing-mpc-emulation", BS_MPC_EMULATION);
    display_timer("signing-build", BS_BUILD_SIG);

    display_timer("verify-share-recompute", BV_SHARE_RECOMPUTE);
    display_timer("verify-tree-expansion", BV_TREE_EXPANSION);
    display_timer("verify-randomness-expansion", BV_RND_EXPANSION);
    display_timer("verify-commitment", BV_COMMITMENT);
    display_timer("verify-share-packing", BV_SHARE_PACKING);
    display_timer("verify-challenge", BV_CHALLENGE);
    display_timer("verify-mpc-emulation", BV_MPC_EMULATION);

    display_timer("mic-pin-a", B_PIN_A);
    display_timer("mic-pin-b", B_PIN_B);
    display_timer("mic-pin-c", B_PIN_C);
    display_timer("mic-pin-d", B_PIN_D);
    #endif

    return 0;
}
