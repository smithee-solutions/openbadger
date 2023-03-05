/*
  ob-init - common initialization routines


  (C)Copyright 2023 Smithee Solutions LLC

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


//#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include <jansson.h>


#include <openbadger-an10957.h>
#include <openbadger.h>
OB_CONTEXT openbadger_context;
struct option
  longopts [] = {
    {"help", 0, &(openbadger_context.action), OB_HELP},
    {"selftest", 0, &(openbadger_context.action), OB_SELFTEST},
    {"settings", required_argument, &(openbadger_context.action), OB_SETTINGS},
    {"verbosity", required_argument, &(openbadger_context.action), OB_VERBOSITY},
    {0, 0, 0, 0}
  };


int
  openbadger_initialize
    (OB_CONTEXT **initialized_context,
    char *settings_filename)

{ /* openbadger_initialize */

  OB_CONTEXT *ctx;
  int i;
  json_t *settings;
  int status;
  json_error_t status_json;
  json_t *value;


  status = ST_OK;

  ctx = &openbadger_context;
  if (initialized_context != NULL)
  {
    *initialized_context = &openbadger_context;
    memset(ctx, 0, sizeof(*ctx));

    // default program settings
    ctx->verbosity = 3;

    // default credential settinga

    ctx->uid_size = OB_UID_SIZE;
  };

  // read the designated settings file

  settings = json_load_file(settings_filename, 0, &status_json);
  if (settings EQUALS NULL)
  {
    if (ctx->verbosity > 3)
      fprintf(LOG, "Can't read settings file %s (error %s)\n",
        settings_filename, status_json.text);
    status = STOB_SETTINGS_ERROR;
  };

  if ((status EQUALS ST_OK) && (settings != NULL))
  {
    value = json_object_get(settings, "verbosity");
    if (json_is_string(value))
    {
      sscanf(json_string_value(value), "%d", &i);
      ctx->verbosity = i;
    };

    if (ctx->verbosity > 3)
      fprintf(LOG, "settings file %s loaded.\n", settings_filename);

    value = json_object_get(settings, "UID");
    if (json_is_string(value))
    {
      int uid_length;

      memcpy(ctx->uid, string_buffer_hex(ctx, json_string_value(value), &uid_length), sizeof(ctx->uid));
    };
  };

  return(status);

} /* openbadger_initialize */

#if 0

#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include <jansson.h>
#include <aes.h>


#include <openbadger-version.h>
extern unsigned char secret_key_default [];
extern unsigned char uid_default [];
extern int uid_default_size;
extern unsigned char expected_K0 [];
extern unsigned char xor_K1 [];
extern unsigned char xor_K2 [];


int
  aes_encrypt
    (OB_CONTEXT *ctx,
    unsigned char *plaintext,
    unsigned char *ciphertext,
    unsigned char *key,
    int *length)

{ /* aes_encrypt */

  struct AES_ctx crypto_context;


  if (ctx->verbosity > 3)
    fprintf(LOG, "%s", buffer_dump_string(ctx, plaintext, *length, "encrypt: plaintext input: "));
  memcpy(ciphertext, plaintext, *length);
  AES_init_ctx_iv(&crypto_context, ctx->secret_key, ctx->iv);
  AES_CBC_encrypt_buffer(&crypto_context, ciphertext, *length);
  if (ctx->verbosity > 3)
    fprintf(LOG, "%s", buffer_dump_string(ctx, ciphertext, *length, "encrypt: ciphertext output: "));
  return(ST_OK);

} /* aes_encrypt */


int
  ob_init
    (OB_CONTEXT *ctx)

{ /* ob_init */

  char settings_filename [1024];
  int status;


  status = ST_OK;

  ctx->tool_identifier = OB_TOOL_DIVUTIL;
  strcpy(settings_filename, OB_SETTINGS_FILE_DEFAULT);
  
  return(status);

} /* ob_init */



int
  main
    (int argc,
    char *argv [])

{ /* main for divutil */

  int comparison;
  OB_CONTEXT *ctx;
  unsigned char div_encrypted [2*OB_KEY_SIZE_10957];
  unsigned char div_input [2*OB_KEY_SIZE_10957];
  unsigned char div_input_2 [2*OB_KEY_SIZE_10957];
  unsigned char div_input_new [2*OB_KEY_SIZE_10957];
  OB_CONTEXT divutil_context;
  int done;
  int encrypted_length;
  int found_something;
  int i;
  unsigned char K0 [OB_KEY_SIZE_10957];
  unsigned char K0_plaintext [OB_KEY_SIZE_10957];
  unsigned char K1 [OB_KEY_SIZE_10957];
  unsigned char K1_new [OB_KEY_SIZE_10957];
  unsigned char K2 [OB_KEY_SIZE_10957];
  unsigned char K2_new [OB_KEY_SIZE_10957];
  int longindex;
  char optstring [OB_STRING_MAX];
  int selftest;
  int status;
  int status_opt;
  struct option
    longopts [] = {
      {"details", required_argument, &(divutil_context.action), OB_DETAILS},
      {"help", 0, &(divutil_context.action), OB_HELP},
      {"selftest", 0, &(divutil_context.action), OB_SELFTEST},
      {"settings", required_argument, &(divutil_context.action), OB_SETTINGS},
      {"verbosity", required_argument, &(divutil_context.action), OB_VERBOSITY},
      {0, 0, 0, 0}
    };


  ctx = &divutil_context;
  memset(ctx, 0, sizeof(*ctx));
  status = ST_OK;
  if (status EQUALS ST_OK)
  {
    if (ctx->verbosity > 3)
      fprintf(LOG, " Verbosity: %d\n", ctx->verbosity);

    if (selftest)
    {
      ctx->uid_size = uid_default_size;
      memcpy(ctx->uid, uid_default, uid_default_size);
    };

    fprintf(LOG, "Secret Key: %s\n", string_hex_buffer(ctx, ctx->secret_key, OB_KEY_SIZE_10957));
    fprintf(LOG, "       UID: %s\n", string_hex_buffer(ctx, ctx->uid, ctx->uid_size));

    fprintf(LOG, "---> Step 1 Generate K0 <---\n");
    memset(K0_plaintext, 0, sizeof(K0_plaintext));
    encrypted_length = sizeof(K0_plaintext);
    status = aes_encrypt(ctx, K0_plaintext, K0, ctx->secret_key, &encrypted_length);
    if (status != ST_OK)
      fprintf(LOG, "encrypt K0 failed.\n");
  };
  if (status EQUALS ST_OK)
  {
    if (selftest)
    {
      comparison = memcmp(expected_K0, K0, OB_KEY_SIZE_10957);
      if (ctx->verbosity > 9)
        fprintf(stderr, "K0 compare test returned %d.\n", comparison);
      if (comparison != 0)
        fprintf(LOG, "K0 mismatch:\n    Calc %s\nExpected %s\n",
          string_hex_buffer(ctx, K0, sizeof(K0)),
          string_hex_buffer(ctx, expected_K0, sizeof(K0)));
      else
        fprintf(LOG, "K0 selftest passed.\n");

    };
    fprintf(LOG, "        K0: %s\n", string_hex_buffer(ctx, K0, OB_KEY_SIZE_10957));
  };
  if (status EQUALS ST_OK)
  {
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
    memcpy(div_input+1, uid_default, uid_default_size);
    div_input [1+uid_default_size] = 0x80; // padding start
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

  if (status != 0)
    fprintf(LOG, "divutil exit status %d.\n", status);

  return(status);

} /* main for divutil */

#endif

