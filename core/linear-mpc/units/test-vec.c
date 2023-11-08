#include "witness.h"
#include "mpc-all.h"
#include "test-vec.h"

int randombytes(unsigned char* x, unsigned long long xlen);

void prepare_raw_challenge(vec_elt_t* original, vec_elt_t* copy, uint32_t global_size) {
    randombytes((uint8_t*) original, global_size*sizeof(vec_elt_t));
    if(copy != NULL)
        memcpy(copy, original,  global_size*sizeof(vec_elt_t));
}

void prepare_challenge(vec_elt_t* original, vec_elt_t* copy, uint32_t global_size, samplable_t* entropy) {
    vec_rnd(original, global_size, entropy);
    if(copy != NULL)
        memcpy(copy, original,  global_size*sizeof(vec_elt_t));
}

int check_overflow(vec_elt_t* original, vec_elt_t* modified, uint32_t global_size, uint32_t left_margin, uint32_t tested_size) {
    uint8_t* ref_chunk;
    uint8_t* tested_chunk;
    uint32_t chunk_size;

    // Left side
    ref_chunk = (uint8_t*) original;
    tested_chunk = (uint8_t*) modified;
    chunk_size = left_margin*sizeof(vec_elt_t);
    if(memcmp(ref_chunk, tested_chunk, chunk_size) != 0)
        return -1;

    // Right side
    ref_chunk = (uint8_t*) (original+left_margin+tested_size);
    tested_chunk = (uint8_t*) (modified+left_margin+tested_size);
    chunk_size = (global_size-(left_margin+tested_size))*sizeof(vec_elt_t);
    if(memcmp(ref_chunk, tested_chunk, chunk_size) != 0)
        return -1;

    return 0;
}

int test_vec_isequal(samplable_t* entropy, char* last_error) {
    vec_elt_t original[1000];
    vec_elt_t copy[1000];

    for(int size=50; size<=700; size+=10) {
        prepare_challenge(original, copy, size, entropy);
        
        // Choose a coordinate to change
        uint32_t pos;
        randombytes((uint8_t*) &pos, sizeof(pos));
        pos %= size;

        // In case we are working on a small field,
        //   we try to change the coordinate many times.
        uint8_t invalid = 1;
        for(int i=0; i<128; i++) {
            // Sample the new value of the coordinate
            vec_elt_t x;
            vec_rnd(&x, 1, entropy);
            // Change the coordinate and compare
            memcpy(&copy[pos], &x, sizeof(vec_elt_t));
            invalid &= (vec_isequal(original, copy, size) != 0);
        }
        if(invalid) {
            strcpy(last_error, "vec_isequal fails to detect a modification on one coordinate.");
            return -1;
        }
    }

    return 0;
}

int test_vec_rnd(samplable_t* entropy, char* last_error) {
    vec_elt_t original[1000];
    vec_elt_t copy[1000];

    int left = 100;

    for(int size=50; size<=700; size+=10) {
        prepare_raw_challenge(original, copy, 1000);

        vec_rnd(copy+left, size, entropy);
        if(check_overflow(original, copy, 1000, left, size)) {
            strcpy(last_error, "Buffer overflow.");
            return -1;
        }

        if(size >= 128) {
            // We avoid small sizes because, if working on small fields,
            // we can obtain the same vector with not-negliguble proba.
            if(vec_isequal(original+left, copy+left, size)) {
                strcpy(last_error, "Vector is unchanged after vec_rnd.");
                return -1;
            }
        }
    }

    return 0;
}

