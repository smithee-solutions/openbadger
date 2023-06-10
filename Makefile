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

all:
	(cd test-800-73; make);
	(cd test-an10957; make);

clean:
	(cd test-800-73; make clean);
	(cd test-an10957; make clean);
	(cd package; make clean)
	rm -rf opt *deb

package:	all
	(cd package; make package)

