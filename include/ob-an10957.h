/*
  ob-an10957 - definitions for AN-10957 format

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


typedef struct ob_ctx_AN10957
{
  unsigned char diversified_key [OB_AES128_KEY_SIZE];
} OB_CONTEXT_AN10957;
int diversify_AN10957(OB_CONTEXT *ctx);

