#include "sign-mpcith.h"
#include "rnd.h"
#include "merkle-tree.h"
#include "commit.h"
#include "hash.h"
#include "keygen.h"
#include "mpc.h"
#include "views.h"
#include "benchmark.h"
#include "parameters-all.h"
#include "share.h"
#include "serialization.h"
#include "mpc-protocol.h"
#include <string.h>
#include <stdio.h>
#include "../units/debug.h"

/***********************************************
 *            Signature Structure              *
 ***********************************************/

typedef struct proof_tcith_mt_t {
    uint8_t* cv_info; // Information required to authenticate the opened commitment in the Merkle tree
    uint32_t len_cv_info; // Length of cv_info buffer

    // All the broadcast messages
    vec_broadcast_t* broadcast[PARAM_CIRCUIT_DEPTH-1];

    // Revealed party's shares
    vec_wit_t* wit[PARAM_NB_REVEALED];
    vec_unif_t* unif[PARAM_NB_REVEALED];
} proof_tcith_mt_t;

typedef struct signature_tcith_mt_t {
    uint8_t* salt;
    uint8_t* dc_challenge_hash;
    uint8_t* mpc_challenge_hash;
    uint8_t* view_challenge_hash;
    vec_dc_broadcast_t* plain_dc_broadcast;
    proof_tcith_mt_t proofs[PARAM_NB_EXECUTIONS];
    uint8_t* allocated_memory; // Just to manage the memory
} signature_tcith_mt_t;

// For parsing
typedef struct const_proof_tcith_mt_t {
    const uint8_t* cv_info; // Information required to authenticate the opened commitment in the Merkle tree
    uint32_t len_cv_info; // Length of cv_info buffer

    // All the broadcast messages
    const vec_broadcast_t* broadcast[PARAM_CIRCUIT_DEPTH-1];

    // Revealed party's shares
    const vec_wit_t* wit[PARAM_NB_REVEALED];
    const vec_unif_t* unif[PARAM_NB_REVEALED];
} const_proof_tcith_mt_t;

typedef struct const_signature_tcith_mt_t {
    const uint8_t* salt;
    const uint8_t* dc_challenge_hash;
    const uint8_t* mpc_challenge_hash;
    const uint8_t* view_challenge_hash;
    const vec_dc_broadcast_t* plain_dc_broadcast;
    const_proof_tcith_mt_t proofs[PARAM_NB_EXECUTIONS];
    uint8_t* allocated_memory; // Just to manage the memory
} const_signature_tcith_mt_t;

// Free signature structure
void free_signature(signature_tcith_mt_t* sig);
void free_const_signature(const_signature_tcith_mt_t* sig);

// For signing
signature_tcith_mt_t* init_signature_structure(const uint8_t* salt, uint8_t* buf, size_t buflen);
int build_signature(const signature_tcith_mt_t* sig, uint8_t* buf, size_t buflen);

// For verification
const_signature_tcith_mt_t* parse_signature(const uint8_t* buf, size_t buflen, uint16_t open_views[PARAM_NB_EXECUTIONS][PARAM_NB_REVEALED]);

/***********************************************
 *             Hash for challenge              *
 ***********************************************/

static void hash_for_dc_challenge(uint8_t challenge_hash[PARAM_DIGEST_SIZE], uint8_t merkle_roots[PARAM_NB_EXECUTIONS][PARAM_DIGEST_SIZE],
    const instance_t* inst, const uint8_t* salt, const uint8_t* message, size_t mlen)
{
    hash_context ctx;
    hash_init_prefix(&ctx, HASH_PREFIX_FIRST_CHALLENGE);
    if(inst != NULL)
        hash_update_instance(&ctx, inst);
    if(mlen > 0)
        hash_update(&ctx, message, mlen);
    hash_update(&ctx, salt, PARAM_SALT_SIZE);

    for (size_t e=0; e<PARAM_NB_EXECUTIONS; e++)
        hash_update(&ctx, merkle_roots[e], PARAM_DIGEST_SIZE);

    hash_final(&ctx, challenge_hash);
}

