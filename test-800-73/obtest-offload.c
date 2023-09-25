#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#include <PCSC/wintypes.h>
#include <PCSC/pcsclite.h>
#include <PCSC/winscard.h>


#include <ob-crypto.h>
#include <ob-7816.h>
#include <openbadger-common.h>
#include <openbadger-version.h>
#include <ob-pcsc.h>
#include <ob-73-4.h>

// this is defined in NIST SP-800-73-4 Part 1 Section 2.2

#define OB_OFFLOAD_RSA { 0xa0, 0x00, 0x00, 0x00, 0x07, 0x93, 0x01, 0x00, 0x00, 0x01 };
unsigned char ob_test_offload [] = OB_OFFLOAD_RSA;



int ob_build_7816_message
    (OB_CONTEXT *ctx,
    unsigned char msg_cla,
    unsigned char msg_ins, 
    unsigned char msg_p1,
    unsigned char msg_p2,
    unsigned char msg_lc,
    unsigned char msg_le,
    unsigned char *payload,
    int payload_length,
    int flags,
    unsigned char *message_7816,
    int *message_7816_length)

{ /* oo_build_7816_message */

  int payload_offset;
  int status;


  status = 0;
  if (ctx->verbosity > 3)
  {
    fprintf (stderr, "7816 PDU: payload %d.\n",
      payload_length);
  };

  // clean out buffer
  // +6 for cla ins p1 p2 Lc Le
  memset (message_7816, 0, 6+ctx->apdu_payload_max_7816);

  // make sure it will fit

  if (payload_length > ctx->apdu_payload_max_7816)
    status = -3;

  if (status == 0)
  {
    payload_offset = 5;
    message_7816 [0] = msg_cla;
    message_7816 [1] = msg_ins;
    message_7816 [2] = msg_p1;
    message_7816 [3] = msg_p2;
    message_7816 [4] = msg_lc;

    memcpy (message_7816 + payload_offset, payload, payload_length);
    *message_7816_length = payload_offset + payload_length;

    // always append Le
    message_7816 [payload_offset + payload_length] = msg_le;
    (*message_7816_length)++;
  };

  return (status);

} /* oo_build_7816_message */




int main
  (int argc,
  char *argv [])

{ /* main for obtest-challenge */

  unsigned char challenge_message [OB_RSA2048_KEY_SIZE];
  FILE *challenge_message_file;
  OB_CONTEXT *ctx;
  DWORD dwRecvLength;
  BYTE pbRecvBuffer [2*OB_7816_APDU_PAYLOAD_MAX]; // probably actually 1x plus 2
  OB_RDRCTX pcsc_reader_context;
  OB_RDRCTX *rdrctx;
  unsigned char smartcard_select_command [OB_7816_BUFFER_MAX];
  int smartcard_select_command_length;
  int status;
  LONG status_pcsc;
  OB_CONTEXT test_piv_context;


  ctx = &test_piv_context;
  memset(ctx, 0, sizeof(*ctx));
  ctx->rdrctx = &pcsc_reader_context;
  ctx->test_case = OB_TEST_PIV;
  ctx->apdu_payload_max_7816 = OB_7816_APDU_PAYLOAD_MAX;
  ctx->payload_file = malloc(1024);
  status = ob_read_settings(ctx);

  rdrctx = ctx->rdrctx;
  smartcard_select_command_length = 0;
  if (argc > 1)
  {
    int rdr;
    rdr = *(0+(argv[1]));
    ctx->reader_index = rdr - 0x30;
    fprintf(stderr, "Reader %d.\n", ctx->reader_index);
  };

  if (ctx->payload_file)
  {
    challenge_message_file = fopen(ctx->payload_file, "r");
    if (challenge_message_file != NULL)
    {
      fread(ctx->challenge_message, sizeof(challenge_message [0]), sizeof(ctx->challenge_message), challenge_message_file);
      fprintf(stderr, "Reading challenge from file %s\n", argv [3]);
    }
    else
      fprintf(stderr, "Cannot read challenge file %s\n", argv [3]);
  };

  fprintf(stderr, "test-piv-challenge %s\n", OPENBADGER_VERSION);
  fprintf(stderr, "APDU Payload max: %d.\n", ctx->apdu_payload_max_7816);

  status = ob_init_smartcard(ctx);

  if (status EQUALS ST_OK)
  {
    switch(ctx->test_case)
    {
    case OB_TEST_PIV:
      memcpy(smartcard_select_command, ob_test_offload, sizeof(ob_test_offload));
      smartcard_select_command_length = sizeof(ob_test_offload);
      break;
    default:
      status= STOB_UNKNOWN_TEST_CASE;
      break;
    };
  };

  if (ctx->verbosity > 3)
  {
    ob_dump_buffer (ctx, smartcard_select_command, smartcard_select_command_length, 0);
  };

  // send application select to card

  dwRecvLength = sizeof(pbRecvBuffer);
  status_pcsc = SCardTransmit(rdrctx->pcsc, &rdrctx->pioSendPci, smartcard_select_command, smartcard_select_command_length, NULL, pbRecvBuffer, &dwRecvLength);
  rdrctx->last_pcsc_status = status_pcsc;
  if (SCARD_S_SUCCESS != status_pcsc)
    status = STOB_PCSC_TRANSMIT_1;
  if (status != ST_OK)
  {
    fprintf(stderr, "Select returns: ");
    ob_dump_buffer (ctx, pbRecvBuffer, dwRecvLength, 0);
  };

  if (status EQUALS ST_OK)
  {
    unsigned char buffer [8192];
    FILE *f1;
    int status_io;
    unsigned char msg_cla;
    unsigned char msg_ins;
    unsigned long int msg_lc_extended;
    unsigned char msg_p1;
    unsigned char msg_p2;
    int message_7816_length;
    unsigned char msg_7816_extended [8192];
    unsigned char response_7816 [1024];
  LPDWORD response_7816_lth; // not DWORD

fprintf(stderr, "DEBUG: RSA offload now...\n");
    f1 = fopen("extracted", "r");
    status_io = fread(buffer, sizeof(buffer[0]), sizeof(buffer), f1);
    msg_cla = 0x80;
    msg_ins = 0x20;
    msg_p1 = 0x01;
    msg_p2 = 0x00;
    msg_lc_extended = status_io;

    status = ob_build_7816_message(ctx, msg_cla, msg_ins, msg_p1, msg_p2,
      msg_lc_extended, 0, buffer, msg_lc_extended, 0, msg_7816_extended, &message_7816_length);

    response_7816_lth = 0;
    status_pcsc = SCardTransmit(rdrctx->pcsc, &(rdrctx->pioSendPci), msg_7816_extended, message_7816_length, NULL, response_7816, response_7816_lth);
  };

  if (status != ST_OK)
    fprintf(stderr, "return status %d. last PCSC status %lX %s\n", status, rdrctx->last_pcsc_status, ob_pcsc_error_string(rdrctx->last_pcsc_status));
  return(status);

} /* main for test-offload-rsa */

