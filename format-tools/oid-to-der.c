/*
  oid-to-der - convert a text string Object Identifier into DER-encoded binary.

  Usage:

    oid-to-der 1.3.6.1.4.1.53206.1

    output goes to stderr (text) and encoded-oid.der (binary)
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void encode_number(unsigned long int value, unsigned char *encoded, int *octets);
int extract_numbers_from_string(char *string, unsigned long int *oid_numbers, int *oid_tuple_count);


int main
  (int argc,
  char *argv [])

{ /* main for oid-to-der.c */

  int eidx;
  FILE *encfile;
  int encoded_length;
  unsigned char encoded_oid [1024];
  int i;
  int ocount;
  unsigned long int oid_values [1024];
  int status;
  int tuple_count;


  status = 0;
  encoded_length = 0;
  memset(encoded_oid, 0, sizeof(encoded_oid));

  status = extract_numbers_from_string(argv [1], oid_values, &tuple_count);
  if (status == 0)
  {
    fprintf(stderr, "oid tuple is");
    for (i=0; i<tuple_count; i++)
    {
      fprintf(stderr, " %lu", oid_values [i]);
    };
    fprintf(stderr, "\n");
  };

  if (status == 0)
  {
    eidx = 0;
    encoded_oid [eidx] = (oid_values [0] * 40) + oid_values [1];
    eidx = 1;
    for (i=2; i<tuple_count; i++)
    {
      if (oid_values [i] < 128)
      {
        encoded_oid [eidx] = oid_values [i];
        eidx++;
      };
      if (oid_values [i] > 127)
      {
        encode_number(oid_values [i], encoded_oid+eidx, &ocount);
        eidx = eidx + ocount;
      };
    };
    encoded_length = eidx;
  };

  if (status == 0)
  {
    fprintf(stderr, "Object Identifier (hex) ");
    for (i=0; i<encoded_length; i++)
    {
      fprintf(stderr, "%02X", encoded_oid [i]);
    };
    fprintf(stderr, "\n");

    encfile = fopen("encoded-oid.der", "w");
    if (encfile != NULL)
    {
      fwrite(encoded_oid, sizeof(encoded_oid[0]), encoded_length, encfile);
    };
  };

} /* main for oid-to-der.c */


void encode_number(unsigned long int value, unsigned char *encoded, int *octets)
{
  unsigned long int remainder;
  unsigned char buffer [1024];
  int i;
  int j;

  remainder = value;
  i = 0;
  while (remainder > 0)
  {
    buffer [i] = 0x80 | (remainder & 0x7f);
    remainder = remainder >> 7;
    i++;
  };
  buffer [0] = 0x7F & buffer [0];
  for (j=0; j<i; j++)
    encoded [j] = buffer [i-j-1];
  *octets = i;
}


int extract_numbers_from_string
  (char *string,
  unsigned long int *oid_numbers,
  int *oid_tuple_count)

{

  int done;
  int i;
  char *next_token;
  int status;


  status = 0;
  *oid_tuple_count = 0;
  done = 0;
  next_token = strtok(string, ".");
  if (!next_token)
  {
    done = 1;
  }
  else
  {
    sscanf(next_token, "%d", &i);
    oid_numbers [*oid_tuple_count] = i;
    (*oid_tuple_count)++;
  };
  while (!done)
  {
    next_token = strtok(NULL, ".");
    if (!next_token)
      done = 1;
    if (!done)
    {
      sscanf(next_token, "%d", &i);
      oid_numbers [*oid_tuple_count] = i;
      (*oid_tuple_count)++;
    };
  };

  return(status);
}