static void hash_for_mpc_challenge(uint8_t challenge_hash[PARAM_DIGEST_SIZE], const uint8_t dc_challenge_hash[PARAM_DIGEST_SIZE], const vec_dc_broadcast_t* dc_broadcast[PARAM_NB_EXECUTIONS][PARAM_NB_REVEALED], const vec_dc_broadcast_t* plain_dc_broadcast,
    const uint8_t* salt, const uint8_t* message, size_t mlen)
{
    hash_context ctx;
    hash_init_prefix(&ctx, HASH_PREFIX_SECOND_CHALLENGE);
    if(mlen > 0)
        hash_update(&ctx, message, mlen);
    hash_update(&ctx, salt, PARAM_SALT_SIZE);
    hash_update(&ctx, dc_challenge_hash, PARAM_DIGEST_SIZE);

    hash_update(&ctx, (uint8_t*) plain_dc_broadcast, PARAM_DC_BR_SIZE);
    for (size_t e=0; e<PARAM_NB_EXECUTIONS; e++) {
        for(uint32_t j=0; j<PARAM_NB_REVEALED; j++) {
            hash_update(&ctx, (uint8_t*) dc_broadcast[e][j], PARAM_DC_BR_SIZE);
        }
    }

    hash_final(&ctx, challenge_hash);
}

static void hash_for_view_challenge(uint8_t challenge_hash[PARAM_DIGEST_SIZE], const uint8_t dc_challenge_hash[PARAM_DIGEST_SIZE], const uint8_t mpc_challenge_hash[PARAM_DIGEST_SIZE],
    const vec_broadcast_t* broadcast[PARAM_NB_EXECUTIONS][PARAM_CIRCUIT_DEPTH], 
    const uint8_t* salt, const uint8_t* message, size_t mlen)
{
    hash_context ctx;
    hash_init_prefix(&ctx, HASH_PREFIX_THIRD_CHALLENGE);
    if(mlen > 0)
        hash_update(&ctx, message, mlen);
    hash_update(&ctx, salt, PARAM_SALT_SIZE);
    hash_update(&ctx, dc_challenge_hash, PARAM_DIGEST_SIZE);
    hash_update(&ctx, mpc_challenge_hash, PARAM_DIGEST_SIZE);

    for (size_t e=0; e<PARAM_NB_EXECUTIONS; e++)
        for(uint32_t j=0; j<PARAM_CIRCUIT_DEPTH; j++)
            hash_update(&ctx, (uint8_t*) broadcast[e][j], PARAM_BR_SIZE);

    hash_final(&ctx, challenge_hash);
}

/***********************************************
 *             Signing Algorithm               *
 ***********************************************/

