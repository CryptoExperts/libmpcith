#ifndef LIBMPCITH_PARAMETERS_VARIANT_H
#define LIBMPCITH_PARAMETERS_VARIANT_H

// Name of the variant
#define PARAM_VARIANT "threshold-ggm-lifting-5r"

// Require PARAM_FIELD_SIZE
#ifndef PARAM_FIELD_SIZE
#error Missing parameter: PARAM_FIELD_SIZE
#endif

// Upper bound of the signature size
#include "parameters-variant-skeleton.h"

// Checking
#if PARAM_NB_PARTIES != (1<<PARAM_LOG_NB_PARTIES)
#error "PARAM_NB_PARTIES should be equal to 2^PARAM_LOG_NB_PARTIES."
#endif

#define PARAM_LOG_NB_PARTIES_ON_EDGE ((PARAM_LOG_NB_PARTIES+PARAM_HYPERCUBE_DIMENSION-1)/PARAM_HYPERCUBE_DIMENSION)
#define PARAM_NB_PARTIES_ON_EDGE (1<<PARAM_LOG_NB_PARTIES_ON_EDGE)
#if PARAM_NB_PARTIES_ON_EDGE > PARAM_FIELD_SIZE
#error The number of parties on a hypercube edge should be smaller than or equal to the field size (PARAM_FIELD_SIZE).
#endif

// Number of packings
#define PARAM_NB_PACKS (PARAM_HYPERCUBE_DIMENSION)

#endif /* LIBMPCITH_PARAMETERS_VARIANT_H */
