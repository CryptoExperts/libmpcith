#ifndef PACKING_STRUCT_H
#define PACKING_STRUCT_H

#include <stdint.h>
#include "parameters.h"
#include "parameters-variant.h"
 
#define PARAM_LOG_DEFAULT_EDGE_SIZE ((PARAM_LOG_NB_PARTIES+PARAM_HYPERCUBE_DIMENSION-1)/PARAM_HYPERCUBE_DIMENSION)
#define PARAM_LOG_LAST_EDGE_SIZE (PARAM_LOG_NB_PARTIES-(PARAM_HYPERCUBE_DIMENSION-1)*PARAM_LOG_DEFAULT_EDGE_SIZE)
#define PARAM_DEFAULT_EDGE_SIZE (1<<PARAM_LOG_DEFAULT_EDGE_SIZE)
#define PARAM_LAST_EDGE_SIZE (1<<PARAM_LOG_LAST_EDGE_SIZE)

typedef struct packing_context_t {
    vec_share_t** packed_shares;
  #if PARAM_NB_PACKS > 1
    vec_share_t* accumulators[PARAM_NB_PACKS-1][PARAM_DEFAULT_EDGE_SIZE-1];
  #endif
    vec_share_t* last_accumulators[PARAM_LAST_EDGE_SIZE-1];
} packing_context_t;

typedef struct share_recomputing_context_t {
    vec_share_t** packed_shares;
  #if PARAM_NB_PACKS > 1
    vec_share_t* accumulators[PARAM_NB_PACKS-1][PARAM_DEFAULT_EDGE_SIZE-1];
  #endif
    vec_share_t* last_accumulators[PARAM_LAST_EDGE_SIZE-1];
    uint32_t hidden_view;
} share_recomputing_context_t;



#endif /* PACKING_STRUCT_H */
