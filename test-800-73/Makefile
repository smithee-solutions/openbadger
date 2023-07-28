# test-800-73 - testing of "PIV" cards

# (C)2023 Smithee Solutions LLC

PROGS=obtest-challenge obtest-getcert

CC=clang
# PCSC is included because their own includes get it wrong
CFLAGS=-c -g -I/usr/include/PCSC -I../include -Wall -Werror
LINK=clang
LDFLAGS=-g ../opt/tester/lib/libopenbadger.a -ljansson -lpcsclite 

all:	${PROGS}
	mkdir -p ../opt/tester/bin
	cp ${PROGS} ../opt/tester/bin

clean:
	rm -f core *.o ${PROGS}

package:
	mkdir -p ../opt/tester/bin
	cp ${PROGS} ../opt/tester/bin

obtest-challenge:	Makefile obtest-challenge.o ob-genauth.o
	${LINK} -o obtest-challenge obtest-challenge.o ob-genauth.o ${LDFLAGS}

obtest-getcert:	Makefile obtest-getcert.o ob-genauth.o
	${LINK} -o obtest-getcert obtest-getcert.o ob-genauth.o ${LDFLAGS}

ob-genauth.o:	ob-genauth.c \
	../include/ob-73-4.h ../include/ob-7816.h ../include/ob-pcsc.h ../include/openbadger-common.h
	${CC} ${CFLAGS} ob-genauth.c

obtest-challenge.o:	obtest-challenge.c \
	../include/ob-73-4.h ../include/ob-7816.h ../include/ob-pcsc.h ../include/openbadger-common.h ../include/openbadger-version.h
	${CC} ${CFLAGS} obtest-challenge.c

obtest-getcert.o:	obtest-getcert.c \
	../include/ob-73-4.h ../include/ob-7816.h ../include/ob-pcsc.h ../include/openbadger-common.h ../include/openbadger-version.h
	${CC} ${CFLAGS} obtest-getcert.c

