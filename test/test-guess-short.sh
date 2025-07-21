#! /bin/sh
# Purpose: test charset recognition on tiny text fragments.
. $srcdir/setup.sh
for l in $ALL_TEST_LANGUAGES; do
  $ENCA -p -e -L $l $srcdir/$l-s.* | sed -e "s#^$srcdir/##" >>$TESTNAME.actual || DIE=1
done
. $srcdir/finish.sh
