# decode-tools - decoding tools for DER and other badge related data

# (C)2023 Smithee Solutions LLC

PROGS=decode-oid

CC=clang
CFLAGS=-c -g -I../include -Wall -Werror
LINK=clang
LDFLAGS=-g

all:	${PROGS}
	mkdir -p ../opt/tester/bin
	cp ${PROGS} ../opt/tester/bin

clean:
	rm -f core *.o ${PROGS}

decode-oid:	decode-oid.o Makefile
	${LINK} -o decode-oid decode-oid.o ${LDFLAGS}

decode-oid.o:	decode-oid.c
	${CC} ${CFLAGS} decode-oid.c