int tcith_mt_sign(uint8_t* sig, size_t* siglen,
                const uint8_t* m, size_t mlen,
                const uint8_t* sk,
                const uint8_t* salt, const uint8_t* seed
                ) {
    uint32_t i, j, e, p;
    int ret;

#ifndef NDEBUG
    // In debug mode, let us check if the key generation
    //    produces valid key pair. 
    ret = mpcith_validate_keys(NULL, sk);
    if(ret)
        printf("Error: SK invalid for signing.\n");
#endif

    // Deserialize the private key
    mpcith_secret_key_t ssk;
    ret = deserialize_secret_key(&ssk, sk, PARAM_SECRETKEYBYTES);
    if(ret < 0)
        return ret;
    uncompress_instance(ssk.inst);
    vec_wit_t* plain_wit = ssk.wit;

    // Signature Structure
    signature_tcith_mt_t* ssig = init_signature_structure(salt, sig, PARAM_SIGNATURE_SIZEBYTES);

    /********************************************/
    /**********     INITIALIZATION     **********/
    /********************************************/

    prg_context entropy_ctx;
    prg_init(&entropy_ctx, seed, ssig->salt);
    samplable_t entropy = prg_to_samplable(&entropy_ctx);

    // Allocate memory
    uint8_t merkle_roots[PARAM_NB_EXECUTIONS][PARAM_DIGEST_SIZE];
    merkle_tree_t* merkle_trees[PARAM_NB_EXECUTIONS];
    for(e=0; e<PARAM_NB_EXECUTIONS; e++)
        merkle_trees[e] = malloc_merkle_tree(PARAM_NB_PARTIES);
    uint8_t** share_commitments = (uint8_t**) calloc(PARAM_NB_PARTIES, sizeof(uint8_t*));
    for(i=0; i<PARAM_NB_PARTIES; i++)
        share_commitments[i] = malloc(PARAM_DIGEST_SIZE);

    __BENCHMARK_START__(BS_RND_EXPANSION);
    // Prepare the plain MPC input
    vec_share_t* plain = new_share();
    vec_wit_set(get_wit(plain), plain_wit);
    vec_unif_rnd(get_unif(plain), &entropy);

    /********************************************/
    /********   COMMIT PARTIES' INPUTS   ********/
    /********************************************/

    // Sample randomness for sharing
    vec_share_t* rnd[PARAM_NB_EXECUTIONS][PARAM_NB_REVEALED];
    for(e=0; e<PARAM_NB_EXECUTIONS; e++)
        for(p=0; p<PARAM_NB_REVEALED; p++) {
            rnd[e][p] = new_share();
            vec_share_rnd(rnd[e][p], &entropy);
        }
    __BENCHMARK_STOP__(BS_RND_EXPANSION);

    // Compute and commit the input shares
    __BENCHMARK_START__(BS_SHARE_PREPARATION);
    vec_share_t* shares[PARAM_NB_PARTIES];
    for(i=0; i<PARAM_NB_PARTIES; i++)
        shares[i] = (i<4) ? new_share() : shares[i % 4];
    for(e=0; e<PARAM_NB_EXECUTIONS; e++) {

        // Compute shares and commit them
        // Note: in case the platform has a implementation
        //    to hash four times in parallel, we pack values
        //    to commit in groups of four.
        for(i=0; i+3<PARAM_NB_PARTIES; i+=4) {
            __BENCHMARK_START__(BS_SHARE_PACKING);
            for(j=0; j<4; j++)
                compute_complete_share(shares[j], plain, (vec_share_t const**) rnd[e], (uint8_t) (i+j));
            __BENCHMARK_STOP__(BS_SHARE_PACKING);

            __BENCHMARK_START__(BS_COMMITMENT);
            uint8_t* digests[4] = {
                share_commitments[i],   share_commitments[i+1],
                share_commitments[i+2], share_commitments[i+3]
            };
            const uint16_t is[4] = {(uint16_t) i, (uint16_t) i+1, (uint16_t) i+2, (uint16_t) i+3}; 
            commit_share_x4(digests, (vec_share_t const*const*) shares, ssig->salt, (uint16_t) e, is);
            __BENCHMARK_STOP__(BS_COMMITMENT);
        }
        for(; i<PARAM_NB_PARTIES; i++) {
            __BENCHMARK_START__(BS_SHARE_PACKING);
            compute_complete_share(shares[0], plain, (vec_share_t const**) rnd[e], (uint8_t) i);
            __BENCHMARK_STOP__(BS_SHARE_PACKING);
            __BENCHMARK_START__(BS_COMMITMENT);
            commit_share(share_commitments[i], shares[0], ssig->salt, (uint16_t) e, (uint16_t) i);
            __BENCHMARK_STOP__(BS_COMMITMENT);
        }

        __BENCHMARK_START__(BS_TREE_EXPANSION);
        // Build the Merkle tree
        build_merkle_tree(merkle_trees[e], share_commitments, NULL);
        memcpy(merkle_roots[e], get_root(merkle_trees[e]), PARAM_DIGEST_SIZE);
        __BENCHMARK_STOP__(BS_TREE_EXPANSION);
    }
    for(j=0; j<4; j++)
        free(shares[j]);
    __BENCHMARK_STOP__(BS_SHARE_PREPARATION);

    // Expand the DC challenge
    __BENCHMARK_START__(BS_CHALLENGE);
    vec_dc_challenge_t* dc_challenge = new_dc_challenge();
    hash_for_dc_challenge(ssig->dc_challenge_hash, merkle_roots, ssk.inst, ssig->salt, NULL, 0);
    expand_dc_challenge_hash(&dc_challenge, ssig->dc_challenge_hash, 1);
    __BENCHMARK_STOP__(BS_CHALLENGE);

    __BENCHMARK_START__(BS_MPC_EMULATION);
    mpc_run_degree_checking(ssig->plain_dc_broadcast, dc_challenge, plain, 1);
    // Compute the randomness involved in the broadcast sharings 
    vec_dc_broadcast_t* dc_broadcast[PARAM_NB_EXECUTIONS][PARAM_NB_REVEALED];
    for(e=0; e<PARAM_NB_EXECUTIONS; e++) {
        for(p=0; p<PARAM_NB_REVEALED; p++) {
            dc_broadcast[e][p] = new_dc_br();
            mpc_run_degree_checking(dc_broadcast[e][p], dc_challenge, rnd[e][p], 0);
        }
    }
    __BENCHMARK_STOP__(BS_MPC_EMULATION);

    // Expand the MPC challenge
    __BENCHMARK_START__(BS_CHALLENGE);
    vec_challenge_t* mpc_challenge = new_challenge();
    hash_for_mpc_challenge(ssig->mpc_challenge_hash, ssig->dc_challenge_hash, (const vec_dc_broadcast_t*(*)[PARAM_NB_REVEALED]) dc_broadcast, ssig->plain_dc_broadcast, ssig->salt, NULL, 0);
    expand_mpc_challenge_hash(&mpc_challenge, ssig->mpc_challenge_hash, 1, ssk.inst);
    __BENCHMARK_STOP__(BS_CHALLENGE);

    /********************************************/
    /*********  SIMULATE MPC PROTOCOL  **********/
    /********************************************/

    __BENCHMARK_START__(BS_MPC_EMULATION);
    vec_broadcast_t* broadcast[PARAM_NB_EXECUTIONS][PARAM_CIRCUIT_DEPTH];
    for(e=0; e<PARAM_NB_EXECUTIONS; e++) {
        for(j=0; j<PARAM_CIRCUIT_DEPTH; j++)
            broadcast[e][j] = new_br();
        mpc_compute_communications(broadcast[e], mpc_challenge, plain, rnd[e][0], ssk.inst);
        for(j=0; j<PARAM_CIRCUIT_DEPTH-1; j++)
            vec_br_set(ssig->proofs[e].broadcast[j], broadcast[e][j]);
    }
    __BENCHMARK_STOP__(BS_MPC_EMULATION);

    // Expand the view challenge
    __BENCHMARK_START__(BS_CHALLENGE);
    uint16_t open_views[PARAM_NB_EXECUTIONS][PARAM_NB_REVEALED];
    hash_for_view_challenge(ssig->view_challenge_hash, ssig->dc_challenge_hash, ssig->mpc_challenge_hash, (const vec_broadcast_t*(*)[PARAM_CIRCUIT_DEPTH]) broadcast, ssig->salt, m, mlen);
    expand_view_challenge_hash((uint16_t*) open_views, ssig->view_challenge_hash, PARAM_NB_EXECUTIONS, PARAM_NB_REVEALED);
    __BENCHMARK_STOP__(BS_CHALLENGE);

    /********************************************/
    /**********   FINALIZE SIGNATURE   **********/
    /********************************************/

    __BENCHMARK_START__(BS_BUILD_SIG);
    for(e=0; e<PARAM_NB_EXECUTIONS; e++) {
        // Build the authentication path in the Merkle tree
        uint8_t* ptr = open_merkle_tree(merkle_trees[e], open_views[e], PARAM_NB_REVEALED, &ssig->proofs[e].len_cv_info);
        memcpy(ssig->proofs[e].cv_info, ptr, ssig->proofs[e].len_cv_info);
        free(ptr);

        // Get the share of the witness for the open parties
        vec_share_t* share = new_share();
        for(p=0; p<PARAM_NB_REVEALED; p++) {
            compute_complete_share(share, plain, (vec_share_t const**) rnd[e], (uint8_t) open_views[e][p]);
            vec_wit_set(ssig->proofs[e].wit[p], get_wit(share));
            vec_unif_set(ssig->proofs[e].unif[p], get_unif(share));
        }
        free(share);
    }

    // Serialize the signature
    ret = build_signature(ssig, sig, PARAM_SIGNATURE_SIZEBYTES);
    free_signature(ssig);
    __BENCHMARK_STOP__(BS_BUILD_SIG);

    for(e=0; e<PARAM_NB_EXECUTIONS; e++)
        free_merkle_tree(merkle_trees[e]);
    for(i=0; i<PARAM_NB_PARTIES; i++)
        free(share_commitments[i]);
    free(share_commitments);
    mpcith_free_keys_internal(NULL, &ssk);
    free(dc_challenge);
    free(mpc_challenge);
    free(plain);
    for(e=0; e<PARAM_NB_EXECUTIONS; e++) {
        for(p=0; p<PARAM_NB_REVEALED; p++)
            free(dc_broadcast[e][p]);
        for(j=0; j<PARAM_CIRCUIT_DEPTH; j++)
            free(broadcast[e][j]);
    }

    if(ret < 0)
        return ret;
    *siglen = (size_t) ret;
    return 0;
}

