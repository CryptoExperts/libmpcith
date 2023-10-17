#include "field-interface.h"

int GF_is_zero(const GF x) {
    uint8_t ret = 0;
    for(unsigned int i=0; i<sizeof(GF); i++)
        ret |= ((uint8_t*) x)[i];
    return (ret == 0);
}

void GF_rnd(GF x[], uint32_t nb, samplable_t* entropy) {
    uint8_t* buffer = malloc(nb*PARAM_BLOCK_SIZE);
    byte_sample(entropy, buffer, nb*PARAM_BLOCK_SIZE);
    for(uint32_t i=0; i<nb; i++)
        GF_from_bytes(buffer+i*PARAM_BLOCK_SIZE, x[i]);
    free(buffer);
}

void GF_rnd_x4(GF* const* x, uint32_t nb, samplable_x4_t* entropy) {
    uint8_t* memory = malloc(4*nb*PARAM_BLOCK_SIZE);
    uint8_t* buffer[4] = {
        &memory[0*nb*PARAM_BLOCK_SIZE], &memory[1*nb*PARAM_BLOCK_SIZE],
        &memory[2*nb*PARAM_BLOCK_SIZE], &memory[3*nb*PARAM_BLOCK_SIZE],
    };
    byte_sample_x4(entropy, buffer, nb*PARAM_BLOCK_SIZE);
    for(uint32_t j=0; j<4; j++)
        for(uint32_t i=0; i<nb; i++)
            GF_from_bytes(buffer[j]+i*PARAM_BLOCK_SIZE, x[j][i]);
}

