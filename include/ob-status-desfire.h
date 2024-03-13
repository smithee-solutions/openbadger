/*
  status codes for desfire

  (C)2023-2024 Smithee Solutions LLC
*/

#define STOB_DESFIRE_BAD_ALGO      ( 1 + STOB_FIRST_DESFIRE)
#define STOB_DESFIRE_COMMAND_PARSE ( 2 | STOB_FIRST_DESFIRE)
#define STOB_AUTH_MASTER_SUCCESS   ( 3 | STOB_FIRST_DESFIRE)
#define STOB_NFC_OPEN              ( 4 | STOB_FIRST_DESFIRE)
#define STOB_AUTH_MASTER_FAIL      ( 5 | STOB_FIRST_DESFIRE)
#define STOB_NFC_INIT              ( 7 | STOB_FIRST_DESFIRE)
#define STOB_TAGS                  ( 8 | STOB_FIRST_DESFIRE)
#define STOB_FREEFARE_CONNECT      ( 9 | STOB_FIRST_DESFIRE)
#define STOB_NO_TAG                (10 | STOB_FIRST_DESFIRE)
#define STOB_AID_ERROR             (11 | STOB_FIRST_DESFIRE)
#define STOB_SELECT_ERROR          (12 | STOB_FIRST_DESFIRE)
#define STOB_RDFILE_AUTH           (13 | STOB_FIRST_DESFIRE)
#define STOB_GETVER_ERROR          (14 | STOB_FIRST_DESFIRE)
#define STOB_SETTINGS_KEY_AES      (15 | STOB_FIRST_DESFIRE)
#define STOB_SETTINGS_KEY_3DES     (16 | STOB_FIRST_DESFIRE)
#define STOB_READ                  (17 | STOB_FIRST_DESFIRE)

