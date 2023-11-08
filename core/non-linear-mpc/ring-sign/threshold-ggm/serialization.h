#ifndef LIBMPCITH_SERIALIZATION_H
#define LIBMPCITH_SERIALIZATION_H

#include "parameters-all.h"
#include "mpc-all.h"

// Witness
#ifndef PARAM_WIT_SHORT_SIZE
static inline void wit_serialize(uint8_t* buf, const vec_wit_t* wit) {
    vec_wit_set((vec_wit_t*) buf, wit, NULL);
}
static inline void wit_deserialize(vec_wit_t const** wit, const uint8_t* buf, const uint8_t* memory) {
    (void) memory; // No need to use additional memory
    (*wit) = (const vec_wit_t*) buf;
}
#define PARAM_WIT_SHORT_SIZE PARAM_WIT_SIZE
#define PARAM_WIT_PARSED_SIZE (0)
#else
#define wit_serialize(buf, wit) wit_compress(buf, wit)
static inline void wit_deserialize(vec_wit_t const** wit, const uint8_t* buf, uint8_t* memory) {
    wit_decompress((vec_wit_t*) memory, buf);
    (*wit) = (const vec_wit_t*) memory;
}
#define PARAM_WIT_PARSED_SIZE PARAM_WIT_SIZE
#endif

// Unif
#ifndef PARAM_UNIF_SHORT_SIZE
static inline void unif_serialize(uint8_t* buf, const vec_unif_t* unif) {
    vec_unif_set((vec_unif_t*) buf, unif, NULL);
}
static inline void unif_deserialize(vec_unif_t const** unif, const uint8_t* buf, const uint8_t* memory) {
    (void) memory; // No need to use additional memory
    (*unif) = (const vec_unif_t*) buf;
}
#define PARAM_UNIF_SHORT_SIZE PARAM_UNIF_SIZE
#define PARAM_UNIF_PARSED_SIZE (0)
#else
#define unif_serialize(buf, unif) unif_compress(buf, unif)
static inline void unif_deserialize(vec_unif_t const** unif, const uint8_t* buf, uint8_t* memory) {
    unif_decompress((vec_unif_t*) memory, buf);
    (*unif) = (const vec_unif_t*) memory;
}
#define PARAM_UNIF_PARSED_SIZE PARAM_UNIF_SIZE
#endif

// Broadcast
#ifndef PARAM_BR_SHORT_SIZE
static inline void br_serialize(uint8_t* buf, const vec_broadcast_t* br) {
    vec_br_set((vec_broadcast_t*) buf, br, NULL);
}
static inline void br_deserialize(vec_broadcast_t const** br, const uint8_t* buf, const uint8_t* memory) {
    (void) memory; // No need to use additional memory
    (*br) = (const vec_broadcast_t*) buf;
}
#define PARAM_BR_SHORT_SIZE PARAM_BR_SIZE
#define PARAM_BR_PARSED_SIZE (0)
#else
#define br_serialize(buf, br) br_compress(buf, br)
static inline void br_deserialize(vec_broadcast_t const** br, const uint8_t* buf, uint8_t* memory) {
    br_decompress((vec_broadcast_t*) memory, buf);
    (*br) = (const vec_broadcast_t*) memory;
}
#define PARAM_BR_PARSED_SIZE PARAM_BR_SIZE
#endif

// Selector
#define sel_serialize(buf, sel, info) sel_compress(buf, sel, info)
static inline void sel_deserialize(vec_sel_t const** sel, const uint8_t* buf, uint8_t* memory, sharing_info_t* info) {
    sel_decompress((vec_sel_t*) memory, buf, info);
    (*sel) = (const vec_sel_t*) memory;
}

#endif /* LIBMPCITH_SERIALIZATION_H */
