/*
  obtest-pkoc - - test PKOC challenge/response 

  generates on stdout a command appropriate to cause
  libosdp-conformance to emit an osdp_RAW message via OSDP

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
#include <ob-pcsc.h>
#include <ob-pkoc.h>
#include <openbadger-version.h>
int ob_initialize_pubkey_DER(OB_CONTEXT *ctx, unsigned char *key_buffer, int kblth);
int ob_initialize_signature_DER(OB_CONTEXT *ctx, unsigned char *part_1, int p1lth, unsigned char *part_2, int p2lth);

// skeletons of DER structures

// DER prefix specifically for NIST P256R1

unsigned char ec_public_key_der_skeleton [] =
{
  0x30,0x59,0x30,0x13,0x06,0x07,0x2a,0x86,0x48,0xce,0x3d,0x02,0x01,0x06,0x08,0x2a,0x86,0x48,0xce,0x3d,0x03,0x01,0x07,0x03,0x42,0x00
  // actual 65 byte key goes after this
};
unsigned char ec_signature_der_skeleton_1 [] =
{
  0x30,0x44,0x02,0x20,0x00 
// then 32 of part 1
};
unsigned char ec_signature_der_skeleton_2 [] =
{
  0x02,0x20,0x00 
// then 32 of part 2
};


int main
  (int argc,
  char *argv [])

{ /* main for obtest-pkoc */

  OB_CONTEXT *ctx;
  DWORD dwRecvLength;
  int index_lc;
  unsigned char msg_cla;
  unsigned char msg_ins;
  unsigned char msg_p1;
  unsigned char msg_p2;
  unsigned char msg_lc;
  unsigned char msg_le;
  BYTE pbRecvBuffer [2*OB_7816_APDU_PAYLOAD_MAX];
  OB_RDRCTX pcsc_reader_context;
  OB_RDRCTX *rdrctx;
  OB_CONTEXT test_pkoc_context;
  OB_PKOC_CONTEXT pkoc_context;
  unsigned char smartcard_command [OB_7816_BUFFER_MAX];
  int smartcard_command_length;
  int status;
  LONG status_pcsc;
  char verify_command [1024];


  ctx = &test_pkoc_context;
  memset(ctx, 0, sizeof(*ctx));
  ctx->test_case = OB_TEST_PKOC;
  ctx->rdrctx = &pcsc_reader_context;
  ctx->credential_context = &pkoc_context;

  status = ob_read_settings(ctx);
fprintf(stderr, "DEBUG: settings?\n");

  rdrctx = ctx->rdrctx;
  smartcard_command_length = 0;

  fprintf(stderr, "obtest PKOC tester %s\n", OPENBADGER_VERSION);