int test_vec_neg(samplable_t* entropy, char* last_error) {
    vec_elt_t original[1000];
    vec_elt_t copy[1000];

    int left = 100;

    // "is_identity" stored the negative function
    //     is the identity (for example, on binary fields)
    //  -1 -> unknown
    //   0 -> no
    //   1 -> yes
    int is_identity = -1;

    for(int size=50; size<=700; size+=10) {
        prepare_challenge(original, copy, 1000, entropy);

        vec_neg(copy+left, size);
        if(check_overflow(original, copy, 1000, left, size)) {
            strcpy(last_error, "Buffer overflow (1).");
            return -1;
        }
        if(vec_isequal(copy+left, original+left, size) == 0) {
            if(is_identity < 0)
                is_identity = 0;
            else if(is_identity == 1) {
                strcpy(last_error, "Modified buffer while it is the identity function.");
                return -1;
            }
        } else {
            if(is_identity < 0)
                is_identity = 1;
            else if(is_identity == 0) {
                strcpy(last_error, "Unmodified buffer while it is not the identity function.");
                return -1;
            }
        }

        vec_neg(copy+left, size);
        if(check_overflow(original, copy, 1000, left, size)) {
            strcpy(last_error, "Buffer overflow (2).");
            return -1;
        }

        // Check that neg(neg(.)) is the identity
        if(! vec_isequal(copy+left, original+left, size)) {
            strcpy(last_error, "neg(neg(.)) is not the identity.");
            return -1;
        }
    }

    if(is_identity) {
        strcpy(last_error, "The negation seems to be the identity function.");
        return 1;
    }

    return 0;
}

int test_vec_add(samplable_t* entropy, char* last_error) {
    (void) entropy;
    vec_elt_t original_a[1000];
    vec_elt_t copy_a[1000];
    vec_elt_t original_b[1000];
    vec_elt_t copy_b[1000];
    vec_elt_t original_c[1000];
    vec_elt_t copy_c[1000];
    vec_elt_t copy_d[1000];
    vec_elt_t zero[1000];
    vec_setzero(zero, 1000);

    int left = 100;

    for(int size=50; size<=700; size+=10) {
        prepare_challenge(original_a, copy_a, 1000, entropy);
        prepare_challenge(original_b, copy_b, 1000, entropy);

        vec_add(copy_a+left, copy_b+left, size);
        if(check_overflow(original_a, copy_a, 1000, left, size)) {
            strcpy(last_error, "Buffer overflow.");
            return -1;
        }
    }

    for(int size=50; size<=700; size+=10) {
        prepare_challenge(original_a, copy_a, 1000, entropy);
        prepare_challenge(original_b, copy_b, 1000, entropy);
        prepare_challenge(original_c, copy_c, 1000, entropy);
        memcpy(copy_d, copy_a, sizeof(copy_d));

        vec_add(copy_a+left, copy_b+left, size); // new_a = a + b
        vec_add(copy_a+left, copy_c+left, size); // new_a = (a + b) + c
        vec_add(copy_b+left, copy_c+left, size); // new_b = b + c
        vec_add(copy_b+left, copy_d+left, size); // new_b = a + (b + c)
        
        if(!vec_isequal(copy_a+left, copy_b+left, size)) {
            strcpy(last_error, "(a+b)+c != a+(b+c).");
            return -1;
        }

        memcpy(copy_d, copy_a, sizeof(copy_d));
        vec_neg(copy_d+left, size);
        vec_add(copy_a+left, copy_d+left, size);
        if(!vec_isequal(copy_a+left, zero+left, size)) {
            strcpy(last_error, "a + (-a) != 0.");
            return -1;
        }
    }

    return 0;
}

int test_vec_sub(samplable_t* entropy, char* last_error) {
    vec_elt_t original_a[1000];
    vec_elt_t copy_a[1000];
    vec_elt_t original_b[1000];
    vec_elt_t copy_b[1000];

    int left = 100;

    for(int size=50; size<=700; size+=10) {
        prepare_challenge(original_a, copy_a, 1000, entropy);
        prepare_challenge(original_b, copy_b, 1000, entropy);

        vec_sub(copy_a+left, copy_b+left, size);
        if(check_overflow(original_a, copy_a, 1000, left, size) != 0) {
            strcpy(last_error, "Buffer overflow.");
            return -1;
        }
    }

    for(int size=50; size<=700; size+=10) {
        prepare_challenge(original_a, copy_a, 1000, entropy);
        prepare_challenge(original_b, copy_b, 1000, entropy);

        vec_sub(copy_a+left, copy_b+left, size);
        vec_neg(original_b+left, size);
        vec_add(original_a+left, original_b+left, size);
        if(!vec_isequal(original_a+left, copy_a+left, size)) {
            strcpy(last_error, "a - b != a + (-b).");
            return -1;
        }
    }

    return 0;
}

