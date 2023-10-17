#include "witness.h"
#include "field.h"
#include <stdio.h>

void expand_extended_witness(samplable_t* entropy, point_t x[PARAM_k], point_t mat_e[PARAM_n][PARAM_m], point_t beta[PARAM_r][PARAM_m]) {
    // The free family of size r
    point_t g[PARAM_r][PARAM_m];
    point_t tmp[PARAM_m];
    
    int is_valid_witness = 0;
    while(!is_valid_witness) {
        vec_rnd((point_t*) g, PARAM_r*PARAM_m, entropy);
        is_valid_witness = 1;

        point_t gq[PARAM_r][PARAM_r+1][PARAM_m];
        for(int i=0; i<PARAM_r; i++) {
            memcpy(gq[i][0], g[i], PARAM_m*sizeof(point_t));
            for(int j=1; j<PARAM_r+1; j++) {
                // gq[i][j] = gq[i][j-1] ^ q
                ext_powq(gq[i][j], gq[i][j-1]);
            }
        }

        // Build the polynomial L_U
        const point_t zero[PARAM_m] = {0};
        for(int i=0; i<PARAM_r; i++) {
            point_t* pivot = gq[i][i];
            if(ext_eq(pivot, zero)) {
                is_valid_witness = 0;
                break;
            }
            point_t inv_pivot[PARAM_m];
            ext_inv(inv_pivot, pivot);

            // gq[i][k] <-- gq[i][k]/pivot, for all k > i
            for(int k=i+1; k<PARAM_r+1; k++)
                ext_mul(gq[i][k], inv_pivot, gq[i][k]);

            for(int j=i+1; j<PARAM_r; j++) {
                for(int k=i+1; k<PARAM_r+1; k++) {
                    // gq[j][k] <- gq[j][k] - f*gq[i][k] with f:=gq[j][i]
                    ext_mul(tmp, gq[j][i], gq[i][k]);
                    ext_sub(gq[j][k], gq[j][k], tmp);
                }
            }
        }
        if(!is_valid_witness)
            continue;
        for(int i=PARAM_r-1; i>=0; i--) {
            ext_set(beta[i], gq[i][PARAM_r]);
            ext_neg(beta[i]);
            for(int k=PARAM_r-1; k>i; k--) {
                ext_mul(tmp, gq[i][k], beta[k]);
                ext_sub(beta[i], beta[i], tmp);
            }
        }

        // Build the low-rank matrix mat_e
        for(int i=0; i<PARAM_n; i++) {
            point_t comb[PARAM_r];
            vec_rnd(comb, PARAM_r, entropy);

            ext_setzero(mat_e[i]);
            for(int j=0; j<PARAM_r; j++)
                vec_muladd(mat_e[i], comb[j], g[j], PARAM_m);
        }

        // Build the vector x
        vec_rnd(x, PARAM_k, entropy);
    }
}

void uncompress_instance(instance_t* inst) {
    if(inst->mats == NULL) {
        inst->mats = (point_t(*)[PARAM_k*PARAM_nm])malloc(PARAM_k*PARAM_nm*sizeof(point_t));
        prg_context entropy_ctx;
        samplable_t entropy = prg_to_samplable(&entropy_ctx);
        prg_init(&entropy_ctx, inst->seed_mats, NULL);
        vec_rnd(*inst->mats, PARAM_m*PARAM_n*PARAM_k, &entropy);
    }
}

void generate_instance_with_solution(instance_t** inst, solution_t** sol, samplable_t* entropy) {
    // Allocate
    *sol = (solution_t*) malloc(sizeof(solution_t));
    *inst = (instance_t*) malloc(sizeof(instance_t));
    (*inst)->mats = NULL;
    
    // Extended Witness
    point_t mat_e[PARAM_n][PARAM_m];
    expand_extended_witness(entropy, (*sol)->x, mat_e, (*sol)->beta);

    // Sample a seed for random matrices
    byte_sample(entropy, (*inst)->seed_mats, PARAM_SEED_SIZE);

    // Build random matrices
    uncompress_instance(*inst);

    // Build m0 := mat_e - sum_i x_i mats_i
    memcpy((*inst)->m0, mat_e, PARAM_nm*sizeof(point_t));
    vec_neg((point_t*) (*inst)->m0, PARAM_nm);
    matcols_muladd((point_t*) (*inst)->m0, (*sol)->x, *(*inst)->mats, PARAM_k, PARAM_nm);
    vec_neg((point_t*) (*inst)->m0, PARAM_nm);
}

