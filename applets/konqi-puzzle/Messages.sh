#! /usr/bin/env bash
$XGETTEXT `find . -name \*.qml` -L Java -o $podir/konqi-puzzle.pot
rm -f rc.cpp

