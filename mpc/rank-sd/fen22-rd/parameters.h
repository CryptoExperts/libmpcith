#ifndef PARAMETERS_H
#define PARAMETERS_H

#define PARAM_SECURITY 128
#ifdef PARAM_N32
#define PARAM_LABEL "ranksd-rd-L1-32"
#else
#define PARAM_LABEL "ranksd-rd-L1"
#endif

// Hash Domain Separation
#define HASH_PREFIX_COMMITMENT 0
#define HASH_PREFIX_FIRST_CHALLENGE 1
#define HASH_PREFIX_SECOND_CHALLENGE 2
#define HASH_PREFIX_MERKLE_TREE 3
#define HASH_PREFIX_SEED_TREE 3

/********************************************
 *          OW Function Parameters          *
 ********************************************/

// Parameter: code length n
#define PARAM_n 30
// Parameter: code dimension k
#define PARAM_k 15
// Parameter: code weight r
#define PARAM_r 9
// Parameter: extension degree m
#define PARAM_m 31
// Parameter: soundness booster
#ifdef PARAM_N32
#define PARAM_eta 19
#else
#define PARAM_eta 24
#endif

// Ceil(m/8) where m is the extension degree
#define PARAM_m_div8 ((PARAM_m+7)>>3)
// Ceil(r/8) where r is the code weight
#define PARAM_r_div8 ((PARAM_r+7)>>3)
// Ceil(eta/8) where eta is the soundness booster
#define PARAM_eta_div8 ((PARAM_eta+7)>>3)

#define PARAM_SYNDROME_LENGTH (PARAM_n-PARAM_k)
#define PARAM_SYNDROME_BYTESIZE (PARAM_SYNDROME_LENGTH*PARAM_m_div8)
#define PARAM_SYNDROME_BYTESIZE_CEIL32 (((PARAM_SYNDROME_BYTESIZE+31)>>5)<<5)
#define PARAM_CODEWORD_LENGTH (PARAM_n)
#define PARAM_CODEWORD_BYTESIZE (PARAM_CODEWORD_LENGTH*PARAM_m_div8)
#define PARAM_CODEWORD_BYTESIZE_CEIL32 (((PARAM_CODEWORD_LENGTH*PARAM_m_div8+31)>>5)<<5)
#define PARAM_PLAINTEXT_LENGTH (PARAM_k)
#define PARAM_PLAINTEXT_BYTESIZE (PARAM_PLAINTEXT_LENGTH*PARAM_m_div8)
#define PARAM_PCMATRIX_BYTESIZE (PARAM_k*PARAM_SYNDROME_BYTESIZE_CEIL32)

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
    #define PARAM_NB_EXECUTIONS 33
    #define PARAM_NB_PARTIES 32
    #define PARAM_LOG_NB_PARTIES 5
#else
    #define PARAM_NB_EXECUTIONS 21
    #define PARAM_NB_PARTIES 256
    #define PARAM_LOG_NB_PARTIES 8
#endif
#endif

#ifdef PARAM_HYPERCUBE_5R
#ifdef PARAM_N32
    #define PARAM_NB_EXECUTIONS 33
    #define PARAM_NB_PARTIES 32
    #define PARAM_LOG_NB_PARTIES 5
#else
    #define PARAM_NB_EXECUTIONS 21
    #define PARAM_NB_PARTIES 256
    #define PARAM_LOG_NB_PARTIES 8
#endif
#endif

#endif /* PARAMETERS_H */
