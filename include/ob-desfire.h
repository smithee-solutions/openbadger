#define OBTEST_DESFIRE_VERSION "0.01"

#define EQUALS ==

#define OB_ALGO_3DES (1)
#define OB_ALGO_AES  (2)

#define OB_KEYSIZE_AES128             (128/8)
#define OB_DESFIRE_MAX_KEY_SLOT       (32)
#define OB_DESFIRE_KEYIDX_PICC_MASTER (0)
#define OB_MAX_NFC_DEVICES            (8)

//  each key we know about has a "slot" in an array of keys.

typedef struct ob_desfire_key
{
  int state;
  int algorithm;
  int key_type;
  int key_id; // key number known to the desfire card
  unsigned char key [OB_KEYSIZE_AES128];
  char key_hex [1024];
  MifareDESFireKey freefare_key;
} OB_DESFIRE_KEY;


typedef struct obtest_desfire_context
{
  int verbosity;
  json_t *settings;
  json_t *context;
  char settings_file_path [1024];
  char context_file_path [1024];
  int device_count;
  FreefareTag tag;
  uint8_t tag_key_version;
  int picc_authenticated;
  int nfc_initialized;
} OBTEST_DESFIRE_CONTEXT;

#define ST_OK                      ( 0)
#define STOB_DESFIRE_BAD_ALGO      ( 1)
#define STOB_DESFIRE_COMMAND_PARSE ( 2)
#define STOB_AUTH_MASTER_SUCCESS   ( 3)
#define STOB_NFC_OPEN              ( 4)
#define STOB_AUTH_MASTER_FAIL      ( 5)
// 6
#define STOB_NFC_INIT              ( 7)
// 7
#define STOB_TAGS                  ( 9)
#define STOB_FREEFARE_CONNECT      (10)
#define STOB_NO_TAG                (11)

#define OBTEST_DESFIRE_DEFAULT_CONTEXT_FILE "/opt/smithee/etc/obtest-desfire_context.json"
#define OBTEST_DESFIRE_DEFAULT_SETTINGS_FILE "/opt/smithee/etc/obtest-desfire_settings.json"

// json tags for context
#define OB_TAG_PICC_MASTER ("key-PICC-master")

int ob_desfire_init(OBTEST_DESFIRE_CONTEXT *ctx);
int ob_desfire_load_context(OBTEST_DESFIRE_CONTEXT *ctx);
int ob_desfire_load_context_values(OBTEST_DESFIRE_CONTEXT *ctx);
int ob_desfire_load_settings(OBTEST_DESFIRE_CONTEXT *ctx);
int ob_desfire_load_settings_values(OBTEST_DESFIRE_CONTEXT *ctx);
int ob_desfire_open(OBTEST_DESFIRE_CONTEXT *ctx);
int ob_desfire_save_context(OBTEST_DESFIRE_CONTEXT *ctx);

#ifdef OB_SLOTS
OB_DESFIRE_KEY key_slots [OB_DESFIRE_MAX_KEY_SLOT];
#endif
#ifndef OB_SLOTS
extern OB_DESFIRE_KEY key_slots [];
#endif

