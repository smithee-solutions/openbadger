# test-800-73 - testing of "PIV" cards

# (C)2023 Smithee Solutions LLC

PROGS=test-piv-challenge

CC=clang
# PCSC is included because their own includes get it wrong
CFLAGS=-c -g -I/usr/include/PCSC -I../include -Wall -Werror
LINK=clang
LDFLAGS=-g -lpcsclite

all:	${PROGS}
	mkdir -p ../opt/tester/bin
	cp ${PROGS} ../opt/tester/bin

clean:
	rm -f core *.o ${PROGS}

package:
	echo no package set-up available

test-piv-challenge:	Makefile test-piv-challenge.o ob-genauth.o ob-utils.o
	${LINK} -o test-piv-challenge test-piv-challenge.o ob-genauth.o ob-utils.o ${LDFLAGS}

ob-genauth.o:	ob-genauth.c \
	../include/ob-73-4.h ../include/ob-7816.h ../include/ob-pcsc.h ../include/openbadger-common.h
	${CC} ${CFLAGS} ob-genauth.c

ob-utils.o:	ob-utils.c \
	../include/ob-73-4.h ../include/ob-7816.h ../include/ob-pcsc.h ../include/openbadger-common.h
	${CC} ${CFLAGS} ob-utils.c

test-piv-challenge.o:	test-piv-challenge.c \
	../include/ob-73-4.h ../include/ob-7816.h ../include/ob-pcsc.h ../include/openbadger-common.h ../include/openbadger-version.h
	${CC} ${CFLAGS} test-piv-challenge.c

