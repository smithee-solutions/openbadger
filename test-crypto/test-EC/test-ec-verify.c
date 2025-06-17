/*
  test-ec-verify - exercise EC signature verification

  Status codes are from /opt/openbadger/include/openbadger-common except
  for test-local status codes which are all negative.

  (C)Copyright 2024 Smithee Solutions LLC

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


#include <string.h>
#include <time.h>


#include <jansson.h>

#include <error.h>


#include <eac-encode.h>
extern void eac_log(char *msg);
#include <openbadger-common.h>
int read_parameters(EAC_ENCODE_CONTEXT *ctx);


int initialize_test
  (EAC_ENCODE_CONTEXT *ctx,
  EAC_ENCODE_OBJECT *key)

{ /* initialize_test */

  char log_message [2048];
  int status;


  status = ST_OK;
  memset(ctx, 0, sizeof(*ctx));
  ctx->verbosity = 3;
  ctx->eac_log = &eac_log;
  ctx->iterations = 1;
  status = read_parameters(ctx);

  if (status EQUALS ST_OK)
  {
    if (strlen(ctx->public_key_file) > 0)
    {
      strcpy(key->description, DESC_EC);
      key->key_parameters [0] = EAC_CRYPTO_EC;
      key->key_parameters [1] = EAC_KEY_EC_CURVE_SECP256R1;
      fprintf(stdout, "Using Curve SECP256R1\n");
      status = eac_crypto_pubkey_init_file(ctx, key, ctx->public_key_file);
      if (ctx->verbosity > 3)
      {
        if (strlen(key->description) > 0)
        {
          sprintf(log_message, "public key init status %d public key type is %s file %s\n",
            status, key->description, ctx->public_key_file);
          (*ctx->eac_log)(log_message);
        };
      };
    };
  };

  return(status);

} /* initialize_test */


int main
  (int argc,
  char *argv [])

{ /* main for test-ec-verify */

  EAC_ENCODE_CONTEXT crypto_context;
  size_t digestLen;
  struct timespec end_time;
  FILE *hashfile;
  unsigned char hash_buffer [8192];
  char hash_filename [1024];
  int i;
  EAC_ENCODE_OBJECT key;
  FILE *sigfile;
  unsigned char signature_buffer [8192];
  char signature_filename [1024];
  struct timespec start_time;
  int status;
  int status_io;


  fprintf(stdout, "Initializing CycloneSSL library for ECC Signature Verification\n");
  status = ST_OK;
  status = initialize_test(&crypto_context, &key);
  strcpy(hash_filename, "sha256-hash.bin");
  strcpy(signature_filename, "ec-sig.der");
  fprintf(stdout, "Reading SHA-256 hash from file %s\n", hash_filename);
  fprintf(stdout, "Reading signature from file %s\n", signature_filename);
  fprintf(stdout, "Reading Public Key (DER format) from file %s\n", crypto_context.public_key_file);

  if (status EQUALS ST_OK)
  {
    // read the hash from a file

    hashfile = fopen(hash_filename, "r");
    if (!hashfile)
      status = -1;
  };
  if (status EQUALS ST_OK)
  {
    status_io = fread(hash_buffer, sizeof(hash_buffer[1]), sizeof(hash_buffer), hashfile);
    digestLen = status_io;
    fprintf(stderr, "Hash length is %d.\n", (unsigned int)digestLen);
  };

  if (status EQUALS ST_OK)
  {
    // read the signature from a file

    sigfile = fopen(signature_filename, "r");
    status_io = fread(signature_buffer, sizeof(signature_buffer[1]), sizeof(signature_buffer), sigfile);
    if (status_io < 1)
      status = STOB_FILE_IO_3;
  };
  if (status EQUALS ST_OK)
  {
    fclose(sigfile);

    fprintf(stderr, "Reading %d, bytes from signature file\n", status_io);
    fprintf(stderr, "%d. iterations.\n", crypto_context.iterations);

    (void)clock_gettime(CLOCK_REALTIME, &start_time);
    printf("Start: %09ld %09ld\n", start_time.tv_sec, start_time.tv_nsec);

    for (i=0; (status EQUALS ST_OK) && (i<crypto_context.iterations); i++)
{
//fprintf(stderr, "DEBUG: stubbed out eac_signature_verify\n");
  status = eac_signature_verify(&crypto_context, &key, hash_buffer, digestLen, signature_buffer, status_io);
};

    (void)clock_gettime(CLOCK_REALTIME, &end_time);
    printf("  End: %09ld %09ld\n", end_time.tv_sec, end_time.tv_nsec);
    {
      long duration;
      int milli_start;
      int milli_end;
      int seconds;
      milli_start = start_time.tv_nsec/1000000;
      milli_end = end_time.tv_nsec/1000000;
      duration = milli_end - milli_start;
      seconds = end_time.tv_sec - start_time.tv_sec;
      if (seconds > 0)
        duration = duration + 1000*seconds;
      printf("Duration: %5ld mSec\n", duration);
    };
  };
  if (status != 0)
    fprintf(stderr, "status as %d\n", status);
  if (status EQUALS ST_OK)
    printf("signature was verified.\n");

  return(status);

} /* main for test-ec-verify */


/*
  read_parameters - reads program parameters from openbadger settings file.

  verbosity
  iterations
  public-key-file
*/
int read_parameters
  (EAC_ENCODE_CONTEXT *ctx)

{ /* read_parameters */

  int i;
  json_t *parameters;
  int status;
  json_error_t status_json;
  json_t *value;


  status = STOB_SETTINGS_FILE;
  parameters = json_load_file(OB_LOCAL_SETTINGS_FILE, 0, &status_json);
  if (parameters != NULL)
  {
    status = ST_OK;
    value = json_object_get(parameters, "iterations");
    if (json_is_string(value))
    {
      sscanf(json_string_value(value), "%d", &i);
      ctx->iterations = i;
    };
    if (status EQUALS ST_OK)
    {
      status = STOB_SETTINGS_ERROR;
      value = json_object_get(parameters, "verbosity");
      if (json_is_string(value))
      {
        sscanf(json_string_value(value), "%d", &i);
        ctx->verbosity = i;
        status = ST_OK;
      };
    };
    if (status EQUALS ST_OK)
    {
      value = json_object_get(parameters, "public-key-file");
      if (json_is_string(value))
        strcpy(ctx->public_key_file, json_string_value(value));
    };
  };
  return(status);

} /* read_parameters */

