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