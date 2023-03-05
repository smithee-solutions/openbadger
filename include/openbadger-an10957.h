/*
  openbadger-an10957.h - definitions from NXP AN10957

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

typedef struct __attribute__((packed)) ob_pacs_data_object
{
  unsigned char version_major;
  unsigned char version_minor;
  unsigned char site_customer [5];
  unsigned char credential_ID [8];
  unsigned char reissue_code;
  unsigned char PIN_code [4];
  unsigned char customer_specific_data [20];
} OB_PACS_DATA_OBJECT;
 
