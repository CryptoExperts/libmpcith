#include "witness.h"
#include "gf2to31.h"
#include <stdlib.h>
#include <stdio.h>

void expand_extended_witness(samplable_t* entropy, uint8_t x[PARAM_CODE_LENGTH][PARAM_CODE_EXT_BYTESIZE], uint8_t beta[PARAM_CODE_WEIGHT][PARAM_CODE_EXT_BYTESIZE]) {
    uint8_t g[PARAM_CODE_WEIGHT][PARAM_CODE_EXT_BYTESIZE];
    
    int is_valid_witness = 0;
    while(!is_valid_witness) {
        gf2to31_random_elements((uint8_t*) g, PARAM_CODE_WEIGHT, entropy);
        is_valid_witness = 1;

        uint8_t gq[PARAM_CODE_WEIGHT][PARAM_CODE_WEIGHT+1][PARAM_CODE_EXT_BYTESIZE];
        for(int i=0; i<PARAM_CODE_WEIGHT; i++) {
            memcpy(gq[i][0], g[i], PARAM_CODE_EXT_BYTESIZE);
            for(int j=1; j<PARAM_CODE_WEIGHT+1; j++) {
                // gq[i][j] = gq[i][j-1] ^ q
                uint32_t v = *((uint32_t*) gq[i][j-1]);
                uint32_t r = gf2to31_squ(v);
                *((uint32_t*) gq[i][j]) = r;
            }
        }

        // Build the polynomial L_U
        for(int i=0; i<PARAM_CODE_WEIGHT; i++) {
            uint32_t pivot = *((uint32_t*) gq[i][i]);
            if(pivot == 0) {
                is_valid_witness = 0;
                break;
            }
            uint32_t inv_pivot = gf2to31_inv(pivot);

            // gq[i][k] <-- gq[i][k]/pivot, for all k > i
            for(int k=i+1; k<PARAM_CODE_WEIGHT+1; k++) {
                uint32_t v = *((uint32_t*) gq[i][k]);
                uint32_t r = gf2to31_mul(v,inv_pivot);
                *((uint32_t*) gq[i][k]) = r;
            }

            for(int j=i+1; j<PARAM_CODE_WEIGHT; j++) {
                uint32_t f = *((uint32_t*) gq[j][i]);
                for(int k=i+1; k<PARAM_CODE_WEIGHT+1; k++) {
                    // gq[j][k] <- gq[j][k] - f*gq[i][k]
                    uint32_t v1 = *((uint32_t*) gq[j][k]);
                    uint32_t v2 = *((uint32_t*) gq[i][k]);
                    uint32_t r = gf2to31_add(v1, gf2to31_mul(f, v2));
                    *((uint32_t*) gq[j][k]) = r;
                }
            }
        }
        if(!is_valid_witness)
            continue;
        for(int i=PARAM_CODE_WEIGHT-1; i>=0; i--) {
            uint32_t r = *((uint32_t*) gq[i][PARAM_CODE_WEIGHT]);
            for(int k=PARAM_CODE_WEIGHT-1; k>i; k--) {
                uint32_t v1 = *((uint32_t*) gq[i][k]);
                uint32_t v2 = *((uint32_t*) beta[k]);
                r = gf2to31_add(r, gf2to31_mul(v1,v2));
            }
            *((uint32_t*) beta[i]) = r;
        }

        // Build the vector x
        for(int i=0; i<PARAM_CODE_LENGTH; i++) {
            uint8_t comb[PARAM_CODE_WEIGHT]; // Coefficient in F_2
            byte_sample(entropy, comb, PARAM_CODE_WEIGHT);

            uint32_t r = 0;
            for(int j=0; j<PARAM_CODE_WEIGHT; j++) {
                uint32_t v = *((uint32_t*) g[j]);
                int bit = (comb[j] & 1);
                r = gf2to31_add(r, bit*v);
            }
            *((uint32_t*) x[i]) = r;
        }
    }
}

void uncompress_instance(instance_t* inst) {
    if(inst->mat_H == NULL) {
        inst->mat_H = malloc(PARAM_PCMATRIX_BYTESIZE);
        prg_context entropy_ctx;
        samplable_t entropy = prg_to_samplable(&entropy_ctx);
        prg_init(&entropy_ctx, inst->seed_H, NULL);
        gf2to31_random_elements(*inst->mat_H, PARAM_PCMATRIX_BYTESIZE>>2, &entropy);
    }
}

