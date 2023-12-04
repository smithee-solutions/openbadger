# common routines for openbadger

# (C)2023 Smithee Solutions LLC

CC=clang
# PCSC is included because their own includes get it wrong
CFLAGS=-c -g -I/usr/include/PCSC -I../include -Wall -Werror

all:	libopenbadger.a
	(cd Base64; make)

clean:
	rm -f core *.o *.a
	(cd Base64; make clean)

libopenbadger.a:	ob-settings.o ob-utils.o
	ar r libopenbadger.a ob-settings.o ob-utils.o
	mkdir -p ../opt/tester/lib
	cp libopenbadger.a ../opt/tester/lib

ob-settings.o:	ob-settings.c \
	../include/ob-73-4.h ../include/ob-7816.h ../include/ob-pcsc.h ../include/openbadger-common.h
	${CC} ${CFLAGS} ob-settings.c

ob-utils.o:	ob-utils.c \
	../include/ob-73-4.h ../include/ob-7816.h ../include/ob-pcsc.h ../include/openbadger-common.h
	${CC} ${CFLAGS} ob-utils.c

