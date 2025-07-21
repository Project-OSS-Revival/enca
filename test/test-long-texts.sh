#! /bin/sh
# Purpose: check whether enca correctly prints license, help, version.
. $srcdir/setup.sh
$ENCA --version >$TESTNAME.actual || DIE=1
version=`grep '^AC_INIT' $top_srcdir/configure.ac | sed -e 's/AC_INIT(\[Enca\], \[[0-9.]*\).*/\1/' -e 's/\./\\./'`
grep '^enca '$version $TESTNAME.actual >/dev/null || DIE=1
$ENCA --license >$TESTNAME.actual || DIE=1
if [[ -v MSYSTEM ]]; then
  dos2unix $TESTNAME.actual
fi
diff $top_srcdir/COPYING $TESTNAME.actual || DIE=1
$ENCA --help >$TESTNAME.actual || DIE=1
if [[ -v MSYSTEM ]]; then
  dos2unix $TESTNAME.actual
fi
diff $top_builddir/src/HELP $TESTNAME.actual || DIE=1
grep '^Report bugs to .*http.*' $TESTNAME.actual >/dev/null || DIE=1
. $srcdir/finish.sh
