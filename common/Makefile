# common routines for openbadger

# (C)2023 Smithee Solutions LLC

CC=clang
# PCSC is included because their own includes get it wrong
CFLAGS=-c -g -I/usr/include/PCSC -I../include -Wall -Werror

all:	libopenbadger.a

clean:
	rm -f core *.o *.a

libopenbadger.a:	ob-utils.o
	ar r libopenbadger.a ob-utils.o
	mkdir -p ../opt/tester/lib
	cp libopenbadger.a ../opt/tester/lib

ob-utils.o:	ob-utils.c \
	../include/ob-73-4.h ../include/ob-7816.h ../include/ob-pcsc.h ../include/openbadger-common.h
	${CC} ${CFLAGS} ob-utils.c

