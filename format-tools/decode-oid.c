#include <string.h>
#include <stdio.h>

unsigned long int decode_multibyte_oid(unsigned char *octets, int *consumed);
int hex_string_to_octets(char *hex_string, unsigned char *raw_der, int *encoded_length);

int main
  (int argc,
  char *argv [])
{
  int consumed_octets;
  unsigned char *current_octet;
  int done;
  int encoded_length;
  int i;
  int oidx;
  unsigned long int oid_value;
  unsigned long int oid [1024];
  unsigned char raw_der [1024];
  int status;


  status = 0;
  encoded_length = 0;
  status = hex_string_to_octets(argv [1], raw_der, &encoded_length);
  if (status == 0)
  {
    if (encoded_length > 0)
    {
      oidx = 0;
      current_octet = raw_der;
      oid [oidx] = (*current_octet)/40;
      oidx++; // first two are in the first octet
      oid [oidx] = (*current_octet) % 40;
      oidx++; current_octet++;
      encoded_length --;
    };
    if (encoded_length > 0)
    {
      done = 1;
      if (encoded_length > 0)
        done = 0;
      while (!done)
      {
        if (!(*current_octet & 0x80))
        {
          oid [oidx] = *current_octet;
          oidx++; current_octet++;
          encoded_length --;
        };
        if (*current_octet & 0x80)
        {
          oid_value = decode_multibyte_oid(current_octet, &consumed_octets);
          oid [oidx] = oid_value;
          oidx++;
          current_octet = current_octet + consumed_octets;
          encoded_length = encoded_length - consumed_octets;
        };
        if (!encoded_length)
          done = 1;
      };
    };
  };
  if (status == 0)
  {
    fprintf(stderr, "%ld", oid [0]);
    for (i=1; i<oidx; i++)
    {
      fprintf(stderr, ".%ld", oid [i]);
    };
    fprintf(stderr, "\n");
  };
  return(status);
}


unsigned long int decode_multibyte_oid
  (unsigned char *octets,
  int *consumed)
{
  unsigned char *p;
  unsigned long int return_value;

  return_value = 0l;
  *consumed = 0;
  p = octets;
  while (*p & 0x80)
  {
    return_value = return_value <<7;
    return_value = return_value + (0x7F & *p);
    p ++;
    (*consumed)++;
  };
  return_value = (return_value << 7) + *p;

  return(return_value);

}


int hex_string_to_octets
  (char *hex_string,
  unsigned char *raw_der,
  int *encoded_length)

{
  char hexit [3];
  int octet;
  char *ph;
  unsigned char *pr;
  int lth;


  hexit [2] = 0;
  lth = 0;
  ph = hex_string;
  pr = raw_der;
  while (*ph)
  {
    memcpy(hexit, ph, 2);
    sscanf(hexit, "%x", &octet);
    *pr = octet;
    pr++;
    ph = ph + 2;
    lth++;
  };
  *encoded_length = lth;
  return(0);
}