/***********************************************
 *           Verification Algorithm            *
 ***********************************************/

int tcith_mt_sign_verify(const uint8_t* sig, size_t siglen,
                const uint8_t* m, size_t mlen,
                const uint8_t* pk
                ) {
    uint32_t i, j, e, p;
    int ret;

    // Deserialize the public key
    mpcith_public_key_t ppk;
    ret = deserialize_public_key(&ppk, pk, PARAM_PUBLICKEYBYTES);
    if (ret < 0)
        return ret;
    __BENCHMARK_START__(BV_RND_EXPANSION);
    uncompress_instance(ppk.inst);
    __BENCHMARK_STOP__(BV_RND_EXPANSION);

    // Parse the signature
    // Note: while parsing, it expands the view challenge.
    uint16_t open_views[PARAM_NB_EXECUTIONS][PARAM_NB_REVEALED];
    const_signature_tcith_mt_t* ssig = parse_signature(sig, siglen, open_views);
    if(ssig == NULL) {
        ret = -1;
        mpcith_free_keys_internal(&ppk, NULL);
        return ret;
    }

    // Initialization
    uint8_t merkle_roots[PARAM_NB_EXECUTIONS][PARAM_DIGEST_SIZE];
    uint8_t share_commitments[PARAM_NB_REVEALED][PARAM_DIGEST_SIZE];
    vec_dc_broadcast_t* dc_broadcast[PARAM_NB_EXECUTIONS][PARAM_NB_REVEALED];
    vec_broadcast_t* broadcast[PARAM_NB_EXECUTIONS][PARAM_CIRCUIT_DEPTH];
    for(e=0; e<PARAM_NB_EXECUTIONS; e++) {
        for(p=0; p<PARAM_NB_REVEALED; p++)
            dc_broadcast[e][p] = new_dc_br();
        for(j=0; j<PARAM_CIRCUIT_DEPTH; j++)
            broadcast[e][j] = new_br();
    }

    // Expand MPC Challenge
    __BENCHMARK_START__(BV_CHALLENGE);
    vec_dc_challenge_t* dc_challenge = new_dc_challenge();
    expand_dc_challenge_hash(&dc_challenge, ssig->dc_challenge_hash, 1);
    vec_challenge_t* mpc_challenge = new_challenge();
    expand_mpc_challenge_hash(&mpc_challenge, ssig->mpc_challenge_hash, 1, ppk.inst);
    __BENCHMARK_STOP__(BV_CHALLENGE);

    vec_dc_broadcast_t* open_dc_broadcast[PARAM_NB_REVEALED];
    vec_broadcast_t* open_broadcast[PARAM_NB_REVEALED];
    for(p=0; p<PARAM_NB_REVEALED; p++) {
        open_dc_broadcast[p] = new_dc_br();
        open_broadcast[p] = new_br();
    }
    vec_share_t* share = new_share();
    // Loop over all the open parties
    for(e=0; e<PARAM_NB_EXECUTIONS; e++) {
        for(p=0; p<PARAM_NB_REVEALED; p++) {
            i = open_views[e][p];

            vec_wit_set(get_wit(share), ssig->proofs[e].wit[p]);
            vec_unif_set(get_unif(share), ssig->proofs[e].unif[p]);

            __BENCHMARK_START__(BV_MPC_EMULATION);
            mpc_run_degree_checking(open_dc_broadcast[p], dc_challenge, share, i);
            mpc_emulate_party(open_broadcast[p], mpc_challenge, share, i, ppk.inst);
            __BENCHMARK_STOP__(BV_MPC_EMULATION);

            // Hash the input share of the current party
            __BENCHMARK_START__(BV_COMMITMENT);
            commit_share(share_commitments[p], share, ssig->salt, (uint16_t) e, (uint16_t) i);
            __BENCHMARK_STOP__(BV_COMMITMENT);
        }

        for(j=0; j<PARAM_CIRCUIT_DEPTH-1; j++)
            vec_br_set(broadcast[e][j], ssig->proofs[e].broadcast[j]);

        __BENCHMARK_START__(BV_MPC_EMULATION);
        #if PARAM_NB_REVEALED != 1
        #error "PARAM_NB_REVEALED != 1 not implemented"
        #endif
        if(open_views[e][0] != 0) {
            uint32_t scalar = open_views[e][0];
            // DC Broadcast
            vec_dc_br_set(dc_broadcast[e][0], open_dc_broadcast[0]);
            vec_dc_br_sub(dc_broadcast[e][0], ssig->plain_dc_broadcast);
            vec_dc_br_mul(dc_broadcast[e][0], sca_inverse_tab[scalar], dc_broadcast[e][0]);
            // MPC Broadcast
            vec_br_set(broadcast[e][PARAM_CIRCUIT_DEPTH-1], open_broadcast[0]);
            vec_br_neg(broadcast[e][PARAM_CIRCUIT_DEPTH-1]);
            for(j=0; j<PARAM_CIRCUIT_DEPTH-1; j++) { 
                vec_br_muladd(broadcast[e][PARAM_CIRCUIT_DEPTH-1], scalar, broadcast[e][j]);
                scalar = sca_mul(scalar, open_views[e][0]);
            }
            vec_br_neg(broadcast[e][PARAM_CIRCUIT_DEPTH-1]);
            // broad_coef <- (1/i^*)**d * broad_coef
            vec_br_mul(broadcast[e][PARAM_CIRCUIT_DEPTH-1], sca_inverse_tab[scalar], broadcast[e][PARAM_CIRCUIT_DEPTH-1]);
        } else {
            // DC Broadcast
            vec_dc_br_set(dc_broadcast[e][0], open_dc_broadcast[0]);
            // MPC Broadcast
            vec_br_set(broadcast[e][PARAM_CIRCUIT_DEPTH-1], open_broadcast[0]);
        }
        __BENCHMARK_STOP__(BV_MPC_EMULATION);

        __BENCHMARK_START__(BV_TREE_EXPANSION);
        // Compute the root of the Merkle tree for the current execution
        ret = get_merkle_root_from_auth(merkle_roots[e],
            PARAM_LOG_NB_PARTIES, PARAM_NB_PARTIES, PARAM_NB_REVEALED, // Tree Parameters
            open_views[e], (uint8_t*) share_commitments, // Open Leaves
            ssig->proofs[e].cv_info, ssig->proofs[e].len_cv_info, // Authentication Paths
            NULL // salt (if provided)
        );
        if (ret != 0) {
            ret = -1;
            goto Exit;
        }
        __BENCHMARK_STOP__(BV_TREE_EXPANSION);
    }
    free(share);
    for(p=0; p<PARAM_NB_REVEALED; p++) {
        free(open_dc_broadcast[p]);
        free(open_broadcast[p]);
    }

    // Recompute the hash digest of the MPC challenge
    //    and check it is consistent with the one in the signature
    __BENCHMARK_START__(BV_CHALLENGE);
    uint8_t dc_challenge_hash[PARAM_DIGEST_SIZE];
    uint8_t mpc_challenge_hash[PARAM_DIGEST_SIZE];
    uint8_t view_challenge_hash[PARAM_DIGEST_SIZE];
    hash_for_dc_challenge(dc_challenge_hash, merkle_roots, ppk.inst, ssig->salt, NULL, 0);
    hash_for_mpc_challenge(mpc_challenge_hash, ssig->dc_challenge_hash, (const vec_dc_broadcast_t*(*)[PARAM_NB_REVEALED]) dc_broadcast, ssig->plain_dc_broadcast, ssig->salt, NULL, 0);
    hash_for_view_challenge(view_challenge_hash, ssig->dc_challenge_hash, ssig->mpc_challenge_hash, (const vec_broadcast_t*(*)[PARAM_CIRCUIT_DEPTH]) broadcast, ssig->salt, m, mlen);

    ret = (memcmp(dc_challenge_hash, ssig->dc_challenge_hash, PARAM_DIGEST_SIZE) != 0);
    ret |= (memcmp(mpc_challenge_hash, ssig->mpc_challenge_hash, PARAM_DIGEST_SIZE) != 0);
    ret |= (memcmp(view_challenge_hash, ssig->view_challenge_hash, PARAM_DIGEST_SIZE) != 0);
    __BENCHMARK_STOP__(BV_CHALLENGE);

Exit:
    free(dc_challenge);
    free(mpc_challenge);
    for(e=0; e<PARAM_NB_EXECUTIONS; e++) {
        for(p=0; p<PARAM_NB_REVEALED; p++)
            free(dc_broadcast[e][p]);
        for(p=0; p<PARAM_CIRCUIT_DEPTH; p++)
            free(broadcast[e][p]);
    }
    mpcith_free_keys_internal(&ppk, NULL);
    free_const_signature(ssig);
    return ret;
}