void hash_update_instance(hash_context* ctx, const instance_t* inst) {
    hash_update(ctx, inst->seed_mats, PARAM_SEED_SIZE);
    hash_update(ctx, (uint8_t*) inst->m0, PARAM_m*PARAM_n);
}

void serialize_instance(uint8_t* buf, const instance_t* inst) {
    memcpy(buf, inst->seed_mats, PARAM_SEED_SIZE);
    memcpy(buf + PARAM_SEED_SIZE, inst->m0, PARAM_nm*sizeof(point_t));
}

instance_t* deserialize_instance(const uint8_t* buf) {
    instance_t* inst = (instance_t*) malloc(sizeof(instance_t));
    inst->mats = NULL;
    memcpy(inst->seed_mats, buf, PARAM_SEED_SIZE);
    memcpy(inst->m0, buf + PARAM_SEED_SIZE, PARAM_nm*sizeof(point_t));
    return inst;
}

void serialize_instance_solution(uint8_t* buf, const solution_t* sol) {
    memcpy(buf, sol->x, PARAM_k*sizeof(point_t));
    memcpy(buf+PARAM_k*sizeof(point_t), sol->beta, PARAM_r*PARAM_m*sizeof(point_t));
}

solution_t* deserialize_instance_solution(const uint8_t* buf) {
    solution_t* sol = (solution_t*) malloc(sizeof(solution_t));
    memcpy(sol->x, buf, PARAM_k*sizeof(point_t));
    memcpy(sol->beta, buf+PARAM_k*sizeof(point_t), PARAM_r*PARAM_m*sizeof(point_t));
    return sol;
}

void free_instance_solution(solution_t* sol) {
    free(sol);
}

void free_instance(instance_t* inst) {
    if(inst->mats)
        free(inst->mats);
    free(inst);
}

int are_same_instances(instance_t* inst1, instance_t* inst2) {
    int ret = 0;
    ret |= memcmp(inst1->seed_mats, inst2->seed_mats, PARAM_SEED_SIZE);
    ret |= memcmp(inst1->m0, inst2->m0, PARAM_nm);
    return ((ret) ? 0 : 1);
}

int is_correct_solution(instance_t* inst, solution_t* sol) {
    int ret = 0;
    uncompress_instance(inst);

    // Recompute the low-rank matrix mat_e
    point_t mat_e[PARAM_n][PARAM_m];
    memcpy(mat_e, inst->m0, PARAM_nm*sizeof(point_t));
    matcols_muladd((point_t*) mat_e, sol->x, *inst->mats, PARAM_k, PARAM_nm);

    point_t res[PARAM_m];
    point_t tmp[PARAM_m];
    for(int i=0; i<PARAM_n; i++) {
        // Compute x^{q^j}, for all j>0
        memset(res, 0, PARAM_m*sizeof(point_t));
        for(int j=0; j<PARAM_r; j++) {
            // res += beta_j * mat_e[i]^{q^j}
            ext_mul(tmp, sol->beta[j], mat_e[i]);
            ext_add(res, res, tmp);
            ext_powq(mat_e[i], mat_e[i]);
        }
        // res += mat_e[i]^{q^r}
        ext_add(res, res, mat_e[i]);
        // Check if res is zero
        int zero = 0;
        for(int j=0; j<PARAM_m; j++)
            zero |= res[j];
        if(zero) {
            printf("Error: Coordinate %d is not a root of the q-polynomial.\n", i);
            ret = -1;
        }
    }

    return (ret == 0);
}
