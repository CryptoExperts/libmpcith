#ifndef LIBMPCITH_PARAMETERS_VARIANT_H
#define LIBMPCITH_PARAMETERS_VARIANT_H

// Name of the variant
#define PARAM_VARIANT "threshold-ggm-ring"

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

#endif /* LIBMPCITH_PARAMETERS_VARIANT_H */
