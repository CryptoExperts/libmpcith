#ifndef PARAMETERS_H
#define PARAMETERS_H

#ifdef PARAM_L1
#define PARAM_SECURITY 128
#endif
#ifdef PARAM_L3
#define PARAM_SECURITY 192
#endif
#ifdef PARAM_L5
#define PARAM_SECURITY 256
#endif
#ifndef PARAM_SECURITY
#error "Security level not identified"
#endif

// Hash Domain Separation
#define HASH_PREFIX_COMMITMENT 0
#define HASH_PREFIX_FIRST_CHALLENGE 1
#define HASH_PREFIX_SECOND_CHALLENGE 2
#define HASH_PREFIX_THIRD_CHALLENGE 3
#define HASH_PREFIX_SEED_TREE 4

/********************************************
 *          OW Function Parameters          *
 ********************************************/

#define PARAM_FIELD_SIZE 16
#if PARAM_SECURITY == 128
#if defined(PARAM_AGGRESSIVE)
#define PAR_Q PARAM_FIELD_SIZE
#define PAR_M 15
#define PAR_N 15
#define PAR_K 78
#define PAR_R 6
#elif defined(PARAM_CONSERVATIVE)
#define PAR_Q PARAM_FIELD_SIZE
#define PAR_M 16
#define PAR_N 16
#define PAR_K 142
#define PAR_R 4
#endif

#elif PARAM_SECURITY == 192
#if defined(PARAM_AGGRESSIVE)
#define PAR_Q PARAM_FIELD_SIZE
#define PAR_M 19
#define PAR_N 19
#define PAR_K 109
#define PAR_R 8
#elif defined(PARAM_CONSERVATIVE)
#define PAR_Q PARAM_FIELD_SIZE
#define PAR_M 19
#define PAR_N 19
#define PAR_K 167
#define PAR_R 6
#endif

#elif PARAM_SECURITY == 256
#if defined(PARAM_AGGRESSIVE)
#define PAR_Q PARAM_FIELD_SIZE
#define PAR_M 21
#define PAR_N 21
#define PAR_K 189
#define PAR_R 7
#elif defined(PARAM_CONSERVATIVE)
#define PAR_Q PARAM_FIELD_SIZE
#define PAR_M 22
#define PAR_N 22
#define PAR_K 254
#define PAR_R 6
#endif

#endif

/********************************************
 *           Signature Parameters           *
 ********************************************/

#if defined(PARAM_TCITH_GGM_LIFTING_5R)
#if defined(PARAM_FAST)
#define PARAM_HYPERCUBE_DIMENSION 1
#else
#define PARAM_HYPERCUBE_DIMENSION 2
#endif
#endif

#if PARAM_SECURITY == 128
    #define PARAM_SALT_SIZE (256/8)
    #define PARAM_SEED_SIZE (128/8)
    #define PARAM_DIGEST_SIZE (256/8)

    #if defined(PARAM_FAST)
    #if defined(PARAM_AGGRESSIVE)
    #define PARAM_LABEL "mirith-L1a-fast"
    #elif defined(PARAM_CONSERVATIVE)
    #define PARAM_LABEL "mirith-L1b-fast"
    #endif
    #if defined(PARAM_TRADITIONAL_5R) || defined(PARAM_HYPERCUBE_5R) || defined(PARAM_TCITH_GGM_LIFTING_5R)
        #define PARAM_NB_EXECUTIONS 39
        #define PARAM_NB_PARTIES 16
        #define PARAM_LOG_NB_PARTIES 4
        #define PAR_S 5
    #endif
    #else
    #if defined(PARAM_AGGRESSIVE)
    #define PARAM_LABEL "mirith-L1a-short"
    #elif defined(PARAM_CONSERVATIVE)
    #define PARAM_LABEL "mirith-L1b-short"
    #endif
    #if defined(PARAM_TRADITIONAL_5R) || defined(PARAM_HYPERCUBE_5R) || defined(PARAM_TCITH_GGM_LIFTING_5R)
        #define PARAM_NB_EXECUTIONS 19
        #define PARAM_NB_PARTIES 256
        #define PARAM_LOG_NB_PARTIES 8
        #define PAR_S 9
    #endif
    #endif

#elif PARAM_SECURITY == 192
    #define PARAM_SALT_SIZE (256/8)
    #define PARAM_SEED_SIZE (192/8)
    #define PARAM_DIGEST_SIZE (384/8)

    #if defined(PARAM_FAST)
    #if defined(PARAM_AGGRESSIVE)
    #define PARAM_LABEL "mirith-L3a-fast"
    #elif defined(PARAM_CONSERVATIVE)
    #define PARAM_LABEL "mirith-L3b-fast"
    #endif
    #if defined(PARAM_TRADITIONAL_5R) || defined(PARAM_HYPERCUBE_5R) || defined(PARAM_TCITH_GGM_LIFTING_5R)
        #define PARAM_NB_EXECUTIONS 55
        #define PARAM_NB_PARTIES 16
        #define PARAM_LOG_NB_PARTIES 4
        #define PAR_S 7
    #endif
    #else
    #if defined(PARAM_AGGRESSIVE)
    #define PARAM_LABEL "mirith-L3a-short"
    #elif defined(PARAM_CONSERVATIVE)
    #define PARAM_LABEL "mirith-L3b-short"
    #endif
    #if defined(PARAM_TRADITIONAL_5R) || defined(PARAM_HYPERCUBE_5R) || defined(PARAM_TCITH_GGM_LIFTING_5R)
        #define PARAM_NB_EXECUTIONS 29
        #define PARAM_NB_PARTIES 256
        #define PARAM_LOG_NB_PARTIES 8
        #define PAR_S 9
    #endif
    #endif

#elif PARAM_SECURITY == 256
    #define PARAM_SALT_SIZE (256/8)
    #define PARAM_SEED_SIZE (256/8)
    #define PARAM_DIGEST_SIZE (512/8)

    #if defined(PARAM_FAST)
    #if defined(PARAM_AGGRESSIVE)
    #define PARAM_LABEL "mirith-L5a-fast"
    #elif defined(PARAM_CONSERVATIVE)
    #define PARAM_LABEL "mirith-L5b-fast"
    #endif
    #if defined(PARAM_TRADITIONAL_5R) || defined(PARAM_HYPERCUBE_5R) || defined(PARAM_TCITH_GGM_LIFTING_5R)
        #define PARAM_NB_EXECUTIONS 74
        #define PARAM_NB_PARTIES 16
        #define PARAM_LOG_NB_PARTIES 4
        #define PAR_S 7
    #endif
    #else
    #if defined(PARAM_AGGRESSIVE)
    #define PARAM_LABEL "mirith-L5a-short"
    #elif defined(PARAM_CONSERVATIVE)
    #define PARAM_LABEL "mirith-L5b-short"
    #endif
    #if defined(PARAM_TRADITIONAL_5R) || defined(PARAM_HYPERCUBE_5R) || defined(PARAM_TCITH_GGM_LIFTING_5R)
        #define PARAM_NB_EXECUTIONS 38
        #define PARAM_NB_PARTIES 256
        #define PARAM_LOG_NB_PARTIES 8
        #define PAR_S 10
    #endif
    #endif

#endif

#endif /* PARAMETERS_H */
