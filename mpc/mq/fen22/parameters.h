#ifndef PARAMETERS_H
#define PARAMETERS_H

#define PARAM_SECURITY 128

// Hash Domain Separation
#define HASH_PREFIX_COMMITMENT 0
#define HASH_PREFIX_FIRST_CHALLENGE 1
#define HASH_PREFIX_SECOND_CHALLENGE 2
#define HASH_PREFIX_THIRD_CHALLENGE 3
#define HASH_PREFIX_SEED_TREE 4

/********************************************
 *          OW Function Parameters          *
 ********************************************/

#ifdef PARAM_GF256
#define PARAM_FIELD_SIZE 256
#ifdef PARAM_N32
#define PARAM_LABEL "mq-256-L1-32"
#else
#define PARAM_LABEL "mq-256-L1"
#endif
#endif
#ifdef PARAM_GF251
#define PARAM_FIELD_SIZE 251
#ifdef PARAM_N32
#define PARAM_LABEL "mq-251-L1-32"
#else
#define PARAM_LABEL "mq-251-L1"
#endif
#endif
#ifndef PARAM_FIELD_SIZE
#error "Field not identified"
#endif

#define PARAM_q PARAM_FIELD_SIZE
#define PARAM_n 40
#define PARAM_m 40
#define PARAM_eta 2

#define PARAM_EXT_DEGREE PARAM_eta

/********************************************
 *           Signature Parameters           *
 ********************************************/

// Size in bytes of the salt
#define PARAM_SALT_SIZE (256/8)
// Size in bytes of a seed
#define PARAM_SEED_SIZE (128/8)
// Size in bytes of a digest
#define PARAM_DIGEST_SIZE (256/8)

#if defined(PARAM_TRADITIONAL_5R) || defined(PARAM_HYPERCUBE_5R)
#ifdef PARAM_N32
#define PARAM_NB_EXECUTIONS 36
#define PARAM_NB_PARTIES 32
#define PARAM_LOG_NB_PARTIES 5
#else
#define PARAM_NB_EXECUTIONS 25
#define PARAM_NB_PARTIES 256
#define PARAM_LOG_NB_PARTIES 8
#endif
#endif
#if defined(PARAM_TCITH_GGM_5R)
#ifdef PARAM_N32
#define PARAM_NB_EXECUTIONS 36
#define PARAM_NB_PARTIES 32
#define PARAM_LOG_NB_PARTIES 5
#else
#define PARAM_NB_EXECUTIONS 25
#define PARAM_NB_PARTIES PARAM_FIELD_SIZE
#define PARAM_LOG_NB_PARTIES 8
#endif
#endif

#endif /* PARAMETERS_H */
