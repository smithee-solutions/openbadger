/*
  divutil - Diversified Key Utility

  (C)Copyright 2017-2023 Smithee Solutions LLC

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

#include <jansson.h>
#include <aes.h>


#include <openbadger.h>
#define EQUALS ==
#define OB_STRING_MAX (1024)
#define OB_KEY_SIZE_10957 (128/8)
#define OB_UID_SIZE (56/8)
typedef struct
{
  int verbosity;
  int uid_size;
  unsigned char secret_key [OB_KEY_SIZE_10957];
  int action;
} OB_CONTEXT;
#define LOG stdout

unsigned char expected_k0 [OB_KEY_SIZE_10957] =
  {0x67, 0x04, 0xA3, 0xAF};


int
  aes_encrypt
    (OB_CONTEXT *ctx,
    unsigned char *plaintext,
    unsigned char *ciphertext,
    unsigned char *key,
    int *length)
{return(-1);}

int
  aes_initialize
    (OB_CONTEXT *ctx)
{return(-1);}


int
  init_parameters
    (OB_CONTEXT *ctx)

//    (OES_PACS_DATA_OBJECT *acdo, OES_KEY_MATERIAL *k, char *parameter_file)

{ /* init_parameters */

  ctx->uid_size = OB_UID_SIZE;
  return(-1);

} /* init_parameters */


char
  *string_hex_buffer
    (unsigned char *buf,
    int buf_lth)

{ /* string_hex_to_buffer */

  static char returned_string [OB_STRING_MAX];

  strcpy(returned_string, "t.b.d.");
  return(returned_string);

} /* string_hex_to_buffer */


int
  main
    (int argc,
    char *argv [])

{ /* main for divutil */

  int comparison;
  OB_CONTEXT *ctx;
  OB_CONTEXT divutil_context;
  int done;
  int encrypted_length;
  int found_something;
  unsigned char k0 [OB_KEY_SIZE_10957];
  unsigned char k0_plaintext [OB_KEY_SIZE_10957];
  int longindex;
  char optstring [OB_STRING_MAX];
  unsigned char uid [OB_UID_SIZE];
  int status;
  int status_opt;
  struct option
    longopts [] = {
      {"details", required_argument, &(divutil_context.action), OB_DETAILS},
      {"help", 0, &(divutil_context.action), OB_HELP},
      {"selftest", 0, &(divutil_context.action), OB_SELFTEST},
      {"settings", required_argument, &(divutil_context.action), OB_SETTINGS},
      {"verbosity", 0, &(divutil_context.action), OB_VERBOSITY},
      {0, 0, 0, 0}
    };


  ctx = &divutil_context;
  memset(ctx, 0, sizeof(*ctx));
  status = ST_OK;
  found_something = 0;
  memset(uid, 0, sizeof(uid));

  done = 0;
  while (!done)
  {
    status_opt = getopt_long (argc, argv, optstring, longopts, &longindex);
    if (!found_something)
      if (status_opt EQUALS -1)
        ctx->action = OB_HELP;
    switch (ctx->action)
    {
    case OB_DETAILS:
fprintf(stderr, "DEBUG: json load as details\n");
// load value "uid", infer length from decode of hex
// load value "secret-key", must be 128 bit value
      status = ST_OK;
      found_something = 1;
      break;
    case OB_NOOP:
      // stay silent if looping around after an option was found.
      break;
    case OB_SELFTEST:
// copy defaults into parameters
      fprintf(LOG, "Self-test selected, loading parameters.\n");
      status = ST_OK;
      found_something = 1;
      break;
    case OB_SETTINGS:
fprintf(stderr, "DEBUG: json load /opt/tester/etc/openbadger-settings.json or local copy\n");
// load value "uid-default" with value from AN-10957
// load value "secret-key-deault" with value from AN-109570
      status = ST_OK;
      found_something = 1;
      break;

    case OB_HELP:
      found_something = 1;
      // fall through to default on purpose
    default:
      fprintf(LOG, "--help - display this help text.\n");
      fprintf(LOG, "--verbosity (min 1 max 9)\n");
      fprintf(LOG, "--details <json file> - details for this calculation\n");
      fprintf(LOG, "--settings <json file> - configured settings for the tool\n");
      fprintf(LOG, "--selftest - use the values in AN-10957\n");
      status = STOB_NO_ARGUMENTS;
      break;
    };
    ctx->action = OB_NOOP; // reset from whatever getopt_long set it to
    if (status_opt EQUALS -1)
      done = 1;
  };
  if (status EQUALS ST_OK)
  {
  fprintf(LOG, "Secret Key: %s\n", string_hex_buffer(ctx->secret_key, OB_KEY_SIZE_10957));
  fprintf(LOG, "       UID: %s\n", string_hex_buffer(uid, ctx->uid_size));

  status = aes_initialize(ctx);
  if (status EQUALS ST_OK)
  {
    fprintf(LOG, "---> Step 1 Generate K0 <---\n");
    memset(k0_plaintext, 0, sizeof(k0_plaintext));
    encrypted_length = sizeof(k0_plaintext);
    status = aes_encrypt(ctx, k0_plaintext, k0, ctx->secret_key, &encrypted_length);
    if (status != ST_OK)
      fprintf(LOG, "encrypt K0 failed.\n");
  };
  if (status EQUALS ST_OK)
  {
    if (ctx->action EQUALS OB_SELFTEST)
    {
      comparison = memcmp(expected_k0, k0, sizeof(k0));
      if (!comparison)
        fprintf(LOG, "K0 mismatch:\n   Calc %s\nExpected %s\n",
          string_hex_buffer(k0, sizeof(k0)),
          string_hex_buffer(expected_k0, sizeof(k0)));
    };
  };

// generate K0 - aes-128 bytes of 0 then aes encrypt (secret_key, buffer);
// expected value 0x067...
// generate K1: of msb k0 0 then k1=k0 <<1
// else K1 is (K0 <<1) xor 0x00...87
// generate K2: if msb K1 - then K2=K1<<1
// else K2 = (K1 <<1) xori 0x00...87
// div_input (31 bytes)  is div constant 0x01 plus UID plus 0x80 padding (aes-128 bits?)
// xor K2 with div input
// encrypt with secret key
// take last 16 byte block of enc result
  };

  if (status != 0)
    fprintf(LOG, "divutil exit status %d.\n", status);

  return(status);

} /* main for divutil */


