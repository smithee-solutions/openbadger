/*
  ob-prims - primitives to support openbadger

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

#include <stdio.h>
#include <string.h>

#include <openbadger.h>


char
  *buffer_dump_string
    (OB_CONTEXT *ctx,
    unsigned char *buffer,
    int buffer_length,
    char *tag)

{ /* buffer_dump_string */

  int buffer_size;
  int i;
  static char string_buffer [8*OB_STRING_MAX];
  char tmps [OB_STRING_MAX];


  buffer_size = OB_KEY_SIZE_10957;
  sprintf(string_buffer, "%s", tag);
  for (i=0; i<buffer_size; i++)
  {
    sprintf(tmps, "%02X", buffer [i]);
    strcat(string_buffer, tmps);
  };
  strcat(string_buffer, "\n");

  return(string_buffer);

} /* buffer_dump_string */


void
  array_shift_left
    (OB_CONTEXT *ctx,
    unsigned char *from,
    unsigned char *to)

{ /* array_shift_left */

  int buffer_size;
  unsigned int carry;
  int i;
  unsigned int next_carry;


  buffer_size = OB_KEY_SIZE_10957;
  if (ctx->verbosity > 9)
  {
    fprintf(stderr, "DEBUG: shift (size=%d) input:\n", buffer_size);
    for (i=0; i<buffer_size; i++)
    {
      fprintf(stderr, "%02X", from [i]);
    };
    fprintf(stderr, "\n");
  };
  carry = 0;
  for (i=(buffer_size-1); i>=0; i--)
  {
    if (from [i] & 0x80)
      next_carry = 1;
    else
      next_carry = 0;
    to [i] = (from [i] << 1) | carry;
    if (ctx->verbosity > 9)
      fprintf(stderr, "DEBUG: (%2d) from %02X to %02X c%d nc%d\n",
        i, from [i], to[i], carry, next_carry);
    carry = next_carry;
  };
  if (ctx->verbosity > 9)
  {
    fprintf(stderr, "DEBUG: shift (size=%d) results:\n", buffer_size);
    for (i=0; i<buffer_size; i++)
    {
      fprintf(stderr, "%02X", to [i]);
    };
    fprintf(stderr, "\n");
  };

} /* array_shift_left */


void
  array_xor
    (OB_CONTEXT *ctx,
    unsigned char *result,
    unsigned char *xor_left,
    unsigned char *xor_right)

{ /* array_xor */

  int buffer_size;
  int i;


  buffer_size = OB_KEY_SIZE_10957;
  if (ctx->verbosity > 3)
    fprintf(LOG, "%s", buffer_dump_string(ctx, xor_left, buffer_size, "XOR (Left):"));
  if (ctx->verbosity > 3)
    fprintf(LOG, "%s", buffer_dump_string(ctx, xor_right, buffer_size, "XOR (Right):"));
  for (i=0; i<buffer_size; i++)
  {
    result [i] = xor_left [i] ^ xor_right [i];
  };
  if (ctx->verbosity > 3)
  {
    fprintf(stderr, "DEBUG: xor (size=%d) results:\n", buffer_size);
    for (i=0; i<buffer_size; i++)
    {
      fprintf(stderr, "%02X", result [i]);
    };
    fprintf(stderr, "\n");
  };
  if (ctx->verbosity > 3)
    fprintf(LOG, "%s", buffer_dump_string(ctx, result, buffer_size, "XOR Results:"));

} /* array_xor */


char
  *string_hex_buffer
    (OB_CONTEXT *ctx,
    unsigned char *buf,
    int buf_lth)

{ /* string_hex_to_buffer */

  int i;
  static char returned_string [OB_STRING_MAX];
  char tmps [3];


  returned_string [0] = 0;
  for (i=0; i<buf_lth; i++)
  {
    sprintf(tmps, "%02X", *(buf+i));
    strcat(returned_string, tmps);
    if (ctx->verbosity > 3)
      if (strlen(returned_string) > (OB_STRING_MAX-8))
        fprintf(stderr, "string too long\n");
  };
  return(returned_string);

} /* string_hex_to_buffer */

