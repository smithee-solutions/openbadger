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

// "type 1" access control card contents

#define OR_CREDENTIAL_LENGTH_OCTETS (8)
#define OR_CREDENTIAL_LENGTH_DIGITS (16)
#define OR_RAW_LENGTH_OCTETS        (16)
#define OR_RESERVED_1               (9)
#define OR_SITE_LENGTH_OCTETS       (5)
#define OR_SITE_LENGTH_DIGITS       (10)
#define OR_VENDOR_LENGTH_OCTETS     (5)
#define OR_VENDORID_LENGTH_DIGITS   (4)

typedef structure or_access_control_1
{
  unsigned char major_version;
  unsigned char minor_version;
  unsigned char site_code_bcd [OR_SITE_LENGTH_OCTETS];
  unsigned char credential [OR_CREDENTIAL_LENGTH_OCTETS];
  unsigned char format;
  unsigned char bits;
  unsigned char raw [OR_RAW_LENGTH_OCTETS];
  unsigned char external [OR_CREDENTIAL_LENGTH_OCTETS];
  unsigned char vendor [OR_VENDOR_LENGTH_OCTETS];
  unsigned char re_issue;
  unsigned char rfu_1 [OR_RESERVED_1];
  unsigned char issuer_auth [OR_CMAC_SIG_LENGTH];
  unsigned char issued_auth [OR_ISSUED_MAX] [OR_CMAC_SIG_LENGTH];
} OR_ACCESS_CONTROL_1;

