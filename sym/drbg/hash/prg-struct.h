#ifndef LIBMPCITH_PRG_STRUCT_H
#define LIBMPCITH_PRG_STRUCT_H

#include "xof.h"
typedef xof_context prg_context;

#ifdef PRGX4
typedef xof_context_x4 prg_context_x4;
#endif

#endif /* LIBMPCITH_PRG_STRUCT_H */
