/*
  ob-desfire-card-utils
*/

#include <string.h>

#include <jansson.h>

#include <nfc/nfc.h>
#include <freefare.h>


#define OB_SLOTS 1
#include <ob-crypto.h>
#include <openbadger-common.h>
#include <ob-desfire.h>
#include <ob-status-desfire.h>

OB_DESFIRE_KEY key_slots [OB_DESFIRE_MAX_KEY_SLOT];


int ob_desfire_authenticate_picc
  (OBTEST_DESFIRE_CONTEXT *ctx)

{ /* ob_desfire_authenticate_picc */

  uint8_t key_version;
  int status;
  int status_freefare;


  if (ctx->verbosity > 3)
    fprintf(stderr, "  Authenticating to PICC.\n");
  status = ob_desfire_open(ctx);
  if (status EQUALS ST_OK)
  {
    status_freefare = mifare_desfire_select_application(ctx->tag, NULL);
    if (status_freefare < 0)
      status = -2;
  };
  if (status EQUALS ST_OK)
  {
    status_freefare = mifare_desfire_get_key_version(ctx->tag, 0, &key_version);
    if (status_freefare < 0)
    {
      status = -99;
    }
    else
    {
      if (ctx->verbosity > 3)
        fprintf(stderr, "PICC Master Key version=%02x\n", key_version);
    };
  };
  if (status EQUALS ST_OK)
  {
    status = STOB_AUTH_MASTER_SUCCESS;

//    if (key_version != 0x00)
    {
fprintf(stderr, "PICC AES tagKeyType %02x\n", ctx->tag_key_version);
      status_freefare = mifare_desfire_authenticate(ctx->tag, 0, key_slots [OB_DESFIRE_KEYIDX_PICC_MASTER].freefare_key);
    };
    if (status_freefare < 0)
    {
      fprintf(stderr, "mifare_desfire_authenticate error, last PICC error %02x\n",
        mifare_desfire_last_picc_error(ctx->tag));
      status = STOB_AUTH_MASTER_FAIL;
      if (ctx->verbosity > 3)
        fprintf(stderr, "Failed to authenticate\n");
    };
    if (status EQUALS STOB_AUTH_MASTER_SUCCESS)
    {
      ctx->picc_authenticated = 1;
      fprintf(stderr, "Authenticated with PICC Master key\n");
    };
  };
  return(status);

} /* obf_authenticate_picc */


/*
  ctx - context
  key_number - ordinal number into key list
  slot_tag - identification string
*/
int ob_desfire_load_key
  (OB_CONTEXT *ctx,
  int key_number,
  char *slot_tag,
  json_t *settings,
  int key_type_requested)

