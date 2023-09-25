---
title: OpenBadger Test - DESFire
author: Smithee Solutions, LLC
date: August 13, 2023
header-includes: |
  \usepackage{fancyhdr}
  \pagestyle{fancy}
  \fancyfoot[CO,CE]{OpenBadger Test: DESFire}
  \fancyfoot[LE,RO]{\thepage}
include-before:
- '`\newpage{}`{=latex}'
---


\newpage{}

Introduction
============

this describes using the obtest-desfire tool set to manipulate desfire cards.

\newpage{}

Settings
========

settings file

context file

Tools
=====

obtest-desfire-init
-------------------

???
  init card PICC master from settings

obtest-desfire-format
---------------------

format card from settings


other tools
-----------

  init application AID, application master key, 1 application read key, 1 application write key

  init file, aid in settings, application master key in settings

  write file, application write key in settings

  read file, application read key in settings

\newpage{}

finis.

