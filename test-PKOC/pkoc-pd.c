#if 0
pd side stuff, anything that can be done on a command line.

also has a settings file, openbadger filename style

openbadger-settings.json
#endif

#include <stdio.h>
#include <getopt.h>
#include <string.h>

// OSDP parameters

#define OSDP_RAW_FORMAT_PRIVATE (0x80)

// spec parameters

#define PKOC_TAG_ERROR_7816 (0x7F)
#define PKOC_TAG_ERROR_GENERAL (0x7E)
typedef struct __attribute__((packed)) pkoc_raw_card_present_payload
{
  unsigned char format_code;
  unsigned char bits_lsb;
  unsigned char bits_msb;
  unsigned char data [1024];
} PKOC_RAW_CARD_PRESENT_PAYLOAD;


#define PKOC_CARD_PRESENT_MFG (0)
#define PKOC_CARD_PRESENT_RAW (1)

typedef struct pkoc_context
{
  int option;
  int verbosity;
  FILE *log;
  FILE *console;
  char control_port [1024];
  int bits;
  char error_tlv_hex [1024];
  int card_present_method;
} PKOC_CONTEXT;


#define OBPKOCOPT_HELP         (  1)
#define OBPKOCOPT_VERBOSITY (  2)
#define OBPKOCOPT_NOOP      (  3)
#define OBPKOCOPT_BITS      (  4)
#define OBPKOCOPT_CONTROL_PORT (  5)
#define OBPKOCOPT_CARD_VERSION (  6)
#define OBPKOCOPT_ERROR        (  7)
#define OBPKOCOPT_RESPONSE_RAW (  8)
#define OBPKOCOPT_OUI          (  9)
#if 0
no-reader-id
no-transaction-id
settings
#endif
#define EQUALS ==
#define ST_OK (  0)

void bytes_to_hex(unsigned char *raw, int length, char *byte_string);
int hex_to_binary(PKOC_CONTEXT *ctx, char *hex, unsigned char *binary, int *length);
int osdp_send_response_RAW(PKOC_CONTEXT *ctx, PKOC_RAW_CARD_PRESENT_PAYLOAD *raw);
int pkoc_help(PKOC_CONTEXT *ctx);

PKOC_CONTEXT pkoc_context;

struct option longopts [] = {
      {"bits", required_argument, &pkoc_context.option, OBPKOCOPT_BITS},
      {"card-version", required_argument, &pkoc_context.option, OBPKOCOPT_CARD_VERSION},
      {"control-port", required_argument, &pkoc_context.option, OBPKOCOPT_CONTROL_PORT},
      {"error", required_argument, &pkoc_context.option, OBPKOCOPT_ERROR},
      {"help", 0, &pkoc_context.option, OBPKOCOPT_HELP},
      {"OUI", required_argument, &pkoc_context.option, OBPKOCOPT_OUI},
      {"response-raw", 0, &pkoc_context.option, OBPKOCOPT_RESPONSE_RAW},
      {"verbosity", required_argument, &pkoc_context.option, OBPKOCOPT_VERBOSITY},
      {0, 0, 0, 0}};

char optstring [1024];


void bytes_to_hex
  (unsigned char *raw,
  int length,
  char *byte_string)

{ /* bytes_to_hex */

  int i;
  char *p;

  p = byte_string;
  for (i=0; i<length; i++)
  {
    sprintf(p, "%02X", *(raw+i));
    p++;
  };

} /* bytes_to_hex */


int hex_to_binary
  (PKOC_CONTEXT *ctx,
  char *hex,
  unsigned char *binary,
  int *length)

{ /* hex_to_binary */

  int count;
  int hexit;
  char octet_string [3];
  char *p;
  unsigned char *pbinary;


  p = hex;
  pbinary = binary;
  count = strlen(hex);
  if ((count % 2) != 0)
  {
    count = count - 1;
    fprintf(ctx->log, "trimming hex string to even number of hexits.\n");
  };
  while (count > 0)
  {
    memcpy(octet_string, p, 2);
    octet_string [2] = 0;
    sscanf(octet_string, "%x", &hexit);
    *pbinary = hexit;
    pbinary++;
    p = p + 2;
    count = count - 2;
  };

  return(ST_OK);

} /* hex_to_binary */


int main
  (int argc,
  char *argv[])

