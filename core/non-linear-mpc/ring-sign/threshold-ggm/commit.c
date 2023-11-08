#include "commit.h"
#include "hash.h"

// Commitment x1
static void commit_string(uint8_t* digest, const uint8_t* str, size_t len, const uint8_t* salt, uint16_t e, uint16_t i) {
    hash_context ctx;
    hash_init_prefix(&ctx, HASH_PREFIX_COMMITMENT);
    hash_update(&ctx, salt, PARAM_SALT_SIZE);
    hash_update_uint16_le(&ctx, e);
    hash_update_uint16_le(&ctx, i);
    hash_update(&ctx, str, len);
    hash_final(&ctx, digest);
}

void commit_seed(uint8_t* digest, const uint8_t* seed, const uint8_t* salt, uint16_t e, uint16_t i) {
    commit_string(digest, seed, PARAM_SEED_SIZE, salt, e, i);
}

void commit_seed_and_aux(uint8_t* digest, const uint8_t* seed, const vec_wit_t* wit, const vec_sel_t* sel, const uint8_t* salt, uint16_t e, uint16_t i, sharing_info_t* info) {
    hash_context ctx;
    hash_init_prefix(&ctx, HASH_PREFIX_COMMITMENT);
    hash_update(&ctx, salt, PARAM_SALT_SIZE);
    hash_update_uint16_le(&ctx, e);
    hash_update_uint16_le(&ctx, i);
    hash_update(&ctx, seed, PARAM_SEED_SIZE);
    hash_update(&ctx, (uint8_t*) wit, PARAM_WIT_SIZE);
    hash_update(&ctx, (uint8_t*) sel, get_selector_bytesize(info));
    hash_final(&ctx, digest);
}


// Commitment x4
static void commit_string_x4(uint8_t** digest, uint8_t const* const* str, size_t len, const uint8_t* salt, uint16_t e, const uint16_t* i) {
    hash_context_x4 ctx;
    hash_init_prefix_x4(&ctx, HASH_PREFIX_COMMITMENT);
    hash_update_x4_1(&ctx, salt, PARAM_SALT_SIZE);
    hash_update_x4_uint16_le(&ctx, e);
    hash_update_x4_uint16s_le(&ctx, i);
    hash_update_x4(&ctx, str, len);
    hash_final_x4(&ctx, digest);
}

void commit_seed_x4(uint8_t** digest, uint8_t const* const* seed, const uint8_t* salt, uint16_t e, const uint16_t* i) {
    commit_string_x4(digest, seed, PARAM_SEED_SIZE, salt, e, i);
}

void commit_seed_and_aux_x4(uint8_t** digest, uint8_t const* const* seed, vec_wit_t const* const* wit, vec_sel_t const* const* sel, const uint8_t* salt, uint16_t e, const uint16_t* i, sharing_info_t* info) {
    hash_context_x4 ctx;
    hash_init_prefix_x4(&ctx, HASH_PREFIX_COMMITMENT);
    hash_update_x4_1(&ctx, salt, PARAM_SALT_SIZE);
    hash_update_x4_uint16_le(&ctx, e);
    hash_update_x4_uint16s_le(&ctx, i);
    hash_update_x4(&ctx, seed, PARAM_SEED_SIZE);
    hash_update_x4(&ctx, (uint8_t const* const*) wit, PARAM_WIT_SIZE);
    hash_update_x4(&ctx, (uint8_t const* const*) sel, get_selector_bytesize(info));
    hash_final_x4(&ctx, digest);
}
