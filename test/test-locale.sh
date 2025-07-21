#! /bin/sh
# Purpose: test whether libiconv interface works.
# FIXME: this may fail when the interface works but libiconv is broken!
. $srcdir/setup.sh
if $ENCA --version | grep ' +language-detection ' >/dev/null; then
  export LC_CTYPE=cs_CZ
  TEST_TEXT=$srcdir/cs-s.iso88592
  OPTS=""
  if locale | grep -q '^LC_CTYPE=cs_CZx$' ; then
    cp $TEST_TEXT $TESTNAME.actual
    $ENCA $OPTS -x UTF-8 $TESTNAME.actual || DIE=1
  fi
  # Failures
  export LC_CTYPE=INVALID
  export LC_COLLATE=INVALID
  export LC_MESSAGES=INVALID
  cp $TEST_TEXT $TESTNAME.actual
  $ENCA $OPTS -x UTF-8 $TESTNAME.actual 2>/dev/null && DIE=1
else
  E77=1
fi
. $srcdir/finish.sh
