# make file for EC crypto testing

# CRYPTO_INCLUDE="-I/opt/crypto/cyclone/cyclone_crypto -I/opt/crypto/cyclone/common"
# CRYPTO_LIB_EXTERNAL="-lcyclone"

PROGS=test-ec-verify_${CRYPTO_LIB}

CC=clang
CFLAGS=-c \
  -D GPL_LICENSE_TERMS_ACCEPTED \
  -g \
  -I. \
  -I../../include \
  -I/opt/openbadger/include \
  -I. ${CRYPTO_INCLUDE} \
  -Wall -Werror
LINK=clang
LDFLAGS=-g \
  -L/opt/openbadger/lib -leac-crypto_${CRYPTO_LIB} -leac-encode \
  -L../../opt/openbadger/lib -lopenbadger \
  ${CRYPTO_LIB_EXTERNAL} \
  -ljansson -lpthread

all:	${PROGS}

clean:
	rm -f a.out *.o ${PROGS}

package:	all
	mkdir -p ../../opt/openbadger/bin
	cp ${PROGS} ../../opt/openbadger/bin

test-ec-verify_${CRYPTO_LIB}:	test-ec-verify.o Makefile
	${LINK} -o test-ec-verify_${CRYPTO_LIB} test-ec-verify.o \
	   ${LDFLAGS} 

test-ec-verify.o:	test-ec-verify.c
	${CC} ${CFLAGS} test-ec-verify.c

