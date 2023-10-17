// -----------------------------------------------------------------------------
// File Name   : aim192.c
// Description : 
// SPDX-License-Identifier: MIT
// -----------------------------------------------------------------------------

#include "aim192.h"
#include "sample.h"
#include "portable_endian.h"

#include "aim_common.c.i"

void mersenne_exp_5(const GF in, GF out);
void mersenne_exp_7(const GF in, GF out);
void mersenne_exp_29(const GF in, GF out);

void mersenne_exp_5(const GF in, GF out)
{
  GF t1 = {0,};
  GF t2 = {0,};

  // t2 = a ^ (2^2 - 1)
  GF_sqr(in, t1);
  GF_mul(t1, in, t2);

  // t1 = a ^ (2^4 - 1)
  GF_sqr(t2, t1);
  GF_sqr(t1, t1);
  GF_mul(t1, t2, t1);

  // out = a ^ (2^5 - 1)
  GF_sqr(t1, t1);
  GF_mul(t1, in, out);
}

void mersenne_exp_7(const GF in, GF out)
{
  GF t1 = {0,};
  GF t2 = {0,};

  // t1 = a ^ (2^2 - 1)
  GF_sqr(in, t1);
  GF_mul(t1, in, t1);

  // t2 = a ^ (2^3 - 1)
  GF_sqr(t1, t1);
  GF_mul(t1, in, t2);

  // t1 = a ^ (2^6 - 1)
  GF_sqr(t2, t1);
  GF_sqr(t1, t1);
  GF_sqr(t1, t1);
  GF_mul(t1, t2, t1);

  // out = a ^ (2^7 - 1)
  GF_sqr(t1, t1);
  GF_mul(t1, in, out);
}

void mersenne_exp_29(const GF in, GF out)
{
  int i;
  GF t1 = {0,};
  GF t2 = {0,};

  // t1 = a ^ (2^2 - 1)
  GF_sqr(in, t1);
  GF_mul(t1, in, t1);

  // t2 = a ^ (2^3 - 1)
  GF_sqr(t1, t1);
  GF_mul(t1, in, t2);

  // t2 = a ^ (2^6 - 1)
  GF_sqr(t2, t1);
  GF_sqr(t1, t1);
  GF_sqr(t1, t1);
  GF_mul(t1, t2, t2);

  // t2 = a ^ (2^7 - 1)
  GF_sqr(t2, t1);
  GF_mul(t1, in, t2);

  // t2 = a ^ (2^14 - 1)
  GF_sqr(t2, t1);
  for (i = 1; i < 7; i++)
  {
    GF_sqr(t1, t1);
  }
  GF_mul(t1, t2, t2);

  // t2 = a ^ (2^28 - 1)
  GF_sqr(t2, t1);
  for (i = 1; i < 14; i++)
  {
    GF_sqr(t1, t1);
  }
  GF_mul(t1, t2, t2);

  // out = a ^ (2^29 - 1)
  GF_sqr(t2, t1);
  GF_mul(t1, in, out);
}

