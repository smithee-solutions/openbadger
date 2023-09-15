# test-cardshell - 7816-4 command processor ("card shell")

# (C)2023 Smithee Solutions LLC

PROGS=obtest-cardshell

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

obtest-cardshell:	Makefile obtest-cardshell.o
	${LINK} -o obtest-cardshell obtest-cardshell.o ${LDFLAGS}

obtest-cardshell.o:	obtest-cardshell.c \
	../include/openbadger-version.h
	${CC} ${CFLAGS} obtest-cardshell.c

