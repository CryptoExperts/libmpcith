#ifndef LIBMPCITH_SIGN_PSEUDO_THRESHOLD_H
#define LIBMPCITH_SIGN_PSEUDO_THRESHOLD_H

#include <stdint.h>
#include <stddef.h>

int tcith_mt_sign(uint8_t* sig, size_t* siglen,
                const uint8_t* m, size_t mlen,
                const uint8_t* sk,
                const uint8_t* salt, const uint8_t* seed
                );
int tcith_mt_sign_verify(const uint8_t* sig, size_t siglen,
                const uint8_t* m, size_t mlen,
                const uint8_t* pk
                );
#define mpcith_sign(sig, siglen, m, mlen, sk, salt, seed) tcith_mt_sign(sig, siglen, m, mlen, sk, salt, seed)
#define mpcith_sign_verify(sig, siglen, m, mlen, pk) tcith_mt_sign_verify(sig, siglen, m, mlen, pk)

#endif /* LIBMPCITH_SIGN_PSEUDO_THRESHOLD_H */
