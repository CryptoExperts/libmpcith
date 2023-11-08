#include <stdio.h>
#include <stdlib.h>

#include "witness.h"
#include "field.h"

static void expand_extended_witness(samplable_t* entropy, uint8_t x_chunks[PARAM_SPLITTING_FACTOR][PARAM_CHUNK_LENGTH], uint8_t q_poly[PARAM_SPLITTING_FACTOR][PARAM_CHUNK_WEIGHT]) {
    uint32_t i, j;

    uint8_t positions[PARAM_CHUNK_WEIGHT];
    uint8_t non_zero_coordinates[PARAM_CHUNK_WEIGHT];

    for(size_t nchunk=0; nchunk<PARAM_SPLITTING_FACTOR; nchunk++) {
        // First, let us compute a list of 'w' positions (without redundancy)
        i = 0;
        while(i<PARAM_CHUNK_WEIGHT) {
            byte_sample(entropy, &positions[i], 1);
            if(positions[i] >= PARAM_CHUNK_LENGTH)
                continue;
            char is_redundant = 0;
            for(j=0; j<i; j++)
                if(positions[i] == positions[j])
                    is_redundant = 1;
            if( !is_redundant )
                i++;
        }

        // Then, compute the non-zero coordinates
        for(i=0; i<PARAM_CHUNK_WEIGHT; i++) {
            vec_rnd(&non_zero_coordinates[i], 1, entropy);
            while(non_zero_coordinates[i] == 0)
                vec_rnd(&non_zero_coordinates[i], 1, entropy);
        }

        // Compute x
        memset(x_chunks[nchunk], 0, PARAM_CHUNK_LENGTH);
        for(i=0; i<PARAM_CHUNK_LENGTH; i++)
            for(j=0; j<PARAM_CHUNK_WEIGHT; j++)
                x_chunks[nchunk][i] ^= non_zero_coordinates[j]*(positions[j] == i);

        // Compute Q
        // Q <- 1
        if(q_poly != NULL) {
            //set_polynomial_as_zero(q_poly[nchunk], DEGREE_Q);
            uint8_t* q_coeffs = q_poly[nchunk];
            memset(q_coeffs, 1, PARAM_CHUNK_WEIGHT);
            for(i=0; i<PARAM_CHUNK_WEIGHT; i++) {
                // Q' <- Q · (X-w_i)
                uint8_t wi = positions[i];
                uint8_t minus_wi = sca_sub(0,wi);
                //q_coeffs[i+1] = q_coeffs[i]; // No need anymore since the vector is filled of 1
                for(j=i; j>=1; j--)
                    q_coeffs[j] = sca_add(q_coeffs[j-1], sca_mul(minus_wi, q_coeffs[j]));
                q_coeffs[0] = sca_mul(minus_wi, q_coeffs[0]);
            }
        }
    }
}

void uncompress_instance(instance_t* inst) {
    if(inst->mat_H == NULL) {
        inst->mat_H = malloc(PARAM_PCMATRIX_BYTESIZE);
        prg_context entropy_ctx;
        samplable_t entropy = prg_to_samplable(&entropy_ctx);
        prg_init(&entropy_ctx, inst->seed_H, NULL);
        vec_rnd(*inst->mat_H, PARAM_PCMATRIX_BYTESIZE, &entropy);
    }
}

void generate_instance_with_solution(instance_t** inst, solution_t** sol, samplable_t* entropy) {
    // Allocate
    *sol = (solution_t*) malloc(sizeof(solution_t));
    *inst = (instance_t*) malloc(sizeof(instance_t));
    (*inst)->mat_H = NULL;

    // Build the polynomial and Q
    uint8_t x_chunks[PARAM_SPLITTING_FACTOR][PARAM_CHUNK_LENGTH];
    uint8_t* x = (uint8_t*) x_chunks;
    expand_extended_witness(entropy, x_chunks, (*sol)->q_poly);

    // Split x as (x_A | x_B)
    uint8_t* x_b = &x[PARAM_PLAINTEXT_LENGTH];
    memcpy((*sol)->x_A, x, PARAM_PLAINTEXT_LENGTH);

    // Sample a seed for matrix H
    byte_sample(entropy, (*inst)->seed_H, PARAM_SEED_SIZE);

    // Build H
    uncompress_instance(*inst);

    // Build y = x_B + H x_A
    memcpy((*inst)->y, x_b, PARAM_SYNDROME_LENGTH);
    matcols_muladd((*inst)->y, (*sol)->x_A, *(*inst)->mat_H, PARAM_PLAINTEXT_LENGTH, PARAM_SYNDROME_LENGTH);
}

