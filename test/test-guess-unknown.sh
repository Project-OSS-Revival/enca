#! /bin/sh
# Purpose: test charset recognition for binary
. $srcdir/setup.sh
$ENCA -P -e -L none < $srcdir/binary >>$TESTNAME.actual && DIE=1
$ENCA -P -e -L none $srcdir/binary >>$TESTNAME.actual && DIE=1
. $srcdir/finish.sh
