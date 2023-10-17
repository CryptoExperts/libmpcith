#include "poly.h"
#include <stdlib.h>

void set_polynomial_as_zero(uint8_t* poly, uint16_t degree) {
    memset(poly, 0, degree+1);
}

void add_polynomials(uint8_t* q_poly, uint8_t* p_poly, uint16_t common_degree) {
    vec_add(q_poly, p_poly, common_degree+1);
}

uint8_t evaluate_polynomial(const uint8_t* poly, uint16_t degree, uint8_t point) {
    // Horner's method
    uint8_t acc = poly[degree];
    for(int i=degree-1; i>=0; i--) {
        acc = sca_mul(acc, point);
        acc = sca_add(acc, poly[i]);
    }
    return acc;
}

uint8_t evaluate_monic_polynomial(const uint8_t* poly, uint16_t degree, uint8_t point) {
    // Horner's method
    uint8_t acc = 1;
    for(int i=degree-1; i>=0; i--) {
        acc = sca_mul(acc, point);
        acc = sca_add(acc, poly[i]);
    }
    return acc;
}

void evaluate_polynomial_in_extfield(uint8_t* out, const uint8_t* poly, uint16_t degree, uint8_t* point, uint8_t ext_degree) {
    // Horner's method
    uint8_t* tmp = calloc(ext_degree, sizeof(uint8_t));
    memset(out, 0, ext_degree);
    out[0] = poly[degree];
    for(int i=degree-1; i>=0; i--) {
        ext_mul(out, out, point);
        tmp[0] = poly[i];
        ext_add(out, out, tmp);
    }
    free(tmp);
}

void evaluate_monic_polynomial_in_extfield(uint8_t* out, const uint8_t* poly, uint16_t degree, uint8_t* point, uint8_t ext_degree) {
    // Horner's method
    uint8_t* tmp = calloc(ext_degree, sizeof(uint8_t));
    memset(out, 0, ext_degree);
    out[0] = 1;
    for(int i=degree-1; i>=0; i--) {
        ext_mul(out, out, point);
        tmp[0] = poly[i];
        ext_add(out, out, tmp);
    }
    free(tmp);
}

void multiply_polynomial_by_scalar(uint8_t* out, const uint8_t* in, uint16_t degree_of_in, uint8_t scalar) {
    vec_mul(out, scalar, in, degree_of_in+1);
}

// If (X-alpha) divides P_in, returns P_in / (X-alpha)
void remove_one_degree_factor(uint8_t* out, const uint8_t* in, uint16_t in_degree, uint8_t alpha) {
    out[in_degree-1] = in[in_degree];
    for(int i=in_degree-2; i>=0; i--)
        out[i] = sca_add(in[i+1], sca_mul(alpha, out[i+1]));
}

// If (X-alpha) divides P_in, returns P_in / (X-alpha)
void remove_one_degree_factor_from_monic(uint8_t* out, const uint8_t* in, uint16_t in_degree, uint8_t alpha) {
    out[in_degree-1] = 1;
    for(int i=in_degree-2; i>=0; i--)
        out[i] = sca_add(in[i+1], sca_mul(alpha, out[i+1]));
}
