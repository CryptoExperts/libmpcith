#ifndef LIBMPCITH_BENCHMARK_H
#define LIBMPCITH_BENCHMARK_H

#define NUMBER_OF_BENCHES 19

// Signing
#define BS_SHARE_PREPARATION 0
#define BS_TREE_EXPANSION 1
#define BS_RND_EXPANSION 2
#define BS_COMMITMENT 3
#define BS_SHARE_PACKING 4
#define BS_CHALLENGE 5
#define BS_MPC_EMULATION 6
#define BS_BUILD_SIG 7

// Verification
#define BV_SHARE_RECOMPUTE 8
#define BV_TREE_EXPANSION 9
#define BV_RND_EXPANSION 10
#define BV_COMMITMENT 11
#define BV_SHARE_PACKING 12
#define BV_CHALLENGE 13
#define BV_MPC_EMULATION 14

// Others
#define B_PIN_A 15
#define B_PIN_B 16
#define B_PIN_C 17
#define B_PIN_D 18

#ifndef BENCHMARK
#define __BENCHMARK_START__(label) {}
#define __BENCHMARK_STOP__(label) {}

#else
#include "timing.h"
extern btimer_t timers[NUMBER_OF_BENCHES];
void btimer_start(btimer_t *timer);
void btimer_end(btimer_t *timer);

#define __BENCHMARK_START__(label) btimer_start(&timers[label])
#define __BENCHMARK_STOP__(label) btimer_end(&timers[label])
#endif

#endif /* LIBMPCITH_BENCHMARK_H */
