# common routines for openbadger

# (C)2023-2024 Smithee Solutions LLC

CC=clang
# PCSC is included because their own includes get it wrong
CFLAGS=-c -g -I/usr/include/PCSC -I../include -Wall -Werror

all:	
	(cd Base64; make)

clean:
	rm -f core *.o *.a
	(cd Base64; make clean)

