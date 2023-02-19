/*
  ob-keys - default and selftest key and other values

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


#include <openbadger.h>


unsigned char secret_key_default [OB_KEY_SIZE_10957] =
  {0xf3, 0xf9, 0x37, 0x76,  0x98, 0x70, 0x7b, 0x68,
   0x8e, 0xaf, 0x84, 0xab,  0xe3, 0x9e, 0x37, 0x91};

unsigned char expected_K0 [OB_KEY_SIZE_10957] =
  {0x67, 0x04, 0xa3, 0xaf,  0x8a, 0xf3, 0xd9, 0x20,
   0xa0, 0xa7, 0x59, 0x4f,  0x5c, 0xeb, 0xf9, 0xfd};

