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

#define PARAM_BLOCK_SIZE (PARAM_SECURITY/8)

/********************************************
 *           Signature Parameters           *
 ********************************************/

#if PARAM_SECURITY == 128
    #define PARAM_SALT_SIZE (256/8)
    #define PARAM_SEED_SIZE (128/8)
    #define PARAM_DIGEST_SIZE (256/8)

    #ifdef PARAM_INST1
    #define PARAM_LABEL "aimer-L1-inst1"
    #if defined(PARAM_TRADITIONAL_5R) || defined(PARAM_HYPERCUBE_5R)
        #define PARAM_NB_EXECUTIONS 33
        #define PARAM_NB_PARTIES 16
        #define PARAM_LOG_NB_PARTIES 4
    #endif
    #endif
    #ifdef PARAM_INST2
    #define PARAM_LABEL "aimer-L1-inst2"
    #if defined(PARAM_TRADITIONAL_5R)
        #define PARAM_NB_EXECUTIONS 23
        #define PARAM_NB_PARTIES 57
        #define PARAM_LOG_NB_PARTIES 6
    #endif
    #endif
    #ifdef PARAM_INST3
    #define PARAM_LABEL "aimer-L1-inst3"
    #if defined(PARAM_TRADITIONAL_5R) || defined(PARAM_HYPERCUBE_5R)
        #define PARAM_NB_EXECUTIONS 17
        #define PARAM_NB_PARTIES 256
        #define PARAM_LOG_NB_PARTIES 8
    #endif
    #endif
    #ifdef PARAM_INST4
    #define PARAM_LABEL "aimer-L1-inst4"
    #if defined(PARAM_TRADITIONAL_5R)
        #define PARAM_NB_EXECUTIONS 13
        #define PARAM_NB_PARTIES 1615
        #define PARAM_LOG_NB_PARTIES 11
    #endif
    #endif

#elif PARAM_SECURITY == 192
    #define PARAM_SALT_SIZE (384/8)
    #define PARAM_SEED_SIZE (192/8)
    #define PARAM_DIGEST_SIZE (384/8)

    #ifdef PARAM_INST1
    #define PARAM_LABEL "aimer-L3-inst1"
    #if defined(PARAM_TRADITIONAL_5R) || defined(PARAM_HYPERCUBE_5R)
        #define PARAM_NB_EXECUTIONS 49
        #define PARAM_NB_PARTIES 16
        #define PARAM_LOG_NB_PARTIES 4
    #endif
    #endif
    #ifdef PARAM_INST2
    #define PARAM_LABEL "aimer-L3-inst2"
    #if defined(PARAM_TRADITIONAL_5R) || defined(PARAM_HYPERCUBE_5R)
        #define PARAM_NB_EXECUTIONS 33
        #define PARAM_NB_PARTIES 64
        #define PARAM_LOG_NB_PARTIES 6
    #endif
    #endif
    #ifdef PARAM_INST3
    #define PARAM_LABEL "aimer-L3-inst3"
    #if defined(PARAM_TRADITIONAL_5R) || defined(PARAM_HYPERCUBE_5R)
        #define PARAM_NB_EXECUTIONS 25
        #define PARAM_NB_PARTIES 256
        #define PARAM_LOG_NB_PARTIES 8
    #endif
    #endif
    #ifdef PARAM_INST4
    #define PARAM_LABEL "aimer-L3-inst4"
    #if defined(PARAM_TRADITIONAL_5R)
        #define PARAM_NB_EXECUTIONS 19
        #define PARAM_NB_PARTIES 1621
        #define PARAM_LOG_NB_PARTIES 11
    #endif
    #endif

#elif PARAM_SECURITY == 256
    #define PARAM_SALT_SIZE (512/8)
    #define PARAM_SEED_SIZE (256/8)
    #define PARAM_DIGEST_SIZE (512/8)

    #ifdef PARAM_INST1
    #define PARAM_LABEL "aimer-L5-inst1"
    #if defined(PARAM_TRADITIONAL_5R) || defined(PARAM_HYPERCUBE_5R)
        #define PARAM_NB_EXECUTIONS 65
        #define PARAM_NB_PARTIES 16
        #define PARAM_LOG_NB_PARTIES 4
    #endif
    #endif
    #ifdef PARAM_INST2
    #define PARAM_LABEL "aimer-L5-inst2"
    #if defined(PARAM_TRADITIONAL_5R)
        #define PARAM_NB_EXECUTIONS 44
        #define PARAM_NB_PARTIES 62
        #define PARAM_LOG_NB_PARTIES 6
    #endif
    #endif
    #ifdef PARAM_INST3
    #define PARAM_LABEL "aimer-L5-inst3"
    #if defined(PARAM_TRADITIONAL_5R) || defined(PARAM_HYPERCUBE_5R)
        #define PARAM_NB_EXECUTIONS 33
        #define PARAM_NB_PARTIES 256
        #define PARAM_LOG_NB_PARTIES 8
    #endif
    #endif
    #ifdef PARAM_INST4
    #define PARAM_LABEL "aimer-L5-inst4"
    #if defined(PARAM_TRADITIONAL_5R)
        #define PARAM_NB_EXECUTIONS 25
        #define PARAM_NB_PARTIES 1623
        #define PARAM_LOG_NB_PARTIES 11
    #endif
    #endif

#endif

#endif /* PARAMETERS_H */
