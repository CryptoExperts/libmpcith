#ifndef PARAMETERS_H
#define PARAMETERS_H

#define PARAM_SECURITY 128

// Hash Domain Separation
#define HASH_PREFIX_COMMITMENT 0
#define HASH_PREFIX_FIRST_CHALLENGE 1
#define HASH_PREFIX_SECOND_CHALLENGE 2
#define HASH_PREFIX_THIRD_CHALLENGE 3
#define HASH_PREFIX_SEED_TREE 4

#ifdef PARAM_N16
#define PARAM_LABEL "biscuit-L1-fast"
#else
#define PARAM_LABEL "biscuit-L1"
#endif

/********************************************
 *          OW Function Parameters          *
 ********************************************/

#define PARAM_FIELD_SIZE 16

#define PARAM_q PARAM_FIELD_SIZE
#define PARAM_n 64
#define PARAM_m 67

/********************************************
 *           Signature Parameters           *
 ********************************************/

// Size in bytes of the salt
#define PARAM_SALT_SIZE (256/8)
// Size in bytes of a seed
#define PARAM_SEED_SIZE (128/8)
// Size in bytes of a digest
#define PARAM_DIGEST_SIZE (256/8)

#ifdef PARAM_TRADITIONAL_5R
#ifdef PARAM_N16
#define PARAM_NB_EXECUTIONS 34
#define PARAM_NB_PARTIES 16
#define PARAM_LOG_NB_PARTIES 4
#else
#define PARAM_NB_EXECUTIONS 18
#define PARAM_NB_PARTIES 256
#define PARAM_LOG_NB_PARTIES 8
#endif
#endif
#ifdef PARAM_HYPERCUBE_5R
#ifdef PARAM_N16
#define PARAM_NB_EXECUTIONS 34
#define PARAM_NB_PARTIES 16
#define PARAM_LOG_NB_PARTIES 4
#else
#define PARAM_NB_EXECUTIONS 18
#define PARAM_NB_PARTIES 256
#define PARAM_LOG_NB_PARTIES 8
#endif
#endif
#ifdef PARAM_TCITH_GGM_LIFTING_5R
#ifdef PARAM_N16
#define PARAM_NB_EXECUTIONS 34
#define PARAM_NB_PARTIES 16
#define PARAM_LOG_NB_PARTIES 4
#define PARAM_HYPERCUBE_DIMENSION 1
#else
#define PARAM_NB_EXECUTIONS 18
#define PARAM_NB_PARTIES 256
#define PARAM_LOG_NB_PARTIES 8
#define PARAM_HYPERCUBE_DIMENSION 2
#endif
#endif

#endif /* PARAMETERS_H */