int test_vec_mul(samplable_t* entropy, char* last_error) {
    vec_elt_t original_a[1000];
    vec_elt_t copy_a[1000];
    vec_elt_t original_b[1000];
    vec_elt_t copy_b[1000];
    vec_elt_t zero[1000];
    vec_setzero(zero, 1000);

    int left = 100;

    for(int size=50; size<=700; size+=10) {
        prepare_challenge(original_a, copy_a, 1000, entropy);
        prepare_challenge(original_b, copy_b, 1000, entropy);

        vec_mul(copy_a+left, sca_one(), copy_b+left, size);
        if(check_overflow(original_a, copy_a, 1000, left, size) != 0) {
            strcpy(last_error, "Buffer overflow.");
            return -1;
        }

        if(!vec_isequal(copy_a+left, copy_b+left, size)) {
            strcpy(last_error, "1*a != a.");
            return -1;
        }

        vec_mul(copy_a+left, sca_zero(), copy_b+left, size);
        if(!vec_isequal(copy_a+left, zero+left, size)) {
            strcpy(last_error, "0*a != 0.");
            return -1;
        }
    }

    return 0;
}

int test_vec_muladd(samplable_t* entropy, char* last_error) {
    vec_elt_t original_a[1000];
    vec_elt_t copy_a[1000];
    vec_elt_t original_b[1000];
    vec_elt_t copy_b[1000];

    int left = 100;

    for(int size=50; size<=700; size+=10) {
        prepare_challenge(original_a, copy_a, 1000, entropy);
        prepare_challenge(original_b, copy_b, 1000, entropy);

        vec_muladd(copy_a+left, sca_one(), copy_b+left, size);
        if(check_overflow(original_a, copy_a, 1000, left, size) != 0) {
            strcpy(last_error, "Buffer overflow.");
            return -1;
        }

        vec_add(original_a+left, original_b+left, size);
        if(!vec_isequal(copy_a+left, original_a+left, size)) {
            strcpy(last_error, "z + 1*x != z+x.");
            return -1;
        }

        prepare_challenge(original_a, copy_a, 1000, entropy);
        vec_muladd(copy_a+left, sca_zero(), copy_b+left, size);
        if(!vec_isequal(copy_a+left, original_a+left, size)) {
            strcpy(last_error, "z + 0*x != z.");
            return -1;
        }
    }

    return 0;
}

int test_vec_muladd2(samplable_t* entropy, char* last_error) {
    vec_elt_t original_a[1000];
    vec_elt_t copy_a[1000];
    vec_elt_t original_b[1000];
    vec_elt_t copy_b[1000];

    int left = 100;

    for(int size=50; size<=700; size+=10) {
        prepare_challenge(original_a, copy_a, 1000, entropy);
        prepare_challenge(original_b, copy_b, 1000, entropy);

        vec_muladd2(copy_a+left, sca_one(), copy_b+left, size);
        if(check_overflow(original_a, copy_a, 1000, left, size) != 0) {
            strcpy(last_error, "Buffer overflow.");
            return -1;
        }

        vec_add(original_a+left, original_b+left, size);
        if(!vec_isequal(copy_a+left, original_a+left, size)) {
            strcpy(last_error, "1*z + x != z+x.");
            return -1;
        }

        prepare_challenge(original_a, copy_a, 1000, entropy);
        vec_muladd2(copy_a+left, sca_zero(), copy_b+left, size);
        if(!vec_isequal(copy_a+left, original_b+left, size)) {
            strcpy(last_error, "0*z + x != z.");
            return -1;
        }
    }

    return 0;
}

int test_vec_bytes(samplable_t* entropy, char* last_error) {
    vec_elt_t a[1000];
    vec_elt_t b[1000];
    uint8_t buffer[sizeof(a)+10];

    for(int size=50; size<=200; size+=1) {
        prepare_challenge(a, NULL, size, entropy);
        vec_to_bytes(buffer, a, size);
        vec_from_bytes(b, buffer, size);
        if(!vec_isequal(a, b, size)) {
            strcpy(last_error, "from_bytes(to_bytes(.)) is not the identity function.");
            return -1;
        }
    }

    return 0;
}
