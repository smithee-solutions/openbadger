/*
  openpkoc - general authenticate (7816) functions

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


#include <PCSC/wintypes.h>
#include <PCSC/pcsclite.h>
#include <PCSC/winscard.h>


#include <ob-crypto.h>
#include <ob-7816.h>
#include <openbadger-common.h>
#include <openbadger-version.h>
#include <ob-pcsc.h>
#include <ob-73-4.h>


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


int ob_challenge_response
    (OB_CONTEXT *ctx)

{ /* ob_challenge_response */

  unsigned char challenge_message [OB_7816_BUFFER_MAX];
  unsigned char dyn_auth_template [8*OB_7816_APDU_PAYLOAD_MAX];  // at least two buffers
  int dyn_auth_template_length;
  unsigned char msg_7816 [OB_7816_BUFFER_MAX];
  int msg_7816_lth;
  unsigned char msg_cla;
  unsigned char msg_ins;
  unsigned char msg_lc;
  unsigned char msg_le;
  unsigned char msg_p1;
  unsigned char msg_p2;
  int part1_length;
  unsigned char response_7816 [2*OB_7816_APDU_PAYLOAD_MAX];
  int response_7816_lth; // not DWORD
  int status;


  status = ST_OK;
  memset(challenge_message, 0, sizeof(challenge_message));
challenge_message [17] = 17;

  // 7816 APDU is a "Dynamic Authenticate"

  dyn_auth_template_length = sizeof(dyn_auth_template);
  memset (dyn_auth_template, 0, sizeof (dyn_auth_template));

  ctx->key_reference = OB_7816_KEY_CARD_AUTHENTICATION;
  ctx->key_size = 2048/8;
  ctx->challenge_type = OB_7816_DYNAUTH_CHALLENGE;

  // set up Dynamic Authenticate (tag 7C)

  dyn_auth_template_length = 0;

  // first is tag for dyn auth obj
  dyn_auth_template [dyn_auth_template_length] = 0x7c;
  dyn_auth_template_length ++;

  // next is TLV length of whole deal
  ob_add_tag_length (dyn_auth_template+dyn_auth_template_length,
    ctx->key_size+2+4, // 2 for response marker, challength tlv
    &dyn_auth_template_length);

  // next is response placeholder
  dyn_auth_template [dyn_auth_template_length] = OB_7816_DYNAUTH_RESPONSE;
  dyn_auth_template_length ++;
  dyn_auth_template [dyn_auth_template_length] = 0;
  dyn_auth_template_length ++;

  // next is challenge with challenge type and data
  dyn_auth_template [dyn_auth_template_length] = 
    ctx->challenge_type;
  dyn_auth_template_length ++;

  // next is TLV length of challenge
  ob_add_tag_length (dyn_auth_template+dyn_auth_template_length,
    ctx->key_size, &dyn_auth_template_length);

  // next is challenge itself (same length as key)
  memcpy ((void *)dyn_auth_template+dyn_auth_template_length,
    (void *)challenge_message, ctx->key_size);
  dyn_auth_template_length = dyn_auth_template_length + ctx->key_size;

  // set up the first 7816 command

  if (dyn_auth_template_length > ctx->apdu_payload_max_7816)
    msg_cla = 0x10;
  else
    msg_cla = 0;
  msg_ins = 0x87;
  msg_p1 = OB_7816_ALGO_REF_RSA2048;
  msg_p2 = ctx->key_reference;

  part1_length = dyn_auth_template_length;
  if (part1_length > ctx->apdu_payload_max_7816)
    part1_length = ctx->apdu_payload_max_7816;
  msg_lc = part1_length;
  msg_le = 0;

  // set up a GENERAL AUTHENTICATE.  See NIST SP800-73-4 part 2 section 3.2.4

  fprintf (stderr, "Setting up GENERAL AUTHENTICATE:\n");
  msg_7816_lth = 0;
{
  unsigned int lc_lth;
  unsigned int payload_lth;
  lc_lth = msg_lc;
  payload_lth = part1_length;
  status = ob_build_7816_message (ctx, msg_cla, msg_ins, msg_p1, msg_p2, lc_lth,
     msg_le, dyn_auth_template, payload_lth, 0, msg_7816, &msg_7816_lth);
};
  response_7816_lth = sizeof (response_7816);
  if (status EQUALS ST_OK)
  {
    status = ob_command_response (ctx, msg_7816, msg_7816_lth, "First GenAuth PDU Request:", "First GenAuth PDU Response:", response_7816, &response_7816_lth);
  };

