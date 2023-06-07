/*
  openbadger - ISO 7816 definitions

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

// generic 7816-4

#define OB_7816_APDU_PAYLOAD_MAX            (255)
#define OB_7816_SEND_LE                     (0x0001)

// NIST SP800-73-4

#define OB_7816_ALGO_REF_RSA2048            (0x07)

#define OB_7816_KEY_CARD_AUTHENTICATION    (0x9E)
#define OB_7816_KEY_MANAGEMENT             (0x9D)
#define OB_7816_KEY_PIV_AUTHENTICATION_KEY (0x9A)
#define OB_7816_KEY_SIGNATURE              (0x9C)

// from 800-73-4 Part 2 Table 7
#define OB_7816_DYNAUTH_WITNESS            (0x80)
#define OB_7816_DYNAUTH_CHALLENGE          (0x81)
#define OB_7816_DYNAUTH_RESPONSE           (0x82)

