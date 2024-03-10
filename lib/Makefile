# Make file for openbadger test tools utilities library

LIB=libopenbadger.a
LIB_OBJECTS=ob-util.o

# (C)2023-2024 Smithee Solutions LLC

CC=clang
CFLAGS=-c -g -I../include -I /opt/openbadger/include -Wall -Werror

all:	${LIB}
	mkdir -p ../opt/openbadger/lib
	cp ${LIB} ../opt/openbadger/lib

clean:
	rm -f ${LIB} *.o

${LIB}:	${LIB_OBJECTS}
	ar r ${LIB} ${LIB_OBJECTS}

ob-util.o:	ob-util.c
	${CC} ${CFLAGS} ob-util.c

