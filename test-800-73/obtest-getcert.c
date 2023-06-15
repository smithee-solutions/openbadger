/*
  obtest-getcert - get a certificate from an 800-73 card.

  Usage:
    obtest-getcert <readernumber>

  readernumber is which reader to use.

  It saves the cert in obtest-retrieved-cert.der

  Uses ./obtest-settings.json and /opt/tester/etc/obtest-settings.json

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
unsigned char ob_test_piv [] = OB_PIV_APPLICATION_IDENTIFIER;
BYTE command_getdata_card_auth_cert [] = { 0x00, 0xCB, 0x3F, 0xFF, 0x05, 0x5C, 0x03, 0x5F, 0xC1, 0x01, 0x00 };
BYTE command_get_results [] = { 0x00, 0xC0, 0x00, 0x00, 0x00 };
#define OBTEST_GETCERT_RESULTS ("obtest-retrieved-cert.der")


int obtest_init_settings
  (OB_CONTEXT *ctx)

{ /* obtest_init_settings */

  int status;


  status = ST_OK;

  // read ./obtest-settings.json if it's there

  // read /opt/tester/etc/obtest-settings.json as an alt source

  ctx->verbosity = 9;
  ctx->test_case = OB_TEST_GETCERT;
  ctx->der_out = fopen(OBTEST_GETCERT_RESULTS, "w");
  if (NULL EQUALS ctx->der_out)
    status = STOB_INIT_FILES;

  return(status);

} /* obtest_init_settings */


int main
  (int argc,
  char *argv [])

{ /* main for obtest-getcert */

  OB_CONTEXT *ctx;
  DWORD dwRecvLength;
  int more;
  BYTE pbRecvBuffer [2*OB_7816_APDU_PAYLOAD_MAX]; // probably actually 1x plus 2
  OB_RDRCTX pcsc_reader_context;
  int rdr;
  OB_RDRCTX *rdrctx;
  unsigned char smartcard_select_command [OB_7816_BUFFER_MAX];
  int smartcard_select_command_length;
  int status;
  LONG status_pcsc;
  unsigned char status_7816_sw1;
  unsigned char status_7816_sw2;
  OB_CONTEXT test_piv_context;


  ctx = &test_piv_context;
  memset(ctx, 0, sizeof(*ctx));
  ctx->rdrctx = &pcsc_reader_context;
  rdrctx = ctx->rdrctx;
  smartcard_select_command_length = 0;
  ctx->apdu_payload_max_7816 = OB_7816_APDU_PAYLOAD_MAX;
  status = obtest_init_settings(ctx);
  if (argc > 1)
  {
    rdr = *(0+(argv[1]));
    ctx->reader_index = rdr - 0x30;
    fprintf(stderr, "Reader %d.\n", ctx->reader_index);
  };
  fprintf(stderr, "obtest-challenge %s\n", OPENBADGER_VERSION);

  if (status EQUALS ST_OK)
  {
    status = ob_init_smartcard(ctx);
  };

  if (status EQUALS ST_OK)
  {

  // send application select to card

  memcpy(smartcard_select_command, ob_test_piv, sizeof(ob_test_piv));
  smartcard_select_command_length = sizeof(ob_test_piv);
  if (ctx->verbosity > 9)
  {
    ob_dump_buffer (ctx, smartcard_select_command, smartcard_select_command_length, 0);
  };
  dwRecvLength = sizeof(pbRecvBuffer);
  status_pcsc = SCardTransmit(rdrctx->pcsc, &rdrctx->pioSendPci, smartcard_select_command, smartcard_select_command_length, NULL, pbRecvBuffer, &dwRecvLength);
  rdrctx->last_pcsc_status = status_pcsc;
  if (SCARD_S_SUCCESS != status_pcsc)
    status = STOB_PCSC_TRANSMIT_1;
  };
  if (status != ST_OK)
  {
    fprintf(stderr, "Select returns: ");
    ob_dump_buffer (ctx, pbRecvBuffer, dwRecvLength, 0);
  }
  else
  {
    fprintf(stderr, "Select succeeded.\n");
  };

  if (status EQUALS ST_OK)
  {
    // ask for the card auth cert

    dwRecvLength = sizeof(pbRecvBuffer);
    if (ctx->verbosity > 3)
      ob_dump_buffer(ctx, command_getdata_card_auth_cert, sizeof (command_getdata_card_auth_cert), 0);
    status_pcsc = SCardTransmit(rdrctx->pcsc, &rdrctx->pioSendPci, command_getdata_card_auth_cert, sizeof(command_getdata_card_auth_cert), NULL, pbRecvBuffer, &dwRecvLength);
    if (SCARD_S_SUCCESS != status_pcsc)
      status = STOB_PIV_GETCERT;
    if (ctx->verbosity > 3)
    {
      fprintf(stderr, "First getcert response ");
      ob_dump_buffer (ctx, pbRecvBuffer, dwRecvLength, 0);
    };
  };
  if (status EQUALS ST_OK)
  {
int offset;
    // buffer first (or only) chunk of response

    offset = 8; // kludge to skip to cert itself (the -2 is for the 7816 status)

    fwrite(offset+pbRecvBuffer, sizeof(pbRecvBuffer[0]), dwRecvLength-offset-2, ctx->der_out);

    // if there's more, loop

    more = 0;
    status_7816_sw1 = pbRecvBuffer [dwRecvLength-2];
    status_7816_sw2 = pbRecvBuffer [dwRecvLength-1];
    if ((status_7816_sw1 EQUALS 0x61) && (status_7816_sw2 EQUALS 0))
      more = 1;
    if (more)
    {
      fprintf(stderr, "Retrieving next part of certificate.\n");
    };
    while (more)
    {
      dwRecvLength = sizeof (pbRecvBuffer);
      status_pcsc = SCardTransmit(rdrctx->pcsc, &rdrctx->pioSendPci, command_get_results, sizeof(command_get_results), NULL, pbRecvBuffer, &dwRecvLength);
      if (SCARD_S_SUCCESS != status_pcsc)
        status = STOB_GETCERT_FRAGMENT;
      if (ctx->verbosity > 3)
      {
        fprintf(stderr, "Last 2: %02x %02x\n", pbRecvBuffer [dwRecvLength-2], pbRecvBuffer [dwRecvLength-1]);
      };
      if (status EQUALS ST_OK)
      {
        fwrite(pbRecvBuffer, sizeof(pbRecvBuffer[0]), dwRecvLength-2, ctx->der_out);
        fprintf(stderr, "Next getcert response ");
        ob_dump_buffer (ctx, pbRecvBuffer, dwRecvLength, 0);
        if (0x61 != pbRecvBuffer [dwRecvLength-2])
          more = 0;
      };
      if (status != ST_OK)
        more = 0;
    };
  };

  if (status != ST_OK)
  {
    fprintf(stderr, "obtest-getcert completed with status %d. (PCSC status %lX\n", status, rdrctx->last_pcsc_status);
  };
  return(status);

} /* main for obtest-getcert */

