/*
  openbadger.h - definitions
*/

#define EQUALS ==

#define LOG stdout

#define OB_NOOP      (0)
#define OB_HELP      (1)
#define OB_VERBOSITY (2)
#define OB_DETAILS   (3)
#define OB_SETTINGS  (4)
#define OB_SELFTEST  (5)

#define OB_KEY_SIZE_10957 (128/8)
#define OB_STRING_MAX (1024)
#define OB_KEY_SIZE_10957 (128/8)
#define OB_UID_SIZE (56/8)

typedef struct
{
  int verbosity;
  unsigned char secret_key [OB_KEY_SIZE_10957];
  unsigned char iv [OB_KEY_SIZE_10957];
  unsigned char uid [OB_UID_SIZE];
  int uid_size;
  int action;
} OB_CONTEXT;

#define ST_OK                (0)
#define STOB_NO_ARGUMENTS    (1)
#define STOB_NOT_IMPLEMENTED (2)

