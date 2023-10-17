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
#define MIRA_PARAM_M 16
#define MIRA_PARAM_N 16
#define MIRA_PARAM_K 120
#define MIRA_PARAM_R 5

#elif PARAM_SECURITY == 192
#define MIRA_PARAM_M 19
#define MIRA_PARAM_N 19
#define MIRA_PARAM_K 168
#define MIRA_PARAM_R 6

#elif PARAM_SECURITY == 256
#define MIRA_PARAM_M 23
#define MIRA_PARAM_N 22
#define MIRA_PARAM_K 271
#define MIRA_PARAM_R 6

#endif

#define MIRA_VEC_BYTES(s) (((s)*4+7)>>3)

/********************************************
 *           Signature Parameters           *
 ********************************************/

#if defined(PARAM_TCITH_GGM_LIFTING_5R)
#define PARAM_HYPERCUBE_DIMENSION 2
#endif

#if PARAM_SECURITY == 128
    #define PARAM_SALT_SIZE (256/8)
    #define PARAM_SEED_SIZE (128/8)
    #define PARAM_DIGEST_SIZE (256/8)

    #if defined(PARAM_FAST)
    #define PARAM_LABEL "mira-L1-fast"
    #if defined(PARAM_TRADITIONAL_5R) || defined(PARAM_HYPERCUBE_5R) || defined(PARAM_TCITH_GGM_LIFTING_5R)
        #define PARAM_NB_EXECUTIONS 28
        #define PARAM_NB_PARTIES 32
        #define PARAM_LOG_NB_PARTIES 5
    #endif
    #else
    #define PARAM_LABEL "mira-L1-short"
    #if defined(PARAM_TRADITIONAL_5R) || defined(PARAM_HYPERCUBE_5R) || defined(PARAM_TCITH_GGM_LIFTING_5R)
        #define PARAM_NB_EXECUTIONS 18
        #define PARAM_NB_PARTIES 256
        #define PARAM_LOG_NB_PARTIES 8
    #endif
    #endif

#elif PARAM_SECURITY == 192
    #define PARAM_SALT_SIZE (256/8)
    #define PARAM_SEED_SIZE (192/8)
    #define PARAM_DIGEST_SIZE (384/8)

    #if defined(PARAM_FAST)
    #define PARAM_LABEL "mira-L3-fast"
    #if defined(PARAM_TRADITIONAL_5R) || defined(PARAM_HYPERCUBE_5R) || defined(PARAM_TCITH_GGM_LIFTING_5R)
        #define PARAM_NB_EXECUTIONS 41
        #define PARAM_NB_PARTIES 32
        #define PARAM_LOG_NB_PARTIES 5
    #endif
    #else
    #define PARAM_LABEL "mira-L3-short"
    #if defined(PARAM_TRADITIONAL_5R) || defined(PARAM_HYPERCUBE_5R) || defined(PARAM_TCITH_GGM_LIFTING_5R)
        #define PARAM_NB_EXECUTIONS 26
        #define PARAM_NB_PARTIES 256
        #define PARAM_LOG_NB_PARTIES 8
    #endif
    #endif

#elif PARAM_SECURITY == 256
    #define PARAM_SALT_SIZE (256/8)
    #define PARAM_SEED_SIZE (256/8)
    #define PARAM_DIGEST_SIZE (512/8)

    #if defined(PARAM_FAST)
    #define PARAM_LABEL "mira-L5-fast"
    #if defined(PARAM_TRADITIONAL_5R) || defined(PARAM_HYPERCUBE_5R) || defined(PARAM_TCITH_GGM_LIFTING_5R)
        #define PARAM_NB_EXECUTIONS 54
        #define PARAM_NB_PARTIES 32
        #define PARAM_LOG_NB_PARTIES 5
    #endif
    #else
    #define PARAM_LABEL "mira-L5-short"
    #if defined(PARAM_TRADITIONAL_5R) || defined(PARAM_HYPERCUBE_5R) || defined(PARAM_TCITH_GGM_LIFTING_5R)
        #define PARAM_NB_EXECUTIONS 34
        #define PARAM_NB_PARTIES 256
        #define PARAM_LOG_NB_PARTIES 8
    #endif
    #endif

#endif

#endif /* PARAMETERS_H */