#if MAYBE_THIS_CODE
      // buffer first chunk of response
      card_buffer_length = dwRecvLength-2;
      cbptr = &(card_buffer [0]);
      memcpy (cbptr, pbRecvBuffer, dwRecvLength-2);
      cbptr = cbptr + dwRecvLength-2;


          wlth = dwRecvLength - 2;
          if (lscard_config.verbosity > 3)
            dump_buffer (&lscard_config, pbRecvBuffer, dwRecvLength, 0);
          card_buffer_length = card_buffer_length + wlth;
          memcpy (cbptr, pbRecvBuffer, wlth);
          cbptr = cbptr + wlth;
          if (lscard_config.verbosity > 3)
            fprintf (stderr, "card buffer length %d\n", card_buffer_length);
        };
      };

      if (lscard_config.verbosity > 3)
      {
        fprintf (stderr, "final card buffer length %d\n", card_buffer_length);
      };
status = dump_card_data (&lscard_config, card_buffer, card_buffer_length);
      if (lscard_config.final_object_length > 0)
      {
        if (lscard_config.verbosity > 0)
          fprintf (stderr, "Writing Card Auth Cert to card_auth_cert.bin\n");
        f = open ("card_auth_cert.bin", O_CREAT|O_RDWR, 0777);
        status_io = write (f, lscard_config.final_object, lscard_config.final_object_length);
        fprintf (stdout, "created card_auth_cert.bin, %d bytes (status errno %d.)\n", status_io, errno);
        close (f);
      };
    };
  };

  // get the PIV Auth Cert if requested

  if (status EQUALS ST_OK)
  {
    if (action_list & MASK_GET_PIV_AUTH_CERT)
    {
      if (lscard_config.verbosity > 0)
        fprintf (stderr, "Requesting PIV Auth Certificate from credential\n");

      dwRecvLength = sizeof(pbRecvBuffer);
      if (lscard_config.verbosity > 3)
      {
        fprintf (stderr, "command to get piv auth cert:\n"); fflush (stdout);
        dump_buffer
         (&lscard_config, command_getdata_piv_auth_cert, sizeof (command_getdata_piv_auth_cert), 0);
      };
      rv = SCardTransmit (lscard_config.pcsc, &lscard_config.pioSendPci,
        command_getdata_piv_auth_cert, sizeof(command_getdata_piv_auth_cert),
        NULL, pbRecvBuffer, &dwRecvLength);
CHECK("SCardTransmit", rv)

      if (0x61 == pbRecvBuffer [dwRecvLength-2])
        if (0x00 == pbRecvBuffer [dwRecvLength-1])
        {
          cbptr = &(card_buffer [0]);
          memcpy (cbptr, pbRecvBuffer, dwRecvLength-2);
          cbptr = cbptr + dwRecvLength-2;
          card_buffer_length = dwRecvLength-2;
          status = get_response_multipart (&lscard_config, card_buffer, &card_buffer_length);
        };

      if (lscard_config.verbosity > 3)
      {
        fprintf (stderr, "PIV Authentication Certificate response:\n");
        dump_buffer (&lscard_config, card_buffer, card_buffer_length, 0);
      };
status = dump_card_data (&lscard_config, card_buffer, card_buffer_length);
      if (lscard_config.final_object_length > 0)
      {
        if (lscard_config.verbosity > 0)
          fprintf (stderr, "Writing PIV Auth Cert to piv_auth_cert.bin\n");
        f = open ("piv_auth_cert.bin", O_CREAT|O_RDWR, 0777);
        status_io = write (f, lscard_config.final_object, lscard_config.final_object_length);
        fprintf (stdout, "created piv_auth_cert.bin, %d bytes (status errno %d.)\n", status_io, errno);
        close (f);
      };
    };
  };


#endif

