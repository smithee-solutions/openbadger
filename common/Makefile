# common routines for openbadger

# (C)2023-2024 Smithee Solutions LLC

CC=clang
CFLAGS=-c -g -I../include -Wall -Werror

all:	openbadger-utils.o
	(cd Base64; make)

clean:
	rm -f core *.o *.a
	(cd Base64; make clean)

openbadger-utils.o:	openbadger-utils.c
	${CC} ${CFLAGS} openbadger-utils.c

