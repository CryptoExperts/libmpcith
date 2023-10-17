#include "rnd.h"
#include "xof.h"

#define KEY_BYTESIZE 32

void prg_init(prg_context* ctx, const uint8_t* seed, const uint8_t* salt) {
    if(salt != NULL) {
        uint8_t seed_material[PARAM_SALT_SIZE+PARAM_SEED_SIZE];
        memcpy(seed_material, salt, PARAM_SALT_SIZE);
        memcpy(seed_material+PARAM_SALT_SIZE, seed, PARAM_SEED_SIZE);
        prg_init_with_seed_material(ctx, seed_material, PARAM_SALT_SIZE+PARAM_SEED_SIZE);
    } else {
        prg_init_with_seed_material(ctx, seed, PARAM_SEED_SIZE);
    }
}

void prg_init_with_seed_material(prg_context* ctx, const uint8_t* seed_material, unsigned int seed_material_size) {
    // Get the seed material
    uint8_t formatted_seed_material[KEY_BYTESIZE] = {0};
    if(seed_material_size > KEY_BYTESIZE) {
        // We need to compress the seed material
        xof_context xof_ctx;
        xof_init(&xof_ctx);
        xof_update(&xof_ctx, seed_material, seed_material_size);
        xof_final(&xof_ctx);
        xof_squeeze(&xof_ctx, formatted_seed_material, KEY_BYTESIZE);
    } else {
        memcpy(formatted_seed_material, seed_material, seed_material_size);
    } 

    aes256ctr_init_without_nonce(&ctx->state, formatted_seed_material);
    ctx->remaining = 0;
}

void prg_sample(prg_context* ctx, uint8_t* out, uint32_t outlen) {
    if(ctx->remaining > 0) {
        uint8_t nb_copied = (outlen < ctx->remaining) ? outlen : ctx->remaining;
        memcpy(out, ctx->source + (64-ctx->remaining), nb_copied);
        out += nb_copied;
        outlen -= nb_copied;
        ctx->remaining -= nb_copied;
    }
    if(outlen > 0) {
        aes256ctr_squeezeblocks(out, outlen>>6, &ctx->state);
        uint8_t rest = (outlen & 0x3F);
        if(rest > 0) {
            out += (outlen-rest);
            aes256ctr_squeezeblocks(ctx->source, 1, &ctx->state);
            memcpy(out, ctx->source, rest);
            ctx->remaining = 64-rest;
        }
    }
}

void prg_init_x4_array(prg_context ctx[4], uint8_t const* const* seed, const uint8_t* salt) {
    // Get size of the seed material
    unsigned int seed_material_size = PARAM_SEED_SIZE;
    seed_material_size += (salt != NULL) ? PARAM_SALT_SIZE : 0;
    
    // Get the seed material
    uint8_t seed_material[4][KEY_BYTESIZE] = {0};
    if(seed_material_size > KEY_BYTESIZE) {
        // We need to compress the seed material
        xof_context_x4 xof_ctx;
        xof_init_x4(&xof_ctx);
        xof_update_x4(&xof_ctx, seed, PARAM_SEED_SIZE);
        if(salt != NULL)
            xof_update_x4_1(&xof_ctx, salt, PARAM_SALT_SIZE);
        xof_final_x4(&xof_ctx);
        uint8_t* ptr_seed_material[4] = {
            seed_material[0], seed_material[1],
            seed_material[2], seed_material[3]
        };
        xof_squeeze_x4(&xof_ctx, ptr_seed_material, KEY_BYTESIZE);
    } else {
        for(unsigned int i = 0; i < 4; ++i) {
            memcpy(seed_material[i], seed[i], PARAM_SEED_SIZE);
            if(salt != NULL)
                memcpy(seed_material[i]+PARAM_SEED_SIZE, salt, PARAM_SALT_SIZE);
        }
    } 
    
    for(unsigned int i = 0; i < 4; ++i) {
        aes256ctr_init_without_nonce(&ctx[i].state, seed_material[i]);
        ctx[i].remaining = 0;
    }
}

#ifdef PRGX4
void prg_init_x4(prg_context_x4* ctx,  uint8_t const* const* seed, const uint8_t* salt) {
    // Get size of the seed material
    unsigned int seed_material_size = PARAM_SEED_SIZE;
    seed_material_size += (salt != NULL) ? PARAM_SALT_SIZE : 0;
    
    // Get the seed material
    uint8_t seed_material[4][KEY_BYTESIZE] = {0};
    if(seed_material_size > KEY_BYTESIZE) {
        // We need to compress the seed material
        xof_context_x4 xof_ctx;
        xof_init_x4(&xof_ctx);
        xof_update_x4(&xof_ctx, seed, PARAM_SEED_SIZE);
        if(salt != NULL)
            xof_update_x4_1(&xof_ctx, salt, PARAM_SALT_SIZE);
        xof_final_x4(&xof_ctx);
        uint8_t* ptr_seed_material[4] = {
            seed_material[0], seed_material[1],
            seed_material[2], seed_material[3]
        };
        xof_squeeze_x4(&xof_ctx, ptr_seed_material, KEY_BYTESIZE);
    } else {
        for(unsigned int i = 0; i < 4; ++i) {
            memcpy(seed_material[i], seed[i], PARAM_SEED_SIZE);
            if(salt != NULL)
                memcpy(seed_material[i]+PARAM_SEED_SIZE, salt, PARAM_SALT_SIZE);
        }
    } 
    
    for(unsigned int i = 0; i < 4; ++i) {
        aes256ctr_init_without_nonce(&ctx->instances[i].state, seed_material[i]);
        ctx->instances[i].remaining = 0;
    }
}
void prg_init_with_seed_material_x4(prg_context_x4* ctx, uint8_t const* const* seed_material, unsigned int seed_material_size) {
    // Get the seed material
    uint8_t formatted_seed_material[4][KEY_BYTESIZE] = {0};
    if(seed_material_size > KEY_BYTESIZE) {
        // We need to compress the seed material
        xof_context_x4 xof_ctx;
        xof_init_x4(&xof_ctx);
        xof_update_x4(&xof_ctx, seed_material, seed_material_size);
        xof_final_x4(&xof_ctx);
        uint8_t* ptr_seed_material[4] = {
            formatted_seed_material[0], formatted_seed_material[1],
            formatted_seed_material[2], formatted_seed_material[3]
        };
        xof_squeeze_x4(&xof_ctx, ptr_seed_material, KEY_BYTESIZE);
    } else {
        memcpy(formatted_seed_material, seed_material, seed_material_size);
    }

    for(unsigned int i = 0; i < 4; ++i) {
        aes256ctr_init_without_nonce(&ctx->instances[i].state, seed_material[i]);
        ctx->instances[i].remaining = 0;
    }
}

#else
void prg_init_x4(prg_context_x4* ctx,  uint8_t const* const* seed, const uint8_t* salt) {
    for(unsigned int i = 0; i < 4; ++i)
        prg_init(&ctx->instances[i], seed[i], salt);
}
void prg_init_with_seed_material_x4(prg_context_x4* ctx, uint8_t const* const* seed_material, unsigned int seed_material_size) {
    for(unsigned int i = 0; i < 4; ++i)
        prg_init_with_seed_material(&ctx->instances[i], seed_material[i], seed_material_size);
}
#endif

void prg_sample_x4(prg_context_x4* ctx, uint8_t* const* out, uint32_t outlen) {
    for (unsigned int i = 0; i < 4; ++i)
        prg_sample(&ctx->instances[i], out[i], outlen);
}
