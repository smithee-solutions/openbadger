# Challenge/Response Testing #

## Set-Up ##

Make sure the reader is there, check the index for the contactless side if it's dual-interface.

```
  opensc-tool --list-readers
```

You should get something like this.

```
  # Detected readers (pcsc)
  Nr.  Card  Features  Name
  0    No              Identiv Identiv uTrust 4701 F Dual Interface Reader [uTrust 4701 F Contact Reader] (55041912205100) 00 00
  1    Yes             Identiv Identiv uTrust 4701 F Dual Interface Reader [uTrust 4701 F CL Reader] (55041912205100) 01 00
```

## Certificate Retrieval ##

Prepare to receive an extracted cert, run the certificate fetch utility.  Specify the proper reader (contactless is number 1 in this example.)

```
  rm -f obtest-retrieved-cert.der
  obtest-getcert 1
```

This extracts a certificate and stores it (DER or compressed format) in file obtest-retrieved-cert.der.

If it starts with the compression Magic Value, decompress it.

```
  od -tx1 obtest-retrieved-cert.der
  0000000 1f 8b 08 00 00 00 00 00 00 ff 33 68 62 11 32 68
  0000020 62 fa b5 80 99 89 91 89 49 a4 a6 6b 4a 60 88 f7
  0000040 79 b9 cf 7c 2c 6d 8c ac 05 e6 99 6a de 85 06 bc
  0000060 6c 9c 5a 6d 1e 6d df 79 19 19 b9 59 19 0c bc 0c
```

Use gunzip to decompress the compressed certificate.

```
mv obtest-retrieved-cert.der compressed.Z
gunzip compressed.Z 
mv compressed obtest-retrieved-cert.der
```

Dump it with the ASN.1 dumper to confirm it looks reasonable.

```
  dumpasn1 obtest-retrieved-cert.der
```

## Challenge Generation ##

Extract the public key using openssl.  Public key will be output in PEM format.

```
  openssl x509 -nocert -pubkey -inform DER -in obtest-retrieved-cert.der >pubkey.pem
```

Create some known plaintext, encrypt it with the public key using openssl.  Note the ciphertext will
be the same size as the public key (256 bytes for RSA2048.)  Note openssl will OEAP wrap it.

```
  echo 1234 >obtest-challenge.bin
  openssl pkeyutl -pubin -inkey pubkey.pem -encrypt -in obtest-challenge.bin -out ciphertext
```

## Challenge and Response to Challenge ##

Send the ciphertext to the card, as a challenge.  In this example it's reader 1 and we set the max APDU payload size
to 200.  This does not use extended format APDU's, it will take two rounds to get the payload there and two rounds to get the response.

```
  obtest-challenge 1 200 ciphertext

```

The result back from the card will be the OEAP wrapped version of the plaintext.  Start at the end, walk back to the null, that's your plaintext.   OEAP includes some entropy 
so repeated wrap operations will produce different wrappers.

```
0000000 00 02 75 b0 52 75 81 6c fc 31 a2 c1 07 5a c9 2b
0000020 66 9a 4d bb 26 a2 30 c2 77 25 2b 61 4a 80 54 20
0000040 be 14 81 da 30 b0 84 23 61 41 63 4a 34 83 2a 28
0000060 f3 65 6a 41 17 4a d2 44 2e 5d d1 d4 a2 f5 b1 15
0000100 26 ac ca db b2 f8 8e 29 e5 30 69 7b 81 9b ba 63
0000120 ac e3 d2 56 dc 21 d8 fa e3 b4 9a 52 fa 3b 50 fe
0000140 a1 3e 42 b6 d6 2c a1 0b 5e 9e 60 1e 3a a2 1f 49
0000160 64 c6 5a 7f 3b 3d b6 da 67 5d ec e5 c6 4f 40 27
0000200 5a db 9a 6e 65 35 a2 91 d1 78 c6 f7 77 8d 62 cd
0000220 64 32 45 c4 56 2b dd 15 a1 c7 4f fc 8e 44 9e 4b
0000240 2f 7c e2 97 68 16 60 dd 79 87 3f 9a 1b 14 d9 bb
0000260 27 8c f9 5a c0 9e fa 2b ea 2b ba 7f 43 4d 60 47
0000300 ed d5 54 3a b7 41 19 51 f8 fc 1f 2a 51 db a9 62
0000320 93 9f 4f c0 38 aa 51 df 32 08 fd 4d 18 ba a4 62
0000340 69 d3 c3 f0 df fa 49 c4 66 da c7 60 05 5f f3 5e
0000360 ff 1d f1 9d 60 0c 08 34 0c ee 00 31 32 33 34 0a
```