// OPENBADGER_VERSION

  fprintf(stderr, "Reader %d.\n", ctx->reader_index);
  fprintf(stderr, "PD Control %s Verbosity %d.\n", ctx->pd_control, ctx->verbosity);

  status = ob_init_smartcard(ctx);

  // test cases: 1: test vectors, 2: PIV, 3: PKOC

  if (status EQUALS ST_OK)
  {
    switch(ctx->test_case)
    {
    case OB_TEST_VECTORS:
      fprintf(stderr, "Test vectors in use, no command sent.\n");
      break;
    case OB_TEST_PKOC:
      memcpy(smartcard_command, SELECT_PKOC, sizeof(SELECT_PKOC));
      smartcard_command_length = sizeof(SELECT_PKOC);
      break;
    default:
      status= STOB_UNKNOWN_TEST_CASE;
      break;
    };
  };

  if (ctx->verbosity > 3)
  {
    ob_dump_buffer(ctx, smartcard_command, smartcard_command_length, 0);
  };

  // send application select to card

  dwRecvLength = sizeof(pbRecvBuffer);
  status_pcsc = SCardTransmit(rdrctx->pcsc, &rdrctx->pioSendPci, smartcard_command, smartcard_command_length, NULL, pbRecvBuffer, &dwRecvLength);
  rdrctx->last_pcsc_status = status_pcsc;
  if (SCARD_S_SUCCESS != status_pcsc)
    status = STOB_PCSC_TRANSMIT_1;
  if (status EQUALS ST_OK)
  {
    if (ctx->verbosity > 3)
    {
      fprintf(stderr, "Select returns: ");
      ob_dump_buffer (ctx, pbRecvBuffer, dwRecvLength, 0);
    };

    status = ob_validate_select_response(ctx, pbRecvBuffer, dwRecvLength);
  };

  if (status EQUALS ST_OK)
  {
    // set up authentication command

    msg_cla = 0x80;
    msg_ins = 0x80;
    msg_p1 = 0x00;
    msg_p2 = 0x01;
    msg_lc = 0x00;
    msg_le = 0x00;
    smartcard_command_length = 0;
    memset(smartcard_command, 0, sizeof(smartcard_command));
    smartcard_command [smartcard_command_length] = msg_cla;
    smartcard_command_length++;
    smartcard_command [smartcard_command_length] = msg_ins;
    smartcard_command_length++;
    smartcard_command [smartcard_command_length] = msg_p1;
    smartcard_command_length++;
    smartcard_command [smartcard_command_length] = msg_p2;
    smartcard_command_length++;
    // fill in Lc later
    index_lc = smartcard_command_length;
    smartcard_command_length++;
    
    // tag,length,value - protocol version

    smartcard_command [smartcard_command_length] = OB_PKOC_TAG_PROTOCOL_VERSION;
    smartcard_command_length++;
    smartcard_command [smartcard_command_length] = 2;
    smartcard_command_length++;
    memcpy(smartcard_command+smartcard_command_length, pkoc_context.protocol_version, 2);
    smartcard_command_length = smartcard_command_length + 2;
    
    // tag,length,value - transaction id

    smartcard_command [smartcard_command_length] = OB_PKOC_TAG_TRANSACTION_IDENTIFIER;
    smartcard_command_length++;
    smartcard_command [smartcard_command_length] = 0x10;
    smartcard_command_length++;
#ifdef XTN_IDENT_TEST
    memset(pkoc_context.transaction_identifier, 0x17, OB_PKOC_TRANSACTID_LENGTH);
#endif
#ifdef XTN_IDENT_SPEC
    memcpy(pkoc_context.transaction_identifier, spec_identifier, OB_PKOC_TRANSACTID_LENGTH);
#endif
    memcpy(smartcard_command+smartcard_command_length, pkoc_context.transaction_identifier, OB_PKOC_TRANSACTID_LENGTH);
    fprintf(stderr, "Transaction Identifer:\n");
    ob_dump_buffer (ctx, smartcard_command+smartcard_command_length, OB_PKOC_TRANSACTID_LENGTH, 0);
    smartcard_command_length = smartcard_command_length + OB_PKOC_TRANSACTID_LENGTH;
    
    // tag,length,value - reader identifier

    smartcard_command [smartcard_command_length] = OB_PKOC_TAG_READER_IDENTIFIER;
    smartcard_command_length++;
    smartcard_command [smartcard_command_length] = 0x20;
    smartcard_command_length++;
    memset(pkoc_context.site_key_identifier, 'S', OB_PKOC_SITE_KEY_IDENTIFIER_LENGTH);
    memcpy(smartcard_command+smartcard_command_length, pkoc_context.site_key_identifier, OB_PKOC_SITE_KEY_IDENTIFIER_LENGTH);
    smartcard_command_length = smartcard_command_length + 0x10;
    memset(pkoc_context.reader_key_identifier, 'r', OB_PKOC_READER_KEY_IDENTIFIER_LENGTH);
    memcpy(smartcard_command+smartcard_command_length, pkoc_context.reader_key_identifier, OB_PKOC_READER_KEY_IDENTIFIER_LENGTH);
    smartcard_command_length = smartcard_command_length + 0x10;
    msg_lc = smartcard_command_length - index_lc - 1;
    smartcard_command [index_lc] = msg_lc;
    smartcard_command [smartcard_command_length] = msg_le;
    smartcard_command_length++;

    fprintf(stderr, "Marshalled Authentication Command:\n");
    ob_dump_buffer (ctx, smartcard_command, smartcard_command_length, 0);
  };

  if (status EQUALS ST_OK)
  {
    // send authentication command to card

    dwRecvLength = sizeof(pbRecvBuffer);
    status_pcsc = SCardTransmit(rdrctx->pcsc, &rdrctx->pioSendPci, smartcard_command, smartcard_command_length, NULL, pbRecvBuffer, &dwRecvLength);
    rdrctx->last_pcsc_status = status_pcsc;
    if (SCARD_S_SUCCESS != status_pcsc)
      status = STOB_PKOC_AUTH;
  };
  if (status EQUALS ST_OK)
  {
    fprintf(stderr, "Authentication returns:\n");
    ob_dump_buffer (ctx, pbRecvBuffer, dwRecvLength, 0);
  };
  if (status EQUALS ST_OK)
  {
    unsigned char *p;
    int payload_size;
    int remainder;

    // extract the signature and public key.  in tlv, arbitrary order.

    p = pbRecvBuffer;
    remainder = dwRecvLength;
    if (*p EQUALS OB_PKOC_TAG_UNCOMP_PUBLIC_KEY)
    {
      p++;
      remainder--;
      payload_size = *p;
      p++;
      remainder--;

      memcpy(ctx->ec_public_key, p, payload_size);
      p = p + payload_size;
      remainder = remainder - payload_size;
    };
    if (*p EQUALS OB_PKOC_TAG_DIGITAL_SIGNATURE)
    {
      p++;
      remainder--;
      payload_size = *p;
      p++;
      remainder--;

      memcpy(ctx->pkoc_signature, p, payload_size);
      p = p + payload_size;
      remainder = remainder - payload_size;
    };
    if (*p EQUALS OB_PKOC_TAG_UNCOMP_PUBLIC_KEY)
    {
      p++;
      remainder--;
      payload_size = *p;
      p++;
      remainder--;

      memcpy(ctx->ec_public_key, p, payload_size);
      p = p + payload_size;
      remainder = remainder - payload_size;
    };
    fprintf(stderr, "Public Key:\n");
    ob_dump_buffer (ctx, ctx->ec_public_key, OB_PKOC_PUBKEY_LENGTH, 0);
    fprintf(stderr, "Signature:\n");
    ob_dump_buffer (ctx, ctx->pkoc_signature, 64, 0);

    // output a DER-formatted copy of the public key.
    status = ob_initialize_pubkey_DER(ctx, ctx->ec_public_key, OB_PKOC_PUBKEY_LENGTH);
    if (status EQUALS ST_OK)
      fprintf(stderr, "file %s created\n", OBTEST_PKOC_PUBLIC_KEY);
  };
  if (status EQUALS ST_OK)
  {
    // output a DER-formatted copy of the signature.
    status = ob_initialize_signature_DER(ctx, ctx->pkoc_signature, 32, ctx->pkoc_signature+32, 32);
    if (status EQUALS ST_OK)
      fprintf(stderr, "file ec-sig.der created\n");
  };
  if (status EQUALS ST_OK)
  {
    FILE *tbs_blob;

    // write the to-be-signed blob to disk and verify the signature.  just the transaction identifier.

    tbs_blob = fopen("tbs-pkoc.bin", "w");
    fwrite(pkoc_context.transaction_identifier, 1, OB_PKOC_TRANSACTID_LENGTH, tbs_blob);
    fclose(tbs_blob);

    /*
      having constructed a proper DER-formatted signature and a proper DER-formatted copy of the public key extracted from the card,
      use openssl to perform an ECDSA signature verification operation.
    */

    sprintf(verify_command, "openssl version;openssl dgst -sha256 -verify %s -signature ec-sig.der tbs-pkoc.bin", OBTEST_PKOC_PUBLIC_KEY);
    if (ctx->verbosity > 3)
      fprintf(stderr, "verify command: %s\n", verify_command);
    if (ctx->verbosity > 0)
      fprintf(stderr, "Checking signature with openssl\n");
    system(verify_command);
  };

  if (status EQUALS ST_OK)
  {
    char command [1024];
    int i;
    char octet_string [3];
    char osdp_command [2048];
    unsigned char raw_key [64];
    FILE *resp;
    int return_size;

    fprintf(stderr, "Public Key Open Credential:\n");
    memset(raw_key, 0, sizeof(raw_key));
    if (ctx->bits_to_return EQUALS 128)
    {
      memcpy(raw_key, ctx->ec_public_key+16, 128/8);
      return_size = 128/8;
    }
    else
    {
      memcpy(raw_key, ctx->ec_public_key, sizeof(raw_key));
      return_size = 64;
    }

    ob_dump_buffer (ctx, raw_key, sizeof(raw_key), 0);
    sprintf(osdp_command, "{\"command\":\"present-card\",\"format\":\"raw\",\"bits\":\"%d\",\"raw\":\"", ctx->bits_to_return);

    for (i=0; i<return_size; i++)
    {
      sprintf(octet_string, "%02X", raw_key [i]);
      strcat(osdp_command, octet_string);
    };
    strcat(osdp_command, "\"}");
    if (ctx->verbosity > 3)
      fprintf(stderr, "OSDP Response will be:\n%s\n", osdp_command);
    if (ctx->verbosity > 0)
      fprintf(stderr, "Issuing OSDP PD response.\n");
    resp = fopen("pkoc-read.json", "w");
    fprintf(resp, "%s", osdp_command);
    fclose(resp);
    sprintf(command, "/opt/osdp-conformance/bin/open-osdp-kick PD <pkoc-read.json");
    system(command);
  };

  if (status != ST_OK)
    fprintf(stderr, "return status %d. last PCSC status %lX %s\n", status, rdrctx->last_pcsc_status, ob_pcsc_error_string(rdrctx->last_pcsc_status));
  return(status);

} /* main for obtest-pkoc */



