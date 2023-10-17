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

#if PARAM_SECURITY == 128
#define RYDE_PARAM_M 31
#define RYDE_PARAM_N 33
#define RYDE_PARAM_K 15
#define RYDE_PARAM_W 10

#elif PARAM_SECURITY == 192
#define RYDE_PARAM_M 37
#define RYDE_PARAM_N 41
#define RYDE_PARAM_K 18
#define RYDE_PARAM_W 13

#elif PARAM_SECURITY == 256
#define RYDE_PARAM_M 43
#define RYDE_PARAM_N 47
#define RYDE_PARAM_K 18
#define RYDE_PARAM_W 17

#endif

#define RYDE_VEC_BYTES(s) (((s)*RYDE_PARAM_M+7)>>3)

/********************************************
 *           Signature Parameters           *
 ********************************************/

#if PARAM_SECURITY == 128
    #define PARAM_SALT_SIZE (256/8)
    #define PARAM_SEED_SIZE (128/8)
    #define PARAM_DIGEST_SIZE (256/8)

    #if defined(PARAM_FAST)
    #define PARAM_LABEL "ryde-L1-fast"
    #if defined(PARAM_TRADITIONAL_5R) || defined(PARAM_HYPERCUBE_5R)
        #define PARAM_NB_EXECUTIONS 30
        #define PARAM_NB_PARTIES 32
        #define PARAM_LOG_NB_PARTIES 5
    #endif
    #else
    #define PARAM_LABEL "ryde-L1-short"
    #if defined(PARAM_TRADITIONAL_5R) || defined(PARAM_HYPERCUBE_5R)
        #define PARAM_NB_EXECUTIONS 20
        #define PARAM_NB_PARTIES 256
        #define PARAM_LOG_NB_PARTIES 8
    #endif
    #endif

#elif PARAM_SECURITY == 192
    #define PARAM_SALT_SIZE (256/8)
    #define PARAM_SEED_SIZE (192/8)
    #define PARAM_DIGEST_SIZE (384/8)

    #if defined(PARAM_FAST)
    #define PARAM_LABEL "ryde-L3-fast"
    #if defined(PARAM_TRADITIONAL_5R) || defined(PARAM_HYPERCUBE_5R)
        #define PARAM_NB_EXECUTIONS 44
        #define PARAM_NB_PARTIES 32
        #define PARAM_LOG_NB_PARTIES 5
    #endif
    #else
    #define PARAM_LABEL "ryde-L3-short"
    #if defined(PARAM_TRADITIONAL_5R) || defined(PARAM_HYPERCUBE_5R)
        #define PARAM_NB_EXECUTIONS 29
        #define PARAM_NB_PARTIES 256
        #define PARAM_LOG_NB_PARTIES 8
    #endif
    #endif

#elif PARAM_SECURITY == 256
    #define PARAM_SALT_SIZE (256/8)
    #define PARAM_SEED_SIZE (256/8)
    #define PARAM_DIGEST_SIZE (512/8)

    #if defined(PARAM_FAST)
    #define PARAM_LABEL "ryde-L5-fast"
    #if defined(PARAM_TRADITIONAL_5R) || defined(PARAM_HYPERCUBE_5R)
        #define PARAM_NB_EXECUTIONS 58
        #define PARAM_NB_PARTIES 32
        #define PARAM_LOG_NB_PARTIES 5
    #endif
    #else
    #define PARAM_LABEL "ryde-L5-short"
    #if defined(PARAM_TRADITIONAL_5R) || defined(PARAM_HYPERCUBE_5R)
        #define PARAM_NB_EXECUTIONS 38
        #define PARAM_NB_PARTIES 256
        #define PARAM_LOG_NB_PARTIES 8
    #endif
    #endif

#endif

#endif /* PARAMETERS_H */
