# Enca - guess and convert encoding of text files

[![build status](https://secure.travis-ci.org/nijel/enca.png)](https://travis-ci.org/nijel/enca)
[![codecov.io](https://codecov.io/github/nijel/enca/coverage.svg?branch=master)](https://codecov.io/github/nijel/enca?branch=master)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/3292/badge.svg)](https://scan.coverity.com/projects/nijel-enca)
[![Coverage Status](https://coveralls.io/repos/nijel/enca/badge.png?branch=master)](https://coveralls.io/r/nijel/enca?branch=master)

## Copyright 

Copyright (C) 2000-2003 David Necas (Yeti) <yeti@physics.muni.cz>

Copyright (C) 2009-2016 Michal Cihar <michal@cihar.com>

## Description

Enca (Extremely Naive Charset Analyser) consists of two main components:

  * libenca, an encoding detection library.  It currently supports
    Belarusian, Bulgarian, Croatian, Czech, Estonian, Hungarian, Latvian,
    Lithuanian, Polish, Russian, Slovak, Slovene, Ukrainian, Chinese, and
    some multibyte encodings independently on language.  The API should be
    relatively stable (to be read as `it will either change only
    marginally, or very drastically').

  * enca, a command line frontend, integrating libenca and several
    charset conversion libraries and tools (GNU recode, UNIX98 iconv,
    perl Unicode::Map, cstocs).


## Installation

Enca should compile and work on every POSIX.1 compliant system with ISO C
compiler, and actually compiles on many noncompliant systems too (see below
for list dependencies).  If you have some of following additional tools,
Enca can use them as external converters:

* GNU recode and the associated recoding library
* Perl charset converters Unicode::Map8 or Unicode::Map
* cstocs, the famous Czech charset converter

Optional features:

* Compilation of GNU recode library interface is controlled by
  `--with-librecode[=DIR]`, `--without-librecode`
  configure parameters.  It is compiled in by default when found.
  Optionally, you can specify a DIR; librecode include files will be
  then searched in DIR/include and the library itself in DIR/lib.

* Compilation of UNIX98 iconv interface is controlled by
  `--with-libiconv=[DIR]`, `--without-libiconv`
  configure parameters.  It is compiled in by default when found
  and considered usable.  Optionally, you can specify a DIR; libiconv
  include files will be then searched in DIR/include and the library
  itself in DIR/lib.

* Compilation of interface to external converter programs is controlled by
  `--enable-external`, `--disable-external`
  configure parameters.  By default is is compiled in.

Don't even try to compile Enca on system not supporting following ISO C and
POSIX features:
* Function prototypes.
* Basic ISO C headers and functions declared there:
  - assert.h, ctype.h, math.h, stdarg.h, stdio.h, stdlib.h
  - any (working) one of string.h, strings.h, memory.h
  - unistd.h, sys/stat.h, sys/types.h

For the impatient: Run

    ./configure
    make
    make check
    make install

as usual.


## License

Enca can be copied and/or modified under the terms of version 2 of
GNU General Public License.  Please see COPYING for details.


## Web resources

Enca can be found at https://github.com/nijel/enca/, you can download 
tarballs from https://cihar.com/software/enca/.


## Bugs

Report problems at <https://github.com/nijel/enca/issues>. Some known bugs have
been collected in BUGS section of enca manual page.


## Hacking (with) Enca

Please see TODO for list of things that should be fixed and features to
be implemented and their priority and also for list of things that
definitely shouldn't be implemented.

- For instructions on adding a new encoding or language, see [DEVELOP.md](DEVELOP.md).
- For a comprehensive, step-by-step guide to adding a new language (example: Finnish), see [ENCA-ADD-LANGUAGE-GUIDE.md](ENCA-ADD-LANGUAGE-GUIDE.md).

The file DEVELOP.md describes what needs to be done to add a new
encoding or language to Enca.

The directory devel-docs/html contains Enca library API documentation in
HTML form.
