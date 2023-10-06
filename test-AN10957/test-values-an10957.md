# AN10957 Test Values #

## Key Allocation ##

Keys are assigned key numbers when used in a DESFire credential.  By convention keys are assigned to specific ordinal slot numbers
when using a slot-based hardware token for key sharing.

- PICC master key is desfire key 0.
- application master key.
- -no- application validation key.
- -no- originality key.
- application read key fro PACS Data object.
- application write key for PACS Data Object.
- application write key for Card Identifier Object

see AN12752?

## Key Slot Assignments ##

| Hardware Token Key Slot | Key Assignment |
| ----------------------- | -------------- |
|                         |                |
| 1                       | PICC Master    |
|                         |                |
| 2                       | Application Master |
|                         |                    |
| 3                       | Application Write (files 1 and 2) |
|                         |                                   |
| 4                       | Application Read (file 2)         |
|                         |                                   |


DESFire credential configuration
--------------------------------

- PICC Master Key diversification per AN10922.
- AppID 00 00 00
- Option 0 Static UID
- Option 2 ATS 06 7B 77 81 02 80
- Option 3 SAK 20
- Option 4 Secure Message 0001
- Option 6 VCIID 0
- Option 0x0C ATQ 0344
- PICC Key Setting 09
- GetVersion Default
- Batch Number - Table 12
- Type ID - Table 12
- Production Week Table 12
- Production Year Table 12
- Originality Signature per 7.14.2

## Test Keys ##

| Key | Test Value |
| --- | ---------- |
|     |            |
| PICC Master | 7EAF0033445566778899aabbccddeeff |
|                    |                                  |
| Application Master | 7EAF0133445566778899aabbccddeeff |
|                    |                                  |
| Application Write (files 1 and 2) | 7EAF1122445566778899aabbccddeeff |
|                                   |                                  |
| Application Read (file 2)         | 7EAF0233445566778899aabbccddeeff |
|                                   |                                  |

## References ##

[1] NXP Application Note AN10957, Generic Access Control Data Model,
Rev. 1.1 -- 7 March 2011, 196811.  https://www.nxp.com/docs/en/application-note/AN10957.pdf

[2] AN10922

