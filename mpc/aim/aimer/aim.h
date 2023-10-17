// -----------------------------------------------------------------------------
// File Name   : aim.h
// Description : 
// SPDX-License-Identifier: MIT
// -----------------------------------------------------------------------------

#ifndef AIM_H
#define AIM_H

//#include "aimer.h"

#if       PARAM_SECURITY == 128
  #include "aim128.h"

#elif     PARAM_SECURITY == 192
  #include "aim192.h"

#elif     PARAM_SECURITY == 256
  #include "aim256.h"

#endif

typedef struct aim_params_t
{
  uint32_t block_size;
  uint32_t num_input_sboxes;
} aim_params_t;

#endif // AIM_H
