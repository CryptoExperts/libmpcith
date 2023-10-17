#ifdef PARAM_GF251
#define PARAM_FIELD_SIZE 251
#endif
#ifdef PARAM_GF256
#define PARAM_FIELD_SIZE 256
#endif
#ifndef PARAM_FIELD_SIZE
#error "Field not identified"
#endif

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

#ifdef PARAM_HYPERCUBE_DELAYED
#define PARAM_HYPERCUBE_5R
#endif

#if PARAM_SECURITY == 128
  /********************************************
   *               Category I                 *
   ********************************************/

  #define PARAM_SEED_SIZE (128/8)
  #define PARAM_SALT_SIZE (256/8)
  #define PARAM_DIGEST_SIZE (256/8)

  #if PARAM_FIELD_SIZE == 256
    #ifdef PARAM_N32
      #define PARAM_LABEL "sd-256-L1-32"
    #else
      #define PARAM_LABEL "sd-256-L1"
    #endif
    #define PARAM_CODE_LENGTH 230
    #define PARAM_CODE_DIMENSION 126
    #define PARAM_CODE_WEIGHT 79
  #elif PARAM_FIELD_SIZE == 251
    #ifdef PARAM_N32
      #define PARAM_LABEL "sd-251-L1-32"
    #else
      #define PARAM_LABEL "sd-251-L1"
    #endif
    #define PARAM_CODE_LENGTH 230
    #define PARAM_CODE_DIMENSION 126
    #define PARAM_CODE_WEIGHT 79
  #endif
  #define PARAM_EXT_DEGREE 4

  // Traditional
  #ifdef PARAM_TRADITIONAL_5R
    #ifndef PARAM_FLEX
      #ifdef PARAM_N32
        #define PARAM_NB_PARTIES 32
        #define PARAM_LOG_NB_PARTIES 5
        #define PARAM_NB_EXECUTIONS 27
        #define PARAM_NB_EVALS_PER_POLY 3
      #else
        #define PARAM_NB_PARTIES 256
        #define PARAM_LOG_NB_PARTIES 8
        #define PARAM_NB_EXECUTIONS 17
        #define PARAM_NB_EVALS_PER_POLY 3
      #endif
    #endif
  #endif

  // Hypercube
  #ifdef PARAM_HYPERCUBE_5R
    #ifndef PARAM_FLEX
      #ifdef PARAM_N32
        #define PARAM_NB_PARTIES 32
        #define PARAM_LOG_NB_PARTIES 5
        #define PARAM_NB_EXECUTIONS 27
        #define PARAM_NB_EVALS_PER_POLY 3
      #else
        #define PARAM_NB_PARTIES 256
        #define PARAM_LOG_NB_PARTIES 8
        #define PARAM_NB_EXECUTIONS 17
        #define PARAM_NB_EVALS_PER_POLY 3
      #endif
    #endif
  #endif

  // Threshold
  #ifdef PARAM_TCITH_MT_5R_NFPR
    #ifndef PARAM_FLEX
      #if PARAM_FIELD_SIZE <= 256
        #define PARAM_NB_PARTIES PARAM_FIELD_SIZE
        #define PARAM_LOG_NB_PARTIES 8
      #endif
      #define PARAM_NB_EXECUTIONS 6
      #define PARAM_NB_REVEALED 3
      #define PARAM_TREE_NB_MAX_OPEN_LEAVES 19
      #define PARAM_NB_EVALS_PER_POLY 7
    #endif
  #endif  

  // Threshold GGM
  #ifdef PARAM_TCITH_GGM_5R
    #ifndef PARAM_FLEX
      #ifdef PARAM_N32
        #define PARAM_NB_PARTIES 32
        #define PARAM_LOG_NB_PARTIES 5
        #define PARAM_NB_EXECUTIONS 27
        #define PARAM_NB_EVALS_PER_POLY 3
      #else
        #if PARAM_FIELD_SIZE <= 256
          #define PARAM_NB_PARTIES PARAM_FIELD_SIZE
        #endif
        #define PARAM_LOG_NB_PARTIES 8
        #define PARAM_NB_EXECUTIONS 17
        #define PARAM_NB_EVALS_PER_POLY 3
      #endif
    #endif
  #endif

