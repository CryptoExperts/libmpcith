#include "sign-mpcith.h"
#include "sample.h"
#include "tree.h"
#include "commit.h"
#include "hash.h"
#include "keygen.h"
#include "mpc.h"
#include "views.h"
#include "benchmark.h"
#include "parameters-all.h"
#include "serialization.h"
#include "sign-mpcith-threshold-ggm-ring.h"
#include "mpc-protocol.h"
#include <string.h>
#include <stdio.h>

/***********************************************
 *            Signature Structure              *
 ***********************************************/

typedef struct proof_tprs_ext_t {
    uint8_t* seed_info; // Information required to compute the tree with seeds of of all opened parties
    uint32_t len_seed_info; // Length of seed_info buffer
    uint8_t* unopened_digest;

    // Plaintext broadcast messages
    vec_broadcast_t* broadcast[PARAM_CIRCUIT_DEPTH-1];
    
    // Last party's share
    vec_wit_t* wit;
    vec_sel_t* sel;
} proof_tprs_ext_t;

typedef struct signature_tprs_ext_t {
    uint8_t* salt;
    uint8_t* mpc_challenge_hash;
    uint8_t* view_challenge_hash;
    proof_tprs_ext_t proofs[PARAM_NB_EXECUTIONS];
    uint8_t* allocated_memory; // Just to manage the memory
} signature_tprs_ext_t;

// For parsing
typedef struct const_proof_tprs_ext_t {
    const uint8_t* seed_info; // Information required to compute the tree with seeds of of all opened parties
    uint32_t len_seed_info; // Length of seed_info buffer
    const uint8_t* unopened_digest;

    // Plaintext broadcast messages
    const vec_broadcast_t* broadcast[PARAM_CIRCUIT_DEPTH-1];
    
    // Last party's share
    const vec_wit_t* wit;
    const vec_sel_t* sel;
} const_proof_tprs_ext_t;

typedef struct const_signature_tprs_ext_t {
    const uint8_t* salt;
    const uint8_t* mpc_challenge_hash;
    const uint8_t* view_challenge_hash;
    const_proof_tprs_ext_t proofs[PARAM_NB_EXECUTIONS];
    uint8_t* allocated_memory; // Just to manage the memory
} const_signature_tprs_ext_t;

// Free signature structure
void free_signature(signature_tprs_ext_t* sig);
void free_const_signature(const_signature_tprs_ext_t* sig);

// For signing
signature_tprs_ext_t* init_signature_structure(const uint8_t* salt, uint8_t* buf, size_t buflen, sharing_info_t* info);
int build_signature(const signature_tprs_ext_t* sig, uint8_t* buf, size_t buflen, uint16_t hidden_views[PARAM_NB_EXECUTIONS], sharing_info_t* info);

// For verification
const_signature_tprs_ext_t* parse_signature(const uint8_t* buf, size_t buflen, uint16_t hidden_views[PARAM_NB_EXECUTIONS], sharing_info_t* info);

/***********************************************
 *             Hash for challenge              *
 ***********************************************/

static void hash_for_mpc_challenge(uint8_t challenge_hash[PARAM_DIGEST_SIZE], uint8_t seed_commitments[PARAM_NB_EXECUTIONS][PARAM_NB_PARTIES][PARAM_DIGEST_SIZE],
    const instance_t* inst, const uint8_t* salt, const uint8_t* message, size_t mlen)
{
    hash_context ctx;
    hash_init_prefix(&ctx, HASH_PREFIX_1);
    if(inst != NULL)
        hash_update_instance(&ctx, inst);
    hash_update(&ctx, (uint8_t*) seed_commitments, PARAM_NB_EXECUTIONS*PARAM_NB_PARTIES*PARAM_DIGEST_SIZE);
    hash_update(&ctx, salt, PARAM_SALT_SIZE);
    if(mlen > 0)
        hash_update(&ctx, message, mlen);
    hash_final(&ctx, challenge_hash);
}

static void hash_for_view_challenge(uint8_t challenge_hash[PARAM_DIGEST_SIZE], const uint8_t mpc_challenge_hash[PARAM_DIGEST_SIZE],
    vec_broadcast_t* broadcast[PARAM_NB_EXECUTIONS][PARAM_CIRCUIT_DEPTH], const uint8_t* salt, const uint8_t* message, size_t mlen)
{
    hash_context ctx;
    hash_init_prefix(&ctx, HASH_PREFIX_2);

    for (size_t e=0; e<PARAM_NB_EXECUTIONS; e++)
        for(uint32_t j=0; j<PARAM_CIRCUIT_DEPTH; j++)
            hash_update(&ctx, (uint8_t*) broadcast[e][j], PARAM_BR_SIZE);

    hash_update(&ctx, salt, PARAM_SALT_SIZE);
    if(mlen > 0)
        hash_update(&ctx, message, mlen);
    hash_update(&ctx, mpc_challenge_hash, PARAM_DIGEST_SIZE);
    hash_final(&ctx, challenge_hash);
}

