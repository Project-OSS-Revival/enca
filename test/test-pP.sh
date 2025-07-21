#! /bin/sh
# Purpose: test whether -p, -P and default file prefixing works
. $srcdir/setup.sh
TEST_TEXT=$srcdir/cs-s.iso88592
OPTS="-e -L cs"
$ENCA $OPTS $TEST_TEXT | sed -e "s#^$srcdir/##" >>$TESTNAME.actual || DIE=1
echo AAA >>$TESTNAME.actual
$ENCA $OPTS -p $TEST_TEXT | sed -e "s#^$srcdir/##" >>$TESTNAME.actual || DIE=1
echo BBB >>$TESTNAME.actual
$ENCA $OPTS -P $TEST_TEXT | sed -e "s#^$srcdir/##" >>$TESTNAME.actual || DIE=1
echo CCC >>$TESTNAME.actual
$ENCA $OPTS <$TEST_TEXT | sed -e "s#^$srcdir/##" >>$TESTNAME.actual || DIE=1
echo DDD >>$TESTNAME.actual
$ENCA $OPTS -p <$TEST_TEXT | sed -e "s#^$srcdir/##" >>$TESTNAME.actual || DIE=1
echo EEE >>$TESTNAME.actual
$ENCA $OPTS -P <$TEST_TEXT | sed -e "s#^$srcdir/##" >>$TESTNAME.actual || DIE=1
echo FFF >>$TESTNAME.actual
$ENCA $OPTS - <$TEST_TEXT | sed -e "s#^$srcdir/##" >>$TESTNAME.actual || DIE=1
echo GGG >>$TESTNAME.actual
$ENCA $OPTS -p - <$TEST_TEXT | sed -e "s#^$srcdir/##" >>$TESTNAME.actual || DIE=1
echo HHH >>$TESTNAME.actual
$ENCA $OPTS -P - <$TEST_TEXT | sed -e "s#^$srcdir/##" >>$TESTNAME.actual || DIE=1
echo III >>$TESTNAME.actual
$ENCA $OPTS $TEST_TEXT $TEST_TEXT | sed -e "s#^$srcdir/##" >>$TESTNAME.actual || DIE=1
echo JJJ >>$TESTNAME.actual
$ENCA $OPTS -p $TEST_TEXT $TEST_TEXT | sed -e "s#^$srcdir/##" >>$TESTNAME.actual || DIE=1
echo KKK >>$TESTNAME.actual
$ENCA $OPTS -P $TEST_TEXT $TEST_TEXT | sed -e "s#^$srcdir/##" >>$TESTNAME.actual || DIE=1
. $srcdir/finish.sh
