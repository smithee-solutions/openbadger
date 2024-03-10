---
title: READ ME
subtitle: obtest-desfire DESFire card processing for OpenBadger
---


check fix
check build notes

# Introduction #

top level doc for test-desfire

programs desfire cards.

based on libfreefare in nfc-tools.

Consists of several single-purpose "tools".  there's a saved context so they can be used together.

each tool is passed a json string as it's command line. it does not know about argc,argv or command line switches
the tool parses that json string in addition to the settings and context files.

# Concepts #

- there's a settings file

verbosity - 0 for quiet, 3 for normal, 9 for debug

reader - reader number, origin 0.

several tools

each tool is a building block for reading and writing desfire cards.

### primitives ###

- list applications
- get card info

# Credential Operations #


# Tools #

## obtest-desfire-info ##

lists details about the card itself.

### settings ###

uses verbosity, reader number

## obtest-desfire-list-applications ##

lists the applications on a DESfire card.

### settings ###

uses verbosity, reader number from settings json.

\newpage {}

# Punchlist #

is it DESFire or DESfire?

is the primitives list accurate

copyright banner

\newpage {}

# Appendix #

## Colophon ##

(C)2024 Smithee Solutions LLC

## build ##

download libnfc source

compile for pcsc


to build libfreefare

LDFLAGS=-L/opt/smithee/lib CFLAGS=-I/opt/smithee/include ./configure --prefix=/opt/smithee

change BSD to DEFAULT in config.h from ./configure