{ /* ob_desfire_load_key */

  char hex_string [1024];
  int i;
  int length;
  int status;
  char tag [1024];
  OB_DESFIRE_KEY *tmp_key_slot;
  json_t *value;


  status = ST_OK;
  hex_string [0] = 0;
  sprintf(tag, "key-value-%02d", key_number);
  if (ctx->verbosity > 3)
    fprintf(stderr, "ob_desfire_load_key: key number %02d tag %s\n", key_number, slot_tag);
  value = json_object_get(settings, tag);
  if (json_is_string(value))
  {
    strcpy(hex_string, json_string_value(value));
  };
  {
    tmp_key_slot = &(key_slots [key_number]);

    if (ctx->verbosity > 3)
      fprintf(stderr, "key string is %s\n", hex_string);
    length = strlen(hex_string);
if (length EQUALS 0)
{
  length = 32;
  fprintf(stderr, "no key found, using all zeroes.\n");
};
    memcpy((unsigned char *)&(tmp_key_slot->key[0]), string_buffer_hex(ctx, hex_string, &length), strlen(hex_string)/2);
    if (ctx->verbosity > 3)
    {
      fprintf(stderr, "Key in binary (%02d. bytes) is", 2*length);
      for (i=0; i<length; i++)
      {
        fprintf(stderr, " %02x", tmp_key_slot->key [i]);
      };
      fprintf(stderr, "\n");
    };

    if ((status EQUALS ST_OK) && (key_type_requested EQUALS OB_KEYTYPE_AES128))
    {
      if (tmp_key_slot->freefare_key != NULL)
        mifare_desfire_key_free(tmp_key_slot->freefare_key);
      tmp_key_slot->freefare_key = mifare_desfire_aes_key_new_with_version(tmp_key_slot->key, 0);
      tmp_key_slot->key_type = OB_KEYTYPE_AES128;
      tmp_key_slot->key_id = key_number;
      if (tmp_key_slot->freefare_key EQUALS NULL)
        status = STOB_SETTINGS_KEY_AES;

    };
    if ((status EQUALS ST_OK) && (key_type_requested EQUALS OB_KEYTYPE_3DES))

    {
      tmp_key_slot->freefare_key = mifare_desfire_3des_key_new(tmp_key_slot->key);
      tmp_key_slot->key_type = OB_KEYTYPE_3DES;
      tmp_key_slot->key_id = key_number;
      if (tmp_key_slot->freefare_key EQUALS NULL)
        status = STOB_SETTINGS_KEY_3DES;
    };
    if (status EQUALS ST_OK)
    {
      tmp_key_slot->state = OB_KEYSLOT_ACTIVE;
      if (ctx->verbosity > 3)
        fprintf(stderr, "Allocated key slot %d from settings tag %s key %s\n", key_number, slot_tag, hex_string);
    };
  };
  return(status);

} /* obf_fill_in_key */


int ob_desfire_open
  (OBTEST_DESFIRE_CONTEXT *ctx)

{ /* ob_desfire_open */

  nfc_device *device;
  nfc_connstring devices[OB_MAX_NFC_DEVICES];
  uint8_t key_version;
  nfc_context *nfc_context;
  int status;
  int status_freefare;
  FreefareTag *tags;


  if (ctx->verbosity > 3)
    fprintf(stderr, "  Opening DESFire credential.\n");
  status = ST_OK;
  status_freefare = 0;
  nfc_init(&nfc_context);
  if (nfc_context EQUALS NULL)
    status = STOB_NFC_INIT;
  if (status EQUALS ST_OK)
  {
    ctx->device_count = nfc_list_devices(nfc_context, devices, OB_MAX_NFC_DEVICES);
    if (ctx->verbosity > 3)
      fprintf(stderr, "%d. devices found\n", ctx->device_count);
  };

  if (status EQUALS ST_OK)
  {
    if (ctx->verbosity > 3)
      fprintf(stderr, "Opening reader %d.\n", ctx->reader);
    device = nfc_open(nfc_context, devices[ctx->reader]);
    if (device EQUALS NULL)
      status = STOB_NFC_OPEN;
  };
  if (status EQUALS ST_OK)
  {
    tags = freefare_get_tags(device);
    if (tags EQUALS NULL)
      status = STOB_TAGS;
  };
  if (status EQUALS ST_OK)
  {
    // just use the first tag
    ctx->tag = tags [0];

    if (ctx->tag != 0)
    {
      status_freefare = mifare_desfire_connect(ctx->tag);
      if (status_freefare < 0)
      {
        status = STOB_FREEFARE_CONNECT;
      };
    }
    else
    {
      status = STOB_NO_TAG;
    };
  };
  if (status EQUALS ST_OK)
  {
    if (ctx->verbosity > 9)
      fprintf(stderr, "context ok.  open device ok.  get tags ok.  connected to tag zero.\n");
  };
  if (status EQUALS ST_OK)
  {
    status_freefare = mifare_desfire_get_key_version(ctx->tag, 0, &key_version);
    if (ctx->verbosity > 3)
      fprintf(stderr, "Key version at open: %02X\n", key_version);
    ctx->tag_key_version = key_version;
    ctx->nfc_initialized = 1;
  };
  if (ctx->verbosity > 9)
  {
    fprintf(stderr, "obf_open_desfire return status %d.\n", status);
  };
  return(status);

} /* ob_desfire_open */


