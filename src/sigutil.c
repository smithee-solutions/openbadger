/*
  sigutil - AN10957 CMAC signature Utility

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


#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include <jansson.h>
#include <aes.h>


#pragma pack(1)
#include <openbadger-an10957.h>
#include <openbadger.h>
#include <openbadger-version.h>
extern unsigned char secret_key_default [];
OB_CONTEXT sigutil_context;
OB_PACS_DATA_OBJECT PACS_data;
extern unsigned char PACS_data_object_default [];
int selftest;


int
  initialize_sigutil
    (OB_CONTEXT *ctx,
    int argc,
    char *argv [])

{ /* initialize_sigutil */

  int done;
  int found_something;
  int i;
  int longindex;
  struct option
    longopts [] = {
      {"details", required_argument, &(sigutil_context.action), OB_DETAILS},
      {"help", 0, &(sigutil_context.action), OB_HELP},
      {"selftest", 0, &(sigutil_context.action), OB_SELFTEST},
      {"settings", required_argument, &(sigutil_context.action), OB_SETTINGS},
      {"verbosity", required_argument, &(sigutil_context.action), OB_VERBOSITY},
      {0, 0, 0, 0}
    };
  char optstring [OB_STRING_MAX];
  json_t *settings;
  char settings_filename [OB_STRING_MAX];
  int status;
  json_error_t status_json;
  int status_opt;
  int uid_length;
  json_t *value;


  status = ST_OK;
  fprintf(LOG, "divutil %s\n", OPENBADGER_VERSION);
  memset(ctx, 0, sizeof(*ctx));
  memset(&PACS_data, 0, sizeof(PACS_data));

  ctx->verbosity = 3;

  /*
    parameters we can likely initailize once.
  */
  ctx->uid_size = OB_UID_SIZE;

  strcpy(settings_filename, OB_SETTINGS_FILE_DEFAULT);
  settings = json_load_file(settings_filename, 0, &status_json);
  if (settings != NULL)
  {
    value = json_object_get(settings, "verbosity");
    if (json_is_string(value))
    {
      sscanf(json_string_value(value), "%d", &i);
      ctx->verbosity = i;
    };
    if (ctx->verbosity > 3)
      fprintf(LOG, "sigutil: settings file %s loaded.\n", OB_SETTINGS_FILE_DEFAULT);

    value = json_object_get(settings, "UID");
    if (json_is_string(value))
    {
      memcpy(ctx->uid, string_buffer_hex(ctx, json_string_value(value), &uid_length), sizeof(ctx->uid));
    };
  };
  if (ctx->verbosity > 2)
  {
    fprintf(LOG, "Settings file %s\n", settings_filename);
  };

  found_something = 0;
  done = 0;
  while (!done)
  {
    status_opt = getopt_long (argc, argv, optstring, longopts, &longindex);
    if (!found_something)
      if (status_opt EQUALS -1)
        ctx->action = OB_HELP;
    switch (ctx->action)
    {
    case OB_DETAILS:
fprintf(stderr, "DEBUG: json load as details\n");
// load value "uid", infer length from decode of hex
// load value "secret-key", must be 128 bit value
      status = ST_OK;
      found_something = 1;
      break;

    case OB_NOOP:
      // stay silent if looping around after an option was found.
      break;

    case OB_SELFTEST:
      fprintf(LOG, "Self-test selected, loading parameters.\n");
      memcpy(ctx->secret_key, &secret_key_default, OB_KEY_SIZE_10957);
      memcpy(&PACS_data, PACS_data_object_default, sizeof(PACS_data));
      status = ST_OK;
      selftest = 1;
      found_something = 1;
      break;

    case OB_SETTINGS:
      status = ST_OK;
      found_something = 1;
      break;

    case OB_VERBOSITY:
      found_something = 1;
      sscanf(optarg, "%d", &i);
      ctx->verbosity = i;
      break;

    case OB_HELP:
      found_something = 1;
      // fall through to default on purpose
    default:
      fprintf(LOG, "--help - display this help text.\n");
      fprintf(LOG, "--verbosity (min 1 max 9)\n");
      fprintf(LOG, "--details <json file> - details for this calculation\n");
      fprintf(LOG, "--settings <json file> - configured settings for the tool\n");
      fprintf(LOG, "--selftest - use the values in AN10957\n");
      status = STOB_NO_ARGUMENTS;
      break;
    };
    ctx->action = OB_NOOP; // reset from whatever getopt_long set it to
    if (status_opt EQUALS -1)
      done = 1;
  };
  return(status);

} /* initialize_sigutil */


int main (int argc, char *argv [])
{
  OB_CONTEXT *ctx;
  int status;


  status = ST_OK;
  ctx = &sigutil_context;
  status = initialize_sigutil(ctx, argc, argv);
if (selftest)
{
  fprintf(stderr, "DEBUG: selftest selected\n");
};
  fprintf(LOG, "PACS Data Object is %lu. bytes\n", sizeof(PACS_data));
  display_PACS_data_object(ctx, &PACS_data);
  fprintf(LOG, "       UID: %s\n", string_hex_buffer(ctx, ctx->uid, ctx->uid_size));
  return(status);

} /* main for sigutil */



#if 0
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
  int encrypted_length;
  int i;
  unsigned char K0 [OB_KEY_SIZE_10957];
  unsigned char K0_plaintext [OB_KEY_SIZE_10957];
  unsigned char K1 [OB_KEY_SIZE_10957];
  unsigned char K1_new [OB_KEY_SIZE_10957];
  unsigned char K2 [OB_KEY_SIZE_10957];
  unsigned char K2_new [OB_KEY_SIZE_10957];
  int status;


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

