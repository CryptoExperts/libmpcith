#ifndef LIBMPCITH_PARAMETERS_VARIANT_H
#define LIBMPCITH_PARAMETERS_VARIANT_H

// Name of the variant
#define PARAM_VARIANT "threshold-ggm-extended"

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

#if PARAM_NB_PARTIES > PARAM_FIELD_SIZE
#error The number of parties (PARAM_NB_PARTIES) should be smaller than or equal to the field size (PARAM_FIELD_SIZE).
#endif

#ifndef PARAM_CIRCUIT_DEPTH
#define PARAM_CIRCUIT_DEPTH 2
#endif

/****************************************************
 *     Definition of the maximal signature size     *
 ****************************************************/

#define PARAM_SIGNATURE_SIZEBYTES (                      \
    4 + 2*PARAM_DIGEST_SIZE + PARAM_SALT_SIZE +          \
    + PARAM_NB_EXECUTIONS*(                              \
        (PARAM_CIRCUIT_DEPTH-1)*PARAM_BR_SHORT_SIZE      \
            + PARAM_WIT_SHORT_SIZE                       \
            + PARAM_LOG_NB_PARTIES*PARAM_SEED_SIZE       \
            + PARAM_DIGEST_SIZE                          \
    )                                                    \
)

#endif /* LIBMPCITH_PARAMETERS_VARIANT_H */