void free_instance_solution(solution_t* sol) {
    free(sol);
}

void free_instance(instance_t* inst) {
    if(inst->mat_H)
        free(inst->mat_H);
    free(inst);
}

int are_same_instances(instance_t* inst1, instance_t* inst2) {
    int ret = 0;
    ret |= memcmp(inst1->seed_H, inst2->seed_H, PARAM_SEED_SIZE);
    ret |= memcmp(inst1->y, inst2->y, PARAM_SYNDROME_LENGTH);
    return ((ret) ? 0 : 1);
}

static uint8_t evaluate_monic_polynomial(const uint8_t* poly, uint16_t degree, uint8_t point) {
    // Horner's method
    uint8_t acc = 1;
    for(int i=degree-1; i>=0; i--) {
        acc = sca_mul(acc, point);
        acc = sca_add(acc, poly[i]);
    }
    return acc;
}

int is_correct_solution(instance_t* inst, solution_t* sol) {
    int ret = 0;
    uncompress_instance(inst);

    /// We recompute the vector x as (x_A||x_B),
    ///      where x_B := y - H' x_A

    // x = x_A || x_B
    uint8_t* x = (uint8_t*) aligned_alloc(32, PARAM_CODEWORD_LENGTH_CEIL32);
    if(x == NULL) {
        printf("Error: Aligned alloc returns NULL.\n");
        return 0;
    }
    memcpy(x, sol->x_A, PARAM_PLAINTEXT_LENGTH);
    memcpy(&x[PARAM_PLAINTEXT_LENGTH], inst->y, PARAM_SYNDROME_LENGTH);

    // x_B = y - H_A x_A
    vec_neg(&x[PARAM_PLAINTEXT_LENGTH], PARAM_SYNDROME_LENGTH);
    matcols_muladd(&x[PARAM_PLAINTEXT_LENGTH], sol->x_A, *inst->mat_H, PARAM_PLAINTEXT_LENGTH, PARAM_SYNDROME_LENGTH);
    vec_neg(&x[PARAM_PLAINTEXT_LENGTH], PARAM_SYNDROME_LENGTH);

    // Interpret the vector s as a vector of polynomials
    uint8_t (*x_chunks)[PARAM_CHUNK_LENGTH] = (void*) x;

    for(size_t h=0; h<PARAM_SPLITTING_FACTOR; h++) {
        /// We check that the evaluations of the polynomial S forms
        ///     a vector of hamming weight equal to w
        int weight = 0;
        for(uint32_t i=0; i<PARAM_CHUNK_LENGTH; i++)
            weight += (x_chunks[h][i] != 0);
        if(weight != PARAM_CHUNK_WEIGHT) {
            printf("Error: x is not a vector with the right hamming weight (w=%d).\n", weight);
            ret = -1;
        }

        /// We check that an evaluation of Q is zero iff the corresponding
        ///    coordinate of x is zero
        for(uint32_t i=0; i<PARAM_CHUNK_LENGTH; i++) {
            uint8_t q_eval = evaluate_monic_polynomial(sol->q_poly[h], PARAM_CHUNK_WEIGHT, (uint8_t) i);
            if((q_eval != 0) == (x_chunks[h][i] != 0)) {
                printf("Error: Wrong Q evaluation (%d,%d,%d).\n", i, q_eval, x_chunks[h][i]);
                ret = -1;
            }
        }
    }

    free(x);
    return (ret == 0);
}
