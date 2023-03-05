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

    value = json_object_get(settings, "secret-key");
    if (json_is_string(value))
    {
      int secret_key_length;
      secret_key_length = sizeof(ctx->secret_key);
      memcpy(ctx->secret_key, string_buffer_hex(ctx, json_string_value(value), &secret_key_length), sizeof(ctx->secret_key));
    };

    value = json_object_get(settings, "UID");
    if (json_is_string(value))
    {
      int uid_length;

      memcpy(ctx->uid, string_buffer_hex(ctx, json_string_value(value), &uid_length), sizeof(ctx->uid));
    };
  };

  return(status);

} /* openbadger_initialize */

