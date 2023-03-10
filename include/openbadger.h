/*
  openbadger.h - definitions

  (C)Copyright 2019-2023 Smithee Solutions LLC

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

#define EQUALS ==

#define LOG stdout

#define OB_SYSTEM_SETTINGS_FILE "/opt/tester/etc/openbadger-settings.json"
#define OB_LOCAL_SETTINGS_FILE "openbadger-settings.json"

#define OB_NOOP      (0)
#define OB_HELP      (1)
#define OB_VERBOSITY (2)
// rfu (3)
#define OB_SETTINGS  (4)
#define OB_SELFTEST  (5)

#define OB_KEY_SIZE_10957 (128/8)
#define OB_STRING_MAX (1024)
#define OB_UID_SIZE (56/8)

#define OB_TOOL_DIVUTIL (0x0001)
#define OB_TOOL_SIGUTIL (0x0002)

typedef struct
{
  int verbosity;
  unsigned char secret_key [OB_KEY_SIZE_10957];
  unsigned char diversified_key [OB_KEY_SIZE_10957];
  unsigned char iv [OB_KEY_SIZE_10957];
  unsigned char uid [OB_UID_SIZE];
  int uid_size;
  int action; // action as specified by command line swtch
  unsigned long int tool_identifier;  // which "tool" is this.
} OB_CONTEXT;

#define ST_OK                (0)
#define STOB_NO_ARGUMENTS    (1)
#define STOB_NOT_IMPLEMENTED (2)
#define STOB_SETTINGS_ERROR  (3)


int aes_encrypt(OB_CONTEXT *ctx, unsigned char *plaintext, unsigned char *ciphertext, unsigned char *key, int *length);
void array_shift_left(OB_CONTEXT *ctx, unsigned char *from, unsigned char *to);
void array_xor(OB_CONTEXT *ctx, unsigned char *result, unsigned char *xor_left, unsigned char *xor_right, int length);
char *bcd_to_string(unsigned char *bcd_buffer, int byte_length);
char *buffer_dump_string(OB_CONTEXT *ctx, unsigned char *buffer, int buffer_length, char *tag);
void display_PACS_data_object(OB_CONTEXT *ctx, OB_PACS_DATA_OBJECT *PACS_data_object);
int diversify_AN10957(OB_CONTEXT *ctx);
int openbadger_initialize(OB_CONTEXT **initiaized_context, char *settings_filename);
unsigned char *string_buffer_hex(OB_CONTEXT *ctx, const char *buf, int *buf_lth);
char *string_hex_buffer(OB_CONTEXT *ctx, unsigned char *buf, int buf_lth);

