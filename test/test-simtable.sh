#! /bin/sh
# Purpose: check simtable rendering
. $srcdir/setup.sh
$builddir/simtable || DIE=1
. $srcdir/finish.sh