#if 0
unsigned char DEFAULT_CUSTOMER_ID [] = { 0x00, 0x00, 0x12, 0x34 };


void
  display_acdo
  (FILE *log,
  OES_CARD_IDENTIFIER_OBJECT *cio,
  OES_PACS_DATA_OBJECT *data)

{ /* display_acdo */

  unsigned char *swapped;

  fprintf(log, "--CIO\n");
  fprintf(log, "  Manufacturer: %s\n",
    cio->manufacturer);
  fprintf(log,
    "  MutualAuth %04x Enc %02x Cust %02x%02x%02x%02x KeyVer %02x\n",
    cio->mutual_authentication_mode, cio->authentication_flag,
    cio->customer_id [0], cio->customer_id [1],
    cio->customer_id [2], cio->customer_id [3],
    cio->key_version);
  fprintf(log, "  Signature: ");
  dump_hex(log, cio->signature, sizeof(cio->signature), 1);
  fprintf(log, "\n");

  if (data->oes_format)
    fprintf(log, "--ACDO\n");
  else
    fprintf(log, "--PACS Data Object:\n");
  fprintf(log, "  Site: ");
  dump_hex(log, data->customer_site_code, data->customer_site_code_length, 1);
  fprintf(log, " ID: ");
  dump_hex(log, data->credential_id, data->credential_id_length, 1);
  fprintf(log, "\n");
  fprintf(log, "  Version: %d.%d", data->version_major, data->version_minor);
  fprintf(log, " Format: %02x", data->data_format);
  fprintf(log, " Cred Ver: ");
  dump_hex(log, data->credential_version, data->credential_version_length, 1);
  fprintf(log, "\n");
  if (data->reissue_present)
  {
    fprintf (log, " Reissue: 0x%lu ", (unsigned long)(data->reissue));
  };
  if (data->pin_present)
  {
    fprintf (log, "            PIN: 0x%lu ", (unsigned long)(data->pin));
  };
  if (data->customer_data_length > 0)
  {
    fprintf(stderr, "  Cust data: ");
    dump_hex(log, data->customer_data, data->customer_data_length, 1);
    fprintf (log, "\n");
  };
  fprintf (log, "  Signature: ");
  dump_hex(log, data->signature, sizeof(data->signature), 1);
  fprintf (log, "\n");

  /*
    now display the ACDO on stdout 
    format is AN10957 (oes_format=0) or INID (oes_format=1)
  */
  if (data->oes_format)
    printf("ACDO-Data: ");
  else
    printf("PACS-Data: ");

  // version
  printf("%02x%02x", data->version_major, data->version_minor);

  // format("magic") and length if OES
  if (data->oes_format)
    printf("%02x%04x", data->data_format, htons(data->oes_data_length));

  // facility, cardholder, card version
  if (data->oes_format)
  {
    swapped = network_short(data->customer_site_code_length);
    printf("%02x%02x%02x", OES_TAG_FACILITY_CODE, swapped [0], swapped [1]);
  };
  dump_hex(stdout, data->customer_site_code, data->customer_site_code_length, 0);

  if (data->oes_format)
  {
    swapped = network_short(data->credential_id_length);
    printf("%02x%02x%02x", OES_TAG_CREDENTIAL_NUMBER, swapped [0], swapped [1]);
  };
  dump_hex(stdout, data->credential_id, data->credential_id_length, 0);

  if (data->oes_format)
  {
    swapped = network_short(data->credential_version_length);
    printf("%02x%02x%02x", OES_TAG_CREDENTIAL_VERSION, swapped [0], swapped [1]);
  };
  dump_hex(stdout, data->credential_version, data->credential_version_length, 0);

  // reissue, PIN
  if (data->reissue_present)
  {
    printf("%lx", (unsigned long)(data->reissue));
  };
  if (data->oes_format)
  {
    if (data->pin_present)
    {
      printf("%lx", (unsigned long)(data->pin));
    };
  }
  else
  {
    dump_hex(stdout, data->pin, 4, 0);
  };
  if (data->customer_data_length > 0)
  {
    dump_hex(stdout, data->customer_data, data->customer_data_length, 0);
  };
  dump_hex(stdout, data->signature, sizeof(data->signature), 0);
  printf("\n");

} /* display_acdo */


