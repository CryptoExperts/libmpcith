#ifndef LIBMPCITH_SIGN_THRESHOLD_NFPR_H
#define LIBMPCITH_SIGN_THRESHOLD_NFPR_H

#include <stdint.h>
#include <stddef.h>

int mpcith_threshold_nfpr_sign(uint8_t* sig, size_t* siglen,
                const uint8_t* m, size_t mlen,
                const uint8_t* sk,
                const uint8_t* salt, const uint8_t* seed
                );
int mpcith_threshold_nfpr_sign_verify(const uint8_t* sig, size_t siglen,
                const uint8_t* m, size_t mlen,
                const uint8_t* pk
                );
#define mpcith_sign(sig, siglen, m, mlen, sk, salt, seed) mpcith_threshold_nfpr_sign(sig, siglen, m, mlen, sk, salt, seed)
#define mpcith_sign_verify(sig, siglen, m, mlen, pk) mpcith_threshold_nfpr_sign_verify(sig, siglen, m, mlen, pk)

#endif /* LIBMPCITH_SIGN_THRESHOLD_NFPR_H */