/***********************************************
 *             Signing Algorithm               *
 ***********************************************/

int mpcith_tprs_ext_rsign(uint8_t* sig, size_t* siglen,
                const uint8_t* m, size_t mlen,
                const uint8_t* sk,
                uint8_t const* const* ring, size_t ring_size,
                const uint8_t* salt, const uint8_t* seed
                ) {
    size_t i, j, e;
    int ret;

#ifndef NDEBUG
    // In debug mode, let us check if the key generation
    //    produces valid key pair. 
    ret = mpcith_validate_keys(NULL, sk);
    if(ret)
        printf("Error: SK invalid for signing.\n");
#endif

    sharing_info_t sharing_info;
    sharing_info_t* sh_info = &sharing_info;
    ret = get_sharing_info(sh_info, ring_size);
    if(ret < 0)
        return ret;

    // Deserialize the private key
    mpcith_secret_key_t ssk;
    ret = deserialize_secret_key(&ssk, sk, PARAM_SECRETKEYBYTES);
    uncompress_instance(ssk.inst);
    if(ret < 0)
        return ret;
    vec_wit_t* plain_wit = ssk.wit;

    instance_t* rring = (instance_t*) malloc(ring_size*sizeof(instance_t));
    for(i=0; i<ring_size; i++) {
        mpcith_public_key_t tmp;
        ret = deserialize_public_key(&tmp, ring[i], PARAM_PUBLICKEYBYTES);
        if (ret < 0)
            return ret; // ALERT MEMORY LEAKAGE
        memcpy(&rring[i], tmp.inst, sizeof(instance_t));
        mpcith_free_keys_internal(&tmp, NULL);
    }
    for(i=1; i<ring_size; i++) {
        if(!has_same_template(&rring[0], &rring[i]))
            return -1; // ALERT MEMORY LEAKAGE
    }
    uint32_t position_in_ring = -1;
    uint8_t found = 0;
    for(i=0; i<ring_size; i++) {
        if(are_same_instances(ssk.inst, &rring[i])) {
            position_in_ring = i;
            found = 1;
            break;
        }
    }
    if(!found)
        return -1; // ALERT MEMORY LEAKAGE

    // Signature Structure
    uint32_t signature_size = mpcith_tprs_ext_rsign_get_size(ring_size);
    signature_tprs_ext_t* ssig = init_signature_structure(salt, sig, signature_size, sh_info);

    /********************************************/
    /**********     INITIALIZATION     **********/
    /********************************************/

    prg_context entropy_ctx;
    samplable_t entropy = prg_to_samplable(&entropy_ctx);
    #ifdef PARAM_RND_EXPANSION_X4
    prg_context_x4 entropy_ctx_x4;
    samplable_x4_t entropy_x4 = prg_to_samplable_x4(&entropy_ctx_x4);
    #else /* PARAM_RND_EXPANSION_X4 */
    prg_context entropy_ctx_x4[4];
    samplable_t entropy_x4[4];
    for(j=0; j<4; j++)
        entropy_x4[j]= prg_to_samplable(&entropy_ctx_x4[j]);
    #endif /* PARAM_RND_EXPANSION_X4 */
    seed_tree_t* seeds_tree[PARAM_NB_EXECUTIONS];
    for(e=0; e<PARAM_NB_EXECUTIONS; e++)
        seeds_tree[e] = malloc_seed_tree(PARAM_LOG_NB_PARTIES, PARAM_NB_PARTIES);
    uint8_t seed_commitments[PARAM_NB_EXECUTIONS][PARAM_NB_PARTIES][PARAM_DIGEST_SIZE];

    // Derive the root seeds for all parallel executions
    uint8_t rseed[PARAM_NB_EXECUTIONS][PARAM_SEED_SIZE];
    prg_init(&entropy_ctx, seed, ssig->salt);
    byte_sample(&entropy, (uint8_t*) rseed, PARAM_NB_EXECUTIONS*PARAM_SEED_SIZE);

    // We manipulate here pointers
    //    to avoid & when using
    //    to be more consistant with plain_wit
    vec_share_t* packed_share[PARAM_NB_EXECUTIONS];
    for(e=0; e<PARAM_NB_EXECUTIONS; e++)
        packed_share[e] = new_share(sh_info);
    vec_unif_t* plain_unif[PARAM_NB_EXECUTIONS];
    vec_sel_t* plain_sel[PARAM_NB_EXECUTIONS];

    /********************************************/
    /********   COMMIT PARTIES' INPUTS   ********/
    /********************************************/

    __BENCHMARK_START__(BS_SHARE_PREPARATION);
    vec_share_t* shares[PARAM_NB_PARTIES];
    for(i=0; i<PARAM_NB_PARTIES; i++)
        shares[i] = (i<4) ? new_share(sh_info) : shares[i % 4];
    vec_share_t* acc = new_share(sh_info);
    packing_context_t packing_ctx[PARAM_NB_EXECUTIONS];
    for(e=0; e<PARAM_NB_EXECUTIONS; e++) {
        // Build the seed tree of the current execution
        __BENCHMARK_START__(BS_TREE_EXPANSION);
        expand_seed_tree(seeds_tree[e], rseed[e], ssig->salt);
        __BENCHMARK_STOP__(BS_TREE_EXPANSION);
        uint8_t** seeds = get_leaves(seeds_tree[e]);
        vec_share_setzero(acc, sh_info);

        __BENCHMARK_START__(BS_SHARE_PACKING);
        packing_init(&packing_ctx[e], packed_share[e], sh_info);
        __BENCHMARK_STOP__(BS_SHARE_PACKING);

        i=0;
        // Let us treat the parties four by four...
        for(; (i+3)+1<PARAM_NB_PARTIES; i+=4) {
            const uint8_t* ptr_seeds[4] = {seeds[i], seeds[i+1], seeds[i+2], seeds[i+3]};
            __BENCHMARK_START__(BS_RND_EXPANSION);
            #ifdef PARAM_RND_EXPANSION_X4
            prg_init_x4(&entropy_ctx_x4, ptr_seeds, NULL);
            vec_share_rnd_x4((void**) &shares[i], &entropy_x4, sh_info);
            #else /* PARAM_RND_EXPANSION_X4 */
            // No need of the salt, we do not care about
            //    collisions of the leave seeds
            prg_init_x4_array(entropy_ctx_x4, ptr_seeds, NULL);
            for(j=0; j<4; j++)
                vec_share_rnd((void*) shares[i+j], &entropy_x4[j], sh_info);
            #endif /* PARAM_RND_EXPANSION_X4 */
            __BENCHMARK_STOP__(BS_RND_EXPANSION);
            for(j=0; j<4; j++) {
                vec_share_add(acc, shares[j], sh_info);
                __BENCHMARK_START__(BS_SHARE_PACKING);
                packing_update(&packing_ctx[e], i+j, shares[i+j], sh_info);
                __BENCHMARK_STOP__(BS_SHARE_PACKING);
            }
            __BENCHMARK_START__(BS_COMMITMENT);
            uint8_t* digests[4] = {
                seed_commitments[e][i+0], seed_commitments[e][i+1],
                seed_commitments[e][i+2], seed_commitments[e][i+3], 
            };
            const uint16_t is[4] = {(uint16_t)i, (uint16_t)(i+1), (uint16_t)(i+2), (uint16_t)(i+3)};
            commit_seed_x4(digests, (uint8_t const*const*) &seeds[i], ssig->salt, e, is);
            __BENCHMARK_STOP__(BS_COMMITMENT);
        }
        // Now we treat the last parties
        for(;i<PARAM_NB_PARTIES; i++) {
            __BENCHMARK_START__(BS_RND_EXPANSION);
            // No need of the salt, we do not care about
            //    collisions of the leave seeds
            prg_init(&entropy_ctx, seeds[i], NULL);
            __BENCHMARK_STOP__(BS_RND_EXPANSION);
            if(i != PARAM_NB_PARTIES-1) {
                // Expand the input share from seed
                __BENCHMARK_START__(BS_RND_EXPANSION);
                vec_share_rnd(shares[i], &entropy, sh_info);
                __BENCHMARK_STOP__(BS_RND_EXPANSION);
                vec_share_add(acc, shares[i], sh_info);
                __BENCHMARK_START__(BS_SHARE_PACKING);
                packing_update(&packing_ctx[e], i, shares[i], sh_info);
                __BENCHMARK_STOP__(BS_SHARE_PACKING);

                // Commit to the party's input (by committing to its seed)
                __BENCHMARK_START__(BS_COMMITMENT);
                commit_seed(seed_commitments[e][i], seeds[i], ssig->salt, e, i);
                __BENCHMARK_STOP__(BS_COMMITMENT);
            } else {
                // Compute plain unif
                __BENCHMARK_START__(BS_RND_EXPANSION);
                vec_unif_rnd(get_unif(shares[i],sh_info), &entropy, sh_info);
                __BENCHMARK_STOP__(BS_RND_EXPANSION);
                plain_unif[e] = new_unif(sh_info);
                vec_unif_set(plain_unif[e], get_unif(shares[i],sh_info), sh_info);
                vec_unif_add(plain_unif[e], get_unif(acc,sh_info), sh_info);
                vec_unif_normalize(plain_unif[e], sh_info);
                plain_sel[e] = new_sel(sh_info);
                build_selector(plain_sel[e], position_in_ring, sh_info);

                // Compute aux
                vec_wit_set(ssig->proofs[e].wit, plain_wit, sh_info);
                vec_wit_sub(ssig->proofs[e].wit, get_wit(acc,sh_info), sh_info);
                vec_sel_set(ssig->proofs[e].sel, plain_sel[e], sh_info);
                vec_sel_sub(ssig->proofs[e].sel, get_sel(acc,sh_info), sh_info);
                vec_wit_normalize(ssig->proofs[e].wit, sh_info);
                vec_sel_normalize(ssig->proofs[e].sel, sh_info);

                // Commit to the party's input
                __BENCHMARK_START__(BS_COMMITMENT);
                commit_seed_and_aux(seed_commitments[e][i], seeds[i], ssig->proofs[e].wit, ssig->proofs[e].sel, ssig->salt, e, i, sh_info);
                __BENCHMARK_STOP__(BS_COMMITMENT);

                __BENCHMARK_START__(BS_SHARE_PACKING);
                vec_wit_set(get_wit(shares[i], sh_info), ssig->proofs[e].wit, sh_info);
                vec_sel_set(get_sel(shares[i], sh_info), ssig->proofs[e].sel, sh_info);
                packing_update(&packing_ctx[e], PARAM_NB_PARTIES-1, shares[i], sh_info);
                __BENCHMARK_STOP__(BS_SHARE_PACKING);
            }
        }

        __BENCHMARK_START__(BS_SHARE_PACKING);
        packing_final(&packing_ctx[e], sh_info);
        __BENCHMARK_STOP__(BS_SHARE_PACKING);
    }
    free(acc);
    for(i=0; i<4; i++)
        free(shares[i]);
    __BENCHMARK_STOP__(BS_SHARE_PREPARATION);


    // Expand the MPC challenge
    __BENCHMARK_START__(BS_CHALLENGE);
    vec_challenge_t* mpc_challenge = new_challenge(sh_info);
    hash_for_mpc_challenge(ssig->mpc_challenge_hash, seed_commitments, NULL, ssig->salt, m, mlen);
    expand_mpc_challenge_hash(&mpc_challenge, ssig->mpc_challenge_hash, 1, ssk.inst, rring, sh_info);
    __BENCHMARK_STOP__(BS_CHALLENGE);

    /********************************************/
    /*********  SIMULATE MPC PROTOCOL  **********/
    /********************************************/

    __BENCHMARK_START__(BS_MPC_EMULATION);
    vec_broadcast_t* broadcast[PARAM_NB_EXECUTIONS][PARAM_CIRCUIT_DEPTH];
    for(e=0; e<PARAM_NB_EXECUTIONS; e++) {
        vec_share_t* plain = new_share(sh_info);
        vec_wit_set(get_wit(plain, sh_info), plain_wit, sh_info);
        vec_unif_set(get_unif(plain, sh_info), plain_unif[e], sh_info);
        vec_sel_set(get_sel(plain, sh_info), plain_sel[e], sh_info);

        for(j=0; j<PARAM_CIRCUIT_DEPTH; j++)
            broadcast[e][j] = new_br(sh_info);
        mpc_compute_communications(broadcast[e], mpc_challenge, plain, packed_share[e], ssk.inst, rring, sh_info);
        for(j=0; j<PARAM_CIRCUIT_DEPTH-1; j++)
            vec_br_set(ssig->proofs[e].broadcast[j], broadcast[e][j], sh_info);
        free(plain);
    }
    __BENCHMARK_STOP__(BS_MPC_EMULATION);

    // Expand the view challenge
    __BENCHMARK_START__(BS_CHALLENGE);
    uint16_t hidden_views[PARAM_NB_EXECUTIONS];
    hash_for_view_challenge(ssig->view_challenge_hash, ssig->mpc_challenge_hash, broadcast, ssig->salt, m, mlen);
    __BENCHMARK_STOP__(BS_CHALLENGE);
    expand_view_challenge_hash((uint16_t*) hidden_views, ssig->view_challenge_hash, PARAM_NB_EXECUTIONS, 1);

    /********************************************/
    /**********   FINALIZE SIGNATURE   **********/
    /********************************************/

    __BENCHMARK_START__(BS_BUILD_SIG);
    for(e=0; e<PARAM_NB_EXECUTIONS; e++) {
        uint16_t num_unopened_party = hidden_views[e];
        ssig->proofs[e].len_seed_info = get_seed_path(ssig->proofs[e].seed_info, seeds_tree[e], num_unopened_party);
        memcpy(ssig->proofs[e].unopened_digest, seed_commitments[e][num_unopened_party], PARAM_DIGEST_SIZE);
    }

    // Serialize the signature
    ret = build_signature(ssig, sig, signature_size, hidden_views, sh_info);
    free_signature(ssig);
    __BENCHMARK_STOP__(BS_BUILD_SIG);

    for(e=0; e<PARAM_NB_EXECUTIONS; e++)
        free_seed_tree(seeds_tree[e]);
    mpcith_free_keys_internal(NULL, &ssk);
    free(rring);
    for(e=0; e<PARAM_NB_EXECUTIONS; e++) {
        free(packed_share[e]);
        for(j=0; j<PARAM_CIRCUIT_DEPTH; j++)
            free(broadcast[e][j]);
        free(plain_unif[e]);
    }
    free(mpc_challenge);

    if(ret < 0)
        return ret;
    *siglen = ret;
    return 0;
}

