#include <stdio.h>
#include <errno.h>


void skip_structure
  (unsigned char **p,
  int *length,
  int *length_length,
  int *payload_length)

{
  unsigned char *current;
  unsigned char tag;


  current = *p;
  *length_length = 0;

  // now pointing at tag

  tag = *current;
  current++; // skip past tag
  if ((*current) & 0x80)
  {
    *length_length = 0x7F & (*current);

    // assumes 2 byte length

    *payload_length = 256 * (*(current+1)) + (*(current+2));
    fprintf(stderr, "tag %02X length-length %d. length %d. value\n",
      tag, *length_length, *payload_length);
    current = current + 3;
  };
  *p = current;
#if 0




  current = *p;
fprintf(stderr, "1b: %02X %02X %02x %02x %02x\n",
    *(current+0), *(current+1), *(current+2), *(current+3), *(current+4));

  *length_length = 0;
  *payload_length = 0;
  length_size = 0;
  // skip tag
  current++;
  (*length)--; // skip tag
  if ((unsigned int)(*current) > (unsigned int)(0x80))
  {
fprintf(stderr, "2b: %02X %02X %02x %02x %02x\n",
    *(current+0), *(current+1), *(current+2), *(current+3), *(current+4));
    length_size = 0x7f & (*current);
    *length_length = length_size;
    (*length)--;
    current++; // now looking at length length
fprintf(stderr, "3b: %02X %02X %02x %02x %02x\n",
    *(current+0), *(current+1), *(current+2), *(current+3), *(current+4));
    *payload_length = (256 * (*current)) + *(current+1);

    *length = *length - *length_length;
  }
  else
  {
    length_size = 1;
    *length_length = length_size;
    *payload_length = *current;
    current++;
    (*length)--;
  };
  *p = current;
  fprintf(stderr, "length size was %d\n", length_size);
#endif
}


int main
  (int argc,
  char *argv [])

{

  FILE *f1;
  FILE *f2;
  char buffer [8192];
  int length;
  int llth;
  unsigned char *p;
  int payload_length;
  int status_io;

  fprintf(stderr, "extract public key raw components from %s\n", argv [1]);
  f1 = fopen(argv [1], "r");
  status_io = fread(buffer, sizeof(buffer[0]), sizeof(buffer), f1);
  printf("read %d\n", status_io);


  p = (unsigned char *)&(buffer[0]);
  length = status_io;
  skip_structure(&p, &length, &llth, &payload_length); // skip outer structure
fprintf(stderr, "length %d llth %d payload %d\n", length, llth, payload_length);
  skip_structure(&p, &length, &llth, &payload_length); // skip wrapper on thing1
fprintf(stderr, "length %d llth %d payload %d\n", length, llth, payload_length);
//  skip_structure(&p, &length, &llth, &payload_length); // skip thing1 inner wrapper
fprintf(stderr, "payload length %d.\n", payload_length);

  f2 = fopen("extracted", "w");
  status_io = fwrite(p, sizeof(buffer[0]), payload_length, f2);
//  status_io = fwrite(buffer+33, sizeof(buffer[0]), 256, f2);
  printf("write %d %d\n", status_io, errno);

//  status_io = fwrite(buffer, sizeof(buffer[0]), 3, f2);
//  printf("write %d %d\n", status_io, errno);
  fclose(f2);

  return(0);
}


