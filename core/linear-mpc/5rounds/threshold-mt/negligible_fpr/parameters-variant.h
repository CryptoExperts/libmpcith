#ifndef LIBMPCITH_PARAMETERS_VARIANT_H
#define LIBMPCITH_PARAMETERS_VARIANT_H

// Name of the variant
#define PARAM_VARIANT "threshold-mt-nfpr-5r"

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

// Require PARAM_NB_REVEALED
#ifndef PARAM_NB_REVEALED
#error Missing parameter: PARAM_NB_REVEALED
#endif

// Require PARAM_TREE_NB_MAX_OPEN_LEAVES
#ifndef PARAM_TREE_NB_MAX_OPEN_LEAVES
#error Missing parameter: PARAM_TREE_NB_MAX_OPEN_LEAVES
#endif

// Require PARAM_FIELD_SIZE
#ifndef PARAM_FIELD_SIZE
#error Missing parameter: PARAM_FIELD_SIZE
#endif

#if PARAM_NB_PARTIES > PARAM_FIELD_SIZE
#error The number of parties (PARAM_NB_PARTIES) should be smaller than or equal to the field size (PARAM_FIELD_SIZE).
#endif

/****************************************************
 *     Definition of the maximal signature size     *
 ****************************************************/

#define PARAM_SIGNATURE_SIZEBYTES (                     \
    2*PARAM_DIGEST_SIZE + PARAM_SALT_SIZE +             \
    + PARAM_COMPRESSED_BR_SIZE                          \
    + PARAM_NB_EXECUTIONS*PARAM_NB_REVEALED*(           \
        PARAM_WIT_SHORT_SIZE + PARAM_CORR_SHORT_SIZE    \
            + PARAM_UNIF_SHORT_SIZE                     \
    ) + PARAM_NB_EXECUTIONS*PARAM_DIGEST_SIZE           \
            * PARAM_TREE_NB_MAX_OPEN_LEAVES             \
)

#endif /* LIBMPCITH_PARAMETERS_VARIANT_H */