/***********************************************
 *            Verication Algorithm             *
 ***********************************************/

int mpcith_tprs_ext_rsign_verify(const uint8_t* sig, size_t siglen,
                const uint8_t* m, size_t mlen,
                uint8_t const* const* ring, size_t ring_size
                ) {
    size_t i, j, e;
    int ret;

    sharing_info_t sharing_info;
    sharing_info_t* sh_info = &sharing_info;
    ret = get_sharing_info(sh_info, ring_size);
    if(ret < 0)
        return ret;

    instance_t* rring = (instance_t*) malloc(ring_size*sizeof(instance_t));
    for(i=0; i<ring_size; i++) {
        mpcith_public_key_t tmp;
        ret = deserialize_public_key(&tmp, ring[i], PARAM_PUBLICKEYBYTES);
        if (ret < 0)
            return ret; // ALERT MEMORY LEAKAGE
        memcpy(&rring[i], tmp.inst, sizeof(instance_t));
        mpcith_free_keys_internal(&tmp, NULL);
    }
    for(i=1; i<ring_size; i++) {
        if(!has_same_template(&rring[0], &rring[i]))
            return -1; // ALERT MEMORY LEAKAGE
    }
    uncompress_instance(&rring[0]);

    // Parse the signature
    // Note: while parsing, it expands the view challenge.
    uint16_t hidden_views[PARAM_NB_EXECUTIONS];
    const_signature_tprs_ext_t* ssig = parse_signature(sig, siglen, hidden_views, sh_info);
    if(ssig == NULL) {
        ret = -1;
        free_instance(&rring[0]); // Free all
        return ret;
    }

    // Initialize
    uint8_t seed_commitments[PARAM_NB_EXECUTIONS][PARAM_NB_PARTIES][PARAM_DIGEST_SIZE];
    prg_context entropy_ctx;
    samplable_t entropy = prg_to_samplable(&entropy_ctx);
    #ifdef PARAM_RND_EXPANSION_X4
    prg_context_x4 entropy_ctx_x4;
    samplable_x4_t entropy_x4 = prg_to_samplable_x4(&entropy_ctx_x4);
    #else /* PARAM_RND_EXPANSION_X4 */
    prg_context entropy_ctx_x4[4];
    samplable_t entropy_x4[4];
    for(j=0; j<4; j++)
        entropy_x4[j]= prg_to_samplable(&entropy_ctx_x4[j]);
    #endif /* PARAM_RND_EXPANSION_X4 */

    // Expand MPC Challenge
    __BENCHMARK_START__(BV_CHALLENGE);
    vec_challenge_t* mpc_challenge = new_challenge(sh_info);
    expand_mpc_challenge_hash(&mpc_challenge, ssig->mpc_challenge_hash, 1, &rring[0], rring, sh_info);
    __BENCHMARK_STOP__(BV_CHALLENGE);

    vec_share_t* shares[PARAM_NB_PARTIES];
    for(i=0; i<PARAM_NB_PARTIES; i++)
        shares[i] = (i<4) ? new_share(sh_info) : shares[i % 4];
    vec_share_t* party_share = new_share(sh_info);
    vec_broadcast_t* broadcast[PARAM_NB_EXECUTIONS][PARAM_CIRCUIT_DEPTH];
    for(e=0; e<PARAM_NB_EXECUTIONS; e++)
        for(j=0; j<PARAM_CIRCUIT_DEPTH; j++)
            broadcast[e][j] = new_br(sh_info);
    share_recomputing_context_t packing_ctx;

    for(e=0; e<PARAM_NB_EXECUTIONS; e++) {
        // Get the open leaf seeds
        __BENCHMARK_START__(BV_TREE_EXPANSION);
        seed_tree_t* seed_tree = (seed_tree_t*)malloc_seed_tree(PARAM_LOG_NB_PARTIES, PARAM_NB_PARTIES);
        reconstruct_tree(seed_tree, hidden_views[e], ssig->proofs[e].seed_info, ssig->salt);
        uint8_t** seeds = get_leaves(seed_tree);
        __BENCHMARK_STOP__(BV_TREE_EXPANSION);

        for(j=0; j<PARAM_CIRCUIT_DEPTH-1; j++)
            vec_br_set(broadcast[e][j], ssig->proofs[e].broadcast[j], sh_info);
        __BENCHMARK_START__(BV_SHARE_PACKING);
        share_recomputing_init(&packing_ctx, hidden_views[e], party_share, sh_info);
        __BENCHMARK_STOP__(BV_SHARE_PACKING);

        i=0;
        // Let us treat the parties four by four...
        for(; (i+3)+1<PARAM_NB_PARTIES; i+=4) {
            __BENCHMARK_START__(BV_COMMITMENT);
            uint8_t* digests[4] = {
                seed_commitments[e][i+0], seed_commitments[e][i+1],
                seed_commitments[e][i+2], seed_commitments[e][i+3], 
            };
            const uint16_t is[4] = {(uint16_t)i, (uint16_t)(i+1), (uint16_t)(i+2), (uint16_t)(i+3)};
            commit_seed_x4(digests, (uint8_t const*const*) &seeds[i], ssig->salt, (uint16_t)e, is);
            __BENCHMARK_STOP__(BV_COMMITMENT);
            const uint8_t* ptr_seeds[4] = {seeds[i], seeds[i+1], seeds[i+2], seeds[i+3]};
            __BENCHMARK_START__(BV_RND_EXPANSION);
            #ifdef PARAM_RND_EXPANSION_X4
            prg_init_x4(&entropy_ctx_x4, ptr_seeds, NULL);
            vec_share_rnd_x4((void**) &shares[i], &entropy_x4, sh_info);
            #else /* PARAM_RND_EXPANSION_X4 */
            // No need of the salt, we do not care about
            //    collisions of the leave seeds
            prg_init_x4_array(entropy_ctx_x4, ptr_seeds, NULL);
            for(j=0; j<4; j++)
                vec_share_rnd((void*) shares[i+j], &entropy_x4[j], sh_info);
            #endif /* PARAM_RND_EXPANSION_X4 */
            __BENCHMARK_STOP__(BV_RND_EXPANSION);
            for(j=0; j<4; j++) {
                if(i+j != hidden_views[e]) {
                    __BENCHMARK_START__(BV_SHARE_PACKING);
                    share_recomputing_update(&packing_ctx, i+j, shares[i+j], sh_info);
                    __BENCHMARK_STOP__(BV_SHARE_PACKING);
                } else {
                    memcpy(seed_commitments[e][i+j], ssig->proofs[e].unopened_digest, PARAM_DIGEST_SIZE);
                }
            }
        }
        // Now we treat the last parties
        for(; i<PARAM_NB_PARTIES; i++) {
            if(i == hidden_views[e]) {
                memcpy(seed_commitments[e][i], ssig->proofs[e].unopened_digest, PARAM_DIGEST_SIZE);
                continue;
            }
            prg_init(&entropy_ctx, seeds[i], NULL);
            if(i != PARAM_NB_PARTIES-1) {
                // Expand the input share from seed
                __BENCHMARK_START__(BV_RND_EXPANSION);
                vec_share_rnd(shares[i], &entropy, sh_info);
                __BENCHMARK_STOP__(BV_RND_EXPANSION);

                // Recompute the party's commitment
                __BENCHMARK_START__(BV_COMMITMENT);
                commit_seed(seed_commitments[e][i], seeds[i], ssig->salt, e, i);
                __BENCHMARK_STOP__(BV_COMMITMENT);

            } else {
                // Get the party's input share
                __BENCHMARK_START__(BV_RND_EXPANSION);
                vec_unif_rnd(get_unif(shares[i], sh_info), &entropy, sh_info);
                __BENCHMARK_STOP__(BV_RND_EXPANSION);
                vec_wit_set(get_wit(shares[i], sh_info), ssig->proofs[e].wit, sh_info);
                vec_sel_set(get_sel(shares[i], sh_info), ssig->proofs[e].sel, sh_info);

                // Recompute the party's commitment
                __BENCHMARK_START__(BV_COMMITMENT);
                commit_seed_and_aux(seed_commitments[e][i], seeds[i], ssig->proofs[e].wit, ssig->proofs[e].sel, ssig->salt, e, i, sh_info);
                __BENCHMARK_STOP__(BV_COMMITMENT);
            }

            __BENCHMARK_START__(BV_SHARE_PACKING);
            share_recomputing_update(&packing_ctx, i, shares[i], sh_info);
            __BENCHMARK_STOP__(BV_SHARE_PACKING);
        }
        __BENCHMARK_START__(BV_SHARE_PACKING);
        share_recomputing_final(&packing_ctx, sh_info);
        __BENCHMARK_STOP__(BV_SHARE_PACKING);
        
        // Re-emulate the computation of the open main parties
        __BENCHMARK_START__(BV_MPC_EMULATION);
        mpc_emulate_party(broadcast[e][PARAM_CIRCUIT_DEPTH-1], mpc_challenge, party_share, hidden_views[e], &rring[0], rring, sh_info);
        recompose_broadcast(broadcast[e], hidden_views[e], sh_info);
        __BENCHMARK_STOP__(BV_MPC_EMULATION);

        free_seed_tree(seed_tree);
    }
    free(party_share);
    for(j=0; j<4; j++)
        free(shares[j]);
    
    // Recompute the hash digests of the challenges
    //    and check they are consistent with the ones in the signature
    uint8_t mpc_challenge_hash[PARAM_DIGEST_SIZE], view_challenge_hash[PARAM_DIGEST_SIZE];
    hash_for_mpc_challenge(mpc_challenge_hash, seed_commitments, NULL, ssig->salt, m, mlen);
    hash_for_view_challenge(view_challenge_hash, ssig->mpc_challenge_hash, broadcast, ssig->salt, m, mlen);
    ret = (memcmp(mpc_challenge_hash, ssig->mpc_challenge_hash, PARAM_DIGEST_SIZE) != 0);
    ret |= (memcmp(view_challenge_hash, ssig->view_challenge_hash, PARAM_DIGEST_SIZE) != 0);

    free_instance(&rring[0]); // Free all
    free_const_signature(ssig);
    for(e=0; e<PARAM_NB_EXECUTIONS; e++)
        for(j=0; j<PARAM_CIRCUIT_DEPTH; j++)
            free(broadcast[e][j]);
    free(mpc_challenge);
    return ret;
}

