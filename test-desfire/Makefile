# Make file for obtest desfire tools

# (C)2023-2024 Smithee Solutions LLC

PROGS=\
  obtest-desfire-info \
  obtest-desfire-list-applications

CC=clang
CFLAGS=-c -g -I../include -I /opt/openbadger/include -Wall -Werror
LINK=clang
LDFLAGS=-g -ljansson -L ../opt/openbadger/lib -lopenbadger -L /opt/openbadger/lib -lnfc -ljansson -lfreefare

all:	${PROGS}

clean:
	rm -f *.o core ${PROGS}

obtest-desfire-info:	obtest-desfire-info.o Makefile \
	  ob-desfire-card-utils.o ob-desfire-utils.o
	${LINK} -o obtest-desfire-info obtest-desfire-info.o \
	  ob-desfire-utils.o ob-desfire-card-utils.o ${LDFLAGS}

obtest-desfire-list-applications:	obtest-desfire-list-applications.o Makefile \
	  ob-desfire-card-utils.o ob-desfire-utils.o
	${LINK} -o obtest-desfire-list-applications obtest-desfire-list-applications.o \
	  ob-desfire-utils.o ob-desfire-card-utils.o ${LDFLAGS}

obtest-desfire-info.o:	obtest-desfire-info.c \
	../include/ob-crypto.h ../include/ob-desfire.h ../include/ob-status-desfire.h ../include/openbadger-common.h
	${CC} ${CFLAGS} obtest-desfire-info.c

obtest-desfire-list-applications.o:	obtest-desfire-list-applications.c \
	../include/ob-crypto.h ../include/ob-desfire.h ../include/ob-status-desfire.h ../include/openbadger-common.h
	${CC} ${CFLAGS} obtest-desfire-list-applications.c

ob-desfire-card-utils.o:	ob-desfire-card-utils.c
	${CC} ${CFLAGS} ob-desfire-card-utils.c

