/*
  ob-pkoc - PKOC-specific routines

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
#include <openbadger-common.h>
#include <ob-7816.h>
#include <ob-pkoc.h>


int ob_validate_select_response
  (OB_CONTEXT *ctx,
  unsigned char *response,
  int response_length)

{ /* ob_validate_select_response */

  unsigned char *p;
  OB_PKOC_CONTEXT *pkoc_ctx;
  int prot_ver_lth;
  int remainder;
  int status;


  status = ST_OK;
  pkoc_ctx = ctx->pkoc_ctx;
  if (ctx->test_case != OB_TEST_PKOC)
    status = STOB_WRONG_TEST_CASE;

  if (status EQUALS ST_OK)
  {
    p = response;
    remainder = response_length;

    if (response_length < OB_PKOC_SELECT_RESPONSE_MIN)
      status = STOB_SELECT_RESPONSE;
  };
  if (status EQUALS ST_OK)
  {
    // first thing is a "Version" TLV.  Check tag

    if (*p != OB_PKOC_TAG_PROTOCOL_VERSION)
      status = STOB_SELRSP_TAG;
  };
  if (status EQUALS ST_OK)
  {
    p++;
    remainder--;

    // sanity check length

    prot_ver_lth = *p;
    p++;
    remainder--;
    if (ctx->verbosity > 3)
      fprintf(stderr, "Select response: protocol version (%d. octets) is %02X%02X\n",
        prot_ver_lth, *p, *(p+1));
    memcpy(pkoc_ctx->protocol_version, p, prot_ver_lth);
    pkoc_ctx->protocol_version_length = prot_ver_lth;
    p = p + prot_ver_lth;
    remainder = remainder - prot_ver_lth;
status = ST_OK;
  };
  if (status EQUALS ST_OK)
  {
    if (remainder > 2)
    {
      // card nonce

      memcpy(pkoc_ctx->card_arbval, p, 2);
      status = ST_OK;
    };
  };

  return(status);

} /* ob_validate_select_response */

