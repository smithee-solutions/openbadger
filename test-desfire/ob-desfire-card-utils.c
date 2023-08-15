#include <jansson.h>


#include <nfc/nfc.h>
#include <freefare.h>


#define OB_SLOTS 1
#include <ob-desfire.h>

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
    // just open the first device
    device = nfc_open(nfc_context, devices[0]);
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