/***********************************************
 *             About Serialization             *
 ***********************************************/

void free_signature(signature_tprs_ext_t* sig) {
    if(sig->allocated_memory != NULL)
        free(sig->allocated_memory);
    free(sig);
}

void free_const_signature(const_signature_tprs_ext_t* sig) {
    if(sig->allocated_memory != NULL)
        free(sig->allocated_memory);
    free(sig);
}

int mpcith_tprs_ext_rsign_get_size(size_t ring_size) {
    sharing_info_t sharing_info;
    get_sharing_info(&sharing_info, ring_size);
    //printf("$ %d %d %d\n", PARAM_NB_EXECUTIONS, get_selector_short_size(sh_info), PARAM_COMPRESSED_BR_SIZE);

    uint32_t proof_size = 0;
    proof_size += (PARAM_CIRCUIT_DEPTH-1)*PARAM_COMPRESSED_BR_SIZE;
    proof_size += PARAM_WIT_SHORT_SIZE+get_selector_short_size(&sharing_info);
    proof_size += PARAM_LOG_NB_PARTIES*PARAM_SEED_SIZE;
    proof_size += PARAM_DIGEST_SIZE;

    uint32_t signature_size = 2*PARAM_DIGEST_SIZE + PARAM_SALT_SIZE;
    signature_size += PARAM_NB_EXECUTIONS*proof_size;
    return signature_size;
}

