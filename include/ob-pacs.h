/*
  openbadger access card formats (various)

   Copyright 2023 Smithee Solutions LLC

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

#define OB_PACS_DATA_SIA26BITWIEGAND (1)
#define OB_PACS_DATA_NXPAN10957      (2)
#define OB_PACS_DATA_3               (3)

// "type 1" - 26 bit wiegand

// TBD

// "type 2" - AN-10957

typedef struct __attribute__((packed)) ob_pacs_AN10957
{
  unsigned char version_major;
  unsigned char version_minor;
  unsigned char site_customer [5];
  unsigned char credential_ID [8];
  unsigned char reissue_code;
  unsigned char PIN_code [4];
  unsigned char customer_specific_data [20];
} OB_PACS_AN10957;


// "type 3" access control card contents

#define OB_CMAC_SIG_LENGTH          (8)
#define OB_CREDENTIAL_LENGTH_OCTETS (8)
#define OB_CREDENTIAL_LENGTH_DIGITS (16)
#define OB_ISSUED_MAX               (8)
#define OB_RAW_LENGTH_OCTETS        (16)
#define OB_RESERVED_1               (9)
#define OB_SITE_LENGTH_OCTETS       (5)
#define OB_SITE_LENGTH_DIGITS       (10)
#define OB_VENDOR_LENGTH_OCTETS     (5)
#define OB_VENDORID_LENGTH_DIGITS   (4)

typedef struct ob_pacs_3
{
  unsigned char major_version;
  unsigned char minor_version;
  unsigned char site_code_bcd [OB_SITE_LENGTH_OCTETS];
  unsigned char credential [OB_CREDENTIAL_LENGTH_OCTETS];
  unsigned char format;
  unsigned char bits;
  unsigned char raw [OB_RAW_LENGTH_OCTETS];
  unsigned char external [OB_CREDENTIAL_LENGTH_OCTETS];
  unsigned char vendor [OB_VENDOR_LENGTH_OCTETS];
  unsigned char re_issue;
  unsigned char rfu_1 [OB_RESERVED_1];
  unsigned char issuer_auth [OB_CMAC_SIG_LENGTH];
  unsigned char issued_auth [OB_ISSUED_MAX] [OB_CMAC_SIG_LENGTH];
} OB_PACS_3;

