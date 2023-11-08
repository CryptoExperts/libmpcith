#include "mpc.h"
#include "parameters-all.h"

// We reimplement the sqrt function to avoid to rely on an external library
// It is not an issue if it is not performant,
//   it will be used only once per signature.
uint32_t sqrt_floor(uint32_t x, uint32_t root) {
    // Base Cases
    if (x == 0 || x == 1)
        return x;
 
    uint64_t maxi = (1L << ((32 + root - 1) / root))-1;

    // Do Binary Search for floor(sqrt(x))
    uint64_t start = 1, end = x / 2, ans = 0;
    if(end > maxi)
        end = maxi;
    while(start <= end) {
        uint64_t mid = (start + end) / 2;
 
        uint64_t v = mid;
        for(uint32_t i=1; i<root; i++)
            v *= mid;

        // If x is a perfect power
        if (v == x)
            return (uint64_t)mid;
 
        // Since we need floor, we update answer when
        // mid^{root} is smaller than x, and move closer to
        // sqrt[root](x)
        if(v < x) {
            start = mid + 1;
            ans = mid;
        }
        else // If mid^{root} is greater than x
            end = mid - 1;
    }
    return (uint32_t)ans;
}

static const uint8_t barrett_reduction_shift[256] = {0, 32, 33, 33, 34, 34, 34, 35, 35, 33, 35, 35, 35, 34, 36, 35, 36, 35, 34, 35, 36, 34, 36, 36, 36, 35, 35, 37, 36, 36, 36, 37, 37, 35, 36, 37, 35, 37, 36, 37, 37, 37, 35, 35, 37, 37, 37, 37, 37, 36, 36, 37, 36, 36, 38, 38, 37, 38, 37, 35, 37, 36, 38, 38, 38, 38, 36, 33, 37, 37, 38, 38, 36, 38, 38, 35, 37, 36, 38, 38, 38, 38, 38, 36, 36, 38, 36, 36, 38, 38, 38, 38, 38, 38, 38, 39, 38, 39, 37, 38, 37, 39, 38, 39, 37, 39, 37, 37, 39, 39, 39, 39, 38, 39, 39, 39, 38, 39, 36, 37, 38, 39, 37, 39, 39, 35, 39, 39, 39, 35, 39, 39, 37, 39, 34, 39, 38, 34, 38, 38, 39, 37, 39, 37, 37, 39, 39, 39, 39, 39, 36, 37, 38, 39, 37, 39, 39, 37, 39, 36, 39, 39, 39, 39, 39, 38, 37, 38, 37, 38, 39, 39, 37, 36, 37, 38, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 40, 40, 39, 39, 36, 36, 40, 38, 38, 39, 38, 38, 33, 40, 39, 39, 38, 36, 37, 38, 39, 40, 39, 38, 39, 38, 38, 40, 39, 40, 40, 40, 39, 40, 40, 39, 39, 40, 37, 40, 38, 40, 40, 39, 39, 40, 40, 37, 39, 38, 39, 39, 36, 40, 39, 38, 40, 40, 40, 40, 40, 36, 39, 40, 39, 40, 39};
static const uint64_t barrett_reduction_inv[256] = {0, 4294967297, 4294967297, 2863311531, 4294967297, 3435973837, 2863311531, 4908534053, 4294967297, 954437177, 3435973837, 3123612579, 2863311531, 1321528399, 4908534053, 2290649225, 4294967297, 2021161081, 954437177, 1808407283, 3435973837, 818089009, 3123612579, 2987803337, 2863311531, 1374389535, 1321528399, 5090331611, 2454267027, 2369637129, 2290649225, 4433514629, 4294967297, 1041204193, 2021161081, 3926827243, 954437177, 3714566311, 1808407283, 3524075731, 3435973837, 3352169597, 818089009, 799063683, 3123612579, 3054198967, 2987803337, 2924233053, 2863311531, 1402438301, 1374389535, 2694881441, 1321528399, 1296593901, 5090331611, 4997780127, 2454267027, 4822419421, 2369637129, 582368447, 2290649225, 1126548799, 4433514629, 4363141381, 4294967297, 4228890877, 1041204193, 128207979, 2021161081, 1991868891, 3926827243, 3871519817, 954437177, 3765450781, 3714566311, 458129845, 1808407283, 892460737, 3524075731, 3479467177, 3435973837, 3393554407, 3352169597, 827945503, 818089009, 3233857729, 799063683, 789879043, 3123612579, 3088515809, 3054198967, 3020636341, 2987803337, 2955676419, 2924233053, 5786903305, 2863311531, 5667585711, 1402438301, 2776544515, 1374389535, 5443126871, 2694881441, 5337435087, 1321528399, 5235769657, 1296593901, 1284476201, 5090331611, 5043631321, 4997780127, 4952755081, 2454267027, 4865095699, 4822419421, 4780485339, 2369637129, 4698767641, 582368447, 1154949189, 2290649225, 4543436479, 1126548799, 4469559463, 4433514629, 274877907, 4363141381, 4328785937, 4294967297, 266354561, 4228890877, 4196609267, 1041204193, 4133502361, 128207979, 4072265289, 2021161081, 125400505, 1991868891, 1977538899, 3926827243, 974744351, 3871519817, 961111563, 954437177, 3791419407, 3765450781, 3739835469, 3714566311, 3689636335, 458129845, 910191745, 1808407283, 3593175255, 892460737, 3546811703, 3524075731, 875407347, 3479467177, 432197967, 3435973837, 3414632385, 3393554407, 3372735055, 3352169597, 1665926709, 827945503, 1645975491, 818089009, 1626496491, 3233857729, 3214946281, 799063683, 397222409, 789879043, 1570730897, 3123612579, 3105965051, 3088515809, 3071261531, 3054198967, 3037324939, 3020636341, 3004130131, 2987803337, 2971653049, 2955676419, 2939870663, 2924233053, 5817521841, 5786903305, 2878302691, 2863311531, 356059465, 354224107, 5638521169, 1402438301, 1395319325, 2776544515, 1381296015, 1374389535, 42735993, 5443126871, 2708156719, 2694881441, 1340867839, 333589693, 663956297, 1321528399, 2630410593, 5235769657, 2605477791, 1296593901, 2581013211, 1284476201, 1278501893, 5090331611, 2533436931, 5043631321, 5020601041, 4997780127, 2487582869, 4952755081, 4930545417, 2454267027, 2443359173, 4865095699, 605457945, 4822419421, 1200340205, 4780485339, 4759790597, 2369637129, 2359467013, 4698767641, 4678772885, 582368447, 2319644785, 1154949189, 2300233531, 2290649225, 285143057, 4543436479, 2262369605, 1126548799, 4487802563, 4469559463, 4451464081, 4433514629, 4415709349, 274877907, 2190262207, 4363141381, 2172947881, 4328785937, 2155905153};

