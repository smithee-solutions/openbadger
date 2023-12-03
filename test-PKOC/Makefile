# make PKOC test tools, examples, and OSDP test harness

#  Copyright 2023 Smithee Solutions LLC
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

PROGS=obtest-pkoc pkoc-pd

CC=clang
LINK=clang
BUILD_DIR=../opt/tester
# PCSC is included because their own includes get it wrong
CFLAGS=-c -g -I. -I../include -I /opt/tester/include -I/usr/include/PCSC -Wall -Werror
LDFLAGS=-L${BUILD_DIR}/lib -lopenbadger -ljansson -lpcsclite

all:	${PROGS}

clean:
	rm -rvf ${PROGS} core *.o

package:	all
	mkdir -p ../opt/tester/bin
	mkdir -p ../opt/tester/doc
	cp ${PROGS} ../opt/tester/bin

obtest-pkoc:	Makefile obtest-pkoc.o ob-pkoc.o ../opt/tester/lib/libopenbadger.a
	${LINK} -o obtest-pkoc obtest-pkoc.o ob-pkoc.o ${LDFLAGS}

pkoc-pd:	pkoc-pd.o Makefile
	${LINK} -o pkoc-pd pkoc-pd.o ${LDFLAGS}

obtest-pkoc.o:	obtest-pkoc.c \
	../include/ob-pkoc.h
	${CC} ${CFLAGS} -DALLOCATE_SELECT obtest-pkoc.c

ob-pkoc.o:	ob-pkoc.c \
	../include/ob-pkoc.h
	${CC} ${CFLAGS} ob-pkoc.c

pkoc-pd.o:	pkoc-pd.c
	${CC} ${CFLAGS} pkoc-pd.c
