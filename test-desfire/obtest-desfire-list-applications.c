/*
  obtest-desfire-list-applications - lists all the AID's


  (C)2022-2024 Smithee Solutions LLC

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

{ /* main for obtest-desfire-list-applications */

  char action_verb [1024];
  MifareDESFireAID *aid_list;
  size_t aid_list_count;
      uint32_t aid_number;
      unsigned char aid_octets [4];
  OBTEST_DESFIRE_CONTEXT *ctx;
  int i;
  OBTEST_DESFIRE_CONTEXT obtest_context_desfire;
  int status;
  int status_freefare;


  // initialize tool

  status = ST_OK;
  ctx = &obtest_context_desfire;
  memset((void *)ctx, 0, sizeof(*ctx));
  fprintf(stderr, "OpenBadger Test: DESFire List Applications %s\n", OPENBADGER_VERSION);
  strcpy(action_verb, "list-applications");
  if (ctx->verbosity > 0)
    fprintf(stderr, "obtest-desfire-%s initializing.\n", action_verb);
  status = ob_desfire_init(ctx);
  if (status EQUALS ST_OK)
  {
    status = ob_desfire_load_settings(ctx);
  };
  if (status EQUALS ST_OK)
  {
    if (ctx->verbosity > 3)
      fprintf(stderr, "settings loaded.\n");
    status = ob_desfire_load_context(ctx);
  };

  status = ob_desfire_open(ctx);
  if (status EQUALS ST_OK)
  {
    if (ctx->verbosity > 3)
      fprintf(stderr, "obtest-desfire-list-applications is in start-up.\n");

    status_freefare = mifare_desfire_get_application_ids(ctx->tag, &aid_list, &aid_list_count);
    if (status_freefare < 0)
      status = STOB_AID_ERROR;
  };
  if (status EQUALS ST_OK)
  {
    for (i=0; i<aid_list_count; i++)
    {
      aid_number = mifare_desfire_aid_get_aid(aid_list[i]);
      memcpy(aid_octets, &aid_number, 4);
      fprintf(stderr, "%2d AID 0x%08X (%02X%02X%02X%02X)\n",
        i, mifare_desfire_aid_get_aid(aid_list[i]), aid_octets [0], aid_octets [1], aid_octets [2], aid_octets [3]);
    };
    if (aid_list_count > 0) // if we looked at anything
      mifare_desfire_free_application_ids(aid_list);
  };
  return(status);

} /* main for obtest-desfire-list-applications */