/***********************************************
 *             About Serialization             *
 ***********************************************/

void free_signature(signature_tcith_mt_t* sig) {
    if(sig->allocated_memory != NULL)
        free(sig->allocated_memory);
    free(sig);
}

void free_const_signature(const_signature_tcith_mt_t* sig) {
    if(sig->allocated_memory != NULL)
        free(sig->allocated_memory);
    free(sig);
}

const_signature_tcith_mt_t* parse_signature(const uint8_t* buf, size_t buflen, uint16_t open_views[PARAM_NB_EXECUTIONS][PARAM_NB_REVEALED]) {
    size_t bytes_required = 3*PARAM_DIGEST_SIZE + PARAM_SALT_SIZE;
    bytes_required += PARAM_DC_BR_SHORT_SIZE;
    bytes_required += PARAM_NB_EXECUTIONS*PARAM_NB_REVEALED*PARAM_BR_SHORT_SIZE;
    bytes_required += PARAM_NB_EXECUTIONS*PARAM_NB_REVEALED*(PARAM_WIT_SHORT_SIZE+PARAM_UNIF_SHORT_SIZE);
    if(buflen < bytes_required) {
        // Buffer too short
        return NULL;
    }

    const_signature_tcith_mt_t* sig = malloc(sizeof(const_signature_tcith_mt_t));
    sig->salt = buf;                buf += PARAM_SALT_SIZE;
    sig->dc_challenge_hash = buf;   buf += PARAM_DIGEST_SIZE;
    sig->mpc_challenge_hash = buf;  buf += PARAM_DIGEST_SIZE;
    sig->view_challenge_hash = buf; buf += PARAM_DIGEST_SIZE;

    sig->allocated_memory = malloc(
        PARAM_DC_BR_PARSED_SIZE + PARAM_NB_EXECUTIONS*(
            PARAM_BR_SIZE*(PARAM_CIRCUIT_DEPTH-1)+PARAM_WIT_PARSED_SIZE+PARAM_UNIF_PARSED_SIZE
        )
    );
    uint8_t* buf_ = sig->allocated_memory;
    // Plain DC Broadcast
    dc_br_deserialize(&sig->plain_dc_broadcast, buf, buf_);
    buf_ += PARAM_DC_BR_PARSED_SIZE;
    buf += PARAM_DC_BR_SHORT_SIZE;
    for(size_t e=0; e<PARAM_NB_EXECUTIONS; e++) {
        for(uint32_t j=0; j<PARAM_CIRCUIT_DEPTH-1; j++) {
            br_deserialize(&sig->proofs[e].broadcast[j], buf, buf_);
            buf += PARAM_BR_SHORT_SIZE;
            buf_ += PARAM_BR_SIZE;
        }
        for(uint32_t p=0; p<PARAM_NB_REVEALED; p++) {
            wit_deserialize(&sig->proofs[e].wit[p], buf, buf_);
            buf += PARAM_WIT_SHORT_SIZE;
            buf_ += PARAM_WIT_PARSED_SIZE;
            unif_deserialize(&sig->proofs[e].unif[p], buf, buf_);
            buf += PARAM_UNIF_SHORT_SIZE;
            buf_ += PARAM_UNIF_PARSED_SIZE;
        }
    }

    expand_view_challenge_hash((uint16_t*) open_views, sig->view_challenge_hash, PARAM_NB_EXECUTIONS, PARAM_NB_REVEALED);

    for(size_t e=0; e < PARAM_NB_EXECUTIONS; e++) {
        sig->proofs[e].len_cv_info = get_auth_size(PARAM_LOG_NB_PARTIES, PARAM_NB_PARTIES, PARAM_NB_REVEALED, open_views[e]);
        bytes_required += sig->proofs[e].len_cv_info;
    }

    /* Fail if the signature does not have the exact number of bytes we expect */
    if(buflen != bytes_required) {
        free(sig->allocated_memory);
        free(sig);
        return NULL;
    }

    for (size_t e=0; e<PARAM_NB_EXECUTIONS; e++) {
        sig->proofs[e].cv_info = buf;
        buf += sig->proofs[e].len_cv_info;
    }

    return sig;
}

