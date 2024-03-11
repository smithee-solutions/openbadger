/*
  obtest-desfire-info - display card information

  (C)2024 Smithee Solutions LLC
*/


#include <string.h>

#include <jansson.h>

#include <nfc/nfc.h>
#include <freefare.h>

#include <openbadger-version.h>
#include <ob-crypto.h>
#include <openbadger-common.h>
#include <ob-desfire.h>
#include <ob-status-desfire.h>


int main
  (int argc,
  char *argv [])

{ /* main for obtest-desfire-info */

  OBTEST_DESFIRE_CONTEXT *ctx;
  OBTEST_DESFIRE_CONTEXT obtest_context_desfire;
  int status;
  int status_freefare;
  struct mifare_desfire_version_info version_info;


  ctx = &obtest_context_desfire;
  status = ob_desfire_load_settings(ctx);
  fprintf(stderr, "OpenBadger Test: DESFire Info %s\n", OPENBADGER_VERSION);
  status = ob_desfire_open(ctx);
  if (status EQUALS ST_OK)
  {
    /* Select the master application */
    status_freefare = mifare_desfire_select_application(ctx->tag, NULL);
    if (status_freefare < 0)
      status = STOB_SELECT_ERROR;
  };
  if (status EQUALS ST_OK)
  {
    /* Get version information */
    status_freefare = mifare_desfire_get_version(ctx->tag, &version_info);
    if (status_freefare < 0)
      status = STOB_GETVER_ERROR;
  };
  if (status EQUALS ST_OK)
  {
    printf("DESFire Card Information\n");
    printf("Hardware:\n  Vendor ID %02X Type %02X %02X Version %02x %02x Size %02X Protocol %02X\n",
      version_info.hardware.vendor_id,
      version_info.hardware.type,
      version_info.hardware.subtype,
      version_info.hardware.version_major,
      version_info.hardware.version_minor,
      version_info.hardware.storage_size,
      version_info.hardware.protocol);
    printf("Software:\n  Vendor ID %02X Type %02X %02X Version %02x %02x Size %02X Protocol %02X\n",
      version_info.software.vendor_id,
      version_info.software.type,
      version_info.software.subtype,
      version_info.software.version_major,
      version_info.software.version_minor,
      version_info.software.storage_size,
      version_info.software.protocol);
    printf("UID: %02X %02X %02X %02X %02X %02X %02X\n",
      version_info.uid [0], version_info.uid [1], version_info.uid [2], version_info.uid [3],
      version_info.uid [4], version_info.uid [5], version_info.uid [6]);
    printf("Batch: %02X %02X %02X %02X %02X Production Week %02X Year %02X\n",
      version_info.batch_number [0], version_info.batch_number [1], version_info.batch_number [2], version_info.batch_number [3],
      version_info.batch_number [4],
      version_info.production_week, version_info.production_year);
  };

  if (status != ST_OK)
  {
    fprintf(stderr, "status %d. freefare status %d.\n", status, status_freefare);
  };
  return(status);

} /* main for obtest-desfire-list-applications */


