#ifndef LIBMPCITH_HASH_STRUCT_H
#define LIBMPCITH_HASH_STRUCT_H

#ifndef SUPERCOP
  //#include <libshake.a.headers/KeccakHash.h>
  #include "sha3/KeccakHash.h"
#else
  /* use SUPERCOP implementation */
  #include <libkeccak.a.headers/KeccakHash.h>
#endif
typedef Keccak_HashInstance hash_context;

#include "parameters.h"
#if PARAM_SECURITY == 128
#define Keccak_HashInitialize_SHA3_CUR Keccak_HashInitialize_SHA3_256
#define Keccak_HashInitializetimes4_SHA3_CUR Keccak_HashInitializetimes4_SHA3_256
#define PARAM_DIGEST_SIZE (256/8)
#elif PARAM_SECURITY == 192
#define Keccak_HashInitialize_SHA3_CUR Keccak_HashInitialize_SHA3_384
#define Keccak_HashInitializetimes4_SHA3_CUR Keccak_HashInitializetimes4_SHA3_384
#define PARAM_DIGEST_SIZE (384/8)
#elif PARAM_SECURITY == 256
#define Keccak_HashInitialize_SHA3_CUR Keccak_HashInitialize_SHA3_512
#define Keccak_HashInitializetimes4_SHA3_CUR Keccak_HashInitializetimes4_SHA3_512
#define PARAM_DIGEST_SIZE (512/8)
#else
#error "No hash implementation for this security level"
#endif

#ifdef HASHX4
/* use the Keccakx4 implementation */
#include "sha3/KeccakHashtimes4.h"
#define ATTR_ALIGNED(i) __attribute__((aligned((i))))
typedef Keccak_HashInstancetimes4 hash_context_x4 ATTR_ALIGNED(32);
#endif

#endif /* LIBMPCITH_HASH_STRUCT_H */
