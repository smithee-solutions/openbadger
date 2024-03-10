# makefile - make openbadger

# (C)Copyright 2019-2023 Smithee Solutions LLC

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

# all does not include
# (cd test-AN10957; make);

# clean does not include
# (cd test-AN10957; make clean);


all:
	(cd lib; make)
	(cd common; make);
	(cd test-800-73; make);
	(cd test-PKOC; make);
	(cd test-desfire; make);
	(cd format-tools; make);
	(cd documentation; make);

clean:
	(cd lib; make clean)
	(cd common; make clean);
	(cd test-800-73; make clean);
	(cd test-PKOC; make clean);
	(cd test-desfire; make clean);
	(cd format-tools; make clean);
	(cd documentation; make clean);
	(cd package; make clean)
	rm -rf opt *deb

package:	all
	(cd test-PKOC; make package);
	mkdir -p opt/tester/include
	cp include/ob-7816.h opt/tester/include
	cp include/ob-crypto.h opt/tester/include
	cp include/ob-pcsc.h opt/tester/include
	cp include/openbadger-common.h opt/tester/include
	cp include/openbadger-version.h opt/tester/include
	(cd documentation; make package)
	(cd package; make package)

