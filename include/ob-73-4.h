/*
  openbadger - NIST SP800-73-4 definitions

  (C)Copyright 2023 Smithee Solutions LLC

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

// NIST SP800-73-4

#define OP_7816_ALGO_REF_RSA2048            (0x07)

#define OP_7816_KEY_CARD_AUTHENTICATION    (0x9E)
#define OP_7816_KEY_MANAGEMENT             (0x9D)
#define OP_7816_KEY_PIV_AUTHENTICATION_KEY (0x9A)
#define OP_7816_KEY_SIGNATURE              (0x9C)

// from 800-73-4 Part 2 Table 7
#define OP_7816_DYNAUTH_WITNESS            (0x80)
#define OP_7816_DYNAUTH_CHALLENGE          (0x81)
#define OP_7816_DYNAUTH_RESPONSE           (0x82)

int ob_challenge_response(OB_CONTEXT *ctx);
char *ob_pcsc_error_string(DWORD status_pcsc);

