# test-800-73 - testing of "PIV" cards

# (C)2023 Smithee Solutions LLC

PROGS=test-piv-challenge

CC=clang
# PCSC is included because their own includes get it wrong
CFLAGS=-c -g -I/usr/include/PCSC -I../include -Wall -Werror
LINK=clang
LDFLAGS=-g -lpcsclite

all:	${PROGS}

clean:
	rm -f core *.o ${PROGS}

test-piv-challenge:	Makefile test-piv-challenge.o op-genauth.o op-utils.o
	${LINK} -o test-piv-challenge test-piv-challenge.o op-genauth.o op-utils.o ${LDFLAGS}

op-genauth.o:	op-genauth.c \
	../include/op-7816.h ../include/openpkoc.h \
	../include/op-testvectors.h ../include/openpkoc-version.h
	${CC} ${CFLAGS} op-genauth.c

op-utils.o:	op-utils.c \
	../include/op-7816.h ../include/openpkoc.h \
	../include/op-testvectors.h ../include/openpkoc-version.h
	${CC} ${CFLAGS} op-utils.c

test-piv-challenge.o:	test-piv-challenge.c \
	../include/ob-73-4.h ../include/ob-7816.h ../include/ob-pcsc.h ../include/openbadger-common.h ../include/openbadger-version.h
	${CC} ${CFLAGS} test-piv-challenge.c

