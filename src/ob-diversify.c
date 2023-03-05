/*
  ob-diversify - do the diversification calculation

  (C)Copyright 2017-2023 Smithee Solutions LLC

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/


#include <stdio.h>
#include <getopt.h>
#include <string.h>


#include <openbadger-an10957.h>
#include <openbadger.h>
extern unsigned char xor_K1 [];
extern unsigned char xor_K2 [];


#if 0

#include <jansson.h>
#include <aes.h>


#include <openbadger-version.h>
extern unsigned char secret_key_default [];
extern unsigned char uid_default [];
extern int uid_default_size;
extern unsigned char expected_K0 [];
#endif


int
  diversify_AN10957
    (OB_CONTEXT *ctx)

{ /* main for divutil */

  unsigned char div_encrypted [2*OB_KEY_SIZE_10957];
  unsigned char div_input [2*OB_KEY_SIZE_10957];
  unsigned char div_input_2 [2*OB_KEY_SIZE_10957];
  unsigned char div_input_new [2*OB_KEY_SIZE_10957];
  int encrypted_length;
  unsigned char K0 [OB_KEY_SIZE_10957];
  unsigned char K0_plaintext [OB_KEY_SIZE_10957];
  unsigned char K1 [OB_KEY_SIZE_10957];
  unsigned char K1_new [OB_KEY_SIZE_10957];
  unsigned char K2 [OB_KEY_SIZE_10957];
  unsigned char K2_new [OB_KEY_SIZE_10957];
  int status;


    fprintf(LOG, "---> Step 1 Generate K0 <---\n");
    memset(K0_plaintext, 0, sizeof(K0_plaintext));
    encrypted_length = sizeof(K0_plaintext);
    status = aes_encrypt(ctx, K0_plaintext, K0, ctx->secret_key, &encrypted_length);
    if (status != ST_OK)
      fprintf(LOG, "encrypt K0 failed.\n");

  if (status EQUALS ST_OK)
  {
    fprintf(LOG, "        K0: %s\n", string_hex_buffer(ctx, K0, OB_KEY_SIZE_10957));

    fprintf(LOG, "---> Step 1 Generate K1 <---\n");

    /*
      if the most significant bit of K0 is a 1, K1 is K0 shifted left by 1 bit.

      if the MSB of K0 is a 0, K1 is K0 << 1 xor'd with 0x00...87
    */ 
    array_shift_left(ctx, K0, K1);
    memcpy(K1_new, K1, sizeof(K1_new));
    if (0x80 & K0 [0])
    {
      fprintf(LOG, "K0 MSB was 1\n");
      array_xor(ctx, K1_new, K1, xor_K1, OB_KEY_SIZE_10957);
    }
    else
    {
      fprintf(LOG, "K0 MSB was 0\n");
    };
    memcpy(K1, K1_new, sizeof(K1));

    fprintf(LOG, "        K1: %s\n", string_hex_buffer(ctx, K1, OB_KEY_SIZE_10957));
  };
  if (status EQUALS ST_OK)
  {
    fprintf(LOG, "---> Step 2 Create DIV Input <---\n");

    if (ctx->verbosity > 3)
      fprintf(LOG, "%s", buffer_dump_string(ctx, K1, OB_KEY_SIZE_10957, "K1 input to step 2:"));
    /*
      if the most significant bit of K1 is a 1, K2 is K1 shifted left by 1 bit.

      if the MSB of K1 is a 0, K2 is K1 << 1 xor'd with 0x00...87
    */ 
    array_shift_left(ctx, K1, K2);
    memcpy(K2_new, K2, sizeof(K2_new));
    if (0x80 & K1 [0])
    {
      fprintf(LOG, "K1 MSB was 1\n");
      array_xor(ctx, K2_new, K2, xor_K2, OB_KEY_SIZE_10957);
    }
    else
    {
      fprintf(LOG, "K2 MSB was 0\n");
    };
    memcpy(K2, K2_new, sizeof(K2));

    fprintf(LOG, "        K2: %s\n", string_hex_buffer(ctx, K2, OB_KEY_SIZE_10957));
  };
  if (status EQUALS ST_OK)
  {
    fprintf(LOG, "---> Step 1 Generate K2 <---\n");

    memset(div_input, 0, sizeof(div_input));
    div_input [0] = 0x01;
    memcpy(div_input+1, ctx->uid, ctx->uid_size);
    div_input [1+ctx->uid_size] = 0x80; // padding start
    fprintf(LOG, "%s", buffer_dump_string(ctx, div_input, sizeof(div_input), "DIV Input "));
  };
  if (status EQUALS ST_OK)
  {
    fprintf(LOG, "---> Step 3 XOR DIV and K2 <---\n");

    memset(div_input_2, 0, sizeof(div_input_2));
    memcpy(div_input_2+OB_KEY_SIZE_10957, K2, OB_KEY_SIZE_10957);
    array_xor(ctx, div_input_new, div_input, div_input_2, 2*OB_KEY_SIZE_10957);
    fprintf(LOG, "%s", buffer_dump_string(ctx, div_input_new, 2*OB_KEY_SIZE_10957, "DIV xor'd with K2 "));
  };
  if (status EQUALS ST_OK)
  {
    fprintf(LOG, "---> Step 4 Encrypt DIV string with Secret Key <---\n");
    encrypted_length = 2*OB_KEY_SIZE_10957;
    status = aes_encrypt(ctx, div_input_new, div_encrypted, ctx->secret_key, &encrypted_length);
    if (status EQUALS ST_OK)
      fprintf(LOG, "%s", buffer_dump_string(ctx, div_encrypted, 2*OB_KEY_SIZE_10957, "DIV encrypted "));
  };
  if (status EQUALS ST_OK)
    fprintf(LOG, "%s", buffer_dump_string(ctx, div_encrypted+OB_KEY_SIZE_10957, OB_KEY_SIZE_10957, "Diversified key: "));

  return(status);

} /* main for divutil */

