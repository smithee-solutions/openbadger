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


#include <openbadger-an10957.h>
#include <openbadger.h>
#include <openbadger-version.h>
extern unsigned char secret_key_default [];
OB_PACS_DATA_OBJECT PACS_data;
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
  int found_something;
  int i;
  int longindex;
  extern struct option longopts [];
  char optstring [OB_STRING_MAX];
  int status;
  int status_opt;


  status = openbadger_initialize(new_ctx, OB_SYSTEM_SETTINGS_FILE);
  ctx = *new_ctx;
  if (status EQUALS STOB_SETTINGS_ERROR)
  {
    if (ctx->verbosity > 3)
      fprintf(LOG, "Error reading settings from %s\n", OB_SYSTEM_SETTINGS_FILE);
    status = ST_OK;
  };
  if (status EQUALS ST_OK)
  {
    status = openbadger_initialize(NULL, OB_LOCAL_SETTINGS_FILE);
    if (status EQUALS STOB_SETTINGS_ERROR)
    {
      if (ctx->verbosity > 3)
        fprintf(LOG, "Error reading settings from %s\n", OB_LOCAL_SETTINGS_FILE);
      status = ST_OK;
    };
  };

  ctx->tool_identifier = OB_TOOL_SIGUTIL;
  fprintf(LOG, "sigutil %s\n", OPENBADGER_VERSION);
  memset(&PACS_data, 0, sizeof(PACS_data));
fprintf(LOG, "DEBUG: pre-loading PACS_data\n");
memcpy(&PACS_data, PACS_data_object_default, sizeof(PACS_data));

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
      found_something = 1;
      // user specified their own settings file.  pile it on top of the others

      status = openbadger_initialize(NULL, optarg);
      if (status != ST_OK)
        fprintf(LOG, "Error reading specified settings file (%s)\n", optarg);
      break;

    case OB_VERBOSITY:
      found_something = 1;
      sscanf(optarg, "%d", &i);
      ctx->verbosity = i;
      break;

    case OB_HELP:
      found_something = 1;
      fprintf(LOG, "--help - display this help text.\n");
      fprintf(LOG, "--verbosity (min 1 max 9)\n");
      fprintf(LOG, "--details <json file> - details for this calculation\n");
      fprintf(LOG, "--settings <json file> - configured settings for the tool\n");
      fprintf(LOG, "--selftest - use the values in AN10957\n");
      status = ST_OK;
      break;

    default:
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


  status = initialize_sigutil(&ctx, argc, argv);
  if (status EQUALS STOB_NO_ARGUMENTS)
  {
    fprintf(LOG, "Signature Utility is in startup.\n");
    status = ST_OK;
  };
  fprintf(LOG, "PACS Data Object is %lu. bytes\n", sizeof(PACS_data));
  display_PACS_data_object(ctx, &PACS_data);
  fprintf(LOG, "Secret Key: %s\n", string_hex_buffer(ctx, ctx->secret_key, OB_KEY_SIZE_10957));
  fprintf(LOG, "       UID: %s\n", string_hex_buffer(ctx, ctx->uid, ctx->uid_size));
  status = diversify_AN10957(ctx);
  if (status != ST_OK)
    fprintf(LOG, "Error %d. in diversification\n", status);
  return(status);

} /* main for sigutil */

