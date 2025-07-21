#! /bin/sh
# Purpose: test whether ENCAOPT works.
. $srcdir/setup.sh
DATATESTNAME=test-guess-short
rm -f $TESTNAME.expected 2>/dev/null
ln -s $srcdir/$DATATESTNAME.expected $TESTNAME.expected
for l in $ALL_TEST_LANGUAGES; do
  ENCAOPT="-L $l -p -e"
  # This is necessary when $ENCA is in fact a libtool script
  export ENCAOPT
  $ENCA $srcdir/$l-s.* | sed -e "s#^$srcdir/##" >>$TESTNAME.actual || DIE=1
done
# Skipping the test due to missing wordexp on MSYS2
if [[ -v MSYSTEM ]]; then
  $ENCA -L none $srcdir/cs-s.utf8 || DIE=1
else
  # Test invalid option string
  ENCAOPT=";"
  export ENCAOPT
  $ENCA -L none $srcdir/cs-s.utf8 || DIE=1
fi
. $srcdir/finish.sh
rm -f $TESTNAME.expected 2>/dev/null
