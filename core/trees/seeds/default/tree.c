#include "tree.h"
#include "hash.h"
#include "parameters.h"
#include <stdlib.h>
#include <string.h>

#if 2*PARAM_SEED_SIZE != PARAM_DIGEST_SIZE
#error "A hash digest does not correspond to two seeds."
#endif

// Form of the tree with N=9 leaves.
//  |       /\       |
//  |     /\  /\     |
//  |    /\/\/\/\    |
//  |   /\           |
// 
// nb_nodes = 2N-1
// last_non_leaf = N-1 (when root = 1)
// first_leaf = N (when root = 1)

#define get_node(tree,idx) tree->nodes[(idx)-1]
seed_tree_t* malloc_seed_tree(uint32_t height, uint32_t nb_leaves) {
    uint32_t nb_nodes = 2*nb_leaves - 1;
    if(((nb_leaves-1)>>(height-1)) == 0 || ((nb_leaves-1)>>height != 0))
        return NULL;

    seed_tree_t* tree = (seed_tree_t*) malloc(sizeof(seed_tree_t));

    tree->height = height;
    tree->nb_leaves = nb_leaves;
    tree->nb_nodes = nb_nodes;
    tree->nodes = (uint8_t**) malloc(nb_nodes * sizeof(uint8_t*));

    uint8_t* slab = (uint8_t*)malloc(nb_nodes*PARAM_SEED_SIZE);
    for (uint32_t i=1; i <= nb_nodes; i++) {
        get_node(tree, i) = slab;
        slab += PARAM_SEED_SIZE;
    }

    return tree;
}

void free_seed_tree(seed_tree_t* tree) {
    if (tree != NULL) {
        free(tree->nodes[0]);
        free(tree->nodes);
        free(tree);
    }
}

uint8_t** get_leaves(seed_tree_t* tree) {
    return &tree->nodes[tree->nb_leaves-1];
}

void expand_seed_tree(seed_tree_t* tree, const uint8_t* root_seed, const uint8_t* salt) {
    memcpy(get_node(tree,1), root_seed, PARAM_SEED_SIZE);
    if(tree->height == 0)
        return; // Height=0 => nothing to expand

    hash_context ctx;
    hash_context_x4 ctx_x4;

    uint32_t first_leaf = (tree->nb_leaves);

    uint32_t i = 1;
    for(; i <= 3 && i<first_leaf; i++) {
        // Expand the three first internal nodes...
        hash_init_prefix(&ctx, HASH_PREFIX_SEED_TREE);
        hash_update(&ctx, salt, PARAM_SALT_SIZE);
        hash_update_uint16_le(&ctx, (uint16_t) i);
        hash_update(&ctx, get_node(tree,i), PARAM_SEED_SIZE);
        // It works since nodes[2*i] and nodes[2*i+1] are consecutive in memory
        hash_final(&ctx, get_node(tree,2*i));
    }
    for(; i+3 < first_leaf; i+=4) {
        // Expand each remaining internal nodes using Hash x4...
        hash_init_prefix_x4(&ctx_x4, HASH_PREFIX_SEED_TREE);
        hash_update_x4_1(&ctx_x4, salt, PARAM_SALT_SIZE);
        const uint16_t nodes[4] = {
            (uint16_t)i, (uint16_t)(i+1),
            (uint16_t)(i+2), (uint16_t)(i+3)
        };
        hash_update_x4_uint16s_le(&ctx_x4, nodes);
        uint8_t const* input_seeds[4] = {
            get_node(tree,i+0), get_node(tree,i+1),
            get_node(tree,i+2), get_node(tree,i+3)
        };
        hash_update_x4(&ctx_x4, input_seeds, PARAM_SEED_SIZE);
        uint8_t* output_chunks[4] = {
            get_node(tree,2*(i+0)), get_node(tree,2*(i+1)),
            get_node(tree,2*(i+2)), get_node(tree,2*(i+3))
        };
        // It works since nodes[2*i] and nodes[2*i+1] are consecutive in memory
        hash_final_x4(&ctx_x4, output_chunks);
    }
    for(; i<first_leaf; i++) {
        // Expand the three first internal nodes...
        hash_init_prefix(&ctx, HASH_PREFIX_SEED_TREE);
        hash_update(&ctx, salt, PARAM_SALT_SIZE);
        hash_update_uint16_le(&ctx, (uint16_t) i);
        hash_update(&ctx, get_node(tree,i), PARAM_SEED_SIZE);
        // It works since nodes[2*i] and nodes[2*i+1] are consecutive in memory
        hash_final(&ctx, get_node(tree,2*i));
    }
}