fprintf(stderr, "assuming response ok... %d. %02X%02x %d.\n", status, response_7816[0], response_7816[1], response_7816_lth);
{
  unsigned int lc_lth;
  unsigned int payload_lth;
  unsigned char *payload;
      msg_cla = 0;
  payload_lth = dyn_auth_template_length - part1_length;
  payload = dyn_auth_template + part1_length;
  fprintf(stderr, "second send: dynauth %d. part2 %d.\n", dyn_auth_template_length, payload_lth);
  msg_lc = payload_lth;
  fprintf(stderr, "total was %d first was %d second is %d\n",
    dyn_auth_template_length, part1_length, msg_lc);
  lc_lth = msg_lc;
  status = ob_build_7816_message (ctx, msg_cla, msg_ins, msg_p1, msg_p2, lc_lth,
     msg_le, payload, payload_lth, 0, msg_7816, &msg_7816_lth);
  if (status EQUALS ST_OK)
  {
    response_7816_lth = sizeof (response_7816);
fprintf(stderr, "7816 cmd %X rsp %X\n", msg_7816_lth, response_7816_lth);
    status = ob_command_response (ctx, msg_7816, msg_7816_lth, "Second GenAuth PDU Request:", "Second GenAuth PDU Response:", response_7816, &response_7816_lth);

{
  DWORD scard_response_length;
  OB_RDRCTX *rdrctx;
  LONG status_pcsc;
  unsigned char get_next [] = {0x00, 0xc0, 0x00, 0x00, 0x00};
  rdrctx = ctx->rdrctx;
  scard_response_length = sizeof(response_7816);
  fprintf (stderr, "---7816 command (get next data)\n");
  ob_dump_buffer (ctx, get_next, sizeof(get_next), 0);
  status_pcsc = SCardTransmit(rdrctx->pcsc, &(rdrctx->pioSendPci), get_next, sizeof(get_next), NULL, response_7816, &scard_response_length);
    fprintf (stderr, "7816 response 3\n");
    ob_dump_buffer (ctx, response_7816, scard_response_length, 0);
fprintf(stderr, "assuming response ok...%02X%02x %d.\n", response_7816[0], response_7816[1], response_7816_lth);

#if 0
  fprintf(stderr, "response 4\n");
  scard_response_length = sizeof(response_7816);
  status_pcsc = SCardTransmit(rdrctx->pcsc, &(rdrctx->pioSendPci), get_next, sizeof(get_next), NULL, response_7816, &scard_response_length);
    fprintf (stderr, "7816 response 4\n");
    ob_dump_buffer (ctx, response_7816, scard_response_length, 0);
#endif
};

  };
};

  return(status);

} /* ob_challenge_response */


int ob_command_response
    (OB_CONTEXT *ctx,
    unsigned char *x7816_buffer,
    int x7816_lth,
    char *prefix_string,
    char *suffix_string,
    unsigned char *r7816_buffer,
    int *r7816_lth)

{ /* ob_command_response */

  OB_RDRCTX *rdrctx;
  LONG status_pcsc;
  DWORD scard_response_length;
  int status;


  if (ctx->verbosity > 3)
    fprintf(stderr, "ob_command_response: top\n");
  status = ST_OK;
  rdrctx = ctx->rdrctx;
  if (ctx->verbosity > 3)
  {
    fprintf (stderr, "--sending 7816 command %s\n", prefix_string);
    ob_dump_buffer (ctx, x7816_buffer, x7816_lth, 0);
  };
  scard_response_length = *r7816_lth;
  status_pcsc = SCardTransmit(rdrctx->pcsc, &(rdrctx->pioSendPci), x7816_buffer, x7816_lth, NULL, r7816_buffer, &scard_response_length);
  *r7816_lth = scard_response_length;
  if (status_pcsc != SCARD_S_SUCCESS)
  {
    rdrctx->last_pcsc_status = status_pcsc;
    status = STOB_PCSC_TRANSMIT_2;
  };
  fprintf(stderr, "back from SCardTransmit, status %d. %lX\n", status, rdrctx->last_pcsc_status);
  if (status EQUALS ST_OK)
  {
    fprintf (stderr, "7186 response %s (%d. bytes)\n", suffix_string, *r7816_lth);
    ob_dump_buffer(ctx, r7816_buffer, *r7816_lth, 0);
  };
  if (ctx->verbosity > 3)
    fprintf(stderr, "ob_command_response: bottom\n");
  return (status);

} /* ob_command_response*/

