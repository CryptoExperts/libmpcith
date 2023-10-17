#ifndef LIBMPCITH_PARAMETERS_VARIANT_H
#define LIBMPCITH_PARAMETERS_VARIANT_H

// Name of the variant
#define PARAM_VARIANT "hypercube-5r"

// Upper bound of the signature size
#include "parameters-variant-skeleton.h"

// Checking
#if PARAM_NB_PARTIES != (1<<PARAM_LOG_NB_PARTIES)
#error "PARAM_NB_PARTIES should be equal to 2^PARAM_LOG_NB_PARTIES."
#endif

// Number of packings
#define PARAM_NB_PACKS (PARAM_LOG_NB_PARTIES)

#endif /* LIBMPCITH_PARAMETERS_VARIANT_H */
