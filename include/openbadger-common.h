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
#define OB_UID_SIZE        (56/8)
#define OB_7816_BUFFER_MAX (1000)  // our buffer not one in the spec

// various formats

#define OB_FORMAT_SIA26BIT (0)
#define OB_FORMAT_PIV      (1)
#define OB_FORMAT_AN10957  (2)
#define OB_FORMAT_LEAF     (3)
#define OB_FORMAT_Pzz1     (4)
#define OB_FORMAT_Pzz2     (5)
#define OB_FORMAT_PRIVATE  (255)

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
  int pacs_data_format;

  // for PCSC reader control
  int reader_index;
  char reader_name [OB_STRING_MAX];
  void *rdrctx;

  // 7816 and specifically general authenticate context

  int apdu_payload_max_7816;
  unsigned char key_reference;
  int key_size;
  unsigned char challenge_type;

  // general DESFire-ish parameters
  unsigned char uid [OB_UID_SIZE];
  int uid_size;

  // crypto keys
  unsigned char secret_key [OB_AES128_KEY_SIZE];
  unsigned char iv [OB_AES128_KEY_SIZE];

  void *an10957ctx;
} OB_CONTEXT;

int aes_encrypt(OB_CONTEXT *ctx, unsigned char *plaintext, unsigned char *ciphertext, unsigned char *key, int *length);
void array_shift_left(OB_CONTEXT *ctx, unsigned char *from, unsigned char *to);
void array_xor(OB_CONTEXT *ctx, unsigned char *result, unsigned char *xor_left, unsigned char *xor_right, int length);
void ob_add_tag_length(unsigned char *buffer, int length, int *new_buffer_length);
int ob_command_response(OB_CONTEXT *ctx, unsigned char *x7816_buffer, int x7816_lth, char *prefix_string, char *suffix_string, unsigned char *r7816_buffer, int *r7816_lth);
void ob_display_PACS_data_object(OB_CONTEXT *ctx, unsigned char *credential_contents);
int ob_diversify_AN10957(OB_CONTEXT *ctx);
int ob_initialize(OB_CONTEXT **initiaized_context, char *settings_filename);
char *ob_buffer_dump_string(OB_CONTEXT *ctx, unsigned char *buffer, int buffer_size, char *tag);
void ob_dump_buffer(OB_CONTEXT *ctx, unsigned char *bytes, int length, int dest);
unsigned char *string_buffer_hex(OB_CONTEXT *ctx, const char *buf, int *buf_lth);
char *string_hex_buffer(OB_CONTEXT *ctx, unsigned char *buf, int buf_lth);

