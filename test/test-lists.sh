#! /bin/sh
# Purpose: check whether enca can list all the lists.  It used to crash ;-)
. $srcdir/setup.sh
$ENCA  --list lists >$TESTNAME.actual || DIE=1
for l in `$ENCA --list lists`; do
  $ENCA --list $l >>$TESTNAME.actual || DIE=1
done
. $srcdir/finish.sh