#elif PARAM_SECURITY == 192
  /********************************************
   *              Category III                *
   ********************************************/

  #define PARAM_SEED_SIZE (192/8)
  #define PARAM_SALT_SIZE (256/8)
  #define PARAM_DIGEST_SIZE (384/8)

  #if PARAM_FIELD_SIZE == 256
    #define PARAM_LABEL "sd-256-L3"
    #define PARAM_CODE_LENGTH 352
    #define PARAM_CODE_DIMENSION 193
    #define PARAM_CODE_WEIGHT 120
    #define PARAM_SPLITTING_FACTOR 2
  #elif PARAM_FIELD_SIZE == 251
    #define PARAM_LABEL "sd-251-L3"
    #define PARAM_CODE_LENGTH 352
    #define PARAM_CODE_DIMENSION 193
    #define PARAM_CODE_WEIGHT 120
    #define PARAM_SPLITTING_FACTOR 2
  #endif
  #define PARAM_EXT_DEGREE 4

  // Traditional
  #ifdef PARAM_TRADITIONAL_5R
    #ifndef PARAM_FLEX
      #define PARAM_NB_PARTIES 256
      #define PARAM_LOG_NB_PARTIES 8
      #define PARAM_NB_EXECUTIONS 26
      #define PARAM_NB_EVALS_PER_POLY 3
    #endif
  #endif

  // Hypercube
  #ifdef PARAM_HYPERCUBE_5R
    #ifndef PARAM_FLEX
      #define PARAM_NB_PARTIES 256
      #define PARAM_LOG_NB_PARTIES 8
      #define PARAM_NB_EXECUTIONS 26
      #define PARAM_NB_EVALS_PER_POLY 3
    #endif
  #endif

  // Threshold
  #ifdef PARAM_TCITH_MT_5R_NFPR
    #ifndef PARAM_FLEX
      #if PARAM_FIELD_SIZE <= 256
        #define PARAM_NB_PARTIES PARAM_FIELD_SIZE
        #define PARAM_LOG_NB_PARTIES 8
      #endif
      #define PARAM_NB_EXECUTIONS 9
      #define PARAM_NB_REVEALED 3
      #define PARAM_TREE_NB_MAX_OPEN_LEAVES 19
      #define PARAM_NB_EVALS_PER_POLY 10
    #endif
  #endif

  // Threshold GGM
  #ifdef PARAM_TCITH_GGM_5R
    #ifndef PARAM_FLEX
      #if PARAM_FIELD_SIZE <= 256
        #define PARAM_NB_PARTIES PARAM_FIELD_SIZE
      #endif
      #define PARAM_LOG_NB_PARTIES 8
      #define PARAM_NB_EXECUTIONS 26
      #define PARAM_NB_EVALS_PER_POLY 3
    #endif
  #endif

#elif PARAM_SECURITY == 256
  /********************************************
   *               Category V                 *
   ********************************************/

  #define PARAM_SEED_SIZE (256/8)
  #define PARAM_SALT_SIZE (256/8)
  #define PARAM_DIGEST_SIZE (512/8)

  #if PARAM_FIELD_SIZE == 256
    #define PARAM_LABEL "sd-256-L5"
    #define PARAM_CODE_LENGTH 480
    #define PARAM_CODE_DIMENSION 278
    #define PARAM_CODE_WEIGHT 150
    #define PARAM_SPLITTING_FACTOR 2
  #elif PARAM_FIELD_SIZE == 251
    #define PARAM_LABEL "sd-251-L5"
    #define PARAM_CODE_LENGTH 480
    #define PARAM_CODE_DIMENSION 278
    #define PARAM_CODE_WEIGHT 150
    #define PARAM_SPLITTING_FACTOR 2
  #endif
  #define PARAM_EXT_DEGREE 4

  // Traditional
  #ifdef PARAM_TRADITIONAL_5R
    #ifndef PARAM_FLEX
      #define PARAM_NB_PARTIES 256
      #define PARAM_LOG_NB_PARTIES 8
      #define PARAM_NB_EXECUTIONS 34
      #define PARAM_NB_EVALS_PER_POLY 4
    #endif
  #endif

  // Hypercube
  #ifdef PARAM_HYPERCUBE_5R
    #ifndef PARAM_FLEX
      #define PARAM_NB_PARTIES 256
      #define PARAM_LOG_NB_PARTIES 8
      #define PARAM_NB_EXECUTIONS 34
      #define PARAM_NB_EVALS_PER_POLY 4
    #endif
  #endif

  // Threshold
  #ifdef PARAM_TCITH_MT_5R_NFPR
    #ifndef PARAM_FLEX
      #if PARAM_FIELD_SIZE <= 256
        #define PARAM_NB_PARTIES PARAM_FIELD_SIZE
        #define PARAM_LOG_NB_PARTIES 8
      #endif
      #define PARAM_NB_EXECUTIONS 12
      #define PARAM_NB_REVEALED 3
      #define PARAM_TREE_NB_MAX_OPEN_LEAVES 19
      #define PARAM_NB_EVALS_PER_POLY 13
    #endif
  #endif

  // Threshold GGM
  #ifdef PARAM_TCITH_GGM_5R
    #ifndef PARAM_FLEX
      #if PARAM_FIELD_SIZE <= 256
        #define PARAM_NB_PARTIES PARAM_FIELD_SIZE
      #endif
      #define PARAM_LOG_NB_PARTIES 8
      #define PARAM_NB_EXECUTIONS 34
      #define PARAM_NB_EVALS_PER_POLY 3
    #endif
  #endif

#endif
