# Comprehensive Guide: Adding Finnish Language Support to Enca

This guide provides a **detailed, step-by-step process** for introducing new language support (Finnish) to the Enca project.  

---

## 1. Obtain a Finnish Text Sample

First, obtain some Finnish text to use as a sample. For example, download a sample from:

- https://wortschatz.uni-leipzig.de/en/download/Finnish#fin-fi_web_2019

Save it as `sampletext.utf8`.

---

## 2. Create Language Data Files

### 2.1 Create the Language Directory

```sh
mkdir -p data/finnish
```

---

### 2.2 Create the Finnish Data Script

Create `data/finnish/doit.sh` with the following content:

```bash
#!/bin/bash
../doit.sh iso88594 cp1257
```

Make it executable:

```sh
chmod a+x data/finnish/doit.sh
```

---

### 2.3 Create Sample Files for Encodings

Convert your sample text to the required encodings.

```sh
iconv -f UTF-8 -t ISO-8859-4 sampletext.utf8 -o sampletext.iso88594
iconv -f UTF-8 -t CP1257 sampletext.utf8 -o sampletext.cp1257
```

---

### 2.4 Generate `rawcounts.` Files

Go to the `enca/data` directory and run:

```sh
make countall
./countall < finnish/sampletext.cp1257 > finnish/rawcounts.cp1257
./countall < finnish/sampletext.iso88594 > finnish/rawcounts.iso88594
```
ы
**Example for `data/finnish/rawcounts.iso88594`:**

```
+0x08 . 7
+0x15 . 1
+0x16 . 1
+0x20   43309824
+0x21 ! 32929
+0x22 " 157749
+0x23 # 2231
+0x24 $ 273
+0x25 % 20671
+0x26 & 10414
+0x27 ' 14661
+0x28 ( 176988
+0x29 ) 173923
+0x2a * 2473
+0x2b + 7176
...
```

---

### 2.5 Generate `.base` Files

Run:

```sh
./normalize.pl < finnish/rawcounts.cp1257 > finnish/cp1257.base
./normalize.pl < finnish/rawcounts.iso88594 > finnish/iso88594.base
```

**Example for `data/finnish/iso88594.base`:**

```
+. 0
+. 0
+. 0
+  5999
+! 4
+" 21
+# 0
+$ 0
+% 2
+& 1
+' 2
+( 24
+) 24
+* 0
++ 0
+, 326
...
```

---

### 2.6 Generate `finnish.h`

```sh
cd data/finnish
./doit.sh
```

This will produce `data/finnish/finnish.h` (full file, do not edit by hand).

---

## 3. Update Data Makefiles

### 3.1 `data/Makefile.am`

**Full diff:**

```diff
--- a/data/Makefile.am
+++ b/data/Makefile.am
@@ -16,6 +16,7 @@ noinst_HEADERS = \
    croatian/croatian.h \
    czech/czech.h \
    estonian/estonian.h \
+   finnish/finnish.h \
    hungarian/hungarian.h \
    latvian/latvian.h \
    lithuanian/lithuanian.h \
@@ -46,6 +47,7 @@ noinst_SCRIPTS = \
    croatian/doit.sh \
    czech/doit.sh \
    estonian/doit.sh \
+   finnish/doit.sh \
    hungarian/doit.sh \
    latvian/doit.sh \
    lithuanian/doit.sh \
@@ -61,6 +63,7 @@ BASES = \
    croatian/cp1250.base \
    czech/iso88592.base \
    estonian/iso88594.base \
+   finnish/iso88594.base \
    hungarian/iso88592.base \
    russian/koi8r.base \
    latvian/cp1257.base \
@@ -76,6 +79,7 @@ RAWCOUNTS = \
    croatian/rawcounts.cp1250 \
    czech/rawcounts.iso88592 \
    estonian/rawcounts.iso88594 \
+   finnish/rawcounts.iso88594 \
    hungarian/rawcounts.iso88592 \
    latvian/rawcounts.cp1257 \
    lithuanian/rawcounts.cp1257 \
```
---

## 4. Create the Finnish Language Implementation File

Add file: `lib/lang_fi.c`:

```c name=lib/lang_fi.c
/*
  encoding data and routines dependent on language; finnish

  Copyright (C) 2025

  This program is free software; you can redistribute it and/or modify it
  under the terms of version 2 of the GNU General Public License as published
  by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
*/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "enca.h"
#include "internal.h"
#include "data/finnish/finnish.h"

/* Local prototypes. */
static int hook(EncaAnalyserState *analyser);
static int hook_iso4cp1257(EncaAnalyserState *analyser);

/**
 * ENCA_LANGUAGE_FI:
 *
 * Finnish language.
 *
 * Everything the world out there needs to know about this language.
 **/
const EncaLanguageInfo ENCA_LANGUAGE_FI = {
    "fi",
    "finnish",
    NCHARSETS,
    CHARSET_NAMES,
    CHARSET_WEIGHTS,
    SIGNIFICANT,
    CHARSET_LETTERS,
    CHARSET_PAIRS,
    WEIGHT_SUM,
    &hook,
    NULL,
    NULL,
    NULL,
};

/**
 * hook:
 * @analyser: Analyser state whose charset ratings are to be modified.
 *
 * Launches language specific hooks for language "fi".
 *
 * Returns: Nonzero if charset ratigns have been actually modified, zero
 * otherwise.
 **/
static int
hook(EncaAnalyserState *analyser)
{
  return hook_iso4cp1257(analyser);
}

/**
 * hook_iso4cp1257:
 * @analyser: Analyser state whose charset ratings are to be modified.
 *
 * Decides between iso8859-4 and cp1257 charsets for language "fi".
 *
 * Returns: Nonzero if charset ratigns have been actually modified, zero
 * otherwise.
 **/
static int
hook_iso4cp1257(EncaAnalyserState *analyser)
{
  static const unsigned char list_iso88594[] = {
      0xb9, 0xbe, 0xa9, 0xae, 0xa8};
  static const unsigned char list_cp1257[] = {
      0xf0, 0xfe, 0xd0, 0xde, 0xb8};
  static EncaLanguageHookData1CS hookdata[] = {
      MAKE_HOOK_LINE(iso88594),
      MAKE_HOOK_LINE(cp1257),
  };

  return enca_language_hook_ncs(analyser, ELEMENTS(hookdata), hookdata);
}

/* vim: ts=2
 */
```

