#ifndef LIBMPCITH_SIGN_PSEUDO_THRESHOLD_H
#define LIBMPCITH_SIGN_PSEUDO_THRESHOLD_H

#include <stdint.h>
#include <stddef.h>

int mpcith_tprs_ext_rsign_get_size(size_t ring_size);
int mpcith_tprs_ext_rsign(uint8_t* sig, size_t* siglen,
                const uint8_t* m, size_t mlen,
                const uint8_t* sk,
                uint8_t const* const* ring, size_t ring_size,
                const uint8_t* salt, const uint8_t* seed
                );
int mpcith_tprs_ext_rsign_verify(const uint8_t* sig, size_t siglen,
                const uint8_t* m, size_t mlen,
                uint8_t const* const* ring, size_t ring_size
                );
#define mpcith_rsign_get_size(ring_size) mpcith_tprs_ext_rsign_get_size(ring_size)
#define mpcith_rsign(sig, siglen, m, mlen, sk, ring, ring_size, salt, seed) mpcith_tprs_ext_rsign(sig, siglen, m, mlen, sk, ring, ring_size, salt, seed)
#define mpcith_rsign_verify(sig, siglen, m, mlen, ring, ring_size) mpcith_tprs_ext_rsign_verify(sig, siglen, m, mlen, ring, ring_size)

#endif /* LIBMPCITH_SIGN_PSEUDO_THRESHOLD_H */
