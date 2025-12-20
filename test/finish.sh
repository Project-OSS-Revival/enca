if test -n "$E77"; then
  exit 77
fi
if  [[ -n "${MSYSTEM+set}" ]]; then
  dos2unix $TESTNAME.actual
fi
if test -f $TESTNAME.expected; then
  diff $TESTNAME.expected $TESTNAME.actual || DIE=1
else
  if test -f $srcdir/$TESTNAME.expected; then
    diff $srcdir/$TESTNAME.expected $TESTNAME.actual || DIE=1
  fi
fi
if test -z "$DIE"; then
  rm -f $TESTNAME.actual $TESTNAME.tmp 2>/dev/null
fi
ls | grep 'core' >/dev/null && DIE=1
rm -f core* 2>/dev/null
test -z "$DIE" || exit 1
