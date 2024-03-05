---
title: openbadger operation
---

Various (one for now) miscellaneous tools for credential a/k/a badging
operations.

# Operation #

## Installation ##

load the package

```
  dpkg -i blah.deb
```

## utilities ##

### divutil ###

divutil performs the calculations listed in AN10957 to perform key diversification.

divutil uses the common settings file /opt/tester/etc/openbadger-settings.json.

## Settings ##

'Settings' are parameters set for every run of the program.
openbadger-settings.json contains the settings.  The default copy is
in /opt/tester/etc/openbadger-settings.json

___verbosity___
"3" for normal output, "9" for debug output.

# Building OpenBadger #

run make from the top level.

assumes libjansson-dev package is installed

assumes https://github/kokke/tiny-AES-c is installed with files in /opt/tester
(see src/Makefile for pointers)

# Appendix #

## A. Colophon ##

(C)Copyright 2023-2024 Smithee Solutions LLC

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

