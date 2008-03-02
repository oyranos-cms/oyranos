#!/bin/bash

if [ $# -lt 1 ]; then
  echo ""
 echo "no xml file given for PO creation."
 echo "Usage:"
 echo "  $0 cmms/xxx.oy_cmm_register.xml"
 echo ""
fi

# change file name extension
name=`echo "$@" | sed 's/.xml/.pot/'`

# find lines containing strings and remove xml tags
n=`cat "$@" | grep -e 'oyNAME>' -e 'oyDESCRIPTION>' | sed 's%<oyNAME>%%g ; s%</oyNAME>%%g ; s%<oyDESCRIPTION>%%g ; s%</oyDESCRIPTION>%%g ; s%^[ ,\t,\n\]*%% '`

# sort and delete double occurencies
n=`echo "$n" | sort | sed '$!N; /^\(.*\)\n\1$/!P; D'`

# calculate number of lines
l=`echo "$n" | wc -l`
if [ $l -lt 2 ]; then
 echo "!!! no strings found !!!"
 echo "did you feed a Oyranos xml file with <oyNAME> and <oyDESCRIPTION> tags?"
 echo ""
 exit 1
fi

if [ -f "$name" ]; then
 file_exist=1
fi
echo " preparing $l strings"

l=$((l+1))
x=1

# write header
echo "# SOME DESCRIPTIVE TITLE.
# Copyright (C) YEAR Kai-Uwe Behrmann
# This file is distributed under the same license as the PACKAGE package.
# FIRST AUTHOR <EMAIL@ADDRESS>, YEAR.
#
#, fuzzy
msgid \"\"
msgstr \"\"
\"Project-Id-Version: PACKAGE VERSION\n\"
\"Report-Msgid-Bugs-To: ku.b@gmx.de\n\"
\"POT-Creation-Date: 2006-07-26 09:09+0200\n\"
\"PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\n\"
\"Last-Translator: FULL NAME <EMAIL@ADDRESS>\n\"
\"Language-Team: LANGUAGE <LL@li.org>\n\"
\"MIME-Version: 1.0\n\"
\"Content-Type: text/plain; charset=CHARSET\n\"
\"Content-Transfer-Encoding: 8bit\n\"
" > "$name"

# define substitutes
exist="msgid"
translated="msgstr"

test -f oy_temp.txt && rm oy_temp.txt

# prepare lineas with PO syntax
while [ $x -lt $l ]; do
 echo -e "$exist \"\c" >> oy_temp.txt
 k=`echo "$n" | sed $x'!d'`
 echo -e "$k\c" >> oy_temp.txt
 echo "\"" >> oy_temp.txt
 x=$((x+1))
 echo -e $translated "\"\"\n" >> oy_temp.txt
 echo -e ".\c"
done

echo " done"

cat oy_temp.txt >> "$name"
if [ -z $file_exist ]; then
 echo -e "created new \c"
else
 echo -e "overwritten old \c"
fi
echo "file \"$name\""
rm oy_temp.txt