signature_tcith_mt_t* init_signature_structure(const uint8_t* salt, uint8_t* buf, size_t buflen) {
    size_t bytes_required = 3*PARAM_DIGEST_SIZE + PARAM_SALT_SIZE;
    if(buflen < bytes_required) {
        return NULL;
    }

    signature_tcith_mt_t* sig = malloc(sizeof(signature_tcith_mt_t));
    sig->salt = buf;                buf += PARAM_SALT_SIZE;
    sig->dc_challenge_hash = buf;   buf += PARAM_DIGEST_SIZE;
    sig->mpc_challenge_hash = buf;  buf += PARAM_DIGEST_SIZE;
    sig->view_challenge_hash = buf; buf += PARAM_DIGEST_SIZE;
    memcpy(sig->salt, salt, PARAM_SALT_SIZE);

    sig->allocated_memory = malloc(
         PARAM_DC_BR_SIZE // Plain DC Broadcast
         + PARAM_NB_EXECUTIONS*(
            PARAM_TREE_NB_MAX_OPEN_LEAVES*PARAM_DIGEST_SIZE // merkle tree
             + PARAM_WIT_SIZE + PARAM_UNIF_SIZE // input share
             + (PARAM_CIRCUIT_DEPTH-1)*PARAM_BR_SIZE // broadcast
        )
    );
    uint8_t* buf_ = sig->allocated_memory;
    sig->plain_dc_broadcast = (void*) buf_;
    buf_ += PARAM_DC_BR_SIZE;
    for(size_t e=0; e<PARAM_NB_EXECUTIONS; e++) {
        sig->proofs[e].cv_info = buf_;
        buf_ += PARAM_TREE_NB_MAX_OPEN_LEAVES*PARAM_DIGEST_SIZE;
        for(uint32_t p=0; p<PARAM_NB_REVEALED; p++) {
            sig->proofs[e].wit[p] = (void*) buf_;
            buf_ += PARAM_WIT_SIZE;
            sig->proofs[e].unif[p] = (void*) buf_;
            buf_ += PARAM_UNIF_SIZE;
        }
        for(size_t j=0; j<PARAM_CIRCUIT_DEPTH-1; j++) {
            sig->proofs[e].broadcast[j] = (vec_broadcast_t*) buf_;
            buf_ += PARAM_BR_SIZE;
        }
    }
    return sig;
}

