#ifndef LIBMPCITH_PARAMETERS_VARIANT_H
#define LIBMPCITH_PARAMETERS_VARIANT_H

// Name of the variant
#define PARAM_VARIANT "threshold-ggm-7r"

// Require PARAM_FIELD_SIZE
#ifndef PARAM_FIELD_SIZE
#error Missing parameter: PARAM_FIELD_SIZE
#endif

// Upper bound of the signature size
#include "parameters-variant-skeleton.h"

#if PARAM_NB_PARTIES > PARAM_FIELD_SIZE
#error The number of parties (PARAM_NB_PARTIES) should be smaller than or equal to the field size (PARAM_FIELD_SIZE).
#endif

// Number of packings
#define PARAM_NB_PACKS (1)

#endif /* LIBMPCITH_PARAMETERS_VARIANT_H */
