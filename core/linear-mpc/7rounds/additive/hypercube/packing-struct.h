#ifndef PACKING_STRUCT_H
#define PACKING_STRUCT_H

#include <stdint.h>
#include "parameters.h"
 
typedef struct packing_context_t {
    vec_share_t** packed_shares;
} packing_context_t;

typedef struct share_recomputing_context_t {
    vec_share_t** packed_shares;
    uint32_t hidden_view;
} share_recomputing_context_t;

#endif /* PACKING_STRUCT_H */
