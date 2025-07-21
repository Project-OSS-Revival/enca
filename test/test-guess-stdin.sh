#! /bin/sh
# Purpose: test charset recognition of samples coming from stdin.
. $srcdir/setup.sh
DATATESTNAME=test-guess-short
rm -f $TESTNAME.expected 2>/dev/null
ln -s $srcdir/$DATATESTNAME.expected $TESTNAME.expected
for l in $ALL_TEST_LANGUAGES; do
  for f in $srcdir/$l-s.*; do
    sf=`echo $f | sed -e "s#^$srcdir/##"`
    x="$sf: "`$ENCA -P -e -L $l <$f` || DIE=1
    echo "$x" >>$TESTNAME.actual
  done
done
if [[ -v MSYSTEM ]]; then
  cp test-guess-stdin.msys2.expected test-guess-stdin.expected
fi
. $srcdir/finish.sh
rm -f $TESTNAME.expected 2>/dev/null
