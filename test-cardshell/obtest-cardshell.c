#if 0
while no more commands
  read command
  parse command
  if verbose dump command
  note start time
  execute command
  note stop time
  log results

commands
nop
finish
select

command aliases
general-authenticate
authentication-request

command=blah
cl
blah
p1
p2
lc
le (default 0)
payload-raw
payload-type
payload-value
payload-length (implicit if you use type and value)
#endif
/*
  obtest-cardshell - ISO 7816-4 command processor ("card shell")

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


#define OB_CARDSHELL_VERSION "0.00"

#define EQUALS ==
#define OB_STRING_MAX (1024)

typedef struct ob_cardshell_context
{
  int verbosity;
  int option_switch;
  FILE *log;
  FILE *script;
  char log_filename [OB_STRING_MAX];
  char script_filename [OB_STRING_MAX];
  char settings_filename [OB_STRING_MAX];
} OB_CARDSHELL_CONTEXT;

typedef struct ob_cardshell_command
{
  int command_id;
  unsigned char thing1;
  unsigned char thing2;
  unsigned char P1;
  unsigned char P2;
  unsigned char Lc;
  unsigned char Le;
} OB_CARDSHELL_COMMAND;

// switches
#define OB_NOOP      ( 0)
#define OB_HELP      ( 1)
#define OB_VERBOSITY ( 2)
#define OB_SETTINGS  ( 3)
#define OB_LOG       ( 4)
#define OB_SCRIPT    ( 5)

#define ST_OK               (  0)
#define STOB_CSH_BAD_LOG    (  1)
#define STOB_CSH_BAD_SCRIPT (  2)

int ob_cardshell_initialize(OB_CARDSHELL_CONTEXT *ctx, int argc, char *argv []);
int ob_cardshell_parse_command(OB_CARDSHELL_CONTEXT *ctx, char *script_line, OB_CARDSHELL_COMMAND *cmd);
int ob_cardshell_read_settings(OB_CARDSHELL_CONTEXT *ctx, char *settings_filename);

OB_CARDSHELL_CONTEXT ob_cardshell_context;
char optstring [OB_STRING_MAX];
struct option longopts [] = {
      {"help", 0, &(ob_cardshell_context.option_switch), OB_HELP},
      {"log", required_argument, &(ob_cardshell_context.option_switch), OB_LOG},
      {"settings", required_argument, &(ob_cardshell_context.option_switch), OB_SETTINGS},
      {"script", required_argument, &(ob_cardshell_context.option_switch), OB_SCRIPT},
      {"verbosity", required_argument, &(ob_cardshell_context.option_switch), OB_VERBOSITY},
      {0, 0, 0, 0}
    };


int main
  (int argc,
  char *argv [])

{ /* main for ob-cardshell */

  OB_CARDSHELL_COMMAND card_command;
  OB_CARDSHELL_CONTEXT *ctx;
  int done;
  char script_line [8192];
  int status;
  char *status_gets;


  status = ST_OK;
  ctx = &ob_cardshell_context;
  memset(ctx, 0, sizeof(*ctx));
  status = ob_cardshell_initialize(ctx, argc, argv);
  fprintf(ctx->log, "cardshell %s\n", OB_CARDSHELL_VERSION);

  done = 0;
  if (status != ST_OK)
    done = 1;
  while (!done)
  {
    // read one line from script
    status_gets = fgets(script_line, sizeof(script_line), ctx->script);
    if (status_gets EQUALS NULL)
      done = 1;

    if (!done)
    {
      status = ob_cardshell_parse_command(ctx, script_line, &card_command);
      if (status EQUALS ST_OK)
      {
        switch(card_command.command_id)
        {
        default:
          if (ctx->verbosity > 3)
            fprintf(ctx->log, "cardshell: command %d. not implemented\n", card_command.command_id);
          break;
        };
      };
    };
    if (status != ST_OK)
      done = 1;
  };

  if (status != ST_OK)
  {
    fprintf(stderr, "ob-cardshell exit status %d.\n", status);
    if (ctx->log != NULL)
      fprintf(ctx->log, "ob-cardshell exit status %d.\n", status);
  };
  return(status);

} /* main for ob-cardshell */


int ob_cardshell_initialize
  (OB_CARDSHELL_CONTEXT *ctx,
  int argc,
  char *argv [])

{ /* ob_cardshell_initialize */

  int done;
  int i;
  int longindex;
  int status;
  int status_opt;


  status = ST_OK;
  ctx->verbosity = 3;
ctx->verbosity = 9;
  ctx->log = stderr;
  strcpy(ctx->settings_filename, "./cardshell-settings.json");
  strcpy(ctx->script_filename, "./cardshell.json");
  strcpy(ctx->log_filename, "./cardshell.log");

  // read switches
  done = 0;
  while (!done)
  {
    status_opt = getopt_long (argc, argv, optstring, longopts, &longindex);
    switch (ctx->option_switch)
    {
    case OB_NOOP:
      // stay silent if looping around after an option was found.
      break;

    case OB_LOG:
      strcpy(ctx->log_filename, optstring);
      break;
    case OB_SCRIPT:
      strcpy(ctx->script_filename, optstring);
      break;
    case OB_SETTINGS:
      strcpy(ctx->settings_filename, optstring);
      break;
    case OB_VERBOSITY:
      sscanf(optarg, "%d", &i);
      ctx->verbosity = i;
      break;

    case OB_HELP:
      fprintf(ctx->log, "--help - display this help text.\n");
      fprintf(ctx->log, "--verbosity (min 1 max 9)\n");
      fprintf(ctx->log, "--log <json file> - log file\n");
      fprintf(ctx->log, "--script <json file> - script file to process\n");
      fprintf(ctx->log, "--settings <json file> - configured settings for the tool\n");
      break;
    };
    ctx->option_switch = OB_NOOP; // reset from whatever getopt_long set it to
    if ((status_opt EQUALS -1) || (status != ST_OK))
      done = 1;
  };

  if (status EQUALS ST_OK)
  {
    ctx->log = fopen(ctx->log_filename, "w");
    if (ctx->log EQUALS NULL)
      status = STOB_CSH_BAD_LOG;
  };
  if (status EQUALS ST_OK)
  {
    if (ctx->verbosity > 3)
    {
      fprintf(ctx->log, "script: %s\n", ctx->script_filename);
    };
    ctx->script = fopen(ctx->script_filename, "r");
    if (ctx->script EQUALS NULL)
      status = STOB_CSH_BAD_SCRIPT;
  };
  if (status EQUALS ST_OK)
  {
    status = ob_cardshell_read_settings(ctx, ctx->settings_filename);
  };

  return(status);

} /* ob_cardshell_initialize */


int ob_cardshell_parse_command
  (OB_CARDSHELL_CONTEXT *ctx,
  char *script_line,
  OB_CARDSHELL_COMMAND *cmd)

{ /* ob_cardshell_parse_command */

  int status;


  status = ST_OK;
  return(status);

} /* ob_cardshell_parse_command */


int ob_cardshell_read_settings
  (OB_CARDSHELL_CONTEXT *ctx,
  char *settings_filename)

{ /* ob_cardshell_read_settings */

  int status;


  status = ST_OK;
// parse verbosity <number>
// parse default-reader <number>
  return(status);

} /* ob_cardshell_read_settings */

