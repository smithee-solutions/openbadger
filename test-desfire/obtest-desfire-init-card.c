/*

  each tool is passed a json string as it's command line. it does not know about argc,argv or command line switches
  the tool parses that json string in addition to the settings and context files.

  initialize-card picc master key
  initialize-application application id application master key
  initialize application read keys
    key-tag 1 key-material 00112233445566778899aabbcddeeff
    key-tag 14 etc.
  initialize application write keys
    key-tag 1 key-material blah
  create-file
  write-file
  read-file

  actions:
    initialize-card key-material=... algorithm=...
    startup - initialize context and other things
*/


#include <string.h>
#include <stdio.h>

#include <jansson.h>

#include <nfc/nfc.h>
#include <freefare.h>


#include <ob-desfire.h>


int ob_desfire_action(char *action_string);
int ob_desfire_initcard_command(OBTEST_DESFIRE_CONTEXT *ctx, char *action);


int main
  (int argc,
  char *argv [])

{ /* main for obtest-desfire-init-card */

  char action_string [1024];
  int status;


  // initialize-card key-material=... algorithm=...
  sprintf(action_string, "{\"action\"=\"initialize-card\",\"key-material\"=\"%s\",\"algorithm\"=\"%s\"}",
    "00112233445566778899AABBCCDDEEFF", "AES");

  status = ob_desfire_action(action_string);
  if (status != ST_OK)
    fprintf(stderr, "final status was %d.\n", status);
  return(status);

} /* main for obtest-desfire-init-card */


int ob_desfire_action
  (char *action_string)

{ /* ob_desfire_action */

  OBTEST_DESFIRE_CONTEXT *ctx;
  OBTEST_DESFIRE_CONTEXT obtest_context_desfire;
  int status;


  status = ST_OK;
  ctx = &obtest_context_desfire;

  status = ob_desfire_init(ctx);
  if (status EQUALS ST_OK)
  {
    if (ctx->verbosity > 1)
      fprintf(stderr, "Action initializing.\n");
    status = ob_desfire_load_settings(ctx);
  };
  if (status EQUALS ST_OK)
  {
    if (ctx->verbosity > 1)
      fprintf(stderr, "obtest-desfire settings loaded.\n");
    status = ob_desfire_load_context(ctx);
  };

  if (status EQUALS ST_OK)
  {
    if (ctx->verbosity > 1)
      fprintf(stderr, "obtest-desfire context loaded.\n");

    status = ob_desfire_initcard_command(ctx, action_string);
  };
  if (status EQUALS ST_OK)
  {
    if (ctx->verbosity > 1)
      fprintf(stderr, "obtest-desfire startup concluded.\n");
    status = ob_desfire_save_context(ctx);
  };
  if (status EQUALS ST_OK)
  {
    if (ctx->verbosity > 1)
      fprintf(stderr, "obtest-desfire context saved.\n");
  };
  return(status);

} /* ob_desfire_action */


// this knows it is the "init-card" action code.

int ob_desfire_initcard_command
  (OBTEST_DESFIRE_CONTEXT *ctx,
  char *action)

{ /* ob_desfire_start_command */

  json_t *command_line;
  int status;
  json_error_t status_json;
  json_t *value;


  status = ST_OK;
  fprintf(stderr, "obtest-desfire-init-card initializing card.\n");
  command_line = json_loads(action, 0, &status_json);
  if (command_line)
  {
    value = json_object_get(ctx->settings, "action");
    if (json_is_string(value))
    {
      // if (valid_action(json_string_value(value))
      fprintf(stderr, "...action is %s.\n", json_string_value(value));
    };
    value = json_object_get(ctx->settings, "key-material");
    if (json_is_string(value))
    {
      // if (valid_keymat(json_string_value(value))
      strcpy(key_slots [OB_DESFIRE_KEYIDX_PICC_MASTER].key_hex, json_string_value(value));
      if (ctx->verbosity > 2)
        fprintf(stderr, "PICC Master Key set.\n");
      if (ctx->verbosity > 8)
        fprintf(stderr, "  key: %s\n", key_slots [OB_DESFIRE_KEYIDX_PICC_MASTER].key_hex);
    };
    value = json_object_get(ctx->settings, "algorithm");
    if (json_is_string(value))
    {
      if (strcmp(json_string_value(value), "AES"))
        status = STOB_DESFIRE_BAD_ALGO;
    };
  }
  else
  {
    status = STOB_DESFIRE_COMMAND_PARSE;
  };

  return(status);

} /* ob_desfire_start_command */