const_signature_tprs_ext_t* parse_signature(const uint8_t* buf, size_t buflen, uint16_t hidden_views[PARAM_NB_EXECUTIONS], sharing_info_t* info) {
    uint32_t selector_short_size = get_selector_short_size(info);
    uint32_t selector_bytesize = get_selector_bytesize(info);

    size_t bytes_required = 2*PARAM_DIGEST_SIZE + PARAM_SALT_SIZE;
    if(buflen < bytes_required) {
        return NULL;
    }

    const_signature_tprs_ext_t* sig = (const_signature_tprs_ext_t*)malloc(sizeof(const_signature_tprs_ext_t));
    sig->salt = buf;                buf += PARAM_SALT_SIZE;
    sig->mpc_challenge_hash = buf;    buf += PARAM_DIGEST_SIZE;
    sig->view_challenge_hash = buf;   buf += PARAM_DIGEST_SIZE;
    expand_view_challenge_hash((uint16_t*) hidden_views, sig->view_challenge_hash, PARAM_NB_EXECUTIONS, 1);

    for(size_t e=0; e<PARAM_NB_EXECUTIONS; e++) {
        sig->proofs[e].len_seed_info = get_seed_path_size(PARAM_LOG_NB_PARTIES, PARAM_NB_PARTIES, hidden_views[e]);
        bytes_required += sig->proofs[e].len_seed_info + PARAM_DIGEST_SIZE + PARAM_COMPRESSED_BR_SIZE*(PARAM_CIRCUIT_DEPTH-1);
        if(hidden_views[e] != PARAM_NB_PARTIES-1) {
            bytes_required += PARAM_WIT_SHORT_SIZE;
            bytes_required += selector_short_size;
        }
    }
    if(buflen != bytes_required) {
        free(sig);
        return NULL;
    }

    sig->allocated_memory = (uint8_t*)malloc(
        PARAM_NB_EXECUTIONS*(
            PARAM_BR_SIZE*(PARAM_CIRCUIT_DEPTH-1)+PARAM_WIT_PARSED_SIZE+selector_bytesize
        )
    );
    uint8_t* buf_ = sig->allocated_memory;
    for(size_t e=0; e<PARAM_NB_EXECUTIONS; e++) {
        sig->proofs[e].unopened_digest = buf;
        buf += PARAM_DIGEST_SIZE;
        for(uint32_t j=0; j<PARAM_CIRCUIT_DEPTH-1; j++) {
            br_deserialize(&sig->proofs[e].broadcast[j], buf, buf_);
            buf += PARAM_COMPRESSED_BR_SIZE;
            buf_ += PARAM_BR_SIZE;
        }
        sig->proofs[e].seed_info = buf;
        buf += sig->proofs[e].len_seed_info;
        if(hidden_views[e] != PARAM_NB_PARTIES-1) {
            wit_deserialize(&sig->proofs[e].wit, buf, buf_);
            buf += PARAM_WIT_SHORT_SIZE;
            buf_ += PARAM_WIT_PARSED_SIZE;
            sel_deserialize(&sig->proofs[e].sel, buf, buf_, info);
            buf += selector_short_size;
            buf_ += selector_bytesize;
        }
    }

    return sig;
}

