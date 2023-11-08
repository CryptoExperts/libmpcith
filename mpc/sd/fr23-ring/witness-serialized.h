// "PARAM_INSTANCE_SIZE" corresponds to the size
//   in bytes of the structure of type "instance_t"
//   when serialized.
#define PARAM_INSTANCE_SIZE (PARAM_SEED_SIZE+PARAM_SYNDROME_LENGTH)

// "PARAM_SOL_SIZE" corresponds to the size
//   in bytes of the structure of type "solution_t"
//   when serialized.
#define PARAM_SOL_SIZE (PARAM_CODE_DIMENSION+PARAM_SPLITTING_FACTOR*PARAM_CHUNK_WEIGHT)

#define PARAM_TEMPLATE_INSTANCE_SIZE (PARAM_SEED_SIZE)