---

## 5. Generate the Finnish Header

After running `doit.sh`, `data/finnish/finnish.h` is generated.  
**Full file content should be committed, do not edit by hand.**

---

## 6. Update Source Makefiles and Language Registration

### 6.1 `lib/Makefile.am`

**Full diff:**

```diff
--- a/lib/Makefile.am
+++ b/lib/Makefile.am
@@ -21,6 +21,7 @@ libenca_la_SOURCES = \
   lang_bg.c \
   lang_cs.c \
   lang_et.c \
+  lang_fi.c \
   lang_hr.c \
   lang_hu.c \
   lang_lt.c \
```

### 6.2 Update Language Registration

**lib/internal.h**

```diff
--- a/lib/internal.h
+++ b/lib/internal.h
@@ -483,6 +483,7 @@ extern const EncaLanguageInfo ENCA_LANGUAGE_BE;
 extern const EncaLanguageInfo ENCA_LANGUAGE_BG;
 extern const EncaLanguageInfo ENCA_LANGUAGE_CS;
 extern const EncaLanguageInfo ENCA_LANGUAGE_ET;
+extern const EncaLanguageInfo ENCA_LANGUAGE_FI;
 extern const EncaLanguageInfo ENCA_LANGUAGE_HR;
 extern const EncaLanguageInfo ENCA_LANGUAGE_HU;
 extern const EncaLanguageInfo ENCA_LANGUAGE_LT;
@@ -504,4 +505,4 @@ extern EncaGuessFunc ENCA_MULTIBYTE_TESTS_8BIT_TOLERANT[];
 /* Locale-independent character type table. */
 extern const short int enca_ctype_data[0x100];

-#endif /* not LIBENCA_H */
+#endif /* not LIBENCA_H */
```

**lib/lang.c**

```diff
--- a/lib/lang.c
+++ b/lib/lang.c
@@ -51,6 +51,7 @@ static const EncaLanguageInfo *const LANGUAGE_LIST[] = {
   &ENCA_LANGUAGE_BG, /* Bulgarian. */
   &ENCA_LANGUAGE_CS, /* Czech. */
   &ENCA_LANGUAGE_ET, /* Estonian. */
+  &ENCA_LANGUAGE_FI, /* Finnish. */
   &ENCA_LANGUAGE_HR, /* Croatian. */
   &ENCA_LANGUAGE_HU, /* Hungarian. */
   &ENCA_LANGUAGE_LT, /* Latvian. */
@@ -347,4 +348,3 @@ enca_get_charset_similarity_matrix(const EncaLanguageInfo *lang)
 }
 /* vim: ts=2
  */
```

---

## 7. Update and Add Tests

### 7.1 Add Test Files

Create `test/fi-s.utf8` with this content:

```
Hyvä ystävä, älä käytä öljyä yöllä. Kävelyä järven äärellä on tärkeää. š
```

Convert to encodings:

```sh
iconv -f UTF-8 -t ISO-8859-4 test/fi-s.utf8 -o test/fi-s.iso88594
iconv -f UTF-8 -t CP1257 test/fi-s.utf8 -o test/fi-s.cp1257
```

**Add these files to the repo.**

---

### 7.2 Update Test Setup

**test/setup.sh**

```diff
--- a/test/setup.sh
+++ b/test/setup.sh
@@ -1,10 +1,11 @@
 ENCA=$top_builddir/src/enca
-TEST_LANGUAGES="be bg cs et hr hu lt lv pl ru sk sl uk zh"
+TEST_LANGUAGES="be bg cs et fi hr hu lt lv pl ru sk sl uk zh"
 ALL_TEST_LANGUAGES="$TEST_LANGUAGES none"
 TEST_PAIR_be="koi8uni cp1251"
 TEST_PAIR_bg="ibm855 cp1251"
 TEST_PAIR_cs="keybcs2 ibm852"
 TEST_PAIR_et="iso885913 baltic"
+TEST_PAIR_fi="iso88594 cp1257"
 TEST_PAIR_hr="ibm852 cp1250"
 TEST_PAIR_hu="cp1250 ibm852"
 TEST_PAIR_lt="iso88594 baltic"
```

**test/simtable.c**

```diff
@@ -59,6 +59,7 @@ main(int argc, char *argv[])
   prl(&ENCA_LANGUAGE_BG, "1251mac");
   prl(&ENCA_LANGUAGE_CS, "isowin 852kam");
   prl(&ENCA_LANGUAGE_ET, "");
+  prl(&ENCA_LANGUAGE_FI, "");
   prl(&ENCA_LANGUAGE_HR, "isowin");
   prl(&ENCA_LANGUAGE_HU, "isocork isowin[XXX]");
   prl(&ENCA_LANGUAGE_LT, "winbalt lat4balt iso13win[XXX]");
@@ -70,4 +71,4 @@ main(int argc, char *argv[])
   prl(&ENCA_LANGUAGE_UK, "macwin isokoi ibm1125");

   return 0;
-}
+}
\ No newline at end