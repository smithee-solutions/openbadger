/*
  test-piv-challenge - test the PIV style of "challenge/response"

  Usage:
    test-piv-challenge <Reader number> <payload-size>]

  Reader number is default 0.  Note Contact/Contactless readers might have the CL interface on reader 1.
  Use "opensc-tool --list-readers" to check.

  Paylaod size is how big to make the APDU.  Default is 255.

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

#define OP_PIV_APPLICATION_IDENTIFIER { 0x00, 0xA4, 0x04, 0x00,  0x09, 0xA0, 0x00, 0x00, 0x03, 0x08, 0x00, 0x00,  0x10, 0x00, 0x00 }
unsigned char op_test_piv [] = OP_PIV_APPLICATION_IDENTIFIER;


int main
  (int argc,
  char *argv [])

{ /* main for test-piv-challenge */

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
  rdrctx = ctx->rdrctx;
  smartcard_select_command_length = 0;
  ctx->test_case = OB_TEST_PIV;
  ctx->apdu_payload_max_7816 = OB_7816_APDU_PAYLOAD_MAX;
ctx->verbosity = 9;
  if (argc > 1)
  {
    int i;
    int rdr;
    rdr = *(0+(argv[1]));
    ctx->reader_index = rdr - 0x30;
    fprintf(stderr, "Reader %d.\n", ctx->reader_index);

    if (argc > 2)
    {
      sscanf(argv [2], "%d", &i);
      ctx->apdu_payload_max_7816 = i;
      if (argc > 3)
      {
        challenge_message_file = fopen(argv [3], "r");
        if (challenge_message_file != NULL)
          fread(ctx->challenge_message, sizeof(challenge_message [0]), sizeof(ctx->challenge_message), challenge_message_file);
        else
          fprintf(stderr, "Cannot read challenge file %s\n", argv [3]);
      };
    };
  };
  fprintf(stderr, "test-piv-challenge %s\n", OPENBADGER_VERSION);
  fprintf(stderr, "APDU Payload max: %d.\n", ctx->apdu_payload_max_7816);

  status = ob_init_smartcard(ctx);

  /*
    either use test vectors (specific to this test) or do the actual thing.
    there are multiple tests in openbadger see openbadger-local.h for the list.
  */

  if (status EQUALS ST_OK)
  {
    switch(ctx->test_case)
    {
    case OB_TEST_VECTORS:
      fprintf(stderr, "Test vectors in use, no command sent.\n");
      break;
    case OB_TEST_PIV:
      memcpy(smartcard_select_command, op_test_piv, sizeof(op_test_piv));
      smartcard_select_command_length = sizeof(op_test_piv);
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
fprintf(stderr, "DEBUG: challenge/response now...\n");
    status = ob_challenge_response(ctx);
  };

  if (status != ST_OK)
    fprintf(stderr, "return status %d. last PCSC status %lX %s\n", status, rdrctx->last_pcsc_status, ob_pcsc_error_string(rdrctx->last_pcsc_status));
  return(status);

} /* main for test-piv-challenge */

