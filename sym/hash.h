#ifndef LIBMPCITH_HASH_H
#define LIBMPCITH_HASH_H

#include <stdint.h>
#include <stddef.h>

/* Prefix values for domain separation. */
static const uint8_t HASH_PREFIX_NONE = 0xFF;
static const uint8_t HASH_PREFIX_0 = 0;
static const uint8_t HASH_PREFIX_1 = 1;
static const uint8_t HASH_PREFIX_2 = 2;
static const uint8_t HASH_PREFIX_3 = 3;
static const uint8_t HASH_PREFIX_4 = 4;
static const uint8_t HASH_PREFIX_5 = 5;

#include "hash-struct.h"

// Simple call
void hash_init(hash_context* ctx);
void hash_init_prefix(hash_context* ctx, const uint8_t prefix);
void hash_update(hash_context* ctx, const uint8_t* data, size_t byte_len);
void hash_update_uint16_le(hash_context* ctx, uint16_t data);
void hash_final(hash_context* ctx, uint8_t* digest);

// Fourfold call
#ifndef HASHX4
typedef struct hash_context_x4_s {
  hash_context instances[4];
} hash_context_x4;
#endif
void hash_init_x4(hash_context_x4* ctx);
void hash_init_prefix_x4(hash_context_x4* ctx, uint8_t prefix);
void hash_update_x4(hash_context_x4* ctx, uint8_t const* const* data, size_t byte_len);
void hash_update_x4_4(hash_context_x4* ctx,
                                const uint8_t* data0, const uint8_t* data1,
                                const uint8_t* data2, const uint8_t* data3,
                                        size_t byte_len);
void hash_update_x4_1(hash_context_x4* ctx, const uint8_t* data, size_t byte_len);
void hash_update_x4_uint16_le(hash_context_x4* ctx, uint16_t data);
void hash_update_x4_uint16s_le(hash_context_x4* ctx, const uint16_t data[4]);
void hash_final_x4(hash_context_x4* ctx, uint8_t* const* digest);

#endif /* LIBMPCITH_HASH_H */
