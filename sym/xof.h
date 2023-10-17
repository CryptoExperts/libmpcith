#ifndef LIBMPCITH_XOF_H
#define LIBMPCITH_XOF_H

#include <stdint.h>
#include <stddef.h>

#include "xof-struct.h"

// Simple call
void xof_init(xof_context* ctx);
void xof_init_prefix(xof_context* ctx, const uint8_t prefix);
void xof_update(xof_context* ctx, const uint8_t* data, size_t byte_len);
void xof_update_uint16_le(xof_context* ctx, uint16_t data);
void xof_final(xof_context* ctx);
void xof_squeeze(xof_context* ctx, uint8_t* buffer, uint32_t byte_len);

// Fourfold call
#ifndef XOFX4
typedef struct xof_context_x4_s {
  xof_context instances[4];
} xof_context_x4;
#endif
void xof_init_x4(xof_context_x4* ctx);
void xof_init_prefix_x4(xof_context_x4* ctx, uint8_t prefix);
void xof_update_x4(xof_context_x4* ctx, uint8_t const* const* data, size_t byte_len);
void xof_update_x4_4(xof_context_x4* ctx,
                                const uint8_t* data0, const uint8_t* data1,
                                const uint8_t* data2, const uint8_t* data3,
                                        size_t byte_len);
void xof_update_x4_1(xof_context_x4* ctx, const uint8_t* data, size_t byte_len);
void xof_update_x4_uint16_le(xof_context_x4* ctx, uint16_t data);
void xof_update_x4_uint16s_le(xof_context_x4* ctx, const uint16_t data[4]);
void xof_final_x4(xof_context_x4* ctx);
void xof_squeeze_x4(xof_context_x4* ctx, uint8_t* const* buffer, uint32_t buflen);

#endif /* LIBMPCITH_XOF_H */
