#!/bin/bash
set -x
READER_NUMBER=1
OBPATH=./
opensc-tool --list-readers
rm -f obtest-retrieved-cert.der
${OBPATH}obtest-getcert ${READER_NUMBER}
echo decompress if necessary
openssl x509 -nocert -pubkey -inform DER -in obtest-retrieved-cert.der >pubkey.pem
echo 1234 >obtest-challenge.bin
openssl pkeyutl -pubin -inkey pubkey.pem -encrypt -in obtest-challenge.bin -out thing2
ls -l obtest-retrieved-cert.der obtest-challenge.bin thing2
echo use the challenge as input to obtest-challenge
ls -l something-results-like