void generate_instance_with_solution(instance_t** inst, solution_t** sol, samplable_t* entropy) {
    // Allocate
    *sol = (solution_t*) malloc(sizeof(solution_t));
    *inst = (instance_t*) malloc(sizeof(instance_t));
    (*inst)->mat_H = NULL;

    // Build the polynomial S, Q and P
    uint8_t x[PARAM_CODE_LENGTH][PARAM_CODE_EXT_BYTESIZE];
    expand_extended_witness(entropy, x, (*sol)->beta);

    // Split x as (x_A | x_B)
    uint8_t* x_b = (uint8_t*) &x[PARAM_PLAINTEXT_LENGTH];
    memcpy((uint8_t*) (*sol)->x_A, x, PARAM_CODE_DIMENSION*PARAM_CODE_EXT_BYTESIZE);

    // Sample a seed for matrix H
    byte_sample(entropy, (*inst)->seed_H, PARAM_SEED_SIZE);

    // Build H
    uncompress_instance(*inst);

    // Build y
    memcpy((uint8_t*) (*inst)->y, x_b, PARAM_SYNDROME_LENGTH*PARAM_CODE_EXT_BYTESIZE);
    gf2to31_matcols_muladd((uint8_t*) (*inst)->y, (uint8_t*) (*sol)->x_A, *(*inst)->mat_H, PARAM_PLAINTEXT_LENGTH, PARAM_SYNDROME_LENGTH);
}

void hash_update_instance(hash_context* ctx, const instance_t* inst) {
    hash_update(ctx, inst->seed_H, PARAM_SEED_SIZE);
    hash_update(ctx, (const uint8_t*) inst->y, PARAM_SYNDROME_LENGTH*PARAM_CODE_EXT_BYTESIZE);
}

void serialize_instance(uint8_t* buf, const instance_t* inst) {
    memcpy(buf, inst->seed_H, PARAM_SEED_SIZE);
    memcpy(buf + PARAM_SEED_SIZE, inst->y, PARAM_SYNDROME_BYTESIZE);
}

instance_t* deserialize_instance(const uint8_t* buf) {
    instance_t* inst = (instance_t*) malloc(sizeof(instance_t));
    inst->mat_H = NULL;
    memcpy(inst->seed_H, buf, PARAM_SEED_SIZE);
    memcpy(inst->y, buf + PARAM_SEED_SIZE, PARAM_SYNDROME_BYTESIZE);
    return inst;
}

void serialize_instance_solution(uint8_t* buf, const solution_t* sol) {
    memcpy(buf, sol->x_A, PARAM_PLAINTEXT_BYTESIZE);
    memcpy(buf+PARAM_PLAINTEXT_BYTESIZE, sol->beta, PARAM_CODE_WEIGHT*PARAM_CODE_EXT_BYTESIZE);
}

solution_t* deserialize_instance_solution(const uint8_t* buf) {
    solution_t* sol = (solution_t*) malloc(sizeof(solution_t));
    memcpy(sol->x_A, buf, PARAM_PLAINTEXT_BYTESIZE);
    memcpy(sol->beta, buf+PARAM_PLAINTEXT_BYTESIZE, PARAM_CODE_WEIGHT*PARAM_CODE_EXT_BYTESIZE);
    return sol;
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
    ret |= memcmp(inst1->y, inst2->y, PARAM_SYNDROME_BYTESIZE);
    return ((ret) ? 0 : 1);
}

int is_correct_solution(instance_t* inst, solution_t* sol) {
    int ret = 0;
    uncompress_instance(inst);

    /// We recompute the vector x as (x_A||x_B),
    ///      where s_B := y - H' s_A

    // x = x_A || x_B
    uint8_t* x = (uint8_t*) aligned_alloc(32, PARAM_CODEWORD_BYTESIZE_CEIL32);
    if(x == NULL) {
        printf("Error: Aligned alloc returns NULL.\n");
        return 0;
    }
    memcpy(x, (uint8_t*) sol->x_A, PARAM_PLAINTEXT_BYTESIZE);
    memcpy(&x[PARAM_PLAINTEXT_BYTESIZE], (uint8_t*) inst->y, PARAM_SYNDROME_BYTESIZE);

    // x_B = y - H_A x_A
    //neg_tab_points(&s[PARAM_PLAINTEXT_LENGTH], PARAM_SYNDROME_LENGTH);
    gf2to31_matcols_muladd(&x[PARAM_PLAINTEXT_BYTESIZE], (uint8_t*) sol->x_A, *inst->mat_H, PARAM_PLAINTEXT_LENGTH, PARAM_SYNDROME_LENGTH);
    //neg_tab_points(&s[PARAM_PLAINTEXT_LENGTH], PARAM_SYNDROME_LENGTH);

    uint8_t* cur_x = x;
    for(int i=0; i<PARAM_CODE_LENGTH; i++) {
        uint32_t x_pow_q_j = *((uint32_t*) cur_x);
        cur_x += PARAM_CODE_EXT_BYTESIZE;

        // Compute x^{q^j}, for all j>0
        uint32_t res = 0;
        for(int j=0; j<PARAM_CODE_WEIGHT; j++) {
            res = gf2to31_add(res, gf2to31_mul(*((uint32_t*)sol->beta[j]), x_pow_q_j));
            x_pow_q_j = gf2to31_squ(x_pow_q_j);
        }
        res = gf2to31_add(res, x_pow_q_j);
        if(res) {
            printf("Error: Coordinate %d is not a root of the q-polynomial.\n", i);
            ret = -1;
        }
    }

    free(x);
    return (ret == 0);
}
