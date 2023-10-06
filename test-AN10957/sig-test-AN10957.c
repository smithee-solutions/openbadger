/*
  sig-test-AN10957 - AN10957 CMAC signature Utility

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


#include <PCSC/wintypes.h>
#include <PCSC/pcsclite.h>
#include <PCSC/winscard.h>


#include <jansson.h>
#include <aes.h>


#include <ob-crypto.h>
#include <ob-pacs.h>
#include <openbadger-common.h>
#include <openbadger-version.h>
#include <ob-an10957.h>
OB_CONTEXT_AN10957 cardctx;
extern unsigned char secret_key_default [];
OB_PACS_AN10957 PACS_data;
extern unsigned char PACS_data_object_default [];
int selftest;


int
  initialize_sigutil
    (OB_CONTEXT **new_ctx,
    int argc,
    char *argv [])

{ /* initialize_sigutil */

  OB_CONTEXT *ctx;
  int done;
  int i;
  int longindex;
  extern struct option longopts [];
  char optstring [OB_STRING_MAX];
  int status;
  int status_opt;


  fprintf(LOG, "sigutil %s\n", OPENBADGER_VERSION);
  status = ob_initialize(new_ctx, OB_SYSTEM_SETTINGS_FILE);
  ctx = *new_ctx;
  ctx->pacs_data_format = OB_FORMAT_AN10957;
  ctx->an10957_ctx = &cardctx;
  if (status EQUALS STOB_SETTINGS_ERROR)
  {
    if (ctx->verbosity > 3)
      fprintf(LOG, "Error reading settings from %s\n", OB_SYSTEM_SETTINGS_FILE);
    status = ST_OK;
  };
  if (status EQUALS ST_OK)
  {
    status = ob_initialize(NULL, OB_LOCAL_SETTINGS_FILE);
    if (status EQUALS STOB_SETTINGS_ERROR)
    {
      if (ctx->verbosity > 3)
        fprintf(LOG, "Error reading settings from %s\n", OB_LOCAL_SETTINGS_FILE);
      status = ST_OK;
    };
  };

  ctx->tool_identifier = OB_TOOL_SIGUTIL;
  memset(&PACS_data, 0, sizeof(PACS_data));
fprintf(LOG, "DEBUG: pre-loading PACS_data\n");
memcpy(&PACS_data, PACS_data_object_default, sizeof(PACS_data));

  done = 0;
  while (!done)
  {
    status_opt = getopt_long (argc, argv, optstring, longopts, &longindex);
    switch (ctx->action)
    {
    case OB_NOOP:
      // stay silent if looping around after an option was found.
      break;

    case OB_SELFTEST:
      fprintf(LOG, "Self-test not implemented.\n");
      status = -1;
      break;

    case OB_SETTINGS:
      // user specified their own settings file.  pile it on top of the others

      status = ob_initialize(NULL, optarg);
      if (status != ST_OK)
        fprintf(LOG, "Error reading specified settings file (%s)\n", optarg);
      break;

    case OB_VERBOSITY:
      sscanf(optarg, "%d", &i);
      ctx->verbosity = i;
      break;

    case OB_HELP:
      fprintf(LOG, "--help - display this help text.\n");
      fprintf(LOG, "--verbosity (min 1 max 9)\n");
      fprintf(LOG, "--details <json file> - details for this calculation\n");
      fprintf(LOG, "--settings <json file> - configured settings for the tool\n");
      status = ST_OK;
      break;
    };
    ctx->action = OB_NOOP; // reset from whatever getopt_long set it to
    if ((status_opt EQUALS -1) || (status != ST_OK))
      done = 1;
  };
  return(status);

} /* initialize_sigutil */


int main (int argc, char *argv [])
{
  OB_CONTEXT *ctx;
  int status;


  status = initialize_sigutil(&ctx, argc, argv);
  ctx->an10957_ctx = &cardctx;
  if (ctx->verbosity > 3)
    fprintf(LOG, "PACS Data Object is %lu. bytes\n", sizeof(PACS_data));
  ob_display_PACS_data_object(ctx, (unsigned char *)&PACS_data);
  fprintf(LOG, 
"            UID: %s\n", string_hex_buffer(ctx, ctx->uid, ctx->uid_size));
  fprintf(LOG, 
"     Secret Key: %s\n", string_hex_buffer(ctx, ctx->secret_key, OB_AES128_KEY_SIZE));
  status = ob_diversify_AN10957(ctx);
  if (status != ST_OK)
    fprintf(LOG, "Error %d. in diversification\n", status);

  memset(ctx->iv, 0, sizeof(ctx->iv));
  memcpy(ctx->iv, ctx->uid, ctx->uid_size);
  ctx->iv [ctx->uid_size] = 0x80; // for padding of IV (apparently.)
  {
    int length;
    unsigned char tmp1 [1000];
    unsigned char tmp2 [1000];
    memset(tmp1, 0, sizeof(tmp1));
    memcpy(tmp1, (unsigned char *)&PACS_data, sizeof(PACS_data));
    //tmp1 [sizeof(PACS_data)] = 0x80; // for padding
int t1,t2,t3,t4;
t1 = sizeof(PACS_data);
t2 = sizeof(PACS_data) + OB_AES128_KEY_SIZE - 1;
t3 = t2 / OB_AES128_KEY_SIZE;
t4 = t3 * OB_AES128_KEY_SIZE;
fprintf(stderr, "t1 %d t2 %d t3 %d t4 %d\n", t1, t2, t3, t4);
    length = (((sizeof(PACS_data) + OB_AES128_KEY_SIZE - 1) / OB_AES128_KEY_SIZE)) * OB_AES128_KEY_SIZE;
fprintf(LOG, "(%d.)%s", length, ob_buffer_dump_string(ctx, tmp1, length, "Signature data: "));
  status = aes_encrypt(ctx, tmp1, tmp2, cardctx.diversified_key, &length);
  fprintf(LOG, "%s", ob_buffer_dump_string(ctx, tmp2, length, "Sig ciphertext: "));
  };
  return(status);

} /* main for sigutil */

