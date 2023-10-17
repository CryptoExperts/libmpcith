#ifndef LIBMPCITH_PARAMETERS_VARIANT_SKELETON_H
#define LIBMPCITH_PARAMETERS_VARIANT_SKELETON_H

/****************************************************
 *    Check that the required parameters are set    *
 ****************************************************/

// Require PARAM_NB_EXECUTIONS
#ifndef PARAM_NB_EXECUTIONS
#error Missing parameter: PARAM_NB_EXECUTIONS
#endif

// Require PARAM_NB_PARTIES
#ifndef PARAM_NB_PARTIES
#error Missing parameter: PARAM_NB_PARTIES
#endif

// Require PARAM_LOG_NB_PARTIES
#ifndef PARAM_LOG_NB_PARTIES
#error Missing parameter: PARAM_LOG_NB_PARTIES
#endif

/****************************************************
 *     Definition of the maximal signature size     *
 ****************************************************/

#define PARAM_SIGNATURE_SIZEBYTES (                     \
    4 + 3*PARAM_DIGEST_SIZE + PARAM_SALT_SIZE +         \
    + PARAM_NB_EXECUTIONS*(                             \
        PARAM_WIT_SHORT_SIZE + PARAM_HINT_SHORT_SIZE    \
            + PARAM_COMPRESSED_BR_SIZE                  \
            + PARAM_LOG_NB_PARTIES*PARAM_SEED_SIZE      \
            + 2*PARAM_DIGEST_SIZE                       \
    )                                                   \
)

#endif /* LIBMPCITH_PARAMETERS_VARIANT_SKELETON_H */
