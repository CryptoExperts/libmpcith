#ifndef LIBMPCITH_SERIALIZATION_H
#define LIBMPCITH_SERIALIZATION_H

#include "parameters-all.h"
#include "mpc-all.h"

// Witness
#ifndef PARAM_WIT_SHORT_SIZE
static inline void wit_serialize(uint8_t* buf, const vec_wit_t* wit) {
    vec_wit_set((vec_wit_t*) buf, wit);
}
static inline void wit_deserialize(vec_wit_t const** wit, const uint8_t* buf, const uint8_t* memory) {
    (void) memory; // No need to use additional memory
    (*wit) = (const vec_wit_t*) buf;
}
#define PARAM_WIT_SHORT_SIZE PARAM_WIT_SIZE
#define PARAM_WIT_PARSED_SIZE (0)
#else
#define wit_serialize(buf, wit) vec_wit_compress(buf, wit)
static inline void wit_deserialize(vec_wit_t const** wit, const uint8_t* buf, uint8_t* memory) {
    vec_wit_decompress((vec_wit_t*) memory, buf);
    (*wit) = (const vec_wit_t*) memory;
}
#define PARAM_WIT_PARSED_SIZE PARAM_WIT_SIZE
#endif

// Unif
#ifndef PARAM_UNIF_SHORT_SIZE
static inline void unif_serialize(uint8_t* buf, const vec_unif_t* unif) {
    vec_unif_set((vec_unif_t*) buf, unif);
}
static inline void unif_deserialize(vec_unif_t const** unif, const uint8_t* buf, const uint8_t* memory) {
    (void) memory; // No need to use additional memory
    (*unif) = (const vec_unif_t*) buf;
}
#define PARAM_UNIF_SHORT_SIZE PARAM_UNIF_SIZE
#define PARAM_UNIF_PARSED_SIZE (0)
#else
#define unif_serialize(buf, unif) vec_unif_compress(buf, unif)
static inline void unif_deserialize(vec_unif_t const** unif, const uint8_t* buf, uint8_t* memory) {
    vec_unif_decompress((vec_unif_t*) memory, buf);
    (*unif) = (const vec_unif_t*) memory;
}
#define PARAM_UNIF_PARSED_SIZE PARAM_UNIF_SIZE
#endif

// Hint
#ifndef PARAM_HINT_SHORT_SIZE
static inline void hint_serialize(uint8_t* buf, const vec_hint_t* hint) {
    vec_hint_set((vec_hint_t*) buf, hint);
}
static inline void hint_deserialize(vec_hint_t const** hint, const uint8_t* buf, const uint8_t* memory) {
    (void) memory; // No need to use additional memory
    (*hint) = (const vec_hint_t*) buf;
}
#define PARAM_HINT_SHORT_SIZE PARAM_HINT_SIZE
#define PARAM_HINT_PARSED_SIZE (0)
#else
#define hint_serialize(buf, hint) vec_hint_compress(buf, hint)
static inline void hint_deserialize(vec_hint_t const** hint, const uint8_t* buf, uint8_t* memory) {
    vec_hint_decompress((vec_hint_t*) memory, buf);
    (*hint) = (const vec_hint_t*) memory;
}
#define PARAM_HINT_PARSED_SIZE PARAM_HINT_SIZE
#endif

// Broadcast
#ifndef PARAM_BR_SHORT_SIZE
static inline void br_serialize(uint8_t* buf, const vec_broadcast_t* br) {
    vec_br_set((vec_broadcast_t*) buf, br);
}
static inline void br_deserialize(vec_broadcast_t const** br, const uint8_t* buf, const uint8_t* memory) {
    (void) memory; // No need to use additional memory
    (*br) = (const vec_broadcast_t*) buf;
}
#define PARAM_BR_SHORT_SIZE PARAM_BR_SIZE
#define PARAM_BR_PARSED_SIZE (0)
#else
#define br_serialize(buf, br) vec_br_compress(buf, br)
static inline void br_deserialize(vec_broadcast_t const** br, const uint8_t* buf, uint8_t* memory) {
    vec_br_decompress((vec_broadcast_t*) memory, buf);
    (*br) = (const vec_broadcast_t*) memory;
}
#define PARAM_BR_PARSED_SIZE PARAM_BR_SIZE
#endif

#endif /* LIBMPCITH_SERIALIZATION_H */
