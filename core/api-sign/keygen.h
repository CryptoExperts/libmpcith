#ifndef LIBMPCITH_KEYGEN_H
#define LIBMPCITH_KEYGEN_H

#include <stdint.h>
#include <stddef.h>
#include "parameters.h"
#include "witness.h"

#define PARAM_PUBLICKEYBYTES (PARAM_INSTANCE_SIZE)
#ifndef SHORT_SECRET_KEY
#define PARAM_SECRETKEYBYTES (PARAM_INSTANCE_SIZE + PARAM_SOL_SIZE)
#else
#define PARAM_SECRETKEYBYTES (PARAM_SEED_SIZE)
#endif

int crypto_sign_keypair(unsigned char *pk, unsigned char *sk);
int crypto_sign_valid_keys(unsigned char *pk, unsigned char *sk);

int mpcith_keygen(unsigned char *pk, unsigned char *sk);
int mpcith_validate_keys(const unsigned char *pk, const unsigned char *sk);

// MPCitH Internal
typedef struct mpcith_public_key_t {
    instance_t* inst;
} mpcith_public_key_t;

typedef struct mpcith_secret_key_t {
    instance_t* inst;
    solution_t* wit;
} mpcith_secret_key_t;

int mpcith_keygen_internal(mpcith_public_key_t* pk, mpcith_secret_key_t* sk);
int mpcith_validate_keys_internal(const mpcith_public_key_t* pk, const mpcith_secret_key_t* sk);

int deserialize_public_key(mpcith_public_key_t* key, const uint8_t* buf, size_t buflen);
int serialize_public_key(uint8_t* buf, const mpcith_public_key_t* key, size_t buflen);

int deserialize_secret_key(mpcith_secret_key_t* key, const uint8_t* buf, size_t buflen);
int serialize_secret_key(uint8_t* buf, const mpcith_secret_key_t* key, size_t buflen);

int mpcith_free_keys_internal(mpcith_public_key_t* pk, mpcith_secret_key_t* sk);

#endif /* LIBMPCITH_KEYGEN_H */
