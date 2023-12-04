CC=clang
CFLAGS=-c -g -I. -Wall -Werror

all:	base64.o
	mkdir -p ../../opt/tester/lib
	cp base64.o ../../opt/tester/lib

clean:
	rm -f core *.o

base64.o:	base64.c base64.h
	${CC} ${CFLAGS} base64.c

