# Developing programs utilizing libenca

* Look at libenca API documentation in devel-docs/html.
* Look into enca source how it uses libenca.  Note enca is quite a simple
  application (practically all libenca interaction is in `src/enca.c`). It's
  single-threaded and uses one language and one analyser all the time.
  Provided each thread has its own analyser, libenca should be thread-safe
  (untested).
* Take names starting with `ENCA`, `Enca`, `enca`, `_ENCA`, `_Enca`, and
  `_enca` as reserved.
* pkgconfig is supported, you can use `PKG_CHECK_MODULES` to check for libenca
  in your configure scripts

# How to add a new charset/encoding

(optional steps are marked _[optional]_):

* `iconvcap.c`:
    * Add a new test (even if you are 100% sure iconv will never support it),
      please see top of `iconvcap.c` for some documentation how it works.
* `tools/encodings.dat`:
    * Add a new entry.
    * Use `@ICONV_NAME_<name>@` (as it will appear in iconvcap output) for
      iconv names.
* `tools/iconvenc.null`:
    * Add it (with NULL)

Specifically, for regular 8bit (language dependent) charsets:

* `lib/unicodemap.c`:
    * Add a new map to Unicode (UCS-2) `unicode_map_...[]`.
    * Add a new `UNICODE_MAP[]` entry.
* `lib/filters.c`: _[optional]_
    * Create a new filter or make an alias of an existing filter.
* `lib/lang_??.c`:
    * Add the new encoding to some existing language(s).
    * Add appropriate filters or hooks _[optional]_.
* `data/maps/??.map`:
    * Add a new map to Unicode (UCS-2)


Specifically, for multibyte encodings:

* `lib/multibyte.c`:
    * Create a new check function.
    * Put it into appropriate ascii/8bit/binary test group
      `ENCA_MULTIBYTE_TESTS_ASCII[]`, `ENCA_MULTIBYTE_TESTS_8BIT[]`,
      `ENCA_MULTIBYTE_TESTS_BINARY[]`.
    * Put strict tests (i.e. test which may fail) first, looks-like tests
      last.


# How to add a new surface

* Try to ask the author what to do, since this may be complicated, or
* Hack, basically it must be added to `lib/enca.h` `EncaSurface enum`, to
  `lib/encnames.c` `SURFACE_INFO[]` a detection method must be added to
  `lib/guess.c` and now the most complicated part: this new method must be used
  in the right places in `lib/guess.c` `make_guess()`.



# How to add a new language

* For a comprehensive, step-by-step guide to adding a new language, see [ENCA-ADD-LANGUAGE-GUIDE.md](ENCA-ADD-LANGUAGE-GUIDE.md).

* Create a new language file:
    * Create new `lib/lang_....c` files by copying some existing (use locale
      code for names)
    * Fill all encoding and occurence data, create filters and hooks (see
      `filters.c` too).  You can do it manually, but look how it's done for
      existing languages in `data/*` and read `data/README`.  
* `lib/internal.h`:
    * Add new `ENCA_LANGUAGE_....`
* `lib/lang.c`:
    * Add a new `LANGUAGE_LIST[]` entry pointing to the `ENCA_LANGUAGE_....`


# Automake, autoconf, libtool, ... note

If you run `./autogen.sh` and it finishes OK, you are lucky and can expect
things to work.

You have to give `--enable-maintainer-mode` to `./configure` (or `./autogen`)
to build dists and/or the strange stuff in `tools/`, `data/`, `tests/`, and
`devel-docs/`.


# Repository and continuous integration

The git repository is located at GitHub:

    https://github.com/nijel/enca

There is also continuous integration on GitHub:

    https://github.com/nijel/enca/actions
