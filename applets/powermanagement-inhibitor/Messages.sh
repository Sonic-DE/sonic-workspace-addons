#! /usr/bin/env bash
# only need to change the name of the applet
$XGETTEXT `find . -name \*.qml` -L Java -o $podir/powermanagement-inhibitor.pot
rm -f rc.cpp

