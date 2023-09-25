FIX MERGES HERE

<<<<<<< HEAD
PROGS=obtest-desfire-format obtest-desfire-init-card obtest-desfire-init-application obtest-desfire-file-init obtest-desfire-file-write obtest-desfire-file-read

CC=clang
CFLAGS=-c -g -Wall -Werror
LINK=clang
LDFLAGS=-g
=======
# Make file for obtest-desfire tools
PROGS=obtest-desfire-startup obtest-desfire-init-card

CC=clang
CFLAGS=-c -g -I../include -I /opt/smithee/include -Wall -Werror
LINK=clang
LDFLAGS=ob-desfire-utils.o -g -L /opt/smithee/lib -ljansson -lfreefare -lnfc
>>>>>>> 2dfcdc849dbed69981c15c610e6cbbd63756f25e

all:	${PROGS}

clean:
<<<<<<< HEAD
	rm -f *.o core ${PROGS}

ob-settings-desfire.o:	ob-settings-desfire.c
	${CC} ${CFLAGS} ob-settings-desfire.c

obtest-desfire-format:	obtest-desfire-format.o Makefile
	${LINK} -o obtest-desfire-format obtest-desfire-format.o ${LDFLAGS}

obtest-desfire-format.o:	obtest-desfire-format.c
	${CC} ${CFLAGS} obtest-desfire-format.c
=======
	rm -f core *.o ${PROGS}

obtest-desfire-init-card:	obtest-desfire-init-card.o Makefile \
	  ob-desfire-card-utils.o ob-desfire-utils.o
	${LINK} -o obtest-desfire-init-card ob-desfire-card-utils.o obtest-desfire-init-card.o ${LDFLAGS}

obtest-desfire-init-card.o:	obtest-desfire-init-card.c \
		../include/ob-desfire.h
	${CC} ${CFLAGS} obtest-desfire-init-card.c

obtest-desfire-startup:	obtest-desfire-startup.o Makefile \
	  ob-desfire-card-utils.o ob-desfire-utils.o
	${LINK} -o obtest-desfire-startup ob-desfire-card-utils.o obtest-desfire-startup.o ${LDFLAGS}

obtest-desfire-startup.o:	obtest-desfire-startup.c \
		../include/ob-desfire.h
	${CC} ${CFLAGS} obtest-desfire-startup.c

ob-desfire-card-utils.o:	ob-desfire-card-utils.c
	${CC} ${CFLAGS} ob-desfire-card-utils.c

ob-desfire-utils.o:	ob-desfire-utils.c
	${CC} ${CFLAGS} ob-desfire-utils.c
>>>>>>> 2dfcdc849dbed69981c15c610e6cbbd63756f25e