void generate_matrices_L_and_U(const uint8_t* iv, GF** matrix_A, GF vector_b)
{
  size_t GF_byte_size = sizeof(GF);
  size_t squeeze_len;
  size_t block_index;

  prg_context entropy_ctx;
  samplable_t entropy = prg_to_samplable(&entropy_ctx);
  prg_init(&entropy_ctx, iv, NULL);

  uint8_t* out = malloc(TAPE_LEN);
  uint8_t* out_ptr;

  for (size_t num = 0; num < NUM_INPUT_SBOX; num++)
  {
    byte_sample(&entropy, out, TAPE_LEN);

    // Lower
    matrix_A[2 * num] = calloc(NUMBITS_FIELD, GF_byte_size);

    squeeze_len = 8;
    block_index = 0;

    out_ptr = out;
    for (size_t row = 0; row < NUMBITS_FIELD; row++)
    {
      uint64_t tmp = 0;
      memcpy((uint8_t *)(&tmp) + (8 - squeeze_len), out_ptr, squeeze_len);
      out_ptr += squeeze_len;
      matrix_A[2 * num][row][block_index] =
        (le64toh(tmp) & and_mask_lower[row & 0x3f]) | or_mask[row & 0x3f];
      for (size_t i = block_index + 1; i < NUMWORDS_FIELD; i++)
      {
        memcpy((uint8_t *)(&tmp), out_ptr, 8);
        out_ptr += 8;
        matrix_A[2 * num][row][i] = le64toh(tmp);
      }
      if ((row & 7) == 6)
      {
        squeeze_len--;
      }
      if ((row & 0x3f) == 0x3f)
      {
        block_index++;
        squeeze_len = 8;
      }
    }

    // Upper
    matrix_A[2 * num + 1] = calloc(NUMBITS_FIELD, GF_byte_size);

    squeeze_len = 0;
    block_index = 0;
    for (size_t row = 0; row < NUMBITS_FIELD; row++)
    {
      uint64_t tmp = 0;
      for (size_t i = 0; i < block_index; i++)
      {
        memcpy((uint8_t *)(&tmp), out_ptr, 8);
        out_ptr += 8;
        matrix_A[2 * num + 1][row][i] = le64toh(tmp);
      }
      memcpy((uint8_t *)(&tmp), out_ptr, squeeze_len);
      out_ptr += squeeze_len;
      matrix_A[2 * num + 1][row][block_index] =
        (le64toh(tmp) & and_mask_upper[row & 0x3f]) | or_mask[row & 0x3f];

      if ((row & 7) == 0)
      {
        squeeze_len++;
      }
      if ((row & 0x3f) == 0x3f)
      {
        block_index++;
        squeeze_len = 0;
      }
    }
  }

  // generate vector B
  byte_sample(&entropy, (uint8_t*)vector_b, BLOCK_SIZE);

  free(out);
}

void generate_matrix_LU(const uint8_t* iv, GF** matrix_A, GF vector_b)
{
  size_t GF_byte_size = sizeof(GF);
  GF** temp_matrix = malloc(2 * NUM_INPUT_SBOX * sizeof(GF*));

  generate_matrices_L_and_U(iv, temp_matrix, vector_b);

  for (size_t i = 0; i < NUM_INPUT_SBOX; i++)
  {
    matrix_A[i] = malloc(NUMBITS_FIELD * GF_byte_size);
    for (size_t j = 0; j < NUMBITS_FIELD; j++)
    {
      GF_transposed_matmul(temp_matrix[2 * i + 1][j], (const GF2_192*) temp_matrix[2 * i],
                           matrix_A[i][j]);
    }
    free(temp_matrix[2 * i]);
    free(temp_matrix[2 * i + 1]);
  }
  free(temp_matrix);
}

void compute_sbox_outputs(const uint8_t* pt, GF* sbox_outputs)
{
  GF pt_GF = {0,};

  GF_from_bytes(pt, pt_GF);

  // Mersenne layer
  mersenne_exp_5(pt_GF, sbox_outputs[0]);
  mersenne_exp_29(pt_GF, sbox_outputs[1]);
}

void aim(const uint8_t* pt, const uint8_t* iv, uint8_t* ct)
{
  GF state[NUM_INPUT_SBOX] = {0,};
  GF pt_GF = {0,};
  GF vector_b = {0,};
  GF **matrix_A = malloc(2 * NUM_INPUT_SBOX * sizeof(GF*));

  GF_from_bytes(pt, pt_GF);

  // Mersenne layer
  mersenne_exp_5(pt_GF, state[0]);
  mersenne_exp_29(pt_GF, state[1]);

  // generate linear layer
  generate_matrices_L_and_U(iv, matrix_A, vector_b);

  // linear layer
  GF_transposed_matmul(state[0], (const GF2_192*) matrix_A[1], state[0]);
  GF_transposed_matmul(state[0], (const GF2_192*) matrix_A[0], state[0]);

  GF_transposed_matmul(state[1], (const GF2_192*) matrix_A[3], state[1]);
  GF_transposed_matmul(state[1], (const GF2_192*) matrix_A[2], state[1]);

  GF_add(state[0], state[1], state[0]);
  GF_add(state[0], vector_b, state[0]);

  // Mersenne layer
  mersenne_exp_7(state[0], state[0]);

  // feedback
  GF_add(state[0], pt_GF, state[0]);

  // ct conversion
  GF_to_bytes(state[0], ct);

  for (size_t i = 0; i < 2 * NUM_INPUT_SBOX; i++)
  {
    free(matrix_A[i]);
  }
  free(matrix_A);
}

