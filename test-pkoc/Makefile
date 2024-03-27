# test-pkoc - Make file for PKOC / OSDP test harness

# (C)2023 Smithee Solutions LLC

PROGS=obtest-pkoc pkoc-pd

CC=clang
CFLAGS= \
  -c -g -I/opt/tester/include -I/usr/include/PCSC -I../include -Wall -Werror
LINK=clang
LDFLAGS=-g -ljansson -lpcsclite 

all:	${PROGS}

clean:
	rm -f core *.o ${PROGS}

obtest-pkoc:	Makefile obtest-pkoc.o ob-pkoc.o ob-settings.o
	${LINK} -o obtest-pkoc obtest-pkoc.o ob-pkoc.o ob-settings.o ${LDFLAGS}

ob-pkoc.o:	ob-pkoc.c \
	../include/ob-pkoc.h
	${CC} ${CFLAGS} ob-pkoc.c

ob-settings.o:	ob-settings.c
	${CC} ${CFLAGS} ob-settings.c

obtest-pkoc.o:	obtest-pkoc.c \
	../include/ob-pkoc.h
	${CC} ${CFLAGS} -DALLOCATE_SELECT obtest-pkoc.c

pkoc-pd:	pkoc-pd.o Makefile
	${LINK} -o pkoc-pd pkoc-pd.o ${LDFLAGS}

pkoc-pd.o:	pkoc-pd.c
	${CC} ${CFLAGS} pkoc-pd.c

