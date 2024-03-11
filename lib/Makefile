# Make file for openbadger test tools utilities library

LIB=libopenbadger.a
LIB_OBJECTS=ob-lib-util.o ob-card-utils.o ob-settings.o

# (C)2023-2024 Smithee Solutions LLC

CC=clang
CFLAGS=-c -g -I../include -I /opt/openbadger/include -I/usr/include/PCSC -Wall -Werror

all:	${LIB}
	mkdir -p ../opt/openbadger/lib
	cp ${LIB} ../opt/openbadger/lib

clean:
	rm -f ${LIB} *.o

${LIB}:	${LIB_OBJECTS}
	ar r ${LIB} ${LIB_OBJECTS}

ob-lib-util.o:	ob-lib-util.c
	${CC} ${CFLAGS} ob-lib-util.c

ob-card-utils.o:	ob-card-utils.c \
	../include/ob-73-4.h ../include/ob-7816.h ../include/ob-pcsc.h ../include/openbadger-common.h
	${CC} ${CFLAGS} ob-card-utils.c

ob-settings.o:	ob-settings.c \
	../include/ob-73-4.h ../include/ob-7816.h ../include/ob-pcsc.h ../include/openbadger-common.h
	${CC} ${CFLAGS} ob-settings.c

