---
title: Using obtest-pkoc

---
\newpage{}
# Introduction #

This tests PKOC (NFC) cards using linux and PCSC.

use "opensc-tool --list-readers" to separately confirm your smartcard
reader is visible.


# Usage #

1. configure openbadger-settings.json with appropriate parameters.
2. run obtest-pkoc (no arguments)

# Settings for PKOC #

bits - number of bits to output.

control - path to libosdp-conformance control socket (to send card reads)

format - "base64" or "osdp-raw" to choose which output format.  base64 is the proper whole DER encoded public key.

reader - smartcard reader index.  Starts at zero.  default is zero.

verbosity - logging level.  3=normal 0=silent 9=debug

# Settings #

Typical openbadger-settings.json

```
{
  "bits"      " "128",
  "reader"    : "1",
  "verbosity" :"3"
}
```