void
  generate_signature
    (OES_PACS_DATA_OBJECT *acdo,
    unsigned char *tbs_buffer,
    int tbs_buffer_length,
    OES_KEY_MATERIAL *k)

{ /* generate_signature */

  uint8_t buffer [2*OES_KEY_SIZE_OCTETS];
  struct AES_ctx crypto_context;
  unsigned char DIV_input [2*OES_KEY_SIZE_OCTETS];
  int i;
  unsigned char IV[OES_KEY_SIZE_OCTETS];
  unsigned char signature_data [3*OES_KEY_SIZE_OCTETS];
  unsigned char XOR_string [2*OES_KEY_SIZE_OCTETS];


  // AN10957 4.5.1 AES-128 "Diversified" Key Generation
  // assumes AN10922 

  // Step 1 K0 ECB encrypt 1 key's worth of 0 with OCPSK

  AES_init_ctx(&crypto_context, k->OCPSK);
  memset (buffer, 0, sizeof (buffer));
  AES_ECB_encrypt(&crypto_context, buffer);
  memcpy (k->K0, buffer, sizeof(k->K0));
  fprintf(stderr, "   K0: ");
  dump_hex(stderr, k->K0, sizeof(k->K0), 1);
  fprintf(stderr, "\n");

  // Step 1 K1

  shift_key_1(k->K0, k->K1);
  // if high bit was a 1 then XOR in 87 to last byte of the one block
  if (k->K0[0] & 0x80)
  {
    k->K1[OES_KEY_SIZE_OCTETS-1] = 0x87 ^ k->K1[OES_KEY_SIZE_OCTETS-1];
  };
  fprintf(stderr, "   K1: ");
  dump_hex(stderr, k->K1, sizeof(k->K1), 1);
  fprintf(stderr, "\n");

  // Step 1 K2

  shift_key_1(k->K1, k->K2);
  // if high bit was a 1 then XOR in 87 to last byte of the one block
  if (k->K1[0] & 0x80)
  {
    k->K2[OES_KEY_SIZE_OCTETS-1] = 0x87 ^ k->K2[OES_KEY_SIZE_OCTETS-1];
  };
  fprintf(stderr, "   K2: ");
  dump_hex(stderr, k->K2, sizeof(k->K2), 1);
  fprintf(stderr, "\n");

  // Step 2 Create DIV Input

  // DIV constant 1 0x01
  // DIV input 2 AES blocks (31 bytes???)
  memset(DIV_input, 0, sizeof(DIV_input));
  DIV_input [0] = 1;
  memcpy(DIV_input+1, k->UID, k->UID_length);
  // padding is 0x80 and then all 0's ???
  DIV_input [1+k->UID_length] = 0x80;
  if (k->verbosity > 3)
  {
    fprintf(stderr, "  DIV: ");
    dump_hex(stderr, DIV_input, sizeof(DIV_input), 1);
    fprintf(stderr, "\n");
  };

  // Step 3 XOR string

  // second of two blocks gets XOR'd with K2

  memcpy (XOR_string, DIV_input, sizeof(XOR_string));
  for (i=0; i<OES_KEY_SIZE_OCTETS; i++)
  {
    XOR_string [OES_KEY_SIZE_OCTETS + i] = XOR_string [OES_KEY_SIZE_OCTETS + i] ^ k->K2[i];
  };
  if (k->verbosity > 3)
  {
    fprintf(stderr, "  XOR: ");
    dump_hex(stderr, XOR_string, sizeof(XOR_string), 1);
    fprintf(stderr, "\n");
  };

  // Step 4 AES CBC Encrypt both blocks of XOR string with OCPSK (IV=0)

  memset (IV, 0, sizeof(IV));
  AES_init_ctx_iv(&crypto_context, k->OCPSK, IV);
  AES_CBC_encrypt_buffer(&crypto_context, XOR_string, sizeof(XOR_string));
  if (k->verbosity > 3)
  {
    fprintf(stderr, "  Enc: ");
    dump_hex(stderr, XOR_string, sizeof(XOR_string), 1);
    fprintf(stderr, "\n");
  };

  // Step 5 extract the last encrypted block
  // IV is the UID with padding )but not the DIV constant

  memcpy (k->diversified_key, XOR_string+OES_KEY_SIZE_OCTETS, OES_KEY_SIZE_OCTETS);

  memset (IV, 0, sizeof(IV));
  memcpy (IV, DIV_input+1, sizeof(IV));
  if (k->verbosity > 3)
  {
    fprintf(stderr, "   IV: ");
    dump_hex(stderr, IV, sizeof(IV), 1);
    fprintf(stderr, "\n");
    fprintf(stderr, "  Key: ");
    dump_hex(stderr, k->diversified_key, sizeof(k->diversified_key), 1);
    fprintf(stderr, "\n");
  };

  memset (signature_data, 0, sizeof(signature_data));
  memcpy (signature_data, tbs_buffer, tbs_buffer_length);
  signature_data [tbs_buffer_length] = 0x80; // start of padding

  if (k->verbosity > 3)
  {
    fprintf(stderr, "  TBS: ");
    dump_hex(stderr, signature_data, sizeof(signature_data), 1);
    fprintf(stderr, "\n");
  };

  AES_init_ctx_iv(&crypto_context, k->diversified_key, IV);
  AES_CBC_encrypt_buffer(&crypto_context, signature_data, sizeof(signature_data));
  if (k->verbosity > 3)
  {
    fprintf(stderr, "  Sig: ");
    dump_hex(stderr, signature_data, sizeof(signature_data), 1);
    fprintf(stderr, "\n");
  };

  // signature is first 8 bytes of last block
  memcpy (acdo->signature, signature_data+(2*OES_KEY_SIZE_OCTETS), 8);

} /* generate_signature */


