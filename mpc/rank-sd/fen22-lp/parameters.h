#ifndef PARAMETERS_H
#define PARAMETERS_H

#define PARAM_SECURITY 128
#ifdef PARAM_N32
#define PARAM_LABEL "ranksd-lp-L1-32"
#else
#define PARAM_LABEL "ranksd-lp-L1"
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

#define PARAM_CODE_LENGTH 30
#define PARAM_CODE_DIMENSION 15
#define PARAM_CODE_WEIGHT 9
#define PARAM_CODE_EXT_DEGREE 31
#define PARAM_FIELD_SIZE (1<<PARAM_CODE_EXT_DEGREE)

#define PARAM_CODE_EXT_BYTESIZE ((PARAM_CODE_EXT_DEGREE+7)>>3)
#define PARAM_EXT_DEGREE 1

#define PARAM_SYNDROME_LENGTH (PARAM_CODE_LENGTH-PARAM_CODE_DIMENSION)
#define PARAM_SYNDROME_BYTESIZE (PARAM_SYNDROME_LENGTH*PARAM_CODE_EXT_BYTESIZE)
#define PARAM_SYNDROME_BYTESIZE_CEIL32 (((PARAM_SYNDROME_BYTESIZE+31)>>5)<<5)
#define PARAM_CODEWORD_LENGTH (PARAM_CODE_LENGTH)
#define PARAM_CODEWORD_BYTESIZE_CEIL32 (((PARAM_CODEWORD_LENGTH*PARAM_CODE_EXT_BYTESIZE+31)>>5)<<5)
#define PARAM_PLAINTEXT_LENGTH (PARAM_CODE_DIMENSION)
#define PARAM_PLAINTEXT_BYTESIZE (PARAM_PLAINTEXT_LENGTH*PARAM_CODE_EXT_BYTESIZE)
#define PARAM_PCMATRIX_BYTESIZE (PARAM_CODE_DIMENSION*PARAM_SYNDROME_BYTESIZE_CEIL32)

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
#ifdef PARAM_N32
    #define PARAM_NB_EXECUTIONS 30
    #define PARAM_NB_PARTIES 32
    #define PARAM_LOG_NB_PARTIES 5
#else
    #define PARAM_NB_EXECUTIONS 20
    #define PARAM_NB_PARTIES 256
    #define PARAM_LOG_NB_PARTIES 8
#endif
#endif

#ifdef PARAM_HYPERCUBE_5R
#ifdef PARAM_N32
    #define PARAM_NB_EXECUTIONS 30
    #define PARAM_NB_PARTIES 32
    #define PARAM_LOG_NB_PARTIES 5
#else
    #define PARAM_NB_EXECUTIONS 20
    #define PARAM_NB_PARTIES 256
    #define PARAM_LOG_NB_PARTIES 8
#endif
#endif

#endif /* PARAMETERS_H */
