#include <stdio.h>
#include "bench-ring.h"
#include "utils.h"
#include "api.h"

#ifdef BENCHMARK_CYCLES
#define display_timer(label,num) printf("   - " label ": %f ms (%f cycles)\n", btimer_get(&timers[num]), btimer_get_cycles(&timers[num]))
#else
#define display_timer(label,num) printf("   - " label ": %f ms\n", btimer_get(&timers[num]))
#endif

int main(int argc, char *argv[]) {

    int nb_tests = get_number_of_tests(argc, argv, 1);
    if(nb_tests < 0)
        exit(EXIT_FAILURE);

    int nb_users = get_positive_integer_from_console(argc, argv, 2, 100);
    if(nb_users < 0)
        exit(EXIT_FAILURE);

    int nb_tests_by_setup = get_positive_integer_from_console(argc, argv, 3, 1);
    if(nb_tests_by_setup < 0)
        exit(EXIT_FAILURE);

    print_configuration();
    printf("\n");

    btimer_t timers_algos[NUMBER_OF_ALGO_BENCHES];
    double std_timer[NUMBER_OF_ALGO_BENCHES];
    double mean_of_sig_size;
    double std_sig_size;

    int score = run_benchmark(nb_users, nb_tests, nb_tests_by_setup, timers_algos, std_timer, &mean_of_sig_size, &std_sig_size);

    // Display Infos
    printf("===== SUMMARY =====\n");
    printf("Type: ring signature\n");
    printf("Nb users: %d\n", nb_users);
    printf("Validation: %d/%d\n", score, nb_tests*nb_tests_by_setup);
    printf("Key Gen: %.2f ms (std=%.2f)\n",
        btimer_get(&timers_algos[B_KEY_GENERATION]),
        std_timer[B_KEY_GENERATION]
    );
    printf("Sign:    %.2f ms (std=%.2f)\n",
        btimer_get(&timers_algos[B_SIGN_ALGO]),
        std_timer[B_SIGN_ALGO]
    );
    printf("Verify:  %.2f ms (std=%.2f)\n",
        btimer_get(&timers_algos[B_VERIFY_ALGO]),
        std_timer[B_VERIFY_ALGO]
    );
    #ifdef BENCHMARK_CYCLES
    printf("Key Gen: %.2f cycles\n", btimer_get_cycles(&timers_algos[B_KEY_GENERATION]));
    printf("Sign:    %.2f cycles\n", btimer_get_cycles(&timers_algos[B_SIGN_ALGO]));
    printf("Verify:  %.2f cycles\n", btimer_get_cycles(&timers_algos[B_VERIFY_ALGO]));
    #endif
    long signature_size = crypto_rsign_get_size(nb_users);
    printf(" -> Signature size (MAX): %ld B (%.2f KB)\n", signature_size, signature_size/1024.);
    printf(" -> Signature size: %.2f KB (std=%.2f)\n", mean_of_sig_size/1024., std_sig_size/1024.);
    printf(" -> Signature size: %.0f B (std=%.0f)\n", mean_of_sig_size, std_sig_size);
    printf(" -> PK size: %ld B\n", CRYPTO_PUBLICKEYBYTES);
    printf(" -> SK size: %ld B\n", CRYPTO_SECRETKEYBYTES);

    #ifdef BENCHMARK
    printf("\n===== DETAILED BENCHMARK =====\n");
    printf(" - Signing\n");
    display_timer("Share Preparation", BS_SHARE_PREPARATION);
    display_timer("Tree Expansion", BS_TREE_EXPANSION);
    display_timer("Randomness Expansion", BS_RND_EXPANSION);
    display_timer("Commitment", BS_COMMITMENT);
    display_timer("Share Packing", BS_SHARE_PACKING);
    display_timer("Challenge", BS_CHALLENGE);
    display_timer("MPC Emulation", BS_MPC_EMULATION);
    display_timer("Build Signature", BS_BUILD_SIG);
    printf(" - Verification\n");
    display_timer("Share Recompute", BV_SHARE_RECOMPUTE);
    display_timer("Tree Expansion", BV_TREE_EXPANSION);
    display_timer("Randomness Expansion", BV_RND_EXPANSION);
    display_timer("Commitment", BV_COMMITMENT);
    display_timer("Share Packing", BV_SHARE_PACKING);
    display_timer("Challenge", BV_CHALLENGE);
    display_timer("MPC Emulation", BV_MPC_EMULATION);
    printf(" - Others\n");
    display_timer("Pin A", B_PIN_A);
    display_timer("Pin B", B_PIN_B);
    display_timer("Pin C", B_PIN_C);
    display_timer("Pin D", B_PIN_D);
    #endif

    return 0;
}