signature_tprs_ext_t* init_signature_structure(const uint8_t* salt, uint8_t* buf, size_t buflen, sharing_info_t* info) {
    size_t bytes_required = 2*PARAM_DIGEST_SIZE + PARAM_SALT_SIZE;
    if(buflen < bytes_required) {
        return NULL;
    }

    signature_tprs_ext_t* sig = (signature_tprs_ext_t*)malloc(sizeof(signature_tprs_ext_t));
    sig->salt = buf;                 buf += PARAM_SALT_SIZE;
    sig->mpc_challenge_hash = buf;   buf += PARAM_DIGEST_SIZE;
    sig->view_challenge_hash = buf;  buf += PARAM_DIGEST_SIZE;
    memcpy(sig->salt, salt, PARAM_SALT_SIZE);

    uint32_t selector_bytesize = get_selector_bytesize(info);
    sig->allocated_memory = (uint8_t*)malloc(
        PARAM_NB_EXECUTIONS*(
            PARAM_LOG_NB_PARTIES*PARAM_SEED_SIZE // seed tree
             + PARAM_DIGEST_SIZE // unopen commitment
             + PARAM_BR_SIZE*(PARAM_CIRCUIT_DEPTH-1) // plain broadcast
             + PARAM_WIT_SIZE // wit of last party
             + selector_bytesize
        )
    );
    uint8_t* buf_ = sig->allocated_memory;
    for(size_t e=0; e<PARAM_NB_EXECUTIONS; e++) {
        sig->proofs[e].unopened_digest = buf_;
        buf_ += PARAM_DIGEST_SIZE;
        for(uint32_t j=0; j<PARAM_CIRCUIT_DEPTH-1; j++) {
            sig->proofs[e].broadcast[j] = (vec_broadcast_t*) buf_;
            buf_ += PARAM_BR_SIZE;
        }
        sig->proofs[e].seed_info = buf_;
        buf_ += PARAM_LOG_NB_PARTIES*PARAM_SEED_SIZE;
        sig->proofs[e].wit = (vec_wit_t*) buf_;   buf_ += PARAM_WIT_SIZE;
        sig->proofs[e].sel = (vec_sel_t*) buf_;   buf_ += selector_bytesize;
    }

    return sig;
}