# Appendix #

## References ##

[1] openssl

[2] PKCS #1 v2.x, RFC 3xxx

## Tools ##

obtest-getcert - part of openbadger.  Fetches Card Authentication Certificate element file from a PIV card.

## Logs ##

### obtest-getcert log ###

```
Reader 1.
obtest-challenge 0.31
DEBUG: SCardEstablishContext return value was 0
DEBUG: SCardListReaders return value was 0
  Reader 0: Identiv Identiv uTrust 4701 F Dual Interface Reader [uTrust 4701 F Contact Reader] (55041912205100) 00 00
  Reader 1: Identiv Identiv uTrust 4701 F Dual Interface Reader [uTrust 4701 F CL Reader] (55041912205100) 01 00
Selected reader (1) is Identiv Identiv uTrust 4701 F Dual Interface Reader [uTrust 4701 F CL Reader] (55041912205100) 01 00
Protocol is T1
SCardTransmit (step 1 select card applet)
Select succeeded.
00cb3fff055c035fc10100
First getcert response 538203f7708203ee1f8b08000000000000ff33686211326862fab58099899189
49a4a66b4a6088f779b9cf7c2c6d8cac05e6996ade8506bc6c9c5a6d1e6ddf79
1919b959190cbc0c350dd4d99843599879144212d37332f3155c5273f3f38a4b
8a124b32f3f380bcb2cce45405674705434301033e90522e1ef6e0d4e4d282c4
62436e034e90089b30936ba8819c38af91b18189a1a5a1a591a1a1659438afb1
01906b6068616a616c1e65a06fa86ba00db14bc5ccd0d22ccdc4c44cd7c4c8c0
5cd7c4c2d85437d1d23855d7c23c39c9d024d1c0c420cdc8a0895109d9bd8cac
0ccc4d8cfc0c40712ea626464686459a1c6b85ee70fe0ac973782eefe3b8e4ba
6100
Retrieving next part of certificate.
Last 2: 61 00
Next getcert response f0ce8297eb97b1ba35192e0ef5ff91bce57e57a375a05962d48b668f82a399f7
26b966ac77f7d9b42cdac2e8c811352997e521ea373e45f2e57126fc17ae5cab
d27dffc8e9778159e98a4f8ffc0b3bbd22af6b16479e7957d2b79d5d864e7bce
08a47168ddc915feac7c20e279f833974c97dc44852dd647753e1fead65faefa
e257dfe535dc398f9c2e3cbec46dfa6256f1e6535f6f1a3dbaefbc2bc5c4d375
f6669f1c8106ff17465b59af5fe0bcc93645d3a931a375c7ec1b6525c293edee
ba316f9bc6dfa9f92a5137fdcce5bf5d6efa33f26a1c85dff579f6edf8ccb863
5d19b36678794c9fdf5679b93fff6caf7c319788adbcf465529a3213332303e3
6100
Last 2: 61 fb
Next getcert response e226464e6088b01ac8038356569945c240ac41c46a165b81d6376bce6593dcd6
cf7d77c7b27fa2f0160361900205161e032e030e36361686fe2a4666032390a0
2a8bb681261b87361b232b2b3b33131b3b88c5c4ccc2c60564b03036998b3031
c1a5450d64417af858c45844b866bb67e7eba54f53ac51fb682677b94e87d9d6
029c4864f91919ffb3b03033b1371808b171438cb96fc5c4cac0c20c94319083
197ddf8a8383458087cf00080c0d20c0d8d84004499e89858d85856b61f21a03
0524514616211681c4695fdc9cd83d4c57dab59f1659e0f0092d7933835289f6
529bdddd69bf1fae3a14b73e31cc38f44f865cfdda975b44f7dcdff8d1b9d3ea
61fb
Last 2: 90 00
Next getcert response 60de2cfe8d9fe7c5ba6dd8c19974b4d143f5e0ca40e7471126eb16c64ebfa512
697af0fbe5c3b1e7fde58aa7045d68d770d05a13b87551af57acb393f01fdd85
8b78d8e7ef1135e753fdd8e6772a9f33f2afb8539dac6d93e0be1bcb0e39ea49
bed55f91bf6b77d41a8f771bbd0f0830293e309fbaf8cfaa2d75a7c4c4ff249e
fea03167767185da722393f40717e43b9fd89ef16ce03db5eb89f4e107d2962a
4f4eecdb54fdd8ef67f4df25f7cad6cf4a9bf632dc246ddddec41b4fb4d689d6
6c6efe95546c5299dad53ea5aeddd7a2a7c1f1ddbc13d19f3fdf373d6ec05e1a
fca4f060eb718f7ef39677cf4d01ac8521c116040000710101fe009000
```

