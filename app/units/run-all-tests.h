#ifndef RUN_ALL_TEST_H
#define RUN_ALL_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include "sample.h"

#define NEW_TEST_SECTION(title) printf("=== " title " ===\n");
#define ADD_TEST(label, fnc)  { int score = 0; \
                                int warning = 0; \
                                strcpy(last_error, ""); \
                                for(int num_test=0; num_test<nb_tests; num_test++) { \
                                    int ret = fnc(entropy, last_error); \
                                    if(ret >= 0) score++; \
                                    if(ret > 0) warning = 1; \
                                } \
                                if(score == nb_tests) \
                                    if(warning) \
                                        printf(" - " label ": OK?\n"); \
                                    else \
                                        printf(" - " label ": OK\n"); \
                                else \
                                    printf(" - " label ": %d/%d\n", score, nb_tests); \
                                if(strlen(last_error) > 0) \
                                    printf("    Error: %s\n", last_error); \
                              }

void run_all_tests(int nb_tests, samplable_t* entropy);

#endif /* RUN_ALL_TEST_H */
