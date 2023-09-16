PROGS=obtest-desfire-format obtest-desfire-init-card obtest-desfire-init-application obtest-desfire-file-init obtest-desfire-file-write obtest-desfire-file-read

CC=clang
CFLAGS=-c -g -Wall -Werror
LINK=clang
LDFLAGS=-g

all:	${PROGS}

clean:
	rm -f *.o core ${PROGS}

ob-settings-desfire.o:	ob-settings-desfire.c
	${CC} ${CFLAGS} ob-settings-desfire.c

obtest-desfire-format:	obtest-desfire-format.o Makefile
	${LINK} -o obtest-desfire-format obtest-desfire-format.o ${LDFLAGS}

obtest-desfire-format.o:	obtest-desfire-format.c
	${CC} ${CFLAGS} obtest-desfire-format.c

