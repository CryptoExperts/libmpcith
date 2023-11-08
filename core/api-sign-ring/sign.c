#ifdef SUPERCOP
#include "crypto_sign.h"
#else
#include "api.h"
#endif

#include <string.h>
#include "parameters.h"
#include "keygen.h"
#include "sign-mpcith.h"
#include "rnd.h"

static void serialize_uint32(uint8_t buf[4], uint32_t x) {
    buf[0] = x & 0xFF;
    buf[1] = (x >> 8) & 0xFF;
    buf[2] = (x >> 16) & 0xFF;
    buf[3] = x >> 24;
}
static uint32_t deserialize_uint32(const uint8_t buf[4]) {
    return ((uint32_t) buf[0]) | ((uint32_t) buf[1] << 8) | ((uint32_t) buf[2] << 16) | ((uint32_t) buf[3] << 24);
}

int crypto_rsign_get_size(unsigned long long ring_size) {
    return sizeof(uint32_t) + mpcith_rsign_get_size(ring_size);
}

int crypto_rsign_signature(uint8_t *sig,
                          unsigned long long *siglen,
                          const uint8_t *m,
                          unsigned long long mlen,
                          const uint8_t *sk,
                          unsigned char const* const* ring,
                          unsigned long long ring_size)
{
    // Sample salt
    uint8_t salt[PARAM_SALT_SIZE];
    sample_salt(salt);

    // Sample seed
    uint8_t seed[PARAM_SEED_SIZE];
    sample_seed(seed);

    size_t siglen2 = 0;
    // Compute the signature
    int ret = mpcith_rsign(sig, &siglen2, m, mlen, sk, ring, ring_size, salt, seed);
    *siglen = (unsigned long long) siglen2;
    return ret;
}

int crypto_rsign(
        unsigned char *sm, unsigned long long *smlen,
        const unsigned char *m, unsigned long long mlen,
        const unsigned char *sk,
        unsigned char const* const* ring, unsigned long long ring_size) {

    uint32_t signature_len = 0;
    memmove(sm + sizeof(signature_len), m, mlen);
    int ret = crypto_rsign_signature(sm + sizeof(signature_len) + mlen, smlen, sm + sizeof(signature_len), mlen, sk, ring, ring_size);
    signature_len = (uint32_t) *smlen;
    *smlen += mlen + sizeof(uint32_t);
    serialize_uint32(sm, signature_len);
    return ret;
}

int crypto_rsign_verify(const uint8_t *sig,
                       unsigned long long siglen,
                       const uint8_t *m,
                       unsigned long long mlen,
                       unsigned char const* const* ring, unsigned long long ring_size)
{
    return mpcith_rsign_verify(sig, siglen, m, mlen, ring, ring_size);
}

int crypto_rsign_open(
        unsigned char *m, unsigned long long *mlen,
        const unsigned char *sm, unsigned long long smlen,
        unsigned char const* const* ring, unsigned long long ring_size) {

    uint32_t signature_len = 0;
    // The signature is too short to hold the signature length.
    if (smlen < sizeof(signature_len)) {
        return -1;
    }
    
    signature_len = deserialize_uint32(sm);
    // The signature is too short to hold the signature
    if (signature_len + sizeof(uint32_t) > smlen) {
        return -1;
    }

    const size_t message_len = smlen - signature_len - sizeof(uint32_t);
    const uint8_t* message   = sm + sizeof(uint32_t);
    const uint8_t* sig       = sm + sizeof(uint32_t) + message_len;

    int ret = crypto_rsign_verify(sig, signature_len, message, message_len, ring, ring_size);
    if (ret) {
        return ret;
    }

    memmove(m, message, message_len);
    *mlen = message_len;
    return 0;
}