int
  main
    (int argc,
    char *argv [])

{ /* main for create-OES-contents */

  OES_PACS_DATA_OBJECT acdo;
  unsigned char acdo_buffer [1024];
  int acdo_length;
  unsigned char assembled_cio_message [2048];
  OES_CARD_IDENTIFIER_OBJECT cio;
  unsigned char cio_message_buffer [1024];
  int cio_message_length;
  OES_KEY_MATERIAL OES_keys;
  char parameter_filename [1024];
  int status;


  fprintf(stderr, "create-OES-contents (part of %s)\n", CSHH_VERSION_STRING);
  strcpy (parameter_filename, "openbadger.json");
  if (argc > 1)
    strcpy (parameter_filename, argv [1]);
  status = init_parameters (&acdo, &OES_keys, parameter_filename);

  if (status EQUALS 0)
  {
    if (acdo.oes_format EQUALS 1)
    {
      fprintf(stderr, "OES Format\n");
    }
    else
    {
      fprintf(stderr, "Classic AN10957 Format\n");
    };
    fprintf(stderr, "  Data Format %d\n", acdo.data_format);

    // initialize CIO
    memset (&cio, 0, sizeof (cio));
    strcpy ((char *)cio.manufacturer, OES_keys.manufacturer);
    cio.mutual_authentication_mode = 0; // ???
    cio.communication_encryption = OES_COMMS_ENCIPHERED;
    memcpy (cio.customer_id, DEFAULT_CUSTOMER_ID, sizeof (cio.customer_id));
    cio.key_version = 0;
    cio.version_major = 1;
    cio.version_minor = 0;
    cio.ID = OES_CARD_TYPE;

    // 3 for AES 0xc0 div AES div data 0 -> 0x0F
    cio.authentication_flag = 0x0F;

    acdo.version_major = 1;
    acdo.version_minor = 0;

    build_acdo(&acdo, acdo_buffer, sizeof(acdo_buffer), &acdo_length);


    // assemble and sign CIO

    cio_message_length = sizeof (cio_message_buffer);
    assemble_OES_CIO (&cio, cio_message_buffer, &cio_message_length);
    if (cio_message_length <= 0)
      status = -1;
    if (status EQUALS 0)
    {
      memcpy (assembled_cio_message, cio_message_buffer, cio_message_length);
      printf("CIO: ");
      dump_hex(stdout, assembled_cio_message, cio_message_length, 0);
      printf("\n");
    };
  };
  if (status EQUALS 0)
  {
fprintf(stderr, "\n...CIO...\n");
    generate_signature(&acdo, cio_message_buffer, cio_message_length,
      &OES_keys);

    memcpy (cio.signature, acdo.signature, sizeof(cio.signature));

fprintf(stderr, "\n...ACDO...\n");
    generate_signature(&acdo, acdo_buffer, acdo_length, &OES_keys);

    display_acdo(stderr, &cio, &acdo);

    if (OES_keys.verbosity > 3)
    {
      fprintf(stderr, "--Keys %d.(bits)\n", 8*OES_KEY_SIZE_OCTETS);
      fprintf(stderr, " PICC: ");
      dump_hex(stderr, OES_keys.PICC, sizeof(OES_keys.PICC), 1);
      fprintf(stderr, "\n");
      fprintf(stderr, "OCPSK: ");
      dump_hex(stderr, OES_keys.OCPSK, OES_keys.OCPSK_length, 1);
      fprintf(stderr, "\n");
      fprintf(stderr, " DivK: ");
      dump_hex(stderr, OES_keys.diversified_key, OES_KEY_SIZE_OCTETS, 1);
      fprintf(stderr, "\n");
      fprintf(stderr, "--UID ");
      dump_hex(stderr, OES_keys.UID, OES_keys.UID_length, 1);
      fprintf (stderr, "\n");
    };
  };
  if (status != 0)
    fprintf(stderr, "Status returned was %d\n", status);
  return (status);

} /* main for create-OES-contents */

#endif

