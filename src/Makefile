# Make file for credential component construction tools
# includes AN10957 stuff

#  Copyright 2017-2023 Smithee Solutions LLC
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.


PROGS=divutil


CC=clang
LINK=clang
BUILD_DIR=../opt/tester
CFLAGS=-c -g -I. -I../include -I/opt/tester/include -Wall -Werror
LDFLAGS=-L${BUILD_DIR}/lib /opt/tester/lib/aes.o -ljansson

all:	${PROGS}

clean:
	rm -rvf ${PROGS} core *.o

build:	all
	mkdir -p ../opt/tester/bin
	cp ${PROGS} ../opt/tester/bin

create-OES-contents:	create-OES-contents.o \
	  oes-prims.o oes-messages.o Makefile
	${LINK} -o create-OES-contents create-OES-contents.o \
	  oes-prims.o oes-messages.o ${LDFLAGS}

create-OES-contents.o:	create-OES-contents.c ../../include/card-whisperer-version.h
	${CC} ${CFLAGS} create-OES-contents.c

oes-messages.o:	oes-messages.c
	${CC} ${CFLAGS} oes-messages.c

oes-prims.o:	oes-prims.c
	${CC} ${CFLAGS} oes-prims.c

read_mifare:	read_mifare.o Makefile
	${LINK} -o read_mifare -g read_mifare.o ${LDFLAGS}

read_mifare.o:	read_mifare.c
	${CC} ${CFLAGS} read_mifare.c

divutil:	divutil.o oo-keys.o Makefile
	${LINK} -o divutil divutil.o oo-keys.o ${LDFLAGS}

divutil.o:	divutil.c ../include/openbadger.h ../include/openbadger-version.h
	${CC} ${CFLAGS} divutil.c

ob-keys.o:	ob-keys.c ../include/openbadger.h
	${CC} ${CFLAGS} ob-keys.c

