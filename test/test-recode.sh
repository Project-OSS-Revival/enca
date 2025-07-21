#! /bin/sh
# Purpose: test whether librecode interface works.
# FIXME: this may fail when the interface works but librecode is broken!
. $srcdir/setup.sh
# Skipping the test on MSYS@ due to mkstemp failure.
if [[ -v MSYSTEM ]]; then
  E77=1
else
  if $ENCA --list converters | grep '^librecode$' >/dev/null; then
    TEST_TEXT=$srcdir/cs-s.iso88592
    OPTS="-L cs -C librecode"
    # File
    cp $TEST_TEXT $TESTNAME.actual
    $ENCA $OPTS -x UTF-8 $TESTNAME.actual || DIE=1
    # Convert more files to hit recode request cache
    $ENCA $OPTS -x UTF-8 $TESTNAME.actual $TESTNAME.actual $TESTNAME.actual $TESTNAME.actual || DIE=1
    $ENCA -L none $TESTNAME.actual | grep UTF-8 >/dev/null || DIE=1
    $ENCA $OPTS -x ISO-8859-2 $TESTNAME.actual || DIE=1
    diff $TEST_TEXT $TESTNAME.actual || DIE=1
    # Pipe
    cp $TEST_TEXT $TESTNAME.actual
    $ENCA $OPTS -x UTF-8 <$TESTNAME.actual >$TESTNAME.tmp || DIE=1
    $ENCA -L none $TESTNAME.tmp | grep UTF-8 >/dev/null || DIE=1
    $ENCA $OPTS -x ISO-8859-2 <$TESTNAME.tmp >$TESTNAME.actual || DIE=1
    diff $TEST_TEXT $TESTNAME.actual || DIE=1
    # Failures
    cp $TEST_TEXT $TESTNAME.actual
    $ENCA $OPTS -x solzenicyn $TESTNAME.actual 2>/dev/null && DIE=1
    diff $TEST_TEXT $TESTNAME.actual || DIE=1
    #     TODO: this is not enough for recode to fail, it just wipes out the
    #     offending characters
    # One copy doesn't contain enough characters to overweight the noise
    #cat $TESTNAME.tmp $TESTNAME.tmp $TESTNAME.tmp >$TESTNAME.actual
    #echo '�' >>$TESTNAME.actual
    #cat $TESTNAME.actual >$TESTNAME.tmp
    #$ENCA $OPTS -x ISO-8859-2 $TESTNAME.tmp && DIE=1
    #diff $TESTNAME.tmp $TESTNAME.actual || DIE=1
  else
    E77=1
  fi
fi
. $srcdir/finish.sh
