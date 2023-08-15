#include <string.h>
#include <stdio.h>

#include <jansson.h>

#include <nfc/nfc.h>
#include <freefare.h>


#include <ob-desfire.h>


int ob_desfire_init
  (OBTEST_DESFIRE_CONTEXT *ctx)

{ /* ob_desfire_init */

  int status;


  // initialize things here.

  status = ST_OK;
  memset(ctx, 0, sizeof(*ctx));
  key_slots [OB_DESFIRE_KEYIDX_PICC_MASTER].algorithm = OB_ALGO_3DES;

  return(status);

} /* ob_desfire_init */


int ob_desfire_load_context
  (OBTEST_DESFIRE_CONTEXT *ctx)

{ /* ob_desfire_load_context */

  int status;
  json_error_t status_json;


  status = ST_OK;
fprintf(stderr, "DEBUG: load context: stub.\n");

  // load things from the context file (either ./obtest-desfire_context.json or /opt/smithee/etc/obtest-desfire_context.json)

  status = ST_OK;
  strcpy(ctx->context_file_path, "./obtest-desfire_context.json");
  ctx->context = json_load_file(ctx->context_file_path, 0, &status_json);
  if (ctx->context != NULL)
  {
    status = ob_desfire_load_context_values(ctx);
  }
  else
  {
    strcpy(ctx->context_file_path, OBTEST_DESFIRE_DEFAULT_CONTEXT_FILE);
    ctx->context = json_load_file(ctx->context_file_path, 0, &status_json);
    if (ctx->settings != NULL)
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
  strcpy(ctx->settings_file_path, "./obtest-desfire_settings.json");
  ctx->settings = json_load_file(ctx->settings_file_path, 0, &status_json);
  if (ctx->settings != NULL)
  {
    status = ob_desfire_load_settings_values(ctx);
  }
  else
  {
    strcpy(ctx->settings_file_path, OBTEST_DESFIRE_DEFAULT_SETTINGS_FILE);
    ctx->settings = json_load_file(ctx->settings_file_path, 0, &status_json);
    if (ctx->settings != NULL)
    {
      status = ob_desfire_load_settings_values(ctx);
    };
  };
  return(status);

} /* ob_desfire_load_settings */


int ob_desfire_load_settings_values
  (OBTEST_DESFIRE_CONTEXT *ctx)

{ /* ob_desfire_load_settings_values */

  int i;
  int status;
  json_t *value;


  status = ST_OK;

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

