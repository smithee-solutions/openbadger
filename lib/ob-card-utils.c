/*
  openbadger - utility functions

  (C)Copyright 2023-2024 Smithee Solutions LLC

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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <jansson.h>
#include <PCSC/wintypes.h>
#include <PCSC/pcsclite.h>
#include <PCSC/winscard.h>


#include <ob-crypto.h>
#include <ob-7816.h>
#include <openbadger-common.h>
#include <openbadger-version.h>
#include <ob-pcsc.h>
#include <ob-73-4.h>


void
  ob_add_tag_length
    (unsigned char *buffer,
    int length,
    int *new_buffer_length)

{ /* ob_add_tag_length */

  int upper;
  int lower;

  lower = length & 0xff;
  upper = (length & 0xff00) >> 8;
  if (length > 255)
  {
    *buffer = 0x82;
    (*new_buffer_length)++;
    *(buffer+1) = upper;
    *(buffer+2) = lower;
    (*new_buffer_length)++;
    (*new_buffer_length)++;
  }
  else
  {
    *(buffer) = lower;
    (*new_buffer_length)++;
  };
} /* ob_add_tag_length */

/*
  hex dump function.

  dest is 0 for stderr 1 for stdout DUMP_INCLUDE to append to include log
*/
void ob_dump_buffer
    (OB_CONTEXT *ctx,
    BYTE *bytes,
    int length,
    int dest)

{ /* dump_buffer */

  int i;
  FILE *inclog;
  int line_length;
  FILE *log_file;


  log_file = stderr;
  if (ctx->current_file != NULL)
    log_file = ctx->current_file;
  else
    log_file = stderr;
  fflush (log_file);

  inclog = NULL;
  if (dest EQUALS OB_DUMP_INCLUDE)
  {
    inclog = fopen("include.log", "a");
    if (inclog)
      fprintf(inclog, "/* dumping 0x%X bytes */\n", length);
  };
  line_length = 32;
  for (i=0; i<length; i++)
  {
    fprintf (log_file, "%02x", bytes [i]);
    if (inclog)
      fprintf(inclog, ", 0X%X", bytes [i]);
    if ((line_length-1) == (i % line_length))
    {
      fprintf (log_file, "\n");
      if (inclog)
        fprintf(inclog, "\n");
    };
  };
  if ((line_length-1) != ((length-1) % line_length))
    fprintf (log_file, "\n");
  fflush (log_file);
  if (inclog)
    fclose(inclog);

} /* dump_buffer */


/*
  ob_init_smartcard - initialize smartcard operations.

  assumes ctx->reader_index is the reader we want.  default is zero.
  uses ctx->verbosity to control logging
*/

int ob_init_smartcard
  (OB_CONTEXT *ctx)

{ /* ob_init_smartcard */

  int done;
  DWORD dwActiveProtocol;
  LPTSTR mszReaders;
  OB_RDRCTX *rdrctx;
  DWORD reader_names_list_size;
  int offset;
  char *protocol_name;
  char *rdr_list;
  int reader_index;
  int status;
  LONG status_pcsc;


  status = ST_OK;
  rdrctx = ctx->rdrctx;

  // initialize PCSC

  status_pcsc = SCardEstablishContext (SCARD_SCOPE_SYSTEM, NULL, NULL, &(rdrctx->hContext));
  if (ctx->verbosity > 3)
  {
    fprintf(stderr, "DEBUG: SCardEstablishContext return value was %lX\n", status_pcsc);
  };
  if (status_pcsc != SCARD_S_SUCCESS)
    status = STOB_SCARD_ESTABLISH;

  if (status EQUALS ST_OK)
  {
  // find out how many readers are attached

  status_pcsc = SCardListReaders (rdrctx->hContext, NULL, NULL, &reader_names_list_size);
  if (ctx->verbosity > 3)
  {
    fprintf(stderr, "DEBUG: SCardListReaders return value was %lX\n", status_pcsc);
  };

  // enumerate to the log all reader names, until we find the one we want

  mszReaders = calloc(reader_names_list_size, sizeof(char));
  status_pcsc = SCardListReaders (rdrctx->hContext, NULL, mszReaders, &reader_names_list_size);
  if (status_pcsc != SCARD_S_SUCCESS)
    status = STOB_SCARD_ERROR;
  };
  if (status EQUALS ST_OK)
  {
    done = 0;
    offset = 0;
    reader_index = 0;
    rdr_list = mszReaders;
    ctx->reader_name [0] = 0;
    while (!done)
    {
      if (ctx->verbosity > 3)
        fprintf(stderr, "  Reader %d: %s\n", reader_index, rdr_list);
      if (reader_index EQUALS ctx->reader_index)
      {
        strcpy (ctx->reader_name, rdr_list);
        done = 1;
      };
      if ((1+offset) >= reader_names_list_size)
        done = 1;
      reader_index ++;
      offset = offset + strlen(rdr_list) + 1;
      rdr_list = strlen(rdr_list) + 1 + mszReaders;
    };

    if (ctx->verbosity > 0)
      if (strlen(ctx->reader_name) > 0)
        fprintf (stderr, "Selected reader (%d) is %s\n", ctx->reader_index, ctx->reader_name);
  };

  // initialize PCSC connection to the desired reader.

  if (status EQUALS ST_OK)
  {
    status_pcsc = SCardConnect (rdrctx->hContext, ctx->reader_name, SCARD_SHARE_SHARED, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, &(rdrctx->pcsc), &dwActiveProtocol);
    if (SCARD_S_SUCCESS != status_pcsc)
      status = STOB_SCARD_CONNECT;
  };
  if (status EQUALS ST_OK)
  {
    // record if it's T0 or T1

    switch (dwActiveProtocol)
    {
    case SCARD_PROTOCOL_T0:
      rdrctx->pioSendPci = *SCARD_PCI_T0;
      protocol_name = "T0";
      break;

    case SCARD_PROTOCOL_T1:
      rdrctx->pioSendPci = *SCARD_PCI_T1;
      protocol_name = "T1";
      break;
    }
    if (ctx->verbosity > 3)
      fprintf (stderr, "Protocol is %s\n", protocol_name);
    if (ctx->verbosity > 3)
      fprintf (stderr, "SCardTransmit (step 1 select card applet)\n");
  };
  rdrctx->last_pcsc_status = status_pcsc;
  return(status);

} /* ob_init_smartcard */


