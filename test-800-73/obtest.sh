#!/bin/bash
set -x
CHALLENGE_FILE=thing2
READER_NUMBER=1
OBPATH=./
opensc-tool --list-readers
rm -f obtest-retrieved-cert.der
${OBPATH}obtest-getcert ${READER_NUMBER}
echo decompress if necessary
openssl x509 -nocert -pubkey -inform DER -in obtest-retrieved-cert.der >pubkey.pem
echo 1234 >obtest-challenge.bin
openssl pkeyutl -pubin -inkey pubkey.pem -encrypt -in obtest-challenge.bin -out ${CHALLENGE_FILE}
${OBPATH}obtest-challenge ${READER_NUMBER} 200 ${CHALLENGE_FILE}
ls -l obtest-retrieved-cert.der obtest-challenge.bin ${CHALLENGE_FILE}