// Division in constant time x/y
//   It assumes that only x is a sensible variable
//   It require that y < 256
uint32_t div_ct(uint32_t x, uint32_t y) {
    uint64_t inv = barrett_reduction_inv[y];
    uint8_t shift = barrett_reduction_shift[y];
    return (((uint64_t)(x) * inv) >> shift);
}

int get_sharing_info(sharing_info_t* info, uint32_t ring_size) {
    info->ring_size = ring_size;
    uint32_t root = sqrt_floor(ring_size, PARAM_SELECTOR_DEGREE);
    uint32_t current_size = 1;
    info->selector_bitsize = 0;
    for(uint32_t i=0; i<PARAM_SELECTOR_DEGREE-1; i++) {
        info->selector_side_size[i] = root;
        current_size *= root;
        info->selector_bitsize += root;
    }
    info->selector_side_size[PARAM_SELECTOR_DEGREE-1] = (
        (ring_size+current_size-1)/current_size
    );
    info->selector_bitsize += info->selector_side_size[PARAM_SELECTOR_DEGREE-1];
    info->selector_bitsize += PARAM_SELECTOR_DEGREE; // Position of the non-zero coordinates
    info->nb_additional_equations = info->selector_bitsize - PARAM_SELECTOR_DEGREE + 1;
    if(info->selector_side_size[PARAM_SELECTOR_DEGREE-1] > 256)
        return -1;
    if(info->selector_side_size[PARAM_SELECTOR_DEGREE-1] > PARAM_FIELD_SIZE)
        return -1;
    info->share_bitsize = sizeof(vec_wit_t)+sizeof(vec_unif_t)+info->selector_bitsize;
    return 0;
}

uint32_t get_share_bytesize(const sharing_info_t* info) {
    return info->share_bitsize;
}

uint32_t get_selector_bytesize(const sharing_info_t* info) {
    return info->selector_bitsize;
}

uint32_t get_selector_short_size(const sharing_info_t* info) {
    return info->selector_bitsize;
}

void sel_compress(uint8_t* buf, const vec_sel_t* sel, const sharing_info_t* info) {
    uint32_t bytesize = get_selector_bytesize(info);
    // No compression
    memcpy(buf, (uint8_t*) sel, bytesize);
}

void sel_decompress(vec_sel_t* sel, const uint8_t* buf, const sharing_info_t* info) {
    uint32_t bytesize = get_selector_bytesize(info);
    // No compression
    memcpy((uint8_t*) sel, buf, bytesize);
}

uint32_t get_challenge_bytesize(const sharing_info_t* info) {
    uint32_t nb_equations = PARAM_m + info->nb_additional_equations;
    return nb_equations*PARAM_eta;
}

void build_selector(vec_sel_t* sel, uint32_t position_in_ring, const sharing_info_t* info) {
    uint32_t pos_in_side[PARAM_SELECTOR_DEGREE];
    uint32_t remaining_position = position_in_ring;
    for(uint32_t i=0; i<PARAM_SELECTOR_DEGREE; i++) {
        uint32_t quotient = div_ct(remaining_position, info->selector_side_size[PARAM_SELECTOR_DEGREE-1-i]);
        uint32_t rest = remaining_position - info->selector_side_size[PARAM_SELECTOR_DEGREE-1-i]*quotient;
        pos_in_side[PARAM_SELECTOR_DEGREE-1-i] = rest;
        remaining_position = quotient;
    }
    uint8_t* sel_array = (uint8_t*) sel;
    memset(sel_array, 0, info->selector_bitsize);
    uint32_t offset = 0;
    for(uint32_t i=0; i<PARAM_SELECTOR_DEGREE; i++) {
        sel_array[offset + pos_in_side[i]] = 1;
        offset += info->selector_side_size[i];
    }
    for(uint32_t i=0; i<PARAM_SELECTOR_DEGREE; i++) {
        sel_array[offset] = pos_in_side[i];
        offset += 1;
    }
}

void parse_selector(uint8_t* sel_vec[PARAM_SELECTOR_DEGREE], uint32_t pos[PARAM_SELECTOR_DEGREE], vec_share_t const* share, const sharing_info_t* info) {
    uint8_t* sel_array = (uint8_t*) get_sel_const(share, info);
    uint32_t offset = 0;
    for(uint32_t i=0; i<PARAM_SELECTOR_DEGREE; i++) {
        sel_vec[i] = sel_array + offset;
        offset += info->selector_side_size[i];
    }
    for(uint32_t i=0; i<PARAM_SELECTOR_DEGREE; i++)
        pos[i] = sel_array[offset+i];
}
