#ifndef SELECTOR_H
#define SELECTOR_H

#include "mpc.h"
#include "parameters-all.h"

void parse_selector(uint8_t* sel_vec[PARAM_SELECTOR_DEGREE], uint32_t pos[PARAM_SELECTOR_DEGREE], vec_share_t const* share, const sharing_info_t* info);

#endif /* SELECTOR_H */
