#ifndef MPC_COMPRESSION_H
#define MPC_COMPRESSION_H

#include "mpc-struct.h"
#include "field.h"

#define wit_compress(buf,wit) vec_to_bytes(buf,wit,sizeof(vec_wit_t))
#define unif_compress(buf,unif) vec_to_bytes(buf,unif,sizeof(vec_unif_t))
#define hint_compress(buf,hint) vec_to_bytes(buf,hint,sizeof(vec_hint_t))
#define br_compress(buf,br) vec_to_bytes(buf,br,sizeof(vec_broadcast_t))
#define wit_decompress(wit,buf) vec_from_bytes(wit,buf,sizeof(vec_wit_t))
#define unif_decompress(unif,buf) vec_from_bytes(unif,buf,sizeof(vec_unif_t))
#define hint_decompress(hint,buf) vec_from_bytes(hint,buf,sizeof(vec_hint_t))
#define br_decompress(br,buf) vec_from_bytes(br,buf,sizeof(vec_broadcast_t))

#define PARAM_WIT_SHORT_SIZE (get_serialized_size(sizeof(vec_wit_t)))
#define PARAM_UNIF_SHORT_SIZE (get_serialized_size(sizeof(vec_unif_t)))
#define PARAM_HINT_SHORT_SIZE (get_serialized_size(sizeof(vec_hint_t)))
#define PARAM_BR_SHORT_SIZE (get_serialized_size(sizeof(vec_broadcast_t)))

//#define PARAM_COMPRESSED_BR_SIZE (sizeof(vec_unif_t))
#define PARAM_COMPRESSED_BR_SIZE (get_serialized_size(sizeof(vec_unif_t)))

#endif /* MPC_COMPRESSION_H */