{ /* main for pkoc-pd */

  unsigned char card_present_payload [1024];
  int card_present_payload_length;
  PKOC_CONTEXT *ctx;
  int done;
  int found_something;
  int i;
  PKOC_RAW_CARD_PRESENT_PAYLOAD response_raw;
  int status;
  int status_opt;


  status = ST_OK;
  memset(&pkoc_context, 0, sizeof(pkoc_context));
  ctx = &pkoc_context;
ctx->verbosity = 9;
  ctx->log = stderr;
  ctx->console = stdout;
  if (status EQUALS ST_OK)
  {
    done = 0;
    while (!done)
    {
      ctx->option = OBPKOCOPT_NOOP;
      status_opt = getopt_long (argc, argv, optstring, longopts, NULL);
      if (status_opt EQUALS -1)
        if (!found_something)
          ctx->option = OBPKOCOPT_HELP;  // found nothing and/or end of list so give help
      if (ctx->verbosity > 9)
        fprintf (ctx->log, "option %2d\n", ctx->option);
      switch (ctx->option)
      {
      case OBPKOCOPT_BITS:
        found_something = 1;
        sscanf(optstring, "%d", &i);
        ctx->bits = i;
fprintf(stderr, "DEBUG: range check bits\n");
        break;

      case OBPKOCOPT_CONTROL_PORT:
        found_something = 1;
        strcpy(ctx->control_port, optstring);
        break;

      case OBPKOCOPT_ERROR:
        found_something = 1;
        strcpy(ctx->error_tlv_hex, optstring);
        break;

      case OBPKOCOPT_HELP:
        found_something = 1;
        status = pkoc_help(ctx);
        break;

      case OBPKOCOPT_RESPONSE_RAW:
        found_something = 1;
        ctx->card_present_method = PKOC_CARD_PRESENT_RAW;
        break;

      case OBPKOCOPT_VERBOSITY:
        found_something = 1;
        sscanf(optstring, "%d", &i);
        ctx->verbosity = i;
        break;

      default:
        fprintf(stderr, "OPTION NOT IMPLEMENTED %d\n", ctx->option);
        break;
      };
      if (status_opt EQUALS -1)
        done = 1;
    };
  };

  // process options

  if (status EQUALS ST_OK)
  {
    if (strlen(ctx->error_tlv_hex) > 0)
    {
      memset(&response_raw, 0, sizeof(response_raw));
      status = hex_to_binary(ctx, ctx->error_tlv_hex, card_present_payload, &card_present_payload_length);
      response_raw.format_code = OSDP_RAW_FORMAT_PRIVATE;
      response_raw.bits_lsb = (card_present_payload_length*8) & 0xff;
      response_raw.bits_msb = ((card_present_payload_length*8) & 0xff00) >> 8;
      memcpy(response_raw.data, card_present_payload, sizeof(card_present_payload));
      if (status EQUALS 0)
      {
        if (ctx->card_present_method EQUALS PKOC_CARD_PRESENT_RAW)
          status = osdp_send_response_RAW(ctx, &response_raw);
        else
fprintf(stderr, "DEBUG: also send as mfg\n");
      };
    };
  };

  if (status != ST_OK)
  {
    fprintf(ctx->log, "pkoc-pd exit status %d.\n", status);
    fprintf(stderr, "pkoc-pd exit status %d.\n", status);
  };
  return(status);

} /* main for pkoc-pd */


int osdp_send_response_RAW
  (PKOC_CONTEXT *ctx,
  PKOC_RAW_CARD_PRESENT_PAYLOAD *raw)

{ /* osdp_send_response_RAW */

  int bits;
  char byte_string [1024];
  char command [1024];


  bytes_to_hex((unsigned char *)raw, sizeof(raw), byte_string);
  bits = sizeof((unsigned char *)raw) * 8;
  sprintf(command, 
"{\"command\":\"present-card\", \"bits\":\"%d\", \"format\":\"80\", \"data\":\"%s\"}\n",
    bits, byte_string);

  return(ST_OK);

} /* osdp_send_response_RAW */


int pkoc_help
  (PKOC_CONTEXT *ctx)

{ /* pkoc_help */

  fprintf(ctx->log, "Usage:\n");
  fprintf(ctx->log, "  pkoc-pd <switches>\n");
  fprintf(ctx->log, "  --bits - number of bits in response\n");
  fprintf(ctx->log, "  --card-version - card version TLV to return to ACU (in hex)\n");
  fprintf(ctx->log, "  --control-port - path of libosdp-conformance control port (default /opt/osdp-conformance/run/PD/osdp-control-port)\n");
  fprintf(ctx->log, "  --help - this message\n");
  fprintf(ctx->log, "  --OUI - organizational unit indicator to be used in MFG commands and responses\n");
  fprintf(ctx->log, "  --response-raw - use osdp_RAW response for card present.\n");
  fprintf(ctx->log, "  --verbosity - logging level.  0=none, 3=normal, 9=debug.  Default is 3.\n");

  return(ST_OK);

} /* pkoc_help */

