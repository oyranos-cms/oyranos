#!/bin/bash

AWK=$(dirname $0)/f_compare.awk

FUNC=$1
FILE1=$2
FILE2=$3

FILE1F="$(basename ${FILE1})-${FUNC}.c"
FILE2F="$(basename ${FILE2})-${FUNC}.c"

AWKTMP=/tmp/f_compare-tmp.awk

cat $AWK | sed "s/__FUNC__/$FUNC/" > $AWKTMP
cat $FILE1 | awk -f $AWKTMP > $FILE1F
cat $FILE2 | awk -f $AWKTMP > $FILE2F

#diff $FILE1F $FILE2F
vimdiff $FILE1F $FILE2F
echo vimdiff $FILE1F $FILE2F
