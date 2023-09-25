/*
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
    startup - initialize context and other things
*/


#include <string.h>
#include <stdio.h>

#include <jansson.h>

#include <nfc/nfc.h>
#include <freefare.h>


#include <ob-desfire.h>


int obtest_desfire_action(char *action_verb, int argc, char *argv []);
int ob_desfire_load_context(OBTEST_DESFIRE_CONTEXT *ctx);
int ob_desfire_load_settings(OBTEST_DESFIRE_CONTEXT *ctx);
int ob_desfire_load_settings_values(OBTEST_DESFIRE_CONTEXT *ctx);
int ob_desfire_save_context(OBTEST_DESFIRE_CONTEXT *ctx);


int main
  (int argc,
  char *argv [])

{ /* main for obtest-desfire-startup */

  int status;


  status = obtest_desfire_action("startup", argc, argv);
  if (status != ST_OK)
    fprintf(stderr, "final status was %d.\n", status);
  return(status);

} /* main for obtest-desfire-startup */


int obtest_desfire_action
  (char *action_verb,
  int argc,
  char *argv [])

{ /* obtest_desfire_action */

  OBTEST_DESFIRE_CONTEXT *ctx;
  OBTEST_DESFIRE_CONTEXT obtest_context_desfire;
  int status;


  status = ST_OK;
  ctx = &obtest_context_desfire;

  status = ob_desfire_init(ctx);
  if (status EQUALS ST_OK)
  {
    if (ctx->verbosity > 1)
      fprintf(stderr, "obtest-desfire-%s initializing.\n", action_verb);
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

    fprintf(stderr, "obtest-desfire-%s is in startup.\n", action_verb);
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

} /* obtest_desfire_action */

