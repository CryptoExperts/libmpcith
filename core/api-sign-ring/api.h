#ifndef LIBMPCITH_API_H
#define LIBMPCITH_API_H

#include "parameters-all.h"
#include "keygen.h"

#define API_SIGN_RING

#define CRYPTO_SECRETKEYBYTES ((long) PARAM_SECRETKEYBYTES)
#define CRYPTO_PUBLICKEYBYTES ((long) PARAM_PUBLICKEYBYTES)
#define CRYPTO_PUBLICPARAMBYTES ((long) PARAM_PUBLICPARAMBYTES)

#define CRYPTO_ALGNAME PARAM_LABEL " - " PARAM_VARIANT
#define CRYPTO_VERSION "0.00"

int crypto_rsign_setup(unsigned char* pp);
int crypto_rsign_keypair(unsigned char* pk, unsigned char* sk, const unsigned char* pp);
int crypto_rsign_valid_keys(unsigned char* pk, unsigned char* sk);
int crypto_rsign_get_size(unsigned long long ring_size);
int crypto_rsign_signature(unsigned char *sig, unsigned long long *siglen, const unsigned char *m,
                          unsigned long long mlen, const unsigned char *sk, unsigned char const* const* ring, unsigned long long ring_size);
int crypto_rsign(unsigned char* sm, unsigned long long* smlen, const unsigned char* m,
                unsigned long long mlen, const unsigned char *sk, unsigned char const* const* ring, unsigned long long ring_size);
int crypto_rsign_verify(const unsigned char *sig, unsigned long long siglen, const unsigned char *m,
                       unsigned long long mlen, unsigned char const* const* ring, unsigned long long ring_size);
int crypto_rsign_open(unsigned char* m, unsigned long long* mlen, const unsigned char* sm,
                     unsigned long long smlen, unsigned char const* const* ring, unsigned long long ring_size);

#endif /* LIBMPCITH_API_H */
