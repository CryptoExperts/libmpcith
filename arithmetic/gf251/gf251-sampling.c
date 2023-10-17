#include "gf251.h"
#include <stdlib.h>
#include <string.h>

#include "sample.h"

#include "gf251-internal.h"

/*************************************************/
/************        RANDOMNESS       ************/
/*************************************************/

void gf251_random_elements(uint8_t points[], uint32_t nb_points, samplable_t* entropy) {
    uint32_t pos = 0;
    do {
        byte_sample(entropy, points+pos, nb_points-pos);
        for(uint32_t i=pos; i<nb_points; i++) {
            if(points[i] < PRIME) {
                points[pos] = points[i];
                pos++;
            }
        }
    } while(pos < nb_points);
}

void gf251_random_elements_x4(uint8_t* const* points, uint32_t nb_points, samplable_x4_t* entropy) {
    uint32_t buffer_size = nb_points+(nb_points>>4);
    uint8_t* buffer_mem = malloc(4*buffer_size);
    uint8_t* const buffer[4] = {
        &buffer_mem[0],             &buffer_mem[  buffer_size],
        &buffer_mem[2*buffer_size], &buffer_mem[3*buffer_size]
    };
    uint32_t pos[4] = {0};
    unsigned int ok[4] = {0};
    unsigned int nb_ok = 0;
    do {
        byte_sample_x4(entropy, buffer, buffer_size);
        for(uint32_t i=0; i<buffer_size; i++) {
            if(!ok[0] && buffer[0][i] < PRIME) {
                points[0][pos[0]] = buffer[0][i];
                pos[0]++;
                if(pos[0]==nb_points) {
                    ok[0] = 1;
                    nb_ok++;
                }
            }
            if(!ok[1] && buffer[1][i] < PRIME) {
                points[1][pos[1]] = buffer[1][i];
                pos[1]++;
                if(pos[1]==nb_points) {
                    ok[1] = 1;
                    nb_ok++;
                }
            }
            if(!ok[2] && buffer[2][i] < PRIME) {
                points[2][pos[2]] = buffer[2][i];
                pos[2]++;
                if(pos[2]==nb_points) {
                    ok[2] = 1;
                    nb_ok++;
                }
            }
            if(!ok[3] && buffer[3][i] < PRIME) {
                points[3][pos[3]] = buffer[3][i];
                pos[3]++;
                if(pos[3]==nb_points) {
                    ok[3] = 1;
                    nb_ok++;
                }
            }
        }
    } while(nb_ok < 4);
    free(buffer_mem);
}
