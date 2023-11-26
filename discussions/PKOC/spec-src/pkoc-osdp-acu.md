---
title: OSDP ACU PKOC Card Processing
include-before:
header-includes: |
  \usepackage{fancyhdr}
  \pagestyle{fancy}
  \fancyfoot[CO,CE]{OSDP ACU PKOC Card Processing 1.50}
  \fancyfoot[LE,RO]{\thepage}
include-before:
- '`\newpage{}`{=latex}'
...


---

Version 1.50

\newpage{}

Introduction
============

This document describes alternatives to process PKOC cards for access control.
It is assumed that these would be considered as an alternative to a reader
that handles the complete PKOC operation and just returns a cardholder number 
to the ACU.
This document describes processing PKOC cards with the work on the ACU
"on-loaded" (from the ACU's point of view), not off-loaded to the PD. 

These all assume the standard OSDP manufacturer specific command
format.  This specification further adopts the multi-part message format 
proposed by Integrated Engineering for PIV.

Use of a PKOC card will require multiple card operations and so the
osdp_KEEPACTIVE command is recommended to ensure the reader maintains
the link to the card during the entire card processing operation.
Since these messages are likely larger than the minimum size OSDP message
it is recommended the ACU send an osdp_ACURXSIZE command with a size of at least 1024
bytes.

This work is licensed under a Creative Commons Attribution-ShareAlike 4.0 International License.  See
http://creativecommons.org/licenses/by-sa/4.0/ for license details.

Identifying PD Configuration
----------------------------

- if the PD handles the entire PKOC NFC card processing,
it will send an osdp_RAW responses with format "unspecified" (0x00)
and a PKOC identifier as the cardholder field.  The ACU can infer it
need not process PKOC operations.
- if the PD is configured such that the ACU provides the PKOC validation
processing it will send an OSDP_PKOC_CARD_PRESENT response when a
card is presented, and ACU processing can proceed from there.
- for ACU validation, if the PD provides the reader identifier it will return an empty
(tag, length 0) reader identifier in the OSDP_PKOC_CARD_PRESENT
response.  The ACU should send an empty reader identifier in the
subsequent OSDP_PKOC_AUTH_REQUEST command.

\newpage{}

Data Format Conventions
-----------------------

In the following tables unless
otherwise specified, when two or more TLV items are listed they are meant to be 
"stacked" one after the other.  They are not wrapped with an outer TLV.  These are meant to 
be in sync with the PKOC base specification's notation.

Several new tags are introduced, see table 1.  Error response codes are listed i table 2.

: Additional TLV Tags

|  Tag | Contents |
| ---- | -------- |
| | |
| 0xFB | Error value |
| | |
| 0xFC | Card Present Payload |
| | |
| 0xFD | Transaction ID Sequence, Length 1 byte |

: Error Codes 

| Error Code | Meaning |
| ---------- | ------- |
|            |         |
| 0x00     | No error |
|            |         |
| 0x01     | ISO 7816 status (SW1/SW2.) |
|            |         |
| 0x02     | Timeout accessing card. |
|            |         |
| 0x03 | Reserved for future use. |
|            |         |
| 0x04 | Missing TLV in data. |
|            |         |
| 0x05 | TLV out of bounds. |
|            |         |
| 0x06 | Missing data to complete request. |
|            |         |
| 0x07 | Invalid data. |
|            |         |
| 0x08 | Multipart out of sequence. |
|            |         |
| 0x09 | Multipart out of bounds. |
|            |         |
| 0x0A-0x8F | Reserved for future use. |
|            |         |
| 0x80-0xFF | Reserved for private use. |
|            |         |

Length varies with code.  Details may or may not be present, variable size.

\newpage{}

Message Flow
============

Reader-Generated Challenge
--------------------------

In this case the reader creates the PKOC Authentication Request,
the ACU is responsible only for the cryptographic processing for
PKOC signature validation.  The ACU receives the request and
response TLV values and validates the values.

- ACU initializes secure channel connection with PD.  Initialization
includes osdp_ACURXSIZE of at least 1024 bytes and osdp_KEEPACTIVE.
- ACU and PD exchange conventional osdp_POLL/osdp_ACK steady-state
traffic.
- cardholder presents card
- PD sends OSDP_CARD_PRESENT to the ACU, including the select response payload.
- ACU sends OSDP_AUTH_REQUEST.  Some fields may be omitted due to pre-processing
or PD-side values being provided.
- PD sends Authentication Request to card
- card provides Authentication Response [pkoc]
- PD sends osdp_MFGREP("OSDP_AUTH_RESPONSE") in response to osdp_POLL
- ACU processes the Authentication Response, including necessary EC crypto operations.
- ACU extracts cardholder number from osdp_AUTH_RESPONSE and proceeds with access control
processing.

\newpage{}

ACU-Generated Challenge
-----------------------

In this case the reader notifies the ACU that the card is present and the
ACU creates the PKOC Authentication Request and performs the signature check (crypto) operation.

- ACU initializes secure channel connection with PD.  Initialization
includes osdp_ACURXSIZE of at least 1024 bytes and osdp_KEEPACTIVE.
- Optionally, ACU sends OSDP_PKOC_NEXT_TRANSACTION to pre-load a transaction identifier in the PD to reduce message
traffic.  This may include a sequence number.
- ACU and PD exchange conventional osdp_POLL/osdp_ACK steady-state
traffic.
- cardholder presents card
- PD identifies the card is a PKOC card and initiates PKOC card dialog.
- PD sends OSDP_PKOC_CARD_PRESENT; ACU creates transaction id; ACU sends OSDP_PKOC_AUTH_REQUEST
- PD formats Authentication Request and sends it to the credential.
- card provides Authentication Response [1]
- PD sends osdp_MFGREP("OSDP_AUTH_RESPONSE") in response to osdp_POLL.  This optionally includes a transaction id sequence to correlate it with the
appropriate previously
supplied transaction id.
- ACU processes the Authentication Response, including necessary EC crypto operations.
- ACU extracts cardholder number from osdp_AUTH_RESPONSE and proceeds with access control
processing.
- optionally PD sends OSDP_PKOC_TRANSACTION_REFRESH
- optionally ACU sends OSDP_PKOC_NEXT_TRANSACTION

\newpage{}

Message Flow
------------

```text

EAC ACU     PD    CARD
--- ---    ---    ----
 |   |      |      |
 |   |      |      |
             <-----
             card is presented
 |   |      |      |
 |   |      |      |
      <-----
      Card present (as MFG Response)
 |   |      |      |
 |   |      |      |
      ----->
Auth Request
 |   |      |      |
 |   |      |      |
             ----->
       Auth Request
 |   |      |      |
 |   |      |      |
      ...
      Poll/Ack Traffic
      ...
 |   |      |      |
 |   |      |      |
             <-----
             Auth Response
 |   |      |      |
 |   |      |      |
      <-----
      Auth Response
 |   |      |      |
 |   |      |      |
  <--
  Card data
```

\newpage{}

## Transaction Identifier Set-up ##

The PD needs the Transaction Identifier ('transaction ID')
to perform the Authentication Request.
This value can be provided by the ACU in response to an OSDP_PKOC_CARD_PRESENT response, or provided in advance of a card read
using the OSDP_PKOC_NEXT_TRANSACTION command.  The PD may request a pre-defined
transaction id by using the OSDP_PKOC_TRANSACTION_REFRESH response.
A sequence number may also be provided with the transaction ID.  This is useful for case where the ACU
is periodically sending transaction identifiers and there is a need to know which one the PD is using.
It is assumed the PD uses the last transaction identifier sent. If the PD provides a transaction identifier
sequence number in the OSDP_PKOC_AUTH_RESPONSE the ACU must use the corresponding transaction identifier in the
validatation process.

Note that the transaction ID is always generated by the ACU.

## Reader Identifier Set-up ##

The reader may or may not generate the "Reader Identifier".  If
it does generate it, a TLV with a length of 0 shall be returned to
the ACU in the OSDP_PKOC_CARD_PRESENT response.
If the ACU is to generate the reader ID, it shall provided it in the
OSDP_AUTH_REQUEST command, otherwise the OSDP_AUTH_REQUEST command
must not include a Reader ID TLV.

## Manufacturer Messages ##

To implement these operations OSDP's manufacturer-specific message
mechanism is used.  Commands and responses are defined here.  These use the 
PSIA OUI, 1A-90-21 to uniquely identify PKOC OSDP operations.  This follows the migration style
of the OSDP standard in order to facilitate future migration into the OSDP mainline specification.

: Request values for MFG and MFGREP

| Name                   | Value |
| ----                   | ----- |
|                        |       |
| OSDP_PKOC_CARD_PRESENT | 0xE0 |
| OSDP_PKOC_AUTH_REQUEST | 0xE1 |
| OSDP_PKOC_AUTH_RESPONSE | 0xE2 |
| OSDP_PKOC_NEXT_TRANSACTION | 0xE3 |
| OSDP_PKOC_TRANSACTION_REFRESH | 0xE4 |
| OSDP_PKOC_READER_ERROR        | 0xFE |

\newpage{}

OSDP_PKOC_AUTH_REQUEST
======================

This REQUEST is sent by the ACU so that the PD may issue an Authentication Request
to the card.  This contains the protocol version, transaction ID, and
"reader" identifier.  The order of these fields does not matter, the TLV tags
identify them.  If the transaction ID has been previously provided the
"Transaction ID TLV" field must be present and use a length of 0.  If the transaction id was 
previously allocated, it's corresponding sequence number may also be sent.

: OSDP_PKOC_AUTH_REQUEST MFG Payload

| Offset | Contents                                             |
|------- | ---------------------------------------------------- |
|     0  | Manufacturer OUI (3 octets)                          |
|        |                                                      |
|     3  | 0xE1 (Mfg Request Code)                              |
|        |                                                      |
|     4  | Total request payload size (Least Significant Octet) |
|        |                                                      |
|     5  | Total request payload size (Most Significant Octet)  |
|        |                                                      |
|     6  | Offset in request (Least Significant Octet)          |
|        |                                                      |
|     7  | Offset in request (Most Significant Octet)           |
|        |                                                      |
|     8  | Command fragment length (Least Significant Octet)    |
|        |                                                      |
|     9  | Command fragment length (Most Significant Octet)     |
|        |                                                      |
|    10  | Auth Command Parameter 1 ("P1") - 1 octet (only in first fragment) |
|        |                                                      |
|    11  | Auth Command Parameter 2 ("P2") - 1 octet (only in first fragment) |
|        |                                                      |
|    12  | Protocol Version TLV                                 |
|        | Transaction ID TLV (see description for use.)        |
|     | Reader Identifer TLV (or indicated to be omitted)    |
|     | Transaction ID Sequence TLV (optional)               |
 
\newpage{}

OSDP_PKOC_AUTH_RESPONSE
=======================

This RESPONSE consists of an osdp_MFGREP response.  It is sent in response to
an osdp_POLL command.
This response is sent after the Authentication Response is received from the card
by the reader.  Note this response is definitely longer than the minimum OSDP packet size and so may
be sent in fragments by the PD.

: OSDP_PKOC_AUTH_RESPONSE

| Offset | Contents                                              |
|------- | ----------------------------------------------------- |
|     0  | Manufacturer OUI (3 octets)                           |
|        |                                                       |
|     3  | 0xE2 (Mfg Response Code)                              |
|        |                                                       |
|     4  | Total response payload size (Least Significant Octet) |
|        |                                                       |
|     5  | Total response payload size (Most Significant Octet)  |
|        |                                                       |
|     6  | Offset in response (Least Significant Octet)          |
|        |                                                       |
|     7  | Offset in response (Most Significant Octet)           |
|        |                                                       |
|     8  | Response fragment length (Least Significant Octet)    |
|        |                                                       |
|     9  | Response fragment length (Most Significant Octet)     |
|        |                                                       |
|    10  | PKOC Authentication Response TLV (contais Public Key and Digital Signature TLV) |
|        | Transaction ID TLV (optional)                         |
|        | Transaction ID Sequence TLV (optional)                |
|        | Error TLV (optional, do not send if no error.)        |

\newpage{}

OSDP_PKOC_CARD_PRESENT
======================

This RESPONSE is sent by the reader to the ACU so that the ACU may specify
the Authentication Request transaction ID.  It returns the "supported protocol versions" TLV structure as received by the reader in the select response.
It can optionally include a transaction sequence value, so that the ACU can maintain sync with respect to ACU-supplied transaction id's.

Note is expected the OSDP_PKOC_AUTH_REQUEST command will be sent soon after the OSDP_PKOC_CARD_PRESENT response is received by the ACU, but not necessarily as the next command.
In general the next command after OSDP_PKOC_CARD_PRESENt is expected to be a poll.

Note this has no multipart header as the response will never exceed the minimum OSDP packet size.  In addition,
it does use an outer TLV to make PD-side processing easier.

: OSDP_PKOC_CARD_PRESENT payload

| Offset | Contents                                             |
|------- | ---------------------------------------------------- |
|      0 | Manufacturer OUI (3 octets)                          |
|        |                                                      |
|      3 | 0xE0 (Mfg Response Code)                             |
|        |                                                      |
|    4-n | Card Present TLV, contains:                          |
|        |   Supported Protocol Versions TLV                    |
|        |   Transaction Sequence TLV (optional)                |
|        |   Error TLV (optional)                               |

### Example ###

```
  0A0017E0FC065C0201004C00
```

meaning OUI 0A0017, OSDP_PKOC_CARD_PRESENT (0xE0), payload FC with length 6, Supported Protocol 0100, Transaction sequence empty (meaning not provided by PD)


\newpage{}

OSDP_PKOC_NEXT_TRANSACTION
==========================

This COMMAND consists of an osdp_MFG command.  It is sent to provide the PD
with a transaction ID to be used in the next Authentication Request.
This command may be sent at any time.
Optionally, a sequence number can be added so the ACU and the PD can track
which transaction ID is being used.  It is assumed the PD will either track these
by sequence number or use the last value sent.

: OSDP_PKOC_NEXT_TRANSACTION Payload

| Offset | Contents                                             |
|------- | ---------------------------------------------------- |
|     0  | Manufacturer OUI (3 octets)                          |
|        |                                                      |
|     3  | 0xE3 (Mfg Command Code)                              |
|        |                                                      |
|     4  | Total command payload size (Least Significant Octet) |
|        |                                                      |
|     5  | Total command payload size (Most Significant Octet)  |
|        |                                                      |
|     6  | Offset in command (Least Significant Octet)          |
|        |                                                      |
|     7  | Offset in command (Most Significant Octet)           |
|        |                                                      |
|     8  | Command fragment length (Least Significant Octet)    |
|        |                                                      |
|     9  | Command fragment length (Most Significant Octet)     |
|        |                                                      |
|    10  | Transaction ID TLV                                   |
|        | Transaction ID Sequence TLV (optional)               |

\newpage{}

OSDP_PKOC_READER_ERROR
======================

This RESPONSE consists of an osdp_MFGREP command and associated payload.  It is sent in response to a poll when
there is an error reading the card.  The code 0xFE was selected as the format
corresponds to response 0xFE in [2].

Note this has no multipart header as the response will never exceed the minimum OSDP packet size.

: OSDP_PKOC_READER_ERROR payload

| Offset | Contents                                             |
|------- | ---------------------------------------------------- |
|      0 | Manufacturer OUI (3 octets)                          |
|        |                                                      |
|      3 | 0xFE (Mfg Response Code)                             |
|        |                                                      |
|    4-n | Error TLV (see above for description.)               |

\newpage{}

OSDP_PKOC_TRANSACTION_REFRESH
=============================

This RESPONSE consists of an OSDP_MFGREP response.  It is sent in response to
an OSDP_POLL command.  
This response is sent when the PD wants to pre-load a transaction id for the next
card read.  It is expected this would be generated within a few poll cycles after an OSDP_PKOC_AUTH_RESPONSE.
Note there is no payload beyond the OUI and the response code.

: OSDP_PKOC_TRANSACTION_REFRESH payload

| Offset | Contents                                             |
|------- | ---------------------------------------------------- |
|      0 | Manufacturer OUI (3 octets)                          |
|        |                                                      |
|      3 | 0xE4 (Mfg Response Code)                             |

\newpage{}

Appendix
========

Colophon
--------

This work is licensed under a Creative Commons Attribution-ShareAlike 4.0 International License.  See
http://creativecommons.org/licenses/by-sa/4.0/ for license details.

This document was written in 'markdown', using pandoc.  PDF converter assistance provided by latex.  Linux command line to create the PDF is

```
  pandoc --toc -o pkoc-osdp-acu.pdf pkoc-osdp-acu.md
```

Document source is in github.

PKOC as used here refers to the card format specificed by PSIA.

This is version 1.50 of this document.

This document originated by Rodney Thayer (Smithee Solutions),
Mike Zercher (Secure Element Solutions),
and Mark de Olde (Integrated Engineering.)  Additional instigation provided
by Ed Chandler (Security By Design.)



Security Considerations
-----------------------

It is assumed all of this message traffic happens inside a proper OSDP
secure channel using a unique paired (not the default) key.

To mitigate potential man-in-the-middle attacks, allocation of the transaction id is limited to the ACU and so 
is different from the way an autonomous PKOC-enabled PD would behave.

OSDP Considerations
-------------------

If you perform PKOC operations on an OSDP "chain" (RS-485 bus with more than one PD)
it is possible that long delays will be introduced between the time the PD
sends an OSDP_PKOC_CARD_PRESENT response and when the ACU sends an
OSDP_PKOC_AUTH_REQUEST.  It is recommended you use a chain with a minimal number
of PD's (preferrably only one) and/or a higher line speed (38,400) and/or use
a PD that connects over TCP/IP.

Assigned Numbers
----------------

This specification assumes certain numbers will be registered.

OUI value (3-byte) - to be registerered with IEEE by (PSIA?.)

Tags FD, FE to be registered in the tag space for PKOC.

\newpage{}

## Glossary ##

7816 - ISO standard for Smart Card communications.

ACU - Access Control Unit.  IEC/OSDP terminolgy for a "panel".

Challenge - value provided to a key-pair holder to prove posession of the
private key.

Command - message from an OSDP ACU to an OSDP PD.

EC - Elliptic Curve.

NFC - Near-Field Communications.

OSDP - Open Supervised Device Protocol. 

osdp_ACK, osdp_ACURXSIZE, osdp_KEEPACTIVE, osdp_MFG, osdp_MFGREP, osdp_POLL, osdp_RAW - specific commands and responses from the OSDP specification (IEC 60839-11-5 / SIA OSDP 2.2.)

OUI - Organizational Unit Identifier - IEEE terminology for their vendor
registry.

Panel - access control device that controls readers.

PD - Peripheral Device.  IEC/OSDP terminology for a "reader" or I/O device.

PIV - Personal Identification Verification. (NIST SP 800-73-4.)

PKOC - Public Key Open Credential.

PKOC Identifier

Reader - Device that reads credentials (i.e. an ISO 14443 or Bluetooth transponder.)

Reader Identifer - PKOC data value providing identification of a PD for PKOC authentication.

Response - message from an OSDP PD to an OSDP ACU.

Secure Channel - OSDP encrypted connection between an ACU and a PD.

Tag - the identifying first octet of a TLV string.

TLV - Tag,Length,Value - variable size data format.

Transaction Identifier, Transaction ID - arbitrary value provide to the credential
to be used in signature validation.

References
----------

[pkoc] PKOC NFC Card Specification, Version 1.0 Rev0, 6/13/2023.  Physical Security Interoperability Alliance.

[2] Integrated Engineering OSDP extensions, document 100-01G-PS-01-INID "Vendor Specific OSDP Extensions v10c".

[3] OSDP, IEC 60839-11-5

[4] ISO 7816-4-2020

\newpage{}

## Change Log ##

```
1.23 - shared at GSX 2023
1.24 - fixed typo's in 1.23
1.30 - added transaction id sequence number, sorted messages,
       corrected "blah" typo.
1.31 - minor formatting changes, removed multi-part header
       from OSDP_PKOC_CARD_PRESENT response
1.40 - updated error TLV description and use; reused tag 0xFC
       for card present payload, added card present payload wrapper TLV
1.41 - correct mfg/mfgrep header to include fragment size;
       specify PSIA OUI
       add transaction id and field to auth response
       transaction id field in auth request usage updated.
       correct and add tag values
1.50   editorial changes to text; removed redundant transaction
       identifier tag; removed osdp_RAW message ; remove PD-side
       transaction ID generation; added glossary; clarify reader ID TLV
       use; add transaction sequence to auth response
```

