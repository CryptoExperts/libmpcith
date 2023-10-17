#ifndef LIBMPCITH_XOF_STRUCT_H
#define LIBMPCITH_XOF_STRUCT_H

#ifndef SUPERCOP
  //#include <libshake.a.headers/KeccakHash.h>
  #include "sha3/KeccakHash.h"
#else
  /* use SUPERCOP implementation */
  #include <libkeccak.a.headers/KeccakHash.h>
#endif
typedef Keccak_HashInstance xof_context;

#include "parameters.h"
#if PARAM_SECURITY == 128
#define Keccak_HashInitialize_SHAKE_CUR Keccak_HashInitialize_SHAKE128
#define Keccak_HashInitializetimes4_SHAKE_CUR Keccak_HashInitializetimes4_SHAKE128
#elif PARAM_SECURITY == 192
#define Keccak_HashInitialize_SHAKE_CUR Keccak_HashInitialize_SHAKE256
#define Keccak_HashInitializetimes4_SHAKE_CUR Keccak_HashInitializetimes4_SHAKE256
#elif PARAM_SECURITY == 256
#define Keccak_HashInitialize_SHAKE_CUR Keccak_HashInitialize_SHAKE256
#define Keccak_HashInitializetimes4_SHAKE_CUR Keccak_HashInitializetimes4_SHAKE256
#else
#error "No XOF implementation for this security level"
#endif

#ifdef XOFX4
/* use the Keccakx4 implementation */
#include "sha3/KeccakHashtimes4.h"
#define ATTR_ALIGNED(i) __attribute__((aligned((i))))
typedef Keccak_HashInstancetimes4 xof_context_x4 ATTR_ALIGNED(32);
#endif

#endif /* LIBMPCITH_XOF_STRUCT_H */
