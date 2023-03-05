// (C)2022 Smithee Solutions LLC


#include <jansson.h>

#include <PCSC/wintypes.h>
#include <PCSC/pcsclite.h>
#include <PCSC/winscard.h>

#include <card-whisperer.h>


void
  dump_buffer_2
    (CSHH_CONTEXT *cfg,
    BYTE *bytes,
    int length,
    int dest)

{ /* dump_buffer_2 */

  int i;
  int line_length;
  FILE *log_file;


  log_file = stderr;
  if (cfg->current_file != NULL)
    log_file = cfg->current_file;
  else
    log_file = stderr;
  fflush (log_file);
  line_length = 32;
  for (i=0; i<length; i++)
  {
    fprintf (log_file, "%02x", bytes [i]);
    if ((line_length-1) == (i % line_length))
      fprintf (log_file, "\n");
  };
  if ((line_length-1) != ((length-1) % line_length))
    fprintf (log_file, "\n");
  fflush (log_file);

} /* dump_buffer_2 */

int
  init_cshh_2
    (CSHH_CONTEXT *ctx)

{ /* init_cshh_2 */

  int i;
  json_t *settings;
  int status;
  json_error_t status_json;
  json_t *value;


  status = ST_OK;
  ctx->verbosity = 3;
  settings = json_load_file(CSHH_SETTINGS_FILE, 0, &status_json);
  if (settings != NULL)
  {
    value = json_object_get(settings, "verbosity");
    if (json_is_string(value))
    {
      sscanf(json_string_value(value), "%d", &i);
      ctx->verbosity = i;
    };
  };
  return(status);

} /* init_cshh_2 */

