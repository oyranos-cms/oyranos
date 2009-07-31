#!/bin/bash

HTML="$1"
BASE=$(basename $HTML .html)
C_FILE=$(echo ${BASE}.c)

cat $HTML |
sed 's/FRAME=VOID.*BORDER=0/CELLSPACING=2 COLS=8 BORDER=2/g' |
elinks -dump |
awk '{print "\" "$0"\\n\""} BEGIN { print "static const char help_message[] = \"\"" } END { print ";" }' > $C_FILE
