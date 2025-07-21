#! /bin/sh
# Purpose: various UTF-8 variant recognition.
. $srcdir/setup.sh
for l in $ALL_TEST_LANGUAGES; do
  x=`ls $srcdir/$l-utf8.* 2>/dev/null`
  if test "x$x" != x; then
    $ENCA -p -f -L $l $srcdir/$l-utf8.* | sed -e "s#^$srcdir/##" >>$TESTNAME.actual || DIE=1
  fi
done
. $srcdir/finish.sh
