#! /bin/sh
# Purpose: test whether TeX decoding works.
. $srcdir/setup.sh
# Skipping the test on MSYS@ due to mkstemp failure.
if [[ -v MSYSTEM ]]; then
  E77=1
else
  if $ENCA --list converters | grep '^librecode$' >/dev/null; then
    TEST_TEXT=$srcdir/cs-s.tex
    OPTS="-L cs -C recode"
    # File
    cp $TEST_TEXT $TESTNAME.actual
    $ENCA $OPTS -x UTF-8 $TESTNAME.actual || DIE=1
    $ENCA -L none $TESTNAME.actual | grep UTF-8 >/dev/null || DIE=1
    $ENCA $OPTS -x TEX $TESTNAME.actual || DIE=1
    diff $TEST_TEXT $TESTNAME.actual || DIE=1
  else
    E77=1
  fi
fi
. $srcdir/finish.sh
