#include "gf251.h"
#include <stdlib.h>
#include <string.h>

#include "gf251-internal.h"

/*************************************************/
/***********      FIELD OPERATIONS     ***********/
/*************************************************/

uint8_t gf251_add(uint8_t a, uint8_t b) {
    uint16_t res = a+b;
    res -= PRIME*(PRIME <= res);
    return res;
}

uint8_t gf251_sub(uint8_t a, uint8_t b) {
    uint16_t res = a+PRIME-b;
    res -= PRIME*(PRIME <= res);
    return res;
}

uint8_t gf251_neg(uint8_t a) {
    return (a != 0)*(PRIME - a);
}

uint8_t gf251_mul(uint8_t a, uint8_t b) {
    uint16_t res = a*b;
    return _gf251_reduce16(res);
}

uint8_t gf251_innerproduct(const uint8_t* x, const uint8_t* y, uint32_t size) {
    uint32_t z = 0;
    for(uint32_t i=0; i<size; i++)
        z += x[i]*y[i];
    return _gf251_reduce32(z);
}

// Warning, getting the inverse of a secret value using this table,
//   would lead to non-constant-time implementation. Only accessing
//   to public positions is allowed.
const uint8_t gf251_inverse_tab[251] = {0, 1, 126, 84, 63, 201, 42, 36, 157, 28, 226, 137, 21, 58, 18, 67, 204, 192, 14, 185, 113, 12, 194, 131, 136, 241, 29, 93, 9, 26, 159, 81, 102, 213, 96, 208, 7, 95, 218, 103, 182, 49, 6, 216, 97, 106, 191, 235, 68, 41, 246, 64, 140, 90, 172, 178, 130, 229, 13, 234, 205, 107, 166, 4, 51, 112, 232, 15, 48, 211, 104, 99, 129, 196, 173, 164, 109, 163, 177, 197, 91, 31, 150, 124, 3, 189, 108, 176, 174, 110, 53, 80, 221, 27, 243, 37, 34, 44, 146, 71, 123, 169, 32, 39, 70, 153, 45, 61, 86, 76, 89, 199, 65, 20, 240, 227, 132, 118, 117, 135, 228, 195, 179, 100, 83, 249, 2, 168, 151, 72, 56, 23, 116, 134, 133, 119, 24, 11, 231, 186, 52, 162, 175, 165, 190, 206, 98, 181, 212, 219, 82, 128, 180, 105, 207, 217, 214, 8, 224, 30, 171, 198, 141, 77, 75, 143, 62, 248, 127, 101, 220, 160, 54, 74, 88, 142, 87, 78, 55, 122, 152, 147, 40, 203, 236, 19, 139, 200, 247, 85, 144, 46, 17, 238, 22, 121, 73, 79, 161, 111, 187, 5, 210, 183, 16, 60, 145, 154, 35, 245, 202, 69, 148, 33, 156, 244, 43, 155, 38, 149, 170, 92, 225, 242, 158, 222, 10, 115, 120, 57, 239, 138, 66, 237, 59, 47, 184, 233, 193, 230, 114, 25, 223, 94, 215, 209, 50, 188, 167, 125, 250};

/*************************************************/
/***********     BATCHED OPERATIONS    ***********/
/*************************************************/

// vz[] += vx[]
void gf251_add_tab(uint8_t* vz, const uint8_t* vx, uint32_t size) {
    uint16_t res;
    for(uint32_t i=0; i<size; i++) {
        res = vz[i]+vx[i];
        res -= PRIME*(PRIME <= res);
        vz[i] = res;
    }
}

// vz[] -= vx[]
void gf251_sub_tab(uint8_t* vz, const uint8_t* vx, uint32_t size) {
    uint16_t res;
    for(uint32_t i=0; i<size; i++) {
        res = vz[i]+PRIME-vx[i];
        res -= PRIME*(PRIME <= res);
        vz[i] = res;
    }
}

// vz[] = vx[] * y
void gf251_mul_tab(uint8_t* vz, const uint8_t* vx, uint8_t y, uint32_t size) {
    uint16_t res;
    for(uint32_t i=0; i<size; i++) {
        res = vx[i]*y;
        vz[i] = _gf251_reduce16(res);
    }
}

// vz[] += vx[] * y
void gf251_muladd_tab(uint8_t* vz, uint8_t y, const uint8_t* vx, uint32_t size) {
    uint16_t res;
    for(uint32_t i=0; i<size; i++) {
        res = vz[i] + vx[i]*y;
        vz[i] = _gf251_reduce16(res);
    }
}

// vz[] = vz[] * y + vx[]
void gf251_mul_and_add_tab(uint8_t* vz, uint8_t y, const uint8_t* vx, uint32_t size) {
    uint16_t res;
    for(uint32_t i=0; i<size; i++) {
        res = vz[i]*y + vx[i];
        vz[i] = _gf251_reduce16(res);
    }
}

// vz[] = -vz[]
void gf251_neg_tab(uint8_t* vz, uint32_t size) {
    for(uint32_t i=0; i<size; i++)
        vz[i] = (vz[i] != 0)*(PRIME - vz[i]);
}

// vz[] += sum_i vx[i][]
void gf251_add_many_tab(uint8_t* vz, uint8_t const* const* vx, uint32_t size, uint32_t nb_tabs) {
    uint32_t* unreduced_vz = (uint32_t*) calloc(size, sizeof(uint32_t));
    for(uint32_t j=0; j<nb_tabs; j++)
        for(uint32_t i=0; i<size; i++)
            unreduced_vz[i] += vx[j][i];
    for(uint32_t i=0; i<size; i++)
        vz[i] = _gf251_reduce32(unreduced_vz[i] + vz[i]);
    free(unreduced_vz);
}
