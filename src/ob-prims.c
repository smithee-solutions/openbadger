#include <stdio.h>
#include <string.h>

#include <openbadger.h>


void
  array_shift_left
    (unsigned char *from,
    unsigned char *to)

{

  unsigned char accumulator;
  int i;


  accumulator = 0;
  for (i=0; i<(OB_KEY_SIZE_10957-2); i++)
  {
    accumulator = (0x80 & from[i+1]) >> 7;
    to [i] = ((0x7F & from [i])<<1) | accumulator;
  };
  to [OB_KEY_SIZE_10957-1] = (0x7F & from [i])<<1;

}


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