### log from dumpasn1 ###

```

   0 1042: SEQUENCE {
   4  762:   SEQUENCE {
   8    3:     [0] {
  10    1:       INTEGER 2
         :       }
  13   20:     INTEGER 7C 8A 94 51 54 4B CF 1E F3 0E 04 86 01 05 70 37 69 26 4B 71
  35   13:     SEQUENCE {
  37    9:       OBJECT IDENTIFIER
         :         sha256WithRSAEncryption (1 2 840 113549 1 1 11)
  48    0:       NULL
         :       }
  50   74:     SEQUENCE {
  52   41:       SET {
  54   39:         SEQUENCE {
  56    3:           OBJECT IDENTIFIER commonName (2 5 4 3)
  61   32:           UTF8String 'Taglio Demonstration Device CA 1'
         :           }
         :         }
  95   16:       SET {
  97   14:         SEQUENCE {
  99    3:           OBJECT IDENTIFIER organizationName (2 5 4 10)
 104    7:           UTF8String 'Secupas'
         :           }
         :         }
 113   11:       SET {
 115    9:         SEQUENCE {
 117    3:           OBJECT IDENTIFIER countryName (2 5 4 6)
 122    2:           PrintableString 'EU'
         :           }
         :         }
         :       }
 126   30:     SEQUENCE {
 128   13:       UTCTime 19/04/2023 19:21:19 GMT
 143   13:       UTCTime 10/04/2030 18:58:37 GMT
         :       }
 158   47:     SEQUENCE {
 160   45:       SET {
 162   43:         SEQUENCE {
 164    3:           OBJECT IDENTIFIER commonName (2 5 4 3)
 169   36:           UTF8String '6196f446-4207-4835-a93e-87cb14a040f2'
         :           }
         :         }
         :       }
 207  290:     SEQUENCE {
 211   13:       SEQUENCE {
 213    9:         OBJECT IDENTIFIER rsaEncryption (1 2 840 113549 1 1 1)
 224    0:         NULL
         :         }
 226  271:       BIT STRING, encapsulates {
 231  266:         SEQUENCE {
 235  257:           INTEGER
         :             00 A2 29 08 AD 12 DC 09 FA 54 6E 40 E7 1F 4C 41
         :             A4 D7 13 B9 70 E9 AF A6 05 46 82 31 A3 55 4F F8
         :             63 B4 DF 8A 81 3B 51 36 61 5A E8 83 48 70 C5 69
         :             DE 92 45 68 AF 47 4C B2 A6 5B 38 32 C4 C4 26 1A
         :             44 A7 54 27 D8 F2 59 0E 6E 09 60 FF 13 79 AD 24
         :             8B DF C4 CB EE 51 6A 67 21 E5 C4 FE 56 CB A8 6E
         :             8A 9A 08 6E 37 8A 62 F6 B9 8A 31 42 BC CC 10 66
         :             08 2A DC 6D 13 F3 23 C0 58 E7 57 E6 44 69 44 6D
         :                     [ Another 129 bytes skipped ]
 496    3:           INTEGER 65537
         :           }
         :         }
         :       }
 501  265:     [3] {
 505  261:       SEQUENCE {
 509   31:         SEQUENCE {
 511    3:           OBJECT IDENTIFIER authorityKeyIdentifier (2 5 29 35)
 516   24:           OCTET STRING, encapsulates {
 518   22:             SEQUENCE {
 520   20:               [0]
         :                 3A 9A 06 70 2A F6 3B 09 A6 92 46 AF 9D EE DC 39
         :                 8F 91 13 B4
         :               }
         :             }
         :           }
 542   19:         SEQUENCE {
 544    3:           OBJECT IDENTIFIER certificatePolicies (2 5 29 32)
 549   12:           OCTET STRING, encapsulates {
 551   10:             SEQUENCE {
 553    8:               SEQUENCE {
 555    6:                 OBJECT IDENTIFIER '0 4 0 2042 1 3'
         :                 }
         :               }
         :             }
         :           }
 563   50:         SEQUENCE {
 565    3:           OBJECT IDENTIFIER extKeyUsage (2 5 29 37)
 570   43:           OCTET STRING, encapsulates {
 572   41:             SEQUENCE {
 574    8:               OBJECT IDENTIFIER clientAuth (1 3 6 1 5 5 7 3 2)
 584    7:               OBJECT IDENTIFIER
         :                 keyPurposeClientAuth (1 3 6 1 5 2 3 4)
 593   10:               OBJECT IDENTIFIER
         :                 smartcardLogon (1 3 6 1 4 1 311 20 2 2)
 605    8:               OBJECT IDENTIFIER
         :                 secureShellClient (1 3 6 1 5 5 7 3 21)
         :               }
         :             }
         :           }
 615   29:         SEQUENCE {
 617    3:           OBJECT IDENTIFIER subjectKeyIdentifier (2 5 29 14)
 622   22:           OCTET STRING, encapsulates {
 624   20:             OCTET STRING
         :               0A 9B 47 6B 6F 2E 67 96 21 7C 26 F1 36 1E D3 7E
         :               2C 03 3D 38
         :             }
         :           }
 646   14:         SEQUENCE {
 648    3:           OBJECT IDENTIFIER keyUsage (2 5 29 15)
 653    1:           BOOLEAN TRUE
 656    4:           OCTET STRING, encapsulates {
 658    2:             BIT STRING 7 unused bits
         :               '1'B (bit 0)
         :             }
         :           }
 662   18:         SEQUENCE {
 664   11:           OBJECT IDENTIFIER '1 3 6 1 4 1 44986 2 5 0'
 677    3:           OCTET STRING, encapsulates {
 679    1:             BOOLEAN TRUE
         :             }
         :           }
 682   30:         SEQUENCE {
 684   10:           OBJECT IDENTIFIER '1 3 6 1 4 1 44986 8 8'
 696   16:           OCTET STRING, encapsulates {
 698   14:             UTF8String '00001000000033'
         :             }
         :           }
 714   20:         SEQUENCE {
 716   10:           OBJECT IDENTIFIER '1 3 6 1 4 1 44986 8 2'
 728    6:           OCTET STRING, encapsulates {
 730    4:             OCTET STRING 0A A1 63 AC
         :             }
         :           }
 736   32:         SEQUENCE {
 738   10:           OBJECT IDENTIFIER '1 3 6 1 4 1 44986 8 1'
 750   18:           OCTET STRING, encapsulates {
 752   16:             OCTET STRING 61 96 F4 46 42 07 48 35 A9 3E 87 CB 14 A0 40 F2
         :             }
         :           }
         :         }
         :       }
         :     }
 770   13:   SEQUENCE {
 772    9:     OBJECT IDENTIFIER sha256WithRSAEncryption (1 2 840 113549 1 1 11)
 783    0:     NULL
         :     }
 785  257:   BIT STRING
         :     2B A5 3C BB 8B 66 FB E1 AA C2 5E AF 61 56 33 55
         :     FC 68 1E 7F AD E9 B4 15 BC DF B1 F1 43 89 3A C1
         :     6E 9A 0F B1 F3 9E 5D 46 B0 B8 09 62 C5 81 48 25
         :     C1 A9 51 43 E2 58 34 AE A1 5D 97 DA 24 59 35 C1
         :     F7 D3 C3 5D CF 4F 1E 73 94 52 D0 87 28 40 2A AC
         :     51 B5 A2 8D 4A 5D 43 42 13 FC 2D A1 A2 0C 07 9F
         :     BC 15 37 0E 25 F1 86 4E CA 6F 09 59 FD 17 42 7E
         :     1D 3D 82 11 BE D8 A6 C2 41 2E 19 ED 2F A8 6F BA
         :             [ Another 128 bytes skipped ]
         :   }

```

