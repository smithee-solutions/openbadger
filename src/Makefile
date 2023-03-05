# Make file for credential component construction tools

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


PROGS=divutil sigutil


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

divutil:	divutil.o ob-crypto.o ob-diversify.o ob-init.o ob-keys.o ob-prims.o Makefile
	${LINK} -o divutil divutil.o ob-crypto.o ob-diversify.o ob-init.o ob-keys.o ob-prims.o ${LDFLAGS}

sigutil:	sigutil.o Makefile ob-crypto.o ob-diversify.o ob-init.o ob-keys.o ob-prims.o
	${LINK} -o sigutil sigutil.o ob-crypto.o ob-diversify.o ob-init.o ob-keys.o ob-prims.o ${LDFLAGS}

divutil.o:	divutil.c ../include/openbadger.h ../include/openbadger-version.h
	${CC} ${CFLAGS} divutil.c

ob-crypto.o:	ob-crypto.c ../include/openbadger.h
	${CC} ${CFLAGS} ob-crypto.c

ob-diversify.o:	ob-diversify.c ../include/openbadger.h
	${CC} ${CFLAGS} ob-diversify.c

ob-init.o:	ob-init.c ../include/openbadger.h
	${CC} ${CFLAGS} ob-init.c

ob-keys.o:	ob-keys.c ../include/openbadger.h
	${CC} ${CFLAGS} ob-keys.c

ob-prims.o:	ob-prims.c ../include/openbadger.h ../include/openbadger-an10957.h
	${CC} ${CFLAGS} ob-prims.c

sigutil.o:	sigutil.c ../include/openbadger.h ../include/openbadger-version.h ../include/openbadger-an10957.h
	${CC} ${CFLAGS} sigutil.c

