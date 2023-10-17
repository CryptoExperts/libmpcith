#ifndef LIBMPCITH_PRG_STRUCT_H
#define LIBMPCITH_PRG_STRUCT_H

#include "aes.h"
typedef struct prg_context {
    aes256ctr_ctx state;
    uint8_t source[64];
    uint8_t remaining;
} prg_context;

#ifdef PRGX4
typedef struct prg_context_x4 {
  prg_context instances[4];
} prg_context_x4;
#endif

#endif /* LIBMPCITH_PRG_STRUCT_H */
