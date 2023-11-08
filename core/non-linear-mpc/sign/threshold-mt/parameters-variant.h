#ifndef LIBMPCITH_PARAMETERS_VARIANT_H
#define LIBMPCITH_PARAMETERS_VARIANT_H

// Name of the variant
#define PARAM_VARIANT "threshold-mt-extended"

/****************************************************
 *    Check that the required parameters are set    *
 ****************************************************/

// Require PARAM_FIELD_SIZE
#ifndef PARAM_FIELD_SIZE
#error Missing parameter: PARAM_FIELD_SIZE
#endif

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
#define PARAM_NB_REVEALED 1
#endif

// Require PARAM_TREE_NB_MAX_OPEN_LEAVES
#ifndef PARAM_TREE_NB_MAX_OPEN_LEAVES
#if PARAM_NB_REVEALED == 1
#define PARAM_TREE_NB_MAX_OPEN_LEAVES PARAM_LOG_NB_PARTIES
#else
#error Missing parameter: PARAM_TREE_NB_MAX_OPEN_LEAVES
#endif
#endif

#if PARAM_NB_PARTIES > PARAM_FIELD_SIZE
#error The number of parties (PARAM_NB_PARTIES) should be smaller than or equal to the field size (PARAM_FIELD_SIZE).
#endif

#ifndef PARAM_CIRCUIT_DEPTH
#define PARAM_CIRCUIT_DEPTH 2
#endif

/****************************************************
 *     Definition of the maximal signature size     *
 ****************************************************/

#if PARAM_NB_REVEALED != 1
#error "PARAM_NB_REVEALED != 1 not implemented"
#endif
#define PARAM_SIGNATURE_SIZEBYTES (                           \
    4 + 3*PARAM_DIGEST_SIZE + PARAM_SALT_SIZE +               \
    + PARAM_DC_BR_SHORT_SIZE                                  \
    + PARAM_NB_EXECUTIONS*(                                   \
        (PARAM_CIRCUIT_DEPTH-1)*PARAM_BR_SHORT_SIZE           \
            + PARAM_WIT_SHORT_SIZE                            \
            + PARAM_UNIF_SHORT_SIZE                           \
            + PARAM_DIGEST_SIZE*PARAM_TREE_NB_MAX_OPEN_LEAVES \
    )                                                         \
)

#endif /* LIBMPCITH_PARAMETERS_VARIANT_H */
