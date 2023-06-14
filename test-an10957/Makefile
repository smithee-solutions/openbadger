# make AN-10957 examples

# assumes github kokke/tiny-AES-c is installed and files are in /opt/tester

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

PROGS=divutil sig-test-AN10957

CC=clang
LINK=clang
BUILD_DIR=../opt/tester
CFLAGS=-c -g -I. -I../include -I /opt/tester/include -I/usr/include/PCSC -Wall -Werror
LDFLAGS=-L${BUILD_DIR}/lib /opt/tester/lib/aes.o -ljansson

all:	${PROGS}
	mkdir -p ../opt/tester/bin
	cp ${PROGS} ../opt/tester/bin

clean:
	rm -rvf ${PROGS} core *.o

package:	all
	mkdir -p ../opt/tester/bin
	cp ${PROGS} ../opt/tester/bin

divutil:	divutil.o ob-crypto.o ob-diversify.o ob-init.o ob-keys.o ob-util.o Makefile
	${LINK} -o divutil divutil.o ob-crypto.o ob-diversify.o ob-init.o ob-keys.o ob-util.o ${LDFLAGS}

sig-test-AN10957:	sig-test-AN10957.o Makefile ob-crypto.o ob-diversify.o ob-init.o ob-keys.o ob-util.o
	${LINK} -o sig-test-AN10957 sig-test-AN10957.o ob-crypto.o ob-diversify.o ob-init.o ob-keys.o ob-util.o ${LDFLAGS}

divutil.o:	divutil.c \
	../include/openbadger-common.h 
	${CC} ${CFLAGS} divutil.c

ob-crypto.o:	ob-crypto.c \
	../include/ob-an10957.h\
	../include/ob-crypto.h \
	../include/openbadger-common.h 
	${CC} ${CFLAGS} ob-crypto.c

ob-diversify.o:	ob-diversify.c \
	../include/ob-crypto.h \
	../include/openbadger-common.h 
	${CC} ${CFLAGS} ob-diversify.c

ob-init.o:	ob-init.c \
	../include/ob-an10957.h\
	../include/ob-crypto.h \
	../include/openbadger-common.h 
	${CC} ${CFLAGS} ob-init.c

ob-keys.o:	ob-keys.c \
	../include/ob-an10957.h\
	../include/ob-crypto.h \
	../include/openbadger-common.h 
	${CC} ${CFLAGS} ob-keys.c

ob-util.o:	ob-util.c \
	../include/ob-an10957.h\
	../include/ob-crypto.h \
	../include/openbadger-common.h \
	../include/ob-pacs.h
	${CC} ${CFLAGS} ob-util.c

sig-test-AN10957.o:	sig-test-AN10957.c \
	../include/ob-an10957.h\
	../include/ob-crypto.h \
	../include/ob-pacs.h \
	../include/openbadger-common.h \
	../include/openbadger-version.h 
	${CC} ${CFLAGS} sig-test-AN10957.c

