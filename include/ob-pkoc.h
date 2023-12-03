/*
  ob-pkoc.h - PKOC parameter definitions

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


#define OB_PKOC_PUBKEY_LENGTH                (65)
#define OB_PKOC_SIG_LENGTH                   (64)
#define OB_PKOC_TRANSACTID_LENGTH            (16)
#define OB_PKOC_SITE_KEY_IDENTIFIER_LENGTH   (16)
#define OB_PKOC_READER_KEY_IDENTIFIER_LENGTH (16)

// tag, 1 byte length, 1 byte value, 2 byte arbval

#define OB_PKOC_TAG_TRANSACTION_IDENTIFIER (0x4C)
#define OB_PKOC_TAG_READER_IDENTIFIER      (0x4D)
#define OB_PKOC_TAG_UNCOMP_PUBLIC_KEY      (0x5A)
#define OB_PKOC_TAG_PROTOCOL_VERSION       (0x5C)
#define OB_PKOC_TAG_DIGITAL_SIGNATURE      (0x9E)

#define OB_PKOC_SELECT_RESPONSE_MIN (3+2)

typedef struct ob_pkoc_context
{
  unsigned char protocol_version [OB_STRING_MAX];
  int protocol_version_length;
  unsigned char transaction_identifier [OB_PKOC_TRANSACTID_LENGTH];
  unsigned char site_key_identifier [OB_PKOC_SITE_KEY_IDENTIFIER_LENGTH];
  unsigned char reader_key_identifier [OB_PKOC_READER_KEY_IDENTIFIER_LENGTH];
  unsigned char card_arbval [2]; // arbitrary value
  unsigned char ec_public_key [65];
  unsigned char pkoc_signature [64];
} OB_PKOC_CONTEXT;

// select command: CLA=00 INS=A4 P1=04 P2=00 Lc=08 <AID> Le=00

#ifdef ALLOCATE_SELECT
unsigned char SELECT_PKOC [] = 
  {0x00, 0xa4, 0x04, 0x00, 0x08, 0xA0, 0x00, 0x00, 0x08, 0x98, 0x00, 0x00, 0x01, 0x00};
#endif
#ifndef ALLOCATE_SELECT
extern unsigned char SELECT_PKOC [];
#endif

#define OBTEST_PKOC_PUBLIC_KEY "ec-public-key.der"

int ob_validate_select_response(OB_CONTEXT *ctx, unsigned char *response, int response_length);

