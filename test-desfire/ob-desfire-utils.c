/*
  (C)2023-2024 Smithee Solutions LLC
*/
#include <string.h>
#include <stdio.h>

#include <jansson.h>

#include <nfc/nfc.h>
#include <freefare.h>


#include <ob-crypto.h>
#include <openbadger-common.h>
#include <ob-desfire.h>
#include <ob-status-desfire.h>


/*
  ob_desfire_init - initialize obtest desfire code

  Assumes the context passed in has been zeroized by the caller.
*/
int ob_desfire_init
  (OBTEST_DESFIRE_CONTEXT *ctx)

{ /* ob_desfire_init */

  int i;
  int status;


  // initialize things here.

  status = ST_OK;

  // zero out the key slots

  for (i=0; i<OB_DESFIRE_MAX_KEY_SLOT; i++)
    memset(&(key_slots [i]), 0, sizeof (key_slots [0]));

  // pre-initialize the PICC key slot's algorithm (and key?)

  key_slots [OB_DESFIRE_KEYIDX_PICC_MASTER].algorithm = OB_ALGO_3DES;

  return(status);

} /* ob_desfire_init */


/*
  ob_desfire_load_context - loads crypto details from a file.

  WARNING THIS STORES KEY MATERIAL IN THE CLEAR.

  Uses file (OB_DESFIRE_PATH_CONTEXT)
*/
int ob_desfire_load_context
  (OBTEST_DESFIRE_CONTEXT *ctx)

{ /* ob_desfire_load_context */

  int status;
  json_error_t status_json;


  status = ST_OK;
fprintf(stderr, "DEBUG: load context: stub.\n");


  status = ST_OK;
  sprintf(ctx->context_file_path, "%s", OB_DESFIRE_PATH_CONTEXT);
  ctx->context = json_load_file(ctx->context_file_path, 0, &status_json);
  if (ctx->context != NULL)
  {
    status = ob_desfire_load_context_values(ctx);
  }
  else
  {
    sprintf(ctx->context_file_path, "%s%s", OB_DESFIRE_PATH_COMMON, OB_DESFIRE_PATH_CONTEXT);
    ctx->context = json_load_file(ctx->context_file_path, 0, &status_json);
    if (ctx->context != NULL)
    {
      status = ob_desfire_load_context_values(ctx);
    };
  };
  return(status);

} /* ob_desfire_load_context */


int ob_desfire_load_context_values
  (OBTEST_DESFIRE_CONTEXT *ctx)

{ /* ob_desfire_load_context_values */

  int status;
  json_t *value;


  status = ST_OK;

  value = json_object_get(ctx->context, "key-PICC-master");
  if (json_is_string(value))
  {
    strcpy(key_slots [OB_DESFIRE_KEYIDX_PICC_MASTER].key_hex, json_string_value(value));
  };
  return(status);

} /* ob_desfire_load_context_values */


int ob_desfire_load_settings
  (OBTEST_DESFIRE_CONTEXT *ctx)

{ /* ob_desfire_load_settings */

  int status;
  json_error_t status_json;


  // load things from the settings file (either ./obtest-desfire_settings.json or /opt/smithee/etc/obtest-desfire_settings.json)

  status = ST_OK;
  sprintf(ctx->settings_file_path, "%s", OB_DESFIRE_PATH_SETTINGS);
  ctx->settings = json_load_file(ctx->settings_file_path, 0, &status_json);
  if (ctx->settings != NULL)
  {
    status = ob_desfire_load_settings_values(ctx);
  }
  else
  {
    sprintf(ctx->settings_file_path, "%s%s", OB_DESFIRE_PATH_COMMON, OB_DESFIRE_PATH_SETTINGS);
    ctx->settings = json_load_file(ctx->settings_file_path, 0, &status_json);
    if (ctx->settings != NULL)
    {
      status = ob_desfire_load_settings_values(ctx);
    };
  };
  return(status);

} /* ob_desfire_load_settings */


/*
  ob_desfire_load_settings_values - pick up individual settings

  settings are:
    AID - 4 byte hex value
    verbosity - 0 for silent, 3 for normal, 9 for debug
*/
int ob_desfire_load_settings_values
  (OBTEST_DESFIRE_CONTEXT *ctx)

{ /* ob_desfire_load_settings_values */

  int i;
  int status;
  json_t *value;


  status = ST_OK;

  value = json_object_get(ctx->settings, "AID");
  if (json_is_string(value))
  {
    sscanf(json_string_value(value), "%x", &(ctx->aid));
  };
  value = json_object_get(ctx->settings, "file");
  if (json_is_string(value))
  {
    sscanf(json_string_value(value), "%d", &i);
    ctx->file_id = i;
  };
  value = json_object_get(ctx->settings, "reader");
  if (json_is_string(value))
  {
    sscanf(json_string_value(value), "%d", &i);
    ctx->reader = i;
  };
  value = json_object_get(ctx->settings, "verbosity");
  if (json_is_string(value))
  {
    sscanf(json_string_value(value), "%d", &i);
    ctx->verbosity = i;
  };
  return(status);

} /* ob_desfire_load_settings_values */


int ob_desfire_save_context
  (OBTEST_DESFIRE_CONTEXT *ctx)

{ /* ob_desfire_save_context */

  FILE *cf;
  int status;


  status = ST_OK;
  if (ctx->verbosity > 2)
    fprintf(stderr, "saving context to %s\n", ctx->context_file_path);
  cf = fopen(ctx->context_file_path, "w");
  if (cf)
  {
    fprintf(cf, "{\"%s\":\"%s\"}", OB_TAG_PICC_MASTER, key_slots[OB_DESFIRE_KEYIDX_PICC_MASTER].key_hex);
    fclose(cf);
  };
  return(status);

} /* ob_desfire_save_context */

