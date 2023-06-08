/*
  openbadger - common definitions

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

#define LOG stdout

#define EQUALS ==
#define OB_STRING_MAX      (1024)
#define OB_AES128_KEY_SIZE (128/8)
#define OB_UID_SIZE        (56/8)
#define OB_7816_BUFFER_MAX (1000)  // our buffer not one in the spec

// test cases exercised (in 800-73 testing)
#define OB_TEST_VECTORS (0)
#define OB_TEST_PIV     (1)

// tool being used (in AN-10957 testing)

#define OB_TOOL_DIVUTIL (0x0001)
#define OB_TOOL_SIGUTIL (0x0002)

#define OB_NOOP      (0)
#define OB_HELP      (1)
#define OB_VERBOSITY (2)
// rfu (3)
#define OB_SETTINGS  (4)
#define OB_SELFTEST  (5)

#define OB_DUMP_INCLUDE (2)
// 0=stderr 1=?

#define OB_SYSTEM_SETTINGS_FILE "/opt/tester/etc/openbadger-settings.json"
#define OB_LOCAL_SETTINGS_FILE "openbadger-settings.json"

#define ST_OK                  (   0)
#define STOB_SCARD_ERROR       (   1)
#define STOB_SCARD_CONNECT     (   2)
#define STOB_UNKNOWN_TEST_CASE (   3)
#define STOB_PCSC_TRANSMIT_1   (   4)
#define STOB_PCSC_TRANSMIT_2   (   5)
#define STOB_WRONG_TEST_CASE   (   6)
#define STOB_SELECT_RESPONSE   (   7)
#define STOB_SELRSP_TAG        (   8)
#define STOB_PKOC_AUTH         (   9)
#define STOB_SCARD_ESTABLISH   (  10)
#define STOB_NO_ARGUMENTS      (  11)
#define STOB_NOT_IMPLEMENTED   (  12)
#define STOB_SETTINGS_ERROR    (  13)

typedef struct ob_context
{
  // general bookkeeping
  int verbosity;
  int test_case; // in case there are test vectors, multiple flavors, etc.
  int action; // action as specified by command line swtch
  unsigned long int tool_identifier;  // which "tool" is this.
  FILE *current_file;

  // for PCSC reader control
  int reader_index;
  char reader_name [OB_STRING_MAX];
  void *rdrctx;

  // general authenticate context

  unsigned char key_reference;
  int key_size;
  unsigned char challenge_type;

  // general DESFire-ish parameters
  unsigned char uid [OB_UID_SIZE];
  int uid_size;
} OB_CONTEXT;

void ob_add_tag_length(unsigned char *buffer, int length, int *new_buffer_length);
int ob_command_response(OB_CONTEXT *ctx, unsigned char *x7816_buffer, int x7816_lth, char *prefix_string, char *suffix_string, unsigned char *r7816_buffer, LPDWORD r7816_lth);
void ob_display_PACS_data_object(OB_CONTEXT *ctx, unsigned char *credential_contents);
int ob_initialize(OB_CONTEXT **initiaized_context, char *settings_filename);
void ob_dump_buffer(OB_CONTEXT *ctx, BYTE *bytes, int length, int dest);
char *string_hex_buffer(OB_CONTEXT *ctx, unsigned char *buf, int buf_lth);


#if 0
  // PKOC context

  unsigned char protocol_version [OP_STRINGMAX];
  int protocol_version_length;
  unsigned char transaction_identifier [OP_PKOC_TRANSACTID_LENGTH];
  unsigned char site_key_identifier [OP_PKOC_SITE_KEY_IDENTIFIER_LENGTH];
  unsigned char reader_key_identifier [OP_PKOC_READER_KEY_IDENTIFIER_LENGTH];
  unsigned char card_arbval [2]; // arbitrary value
  unsigned char ec_public_key [65];
  unsigned char pkoc_signature [64];
} OP_CONTEXT;

#define OP_TEST_PKOC    (2)




int op_build_7816_message
  (OP_CONTEXT *ctx, unsigned char msg_cla, unsigned char msg_ins, unsigned char msg_p1, unsigned char msg_p2,
  unsigned char msg_lc, unsigned char msg_le, unsigned char *payload, int payload_length, int flags,
  unsigned char *message_7816, int *message_7816_length);
int op_challenge_response(OP_CONTEXT *ctx);
int op_init_smartcard(OP_CONTEXT *ctx);
char *op_pcsc_error_string(DWORD status_pcsc);
int op_validate_select_response(OP_CONTEXT *ctx, unsigned char *response, int response_length);


#endif