void aim_mpc(const uint8_t* pt, const GF** matrix_A,
             const GF vector_b, const uint8_t* ct,
             const size_t num_parties, GF *z_shares, GF *x_shares)
{
  GF state[NUM_INPUT_SBOX] = {0,};
  GF ct_GF = {0,};
  GF temp = {0,};
  size_t party_index;

  GF_from_bytes(ct, ct_GF);

  for (size_t party = 0; party < num_parties; party++)
  {
    party_index = party * (NUM_INPUT_SBOX + 1);

    GF_from_bytes(pt + party * BLOCK_SIZE, x_shares[party_index]);

    // linear layer
    GF_transposed_matmul(z_shares[party_index], (const GF2_192*) matrix_A[0], state[0]);
    GF_transposed_matmul(z_shares[party_index + 1], (const GF2_192*) matrix_A[1], state[1]);
    if (party == 0)
    {
      GF_add(state[1], vector_b, state[1]);
    }
    GF_add(state[0], state[1], x_shares[party_index + 2]);

    // optimization B.1.2 in eprint 2022/588
    GF_copy(z_shares[party_index], state[0]);
    GF_copy(z_shares[party_index + 1], state[1]);

    GF_sqr(x_shares[party_index], temp);
    GF_sqr(temp, temp);
    GF_sqr(temp, temp);
    GF_sqr(temp, temp);
    GF_sqr(temp, z_shares[party_index]);

    GF_transposed_matmul(x_shares[party_index], (const GF2_192*) e2_power_matrix,
                         z_shares[party_index + 1]);

    GF_copy(state[0], x_shares[party_index]);
    GF_copy(state[1], x_shares[party_index + 1]);

    GF_sqr(x_shares[party_index + 2], temp);
    GF_sqr(temp, temp);
    GF_sqr(temp, temp);
    GF_sqr(temp, temp);
    GF_sqr(temp, temp);
    GF_sqr(temp, temp);
    GF_sqr(temp, temp);

    GF_mul(ct_GF, x_shares[party_index + 2], state[0]);
    GF_add(temp, state[0], z_shares[party_index + 2]);
  }
}

void aim_mpc_(const GF pt_share, const GF* t_share,
              GF const* const* matrix_A, const GF vector_b, const GF ct,
              GF* z_share, GF* x_share, char has_sharing_offset)
{
  GF state[NUM_INPUT_SBOX] = {0,};
  GF temp = {0,};

  GF_copy(pt_share, x_share[0]);

  // linear layer
  GF_transposed_matmul(t_share[0], (const GF2_192*) matrix_A[0], state[0]);
  GF_transposed_matmul(t_share[1], (const GF2_192*) matrix_A[1], state[1]);
  if (has_sharing_offset)
  {
    GF_add(state[1], vector_b, state[1]);
  }
  GF_add(state[0], state[1], x_share[2]);

  // optimization B.1.2 in eprint 2022/588
  GF_copy(t_share[0], state[0]);
  GF_copy(t_share[1], state[1]);

  GF_sqr(x_share[0], temp);
  GF_sqr(temp, temp);
  GF_sqr(temp, temp);
  GF_sqr(temp, temp);
  GF_sqr(temp, z_share[0]);

  GF_transposed_matmul(x_share[0], (const GF2_192*) e2_power_matrix,
                        z_share[1]);

  GF_copy(state[0], x_share[0]);
  GF_copy(state[1], x_share[1]);

  GF_sqr(x_share[2], temp);
  GF_sqr(temp, temp);
  GF_sqr(temp, temp);
  GF_sqr(temp, temp);
  GF_sqr(temp, temp);
  GF_sqr(temp, temp);
  GF_sqr(temp, temp);

  GF_mul(ct, x_share[2], state[0]);
  GF_add(temp, state[0], z_share[2]);
}
