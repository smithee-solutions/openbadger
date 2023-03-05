/*
  ob-crypto - cryptography (encrypt etc.) routines


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

//#include <jansson.h>
#include <aes.h>


#include <openbadger-an10957.h>
#include <openbadger.h>


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