int build_signature(const signature_tcith_mt_t* sig, uint8_t* buf, size_t buflen) {
    size_t bytes_required = 3*PARAM_DIGEST_SIZE + PARAM_SALT_SIZE;
    bytes_required += PARAM_DC_BR_SHORT_SIZE;
    bytes_required += PARAM_NB_EXECUTIONS*(PARAM_CIRCUIT_DEPTH-1)*PARAM_BR_SHORT_SIZE;
    bytes_required += PARAM_NB_EXECUTIONS*(PARAM_WIT_SHORT_SIZE+PARAM_UNIF_SHORT_SIZE);
    for(size_t e=0; e<PARAM_NB_EXECUTIONS; e++) {
        bytes_required += sig->proofs[e].len_cv_info;
    }
    if(buflen < bytes_required)
        return -1;

    uint8_t* bufBase = buf;
    buf += (3*PARAM_DIGEST_SIZE + PARAM_SALT_SIZE); // salt and mpc challenge hash are already set (via init)

    // warning: must be in the same order than in parsing
    dc_br_serialize(buf, sig->plain_dc_broadcast);
    buf += PARAM_DC_BR_SHORT_SIZE;
    for(size_t e=0; e<PARAM_NB_EXECUTIONS; e++) {
        for(uint32_t j=0; j<PARAM_CIRCUIT_DEPTH-1; j++) {
            br_serialize(buf, sig->proofs[e].broadcast[j]);
            buf += PARAM_BR_SHORT_SIZE;
        }
        for(uint32_t p=0; p<PARAM_NB_REVEALED; p++) {
            wit_serialize(buf, sig->proofs[e].wit[p]);
            buf += PARAM_WIT_SHORT_SIZE;
            unif_serialize(buf, sig->proofs[e].unif[p]);
            buf += PARAM_UNIF_SHORT_SIZE;
        }
    }
    for(size_t e=0; e<PARAM_NB_EXECUTIONS; e++) {
        memcpy(buf, sig->proofs[e].cv_info, sig->proofs[e].len_cv_info);
        buf += sig->proofs[e].len_cv_info;
    }

    return (int)(buf - bufBase);
}