char *ob_pcsc_error_string
  (DWORD status_pcsc)

{
  char *return_value;


  return_value = NULL;
  switch(status_pcsc)
  {
  default:
    return_value = "Unknown";
    break;
  case SCARD_E_INSUFFICIENT_BUFFER:
    return_value = "SCARD_E_INSUFFICIENT_BUFFER";
    break;
  case SCARD_E_INVALID_HANDLE:
    return_value = "SCARD_E_INVALID_HANDLE";
    break;
  case SCARD_E_NO_SERVICE:
    return_value = "SCARD_E_NO_SERVICE";
    break;
  case SCARD_E_NOT_TRANSACTED:
    return_value = "SCARD_E_NOT_TRANSACTED";
    break;
  case SCARD_W_REMOVED_CARD:
    return_value = "SCARD_W_REMOVED_CARD";
    break;
  };
  return(return_value);
}
#if 0
#define SCARD_S_SUCCESS			((LONG)0x00000000) /**< No error was encountered. */
#define SCARD_F_INTERNAL_ERROR		((LONG)0x80100001) /**< An internal consistency check failed. */
#define SCARD_E_CANCELLED		((LONG)0x80100002) /**< The action was cancelled by an SCardCancel request. */
#define SCARD_E_INVALID_PARAMETER	((LONG)0x80100004) /**< One or more of the supplied parameters could not be properly interpreted. */
#define SCARD_E_INVALID_TARGET		((LONG)0x80100005) /**< Registry startup information is missing or invalid. */
#define SCARD_E_NO_MEMORY		((LONG)0x80100006) /**< Not enough memory available to complete this command. */
#define SCARD_F_WAITED_TOO_LONG		((LONG)0x80100007) /**< An internal consistency timer has expired. */
#define SCARD_E_UNKNOWN_READER		((LONG)0x80100009) /**< The specified reader name is not recognized. */
#define SCARD_E_TIMEOUT			((LONG)0x8010000A) /**< The user-specified timeout value has expired. */
#define SCARD_E_SHARING_VIOLATION	((LONG)0x8010000B) /**< The smart card cannot be accessed because of other connections outstanding. */
#define SCARD_E_NO_SMARTCARD		((LONG)0x8010000C) /**< The operation requires a Smart Card, but no Smart Card is currently in the device. */
#define SCARD_E_UNKNOWN_CARD		((LONG)0x8010000D) /**< The specified smart card name is not recognized. */
#define SCARD_E_CANT_DISPOSE		((LONG)0x8010000E) /**< The system could not dispose of the media in the requested manner. */
#define SCARD_E_PROTO_MISMATCH		((LONG)0x8010000F) /**< The requested protocols are incompatible with the protocol currently in use with the smart card. */
#define SCARD_E_NOT_READY		((LONG)0x80100010) /**< The reader or smart card is not ready to accept commands. */
#define SCARD_E_INVALID_VALUE		((LONG)0x80100011) /**< One or more of the supplied parameters values could not be properly interpreted. */
#define SCARD_E_SYSTEM_CANCELLED	((LONG)0x80100012) /**< The action was cancelled by the system, presumably to log off or shut down. */
#define SCARD_F_COMM_ERROR		((LONG)0x80100013) /**< An internal communications error has been detected. */
#define SCARD_F_UNKNOWN_ERROR		((LONG)0x80100014) /**< An internal error has been detected, but the source is unknown. */
#define SCARD_E_INVALID_ATR		((LONG)0x80100015) /**< An ATR obtained from the registry is not a valid ATR string. */
#define SCARD_E_READER_UNAVAILABLE	((LONG)0x80100017) /**< The specified reader is not currently available for use. */
#define SCARD_P_SHUTDOWN		((LONG)0x80100018) /**< The operation has been aborted to allow the server application to exit. */
#define SCARD_E_PCI_TOO_SMALL		((LONG)0x80100019) /**< The PCI Receive buffer was too small. */
#define SCARD_E_READER_UNSUPPORTED	((LONG)0x8010001A) /**< The reader driver does not meet minimal requirements for support. */
#define SCARD_E_DUPLICATE_READER	((LONG)0x8010001B) /**< The reader driver did not produce a unique reader name. */
#define SCARD_E_CARD_UNSUPPORTED	((LONG)0x8010001C) /**< The smart card does not meet minimal requirements for support. */
#define SCARD_E_SERVICE_STOPPED		((LONG)0x8010001E) /**< The Smart card resource manager has shut down. */
#define SCARD_E_UNEXPECTED		((LONG)0x8010001F) /**< An unexpected card error has occurred. */
#define SCARD_E_UNSUPPORTED_FEATURE	((LONG)0x8010001F) /**< This smart card does not support the requested feature. */
#define SCARD_E_ICC_INSTALLATION	((LONG)0x80100020) /**< No primary provider can be found for the smart card. */
#define SCARD_E_ICC_CREATEORDER		((LONG)0x80100021) /**< The requested order of object creation is not supported. */
/* #define SCARD_E_UNSUPPORTED_FEATURE	((LONG)0x80100022) / **< This smart card does not support the requested feature. */
#define SCARD_E_DIR_NOT_FOUND		((LONG)0x80100023) /**< The identified directory does not exist in the smart card. */
#define SCARD_E_FILE_NOT_FOUND		((LONG)0x80100024) /**< The identified file does not exist in the smart card. */
#define SCARD_E_NO_DIR			((LONG)0x80100025) /**< The supplied path does not represent a smart card directory. */
#define SCARD_E_NO_FILE			((LONG)0x80100026) /**< The supplied path does not represent a smart card file. */
#define SCARD_E_NO_ACCESS		((LONG)0x80100027) /**< Access is denied to this file. */
#define SCARD_E_WRITE_TOO_MANY		((LONG)0x80100028) /**< The smart card does not have enough memory to store the information. */
#define SCARD_E_BAD_SEEK		((LONG)0x80100029) /**< There was an error trying to set the smart card file object pointer. */
#define SCARD_E_INVALID_CHV		((LONG)0x8010002A) /**< The supplied PIN is incorrect. */
#define SCARD_E_UNKNOWN_RES_MNG		((LONG)0x8010002B) /**< An unrecognized error code was returned from a layered component. */
#define SCARD_E_NO_SUCH_CERTIFICATE	((LONG)0x8010002C) /**< The requested certificate does not exist. */
#define SCARD_E_CERTIFICATE_UNAVAILABLE	((LONG)0x8010002D) /**< The requested certificate could not be obtained. */
#define SCARD_E_NO_READERS_AVAILABLE    ((LONG)0x8010002E) /**< Cannot find a smart card reader. */
#define SCARD_E_COMM_DATA_LOST		((LONG)0x8010002F) /**< A communications error with the smart card has been detected. Retry the operation. */
#define SCARD_E_NO_KEY_CONTAINER	((LONG)0x80100030) /**< The requested key container does not exist on the smart card. */
#define SCARD_E_SERVER_TOO_BUSY		((LONG)0x80100031) /**< The Smart Card Resource Manager is too busy to complete this operation. */
#define SCARD_W_UNSUPPORTED_CARD	((LONG)0x80100065) /**< The reader cannot communicate with the card, due to ATR string configuration conflicts. */
#define SCARD_W_UNRESPONSIVE_CARD	((LONG)0x80100066) /**< The smart card is not responding to a reset. */
#define SCARD_W_UNPOWERED_CARD		((LONG)0x80100067) /**< Power has been removed from the smart card, so that further communication is not possible. */
#define SCARD_W_RESET_CARD		((LONG)0x80100068) /**< The smart card has been reset, so any shared state information is invalid. */
#define SCARD_W_SECURITY_VIOLATION	((LONG)0x8010006A) /**< Access was denied because of a security violation. */
/** @ingroup ErrorCodes */
#define SCARD_W_WRONG_CHV		((LONG)0x8010006B) /**< The card cannot be accessed because the wrong PIN was presented. */
/** @ingroup ErrorCodes */
#define SCARD_W_CHV_BLOCKED		((LONG)0x8010006C) /**< The card cannot be accessed because the maximum number of PIN entry attempts has been reached. */
/** @ingroup ErrorCodes */
#define SCARD_W_EOF			((LONG)0x8010006D) /**< The end of the smart card file has been reached. */
/** @ingroup ErrorCodes */
#define SCARD_W_CANCELLED_BY_USER	((LONG)0x8010006E) /**< The user pressed "Cancel" on a Smart Card Selection Dialog. */
/** @ingroup ErrorCodes */
#define SCARD_W_CARD_NOT_AUTHENTICATED	((LONG)0x8010006F) /**< No PIN was presented to the smart card. */

#endif

