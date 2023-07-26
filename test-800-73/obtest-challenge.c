/*
  obtest-challenge - test the PIV style of "challenge/response"

  Usage:
    obtest-challenge <Reader number> <payload-size>]

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
        if (*argv [3] != '-')
        {
          challenge_message_file = fopen(argv [3], "r");
          if (challenge_message_file != NULL)
          {
            fread(ctx->challenge_message, sizeof(challenge_message [0]), sizeof(ctx->challenge_message), challenge_message_file);
            fprintf(stderr, "Reading challenge from file %s\n", argv [3]);
          }
          else
            fprintf(stderr, "Cannot read challenge file %s\n", argv [3]);
        }
        else
        {
#define ENC_SAMPLE_2
#ifdef ENC_SAMPLE_1
unsigned char encrypted_payload [] = {
  0x8e, 0x56, 0xfd, 0x27, 0x0b, 0xcc, 0xa4, 0xcd, 0x7b, 0x22, 0x32, 0xd1, 0x40, 0xfd, 0x44, 0x8b
, 0x3f, 0x61, 0x1b, 0x0e, 0x7c, 0x74, 0xe4, 0xbc, 0x2f, 0x38, 0x29, 0x6c, 0x88, 0x89, 0x6f, 0x51
, 0x65, 0xdd, 0xda, 0x1e, 0x96, 0x96, 0x68, 0xe9, 0x73, 0x37, 0x1a, 0x3f, 0x9a, 0xd4, 0x38, 0x23
, 0x8a, 0x77, 0xae, 0x74, 0x25, 0xcb, 0x15, 0x49, 0x6f, 0x8b, 0x4a, 0xce, 0xb4, 0xb6, 0xbe, 0xbc
, 0x33, 0x58, 0x75, 0x1c, 0x8a, 0x44, 0xc3, 0xa4, 0xf7, 0x3d, 0x35, 0xfa, 0x63, 0xe6, 0xa9, 0x44
, 0xaa, 0x77, 0x05, 0x03, 0x2d, 0x3f, 0x02, 0xb2, 0x60, 0x6c, 0xfe, 0xf7, 0xae, 0xfe, 0x9a, 0xe4
, 0x9a, 0x58, 0x92, 0x46, 0xfb, 0xd7, 0xa7, 0x78, 0x8d, 0x8f, 0x4c, 0xcc, 0x53, 0x66, 0xe6, 0xd7
, 0x65, 0x5c, 0x77, 0x9a, 0x5c, 0xda, 0x64, 0xc4, 0xd6, 0x2a, 0x9a, 0xac, 0x6c, 0xdf, 0x4f, 0x87
, 0x2c, 0x00, 0xf2, 0x37, 0xac, 0x8d, 0xa3, 0xe0, 0xc6, 0x32, 0x2b, 0x4f, 0xfc, 0x8e, 0x2d, 0x83
, 0x48, 0x29, 0x26, 0xa6, 0xb0, 0x3c, 0xc5, 0x64, 0x54, 0xc8, 0x07, 0x9a, 0xb7, 0x0b, 0x9e, 0xb0
, 0xeb, 0x5e, 0x59, 0xc1, 0xbf, 0xfe, 0x20, 0xd9, 0xc2, 0xd2, 0xc5, 0x10, 0xf0, 0x64, 0xad, 0x03
, 0x77, 0xdc, 0x1a, 0x2c, 0x1f, 0x48, 0x23, 0x56, 0x02, 0xd2, 0xb1, 0x45, 0x36, 0xd6, 0x69, 0xdb
, 0x58, 0xff, 0xb4, 0x76, 0x23, 0x4a, 0x79, 0x85, 0x9c, 0x7a, 0xb8, 0x7a, 0x45, 0xb5, 0xa1, 0x24
, 0xe2, 0x1f, 0xe8, 0x9b, 0xdc, 0x18, 0x9b, 0xf4, 0x6f, 0x3e, 0x63, 0x6d, 0x38, 0xe1, 0x39, 0x53
, 0x19, 0x86, 0x5a, 0x09, 0x81, 0x64, 0x22, 0x45, 0x1c, 0xe5, 0x4a, 0x04, 0x5c, 0x0f, 0x09, 0x9d
, 0x85, 0xa7, 0x62, 0x43, 0xd7, 0x75, 0x30, 0xc7, 0x54, 0x1f, 0xa9, 0x2b, 0xd3, 0x6c, 0xdc, 0x0e
};
          fprintf(stderr, "challenge payload sample 1\n");
#endif
#ifdef ENC_SAMPLE_2
          unsigned char encrypted_payload [256];
          memset(encrypted_payload, 0x17, sizeof(encrypted_payload));
          fprintf(stderr, "challenge payload sample 2\n");
#endif
          memcpy(ctx->challenge_message, encrypted_payload, sizeof(ctx->challenge_message));
        };
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

