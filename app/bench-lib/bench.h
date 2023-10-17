#ifndef LIBMPCITH_BENCH_H
#define LIBMPCITH_BENCH_H

#include "timing.h"
#include "benchmark.h"

#define B_KEY_GENERATION 0
#define B_SIGN_ALGO 1
#define B_VERIFY_ALGO 2
#define NUMBER_OF_ALGO_BENCHES 3

int run_benchmark(int nb_tests, btimer_t timers_algos[NUMBER_OF_ALGO_BENCHES], double std_timer[NUMBER_OF_ALGO_BENCHES], double* mean_of_sig_size, double* std_sig_size);

#endif /* LIBMPCITH_BENCH_H */