int ob_initialize_pubkey_DER
  (OB_CONTEXT *ctx,
  unsigned char *key_buffer,
  int kblth)

{ /* ob_intiialize_pubkey_DER */

  FILE *ec_der_key;


  ec_der_key = fopen(OBTEST_PKOC_PUBLIC_KEY, "w");
  fwrite(ec_public_key_der_skeleton, 1, sizeof(ec_public_key_der_skeleton), ec_der_key);
  fwrite(key_buffer, 1, kblth, ec_der_key);
  fclose(ec_der_key);
  return(ST_OK);

} /* ob_intiialize_pubkey_DER */



int ob_initialize_signature_DER
  (OB_CONTEXT *ctx,
  unsigned char *part_1,
  int part1lth,
  unsigned char *part_2,
  int part2lth)

{ /* ob_initialize_signature_DER */

  FILE *ec_der_sig;
  int lth;
  int whole_length;


  // if the pieces have the high order bit set insert a null byte

  // fiddle the outer length accordingly

  whole_length = 32 + 32 + 4;
  if (0x80 & *part_1)
  {
    whole_length++;
    ec_signature_der_skeleton_1 [3] = 0x21;
    if (ctx->verbosity > 9)
      fprintf(stderr, "part 1 first octet %02X\n", *part_1);
  };
  if (0x80 & *part_2)
  {
    whole_length++;
    ec_signature_der_skeleton_2 [1] = 0x21;
    if (ctx->verbosity > 9)
      fprintf(stderr, "part 2 first octet %02X\n", *part_2);
  };
  ec_signature_der_skeleton_1 [1] = whole_length;

  ec_der_sig = fopen("ec-sig.der", "w");
  lth = sizeof(ec_signature_der_skeleton_1);
  if (!(0x80 & *part_1))
    lth--;
  if (ctx->verbosity > 9)
    fprintf(stderr, "part 1 write length %d.\n", lth);
  fwrite(ec_signature_der_skeleton_1, 1, lth, ec_der_sig);
  fwrite(part_1, 1, part1lth, ec_der_sig);
  lth = sizeof(ec_signature_der_skeleton_2);
  if (!(0x80 & *part_2))
    lth--;
  if (ctx->verbosity > 9)
    fprintf(stderr, "part 2 write length %d.\n", lth);
  fwrite(ec_signature_der_skeleton_2, 1, lth, ec_der_sig);
  fwrite(part_2, 1, part2lth, ec_der_sig);
  fclose(ec_der_sig);
  return(ST_OK);

} /* ob_initialize_signature_DER */


#if 0
//#define XTN_IDENT_SPEC
#define XTN_IDENT_TEST
unsigned char spec_identifier [] = {
  0x6f, 0xcf, 0x50, 0x12,  0xb2, 0x24, 0x04, 0x3b,
  0x09, 0x35, 0x0a, 0x4f,  0xc5, 0xe5, 0x6a, 0x8f
};


int ob_read_settings(OB_CONTEXT *ctx);







#endif

