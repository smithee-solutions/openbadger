---
title: openbadger operation
---

\newpage {}

# Introduction #

Various (one for now) miscellaneous tools for credential a/k/a badging
operations.

# openbadger operation #

there are various tools.  run them individually.  they use one common
and various other tool-specific settings files.  These files are in JSOn format.

## Settings ##

'Settings' are parameters set for every run of the program.
openbadger-settings.json contains the settings.  The default copy is
in /opt/tester/etc/openbadger-settings.json

___verbosity___
"0" for silent running, "3" for normal output, "9" for debug output.

## Installation ##

load the package

```
  dpkg -i openbadger_x.xx_arm64.deb
```

## Tools ##

### divutil ###

divutil performs the calculations listed in AN10957 to perform key diversification.

divutil uses the common settings file /opt/tester/etc/openbadger-settings.json.

# Building openbadger tools #

## Required Components ##

- various packages: libjansson-dev
- build from source: tiny-AES-C, libnfc, libfreefare
- to build test-crypto -> test-wolfssl, requires WolfSSL library
- to build test-crypto -> test-cyclone, requires Cyclone SSL library

## OpenBadger Package ##

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

## B. Building subcomponents from source ##

### B.1 Building with WolfSSL ###

### B.2 Building with Cyclone ###

### B.3 Building with libnfc and libfreefare ###

### B.4 Building with tiny-AES-c ###

```
git clone https://github.com/kokke/tiny-AES-c
cd tiny-AES-c
make
mkdir -p /opt/openbadger/lib
cp aes.o /opt/openbadger/lib
mkdir -p /opt/openbadger/include
cp aes.h /opt/openbadger/include
```

# Punchlist #

update divutil to use /opt/openbadger

base64?

