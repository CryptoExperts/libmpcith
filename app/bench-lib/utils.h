#ifndef LIBMPCITH_BENCH_UTILS_H
#define LIBMPCITH_BENCH_UTILS_H

#include <stdio.h>
#include "parameters-all.h"

static inline int get_positive_integer_from_console(int argc, char *argv[], int arg_position, int default_value) {
    int value = default_value;
    if(argc > arg_position) {
        if( sscanf(argv[arg_position], "%d", &value) != 1) {
            printf("Integer awaited.\n");
            return -1;
        } else if( value <= 0 ) {
            printf("Need to positive integer.\n");
            return -1;
        }
    }
    return value;
}

static inline int get_number_of_tests(int argc, char *argv[], int default_value) {
    return get_positive_integer_from_console(argc, argv, 1, default_value);
}

static inline void print_configuration(void) {
    printf("===== MPCITH CONFIG =====\n");
#ifdef PARAM_LABEL
    printf("Label: " PARAM_LABEL "\n");
#else
    printf("Label: ?\n");
#endif
#ifdef PARAM_VARIANT
    printf("Variant: " PARAM_VARIANT "\n");
#else
    printf("Variant: ?\n");
#endif
    printf("Selected security level: %d\n", PARAM_SECURITY);
    printf("Instruction Sets:");
#ifdef __SSE__
    printf(" SSE");
#endif
#ifdef __AVX__
    printf(" AVX");
#endif
    printf("\n");
#ifndef NDEBUG
    printf("Debug: On\n");
#else
    printf("Debug: Off\n");
#endif
}

#endif /* LIBMPCITH_BENCH_UTILS_H */