### sample public key ###

```
-----BEGIN PUBLIC KEY-----
MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAoikIrRLcCfpUbkDnH0xB
pNcTuXDpr6YFRoIxo1VP+GO034qBO1E2YVrog0hwxWnekkVor0dMsqZbODLExCYa
RKdUJ9jyWQ5uCWD/E3mtJIvfxMvuUWpnIeXE/lbLqG6KmghuN4pi9rmKMUK8zBBm
CCrcbRPzI8BY51fmRGlEbWEgtDvFLPPCiy+nJej6jtOsC2ziQtDj0gs16Jpzs8r1
2TLi30O6ZDRJRZuzTGwQgE/oMrUF19AJ2QaUKUKBaIW4m9h2dBOTPt1GA7aWD4kp
6mEtZ8zT/YpGL5hufEET7o5JjrjzAbiudgMpV3dcjk61Hx78/j3U9DcYXXnS9JJm
IwIDAQAB
-----END PUBLIC KEY-----
```

### challenge and response log ###

```
reading settings from ./openbadger-settings.json
Reader 1.
Reading challenge from file ciphertext
test-piv-challenge 0.31
APDU Payload max: 200.
DEBUG: SCardEstablishContext return value was 0
DEBUG: SCardListReaders return value was 0
  Reader 0: Identiv Identiv uTrust 4701 F Dual Interface Reader [uTrust 4701 F Contact Reader] (55041912205100) 00 00
  Reader 1: Identiv Identiv uTrust 4701 F Dual Interface Reader [uTrust 4701 F CL Reader] (55041912205100) 01 00
Selected reader (1) is Identiv Identiv uTrust 4701 F Dual Interface Reader [uTrust 4701 F CL Reader] (55041912205100) 01 00
Protocol is T1
SCardTransmit (step 1 select card applet)
00a4040009a0000003080000100000
DEBUG: challenge/response now...
Challenge type 81
Setting up GENERAL AUTHENTICATE:
7816 PDU: payload 200.
ob_command_response: top
--sending 7816 command First GenAuth PDU Request:
1087079ec87c8201068200818201004021a4e60c0550529abe45fc52f6f90b9a
62a5eb02b3ef4e925ce7c52f79c8b808b6d9f96e2268393f5ca4c5014309c1a8
972c98151b7176ec29a68052e4e7fdd01d36b0dc9a0bffd275383a291fa1397d
a5a0d45d479f2f0c47bd24f5e8f22807500a56f973d9600c0e5506f060eab3dc
97e5480029aaabe22fc13798ce06a275a2fe7c6195c9a31220168c333e380017
0876422e41e005ac94859984667fa724d2d73ad8cf6d7ff61664a4cc72670e99
c9e3fe8fe5b4ab7d3963eb197100
back from SCardTransmit, status 0. 0
7186 response First GenAuth PDU Response: (2. bytes)
9000
ob_command_response: bottom
assuming response ok... 0. 9000 2.
second send: dynauth 266. part2 66.
total was 266 first was 200 second is 66
7816 PDU: payload 66.
7816 cmd 48 rsp 1FE
ob_command_response: top
--sending 7816 command Second GenAuth PDU Request:
0087079e4274dc077bd08c4233c0c4a7ebaae2b8c13dd7f140a0c703ca44ed06
e18ff56f80322bf36546f12f8b0320d524bb97da7928493e21e0db26e46e9c99
c4f8579cd50a5800
back from SCardTransmit, status 0. 0
7186 response Second GenAuth PDU Response: (258. bytes)
7c820104828201000002d0da4c69c931174c825a46c5ed1f2c4826e3e1cc8d67
59db30c4a2975bfca7ce809e8ae35e762de71fda68c61cb9a011870a4c449e4e
8557ab3e12cc75d5f26073eab9d3398a8dee71f4b413d296d25b57aba5bc70e5
50736ed9551ce7cfa49216df205d2c3abd07e66950b15d6160622070de8188dc
a13e51acc81cbab71b6e7d95e73a7adda89545d3adca8449642d27a2a1776354
a910294e1320ade527e45bfe45080d5b88d075dcdc4368109ca055450c7b195c
de9856aba94f3676e6982514f3accba53f42d981f5d671894ee5d71ccb36a05f
8ab752d5a892791e563c602b754fecdf7817a2cbf698f83faf955756de8fee0d
6108
ob_command_response: bottom
---7816 command (get next data)
00c0000000
7816 response 3 status_pcsc 0
72d600313233340a9000
assuming response ok...72d6 258.
```