int build_signature(const signature_tprs_ext_t* sig, uint8_t* buf, size_t buflen, uint16_t hidden_views[PARAM_NB_EXECUTIONS], sharing_info_t* info) {
    uint32_t selector_short_size = get_selector_short_size(info);

    size_t bytes_required = (2*PARAM_DIGEST_SIZE + PARAM_SALT_SIZE);
    bytes_required += PARAM_NB_EXECUTIONS*PARAM_COMPRESSED_BR_SIZE*(PARAM_CIRCUIT_DEPTH-1);
    bytes_required += PARAM_NB_EXECUTIONS*PARAM_DIGEST_SIZE;
    for(size_t e=0; e<PARAM_NB_EXECUTIONS; e++) {
        bytes_required += sig->proofs[e].len_seed_info;
        if(hidden_views[e] != PARAM_NB_PARTIES-1) {
            bytes_required += PARAM_WIT_SHORT_SIZE;
            bytes_required += selector_short_size;
        }
    }
    if(buflen < bytes_required)
        return -1;

    uint8_t* bufBase = buf;
    buf += (2*PARAM_DIGEST_SIZE + PARAM_SALT_SIZE); // salt and challenge hashes are already set (via init)
    for(size_t e=0; e<PARAM_NB_EXECUTIONS; e++) {
        memcpy(buf, sig->proofs[e].unopened_digest, PARAM_DIGEST_SIZE);
        buf += PARAM_DIGEST_SIZE;
        for(uint32_t j=0; j<PARAM_CIRCUIT_DEPTH-1; j++) {
            br_serialize(buf, sig->proofs[e].broadcast[j]);
            buf += PARAM_COMPRESSED_BR_SIZE;
        }
        memcpy(buf, sig->proofs[e].seed_info, sig->proofs[e].len_seed_info);
        buf += sig->proofs[e].len_seed_info;
        if(hidden_views[e] != PARAM_NB_PARTIES-1) {
            wit_serialize(buf, sig->proofs[e].wit);
            buf += PARAM_WIT_SHORT_SIZE;
            sel_serialize(buf, sig->proofs[e].sel, info);
            buf += selector_short_size;
        }
    }

    return (int)(buf - bufBase);
}
