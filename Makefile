# makefile - make openbadger

# for stubs only do
# make package CRYPTO_LIB=stubs

# for cyclone do
# make package CRYPTO_LIB=cyclone CRYPTO_INCLUDE="-I/opt/crypto/cyclone/cyclone_crypto -I/opt/crypto/cyclone/common" CRYPTO_LIB_EXTERNAL="-lcyclone"

# (C)Copyright 2019-2025 Smithee Solutions LLC

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

#   http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# make file for openbadger

all:
	(cd common; make);
	(cd test-crypto; make package);
	#(cd test-800-73; make);
	#(cd test-AN10957; make);
	#(cd test-pkix; make)
	#(cd format-tools; make);
	(cd documentation; make);

clean:
	(cd common; make clean);
	(cd test-crypto; make clean);
	#(cd test-800-73; make clean);
	#(cd test-AN10957; make clean);
	#(cd test-pkix; make clean)
	#(cd format-tools; make clean);
	(cd documentation; make clean);
	(cd package; make clean)
	rm -rf opt *deb

includes:
	mkdir -p opt/openbadger/include
	cp include/ob-crypto_*h opt/openbadger/include
	cp include/openbadger-common.h opt/openbadger/include
	cp include/ob-pcsc.h opt/openbadger/include
	cp include/ob-7816.h opt/openbadger/include

package:	all includes
	(cd test-crypto; make package)
	#(cd test-pkix; make package)
	(cd documentation; make package)
	(cd package; make package)