uint32_t get_seed_path(uint8_t* path, const seed_tree_t* tree, uint16_t hidden_leaf) {
    uint32_t first_leaf = (tree->nb_leaves);
    uint32_t hidden_node = hidden_leaf + first_leaf;

    uint8_t* path_base = path;
    while(hidden_node > 1) {
        uint16_t is_right = hidden_node & 0x01;
        uint16_t revealed_seed = (hidden_node & 0xFFFE) + (1-is_right);
        memcpy(path, get_node(tree,revealed_seed), PARAM_SEED_SIZE);
        path += PARAM_SEED_SIZE;
        hidden_node >>= 1; // Go to the parent node
    }
    return (uint32_t)(path-path_base);
}

uint32_t get_seed_path_size(uint32_t height, uint32_t nb_leaves, uint16_t hidden_leaf) {
    uint32_t first_leaf = nb_leaves;
    uint32_t hidden_node = hidden_leaf + first_leaf;
    if(hidden_node >= (uint32_t)(1<<height))
        return (PARAM_SEED_SIZE*height);
    else
        return (PARAM_SEED_SIZE*(height-1));
}

void reconstruct_tree(seed_tree_t* tree, uint16_t hidden_leaf, const uint8_t* path, const uint8_t* salt) {
    uint16_t* pos = (uint16_t*) malloc(tree->height*sizeof(uint16_t));

    uint32_t first_leaf = (tree->nb_leaves);
    uint32_t hidden_node = hidden_leaf + first_leaf;

    hash_context ctx;
    hash_context_x4 ctx_x4;

    // Get the indexes of all the node in the path
    uint32_t is_small_path = (hidden_node < (uint32_t)(1<<tree->height));
    for(uint32_t i=is_small_path; i<tree->height; i++) {
        uint16_t is_right = hidden_node & 0x01;
        uint16_t revealed_seed = (hidden_node & 0xFFFE) + (1-is_right);
        pos[tree->height-i-1] = revealed_seed;
        hidden_node >>= 1; // Go to the parent node
    }

    // Depth 1
    memcpy(get_node(tree,pos[0]), path+PARAM_SEED_SIZE*(tree->height-1-is_small_path), PARAM_SEED_SIZE);
    hash_init_prefix(&ctx, HASH_PREFIX_SEED_TREE);
    hash_update(&ctx, salt, PARAM_SALT_SIZE);
    hash_update_uint16_le(&ctx, (uint16_t) pos[0]);
    hash_update(&ctx, get_node(tree,pos[0]), PARAM_SEED_SIZE);
    // It works since nodes[2*revealed_seed] and nodes[2*revealed_seed+1] are consecutive in memory
    hash_final(&ctx, get_node(tree,2*pos[0]));

    // Depth > 1
    for(uint32_t depth=2; depth<tree->height; depth++) {
        // Let write the given seed of the layer
        memcpy(get_node(tree,pos[depth-1]), path+PARAM_SEED_SIZE*(tree->height-depth-is_small_path), PARAM_SEED_SIZE);
        
        uint32_t i=(1<<depth);
        uint32_t end = (uint32_t)(1<<(depth+1));
        if(depth == tree->height-1) // Last depth
            end = first_leaf;
        for(; i+3 < end; i+=4) {
            // Expand each remaining internal nodes using Hash x4...
            hash_init_prefix_x4(&ctx_x4, HASH_PREFIX_SEED_TREE);
            hash_update_x4_1(&ctx_x4, salt, PARAM_SALT_SIZE);
            const uint16_t nodes[4] = {
                (uint16_t)i, (uint16_t)(i+1),
                (uint16_t)(i+2), (uint16_t)(i+3)
            };
            hash_update_x4_uint16s_le(&ctx_x4, nodes);
            uint8_t const* input_seeds[4] = {
                get_node(tree,i+0), get_node(tree,i+1),
                get_node(tree,i+2), get_node(tree,i+3)
            };
            hash_update_x4(&ctx_x4, input_seeds, PARAM_SEED_SIZE);
            uint8_t* output_chunks[4] = {
                get_node(tree,2*(i+0)), get_node(tree,2*(i+1)),
                get_node(tree,2*(i+2)), get_node(tree,2*(i+3))
            };
            // It works since nodes[2*i] and nodes[2*i+1] are consecutive in memory
            hash_final_x4(&ctx_x4, output_chunks);
        }
        for(; i<end; i++) {
            hash_init_prefix(&ctx, HASH_PREFIX_SEED_TREE);
            hash_update(&ctx, salt, PARAM_SALT_SIZE);
            hash_update_uint16_le(&ctx, (uint16_t) i);
            hash_update(&ctx, get_node(tree,i), PARAM_SEED_SIZE);
            // It works since nodes[2*revealed_seed] and nodes[2*revealed_seed+1] are consecutive in memory
            hash_final(&ctx, get_node(tree,2*i));
        }
    }
    if(hidden_leaf + first_leaf >= (uint32_t)(1<<tree->height))
        memcpy(get_node(tree,pos[tree->height-1]), path, PARAM_SEED_SIZE);
    
    free(pos);
}
