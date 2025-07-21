#! /bin/sh
# Purpose: check how enca reacts on various incorrect inputs
. $srcdir/setup.sh
# These should succeede
$ENCA -L cs -x koi8cs2 </dev/null >/dev/null 2>&1 || DIE=1
$ENCA -L ru -x anoldoak </dev/null >/dev/null 2>&1 || DIE=1
# These should fail.
$ENCA -L pl </dev/null >/dev/null 2>&1 && DIE=1
. $srcdir/finish.sh
