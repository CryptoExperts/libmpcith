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

#ifdef PARAM_GF251
#define PARAM_FIELD_SIZE 251
#if PARAM_SECURITY == 128
#define PARAM_LABEL "minrank-251-L1"
#elif PARAM_SECURITY == 192
#define PARAM_LABEL "minrank-251-L3"
#elif PARAM_SECURITY == 256
#define PARAM_LABEL "minrank-251-L5"
#endif
#endif
#ifdef PARAM_GF256
#define PARAM_FIELD_SIZE 256
#if PARAM_SECURITY == 128
#define PARAM_LABEL "minrank-256-L1"
#elif PARAM_SECURITY == 192
#define PARAM_LABEL "minrank-256-L3"
#elif PARAM_SECURITY == 256
#define PARAM_LABEL "minrank-256-L5"
#endif
#endif
#ifdef PARAM_GF16
#define PARAM_FIELD_SIZE 16
#if PARAM_SECURITY == 128
#ifdef PARAM_N32
#define PARAM_LABEL "minrank-16-L1-32"
#else
#define PARAM_LABEL "minrank-16-L1"
#endif
#elif PARAM_SECURITY == 192
#define PARAM_LABEL "minrank-16-L3"
#elif PARAM_SECURITY == 256
#define PARAM_LABEL "minrank-16-L5"
#endif
#endif
#ifndef PARAM_FIELD_SIZE
#error "Field not identified"
#endif


/********************************************
 *          OW Function Parameters          *
 ********************************************/

#if PARAM_SECURITY == 128
    #if PARAM_FIELD_SIZE == 256 || PARAM_FIELD_SIZE == 251
        #define PARAM_q PARAM_FIELD_SIZE
        #define PARAM_n 13
        #define PARAM_m 12
        #define PARAM_k 55
        #define PARAM_r 5
        #define PARAM_eta 1

        #ifdef PARAM_TRADITIONAL_5R
            #define PARAM_NB_EXECUTIONS 17
            #define PARAM_NB_PARTIES 256
            #define PARAM_LOG_NB_PARTIES 8
        #endif
        #ifdef PARAM_HYPERCUBE_5R
            #define PARAM_NB_EXECUTIONS 17
            #define PARAM_NB_PARTIES 256
            #define PARAM_LOG_NB_PARTIES 8
        #endif
        #ifdef PARAM_TCITH_MT_5R
            #define PARAM_NB_EXECUTIONS 7
            #define PARAM_NB_REVEALED 3
            #define PARAM_NB_PARTIES 251
            #define PARAM_TREE_NB_MAX_OPEN_LEAVES 19
            #define PARAM_LOG_NB_PARTIES 8
        #endif
    #endif
    #if PARAM_FIELD_SIZE == 16
        #define PARAM_q PARAM_FIELD_SIZE
        #define PARAM_n 16
        #define PARAM_m 16
        #define PARAM_k 142
        #define PARAM_r 4
        #define PARAM_eta 1

        #ifdef PARAM_TRADITIONAL_5R
            #ifdef PARAM_N32
                #define PARAM_NB_EXECUTIONS 28
                #define PARAM_NB_PARTIES 32
                #define PARAM_LOG_NB_PARTIES 5
            #else
                #define PARAM_NB_EXECUTIONS 18
                #define PARAM_NB_PARTIES 256
                #define PARAM_LOG_NB_PARTIES 8
            #endif
        #endif
        #ifdef PARAM_HYPERCUBE_5R
            #ifdef PARAM_N32
                #define PARAM_NB_EXECUTIONS 28
                #define PARAM_NB_PARTIES 32
                #define PARAM_LOG_NB_PARTIES 5
            #else
                #define PARAM_NB_EXECUTIONS 18
                #define PARAM_NB_PARTIES 256
                #define PARAM_LOG_NB_PARTIES 8
            #endif
        #endif
        #ifdef PARAM_TCITH_MT_5R
            #error "Threshold Approach is not available for GF(16)"
        #endif
        #ifdef PARAM_TCITH_GGM_LIFTING_5R
            #define PARAM_NB_EXECUTIONS 18
            #define PARAM_NB_PARTIES 256
            #define PARAM_LOG_NB_PARTIES 8
            #define PARAM_HYPERCUBE_DIMENSION 2
        #endif
    #endif
#endif

#if PARAM_SECURITY == 256
    #if PARAM_FIELD_SIZE == 256 || PARAM_FIELD_SIZE == 251
        #define PARAM_q PARAM_FIELD_SIZE
        #define PARAM_n 16
        #define PARAM_m 16
        #define PARAM_k 141
        #define PARAM_r 4
        #define PARAM_eta 1

        #ifdef PARAM_TRADITIONAL_5R
            #define PARAM_NB_EXECUTIONS 34
            #define PARAM_NB_PARTIES 256
            #define PARAM_LOG_NB_PARTIES 8
        #endif
        #ifdef PARAM_HYPERCUBE_5R
            #define PARAM_NB_EXECUTIONS 34
            #define PARAM_NB_PARTIES 256
            #define PARAM_LOG_NB_PARTIES 8
        #endif
        #ifdef PARAM_TCITH_MT_5R
            #define PARAM_NB_EXECUTIONS 14
            #define PARAM_NB_REVEALED 3
            #define PARAM_NB_PARTIES 251
            #define PARAM_TREE_NB_MAX_OPEN_LEAVES 19
            #define PARAM_LOG_NB_PARTIES 8
        #endif
    #endif
#endif

#define PARAM_nm (PARAM_m*PARAM_n)

/********************************************
 *           Signature Parameters           *
 ********************************************/

#if PARAM_SECURITY == 128
    // Size in bytes of the salt
    #define PARAM_SALT_SIZE (256/8)
    // Size in bytes of a seed
    #define PARAM_SEED_SIZE (128/8)
    // Size in bytes of a digest
    #define PARAM_DIGEST_SIZE (256/8)
#elif PARAM_SECURITY == 256
    // Size in bytes of the salt
    #define PARAM_SALT_SIZE (512/8)
    // Size in bytes of a seed
    #define PARAM_SEED_SIZE (256/8)
    // Size in bytes of a digest
    #define PARAM_DIGEST_SIZE (512/8)
#endif

// Hash Domain Separation
#define HASH_PREFIX_COMMITMENT 0
#define HASH_PREFIX_FIRST_CHALLENGE 1
#define HASH_PREFIX_SECOND_CHALLENGE 2
#define HASH_PREFIX_MERKLE_TREE 3
#define HASH_PREFIX_SEED_TREE 3

#endif /* PARAMETERS_H */
