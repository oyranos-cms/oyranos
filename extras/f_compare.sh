#!/bin/bash

function usage() {
  echo "Usage: $0 [-h] <function> [branch:]<file1> [branch:]<file2>"
  echo
  echo -e "-h\tShow this help message"
  echo

  exit 0
}

function cleanup() {
  echo $1
  rm -f $FILE1F $FILE2F

  exit 1
}

case $1 in
  -h|"") usage ;;
esac


AWK=$(dirname $0)/f_compare.awk

FUNC=$1
FILE1=${2#*:}
FILE2=${3#*:}

FILE1F="$(basename ${FILE1})-${FUNC}.c"
FILE2F="$(basename ${FILE2})-${FUNC}.c"

AWKTMP=/tmp/f_compare-tmp.awk

BRANCH=$(git branch|grep '*'|cut -f2 -d' ')
echo "Current branch: $BRANCH"

if echo $2|grep -q : ; then
  BRANCH1=$(echo $2|cut -f1 -d:)
else
  BRANCH1=$BRANCH
fi
if echo $3|grep -q : ; then
  BRANCH2=$(echo $3|cut -f1 -d:)
else
  BRANCH2=$BRANCH
fi
echo "$FILE1 -> $BRANCH1"
echo "$FILE2 -> $BRANCH2"

cat $AWK | sed "s/__FUNC__/$FUNC/" > $AWKTMP

if [ $BRANCH1 != $BRANCH ]; then
  git checkout -q $BRANCH1 || exit 1
fi
cat $FILE1 | awk -f $AWKTMP > $FILE1F

if [ $BRANCH2 != $BRANCH1 ]; then
  git checkout -q $BRANCH2 || exit 1
fi
cat $FILE2 | awk -f $AWKTMP > $FILE2F

if [ $BRANCH != $BRANCH2 ]; then
  git checkout -q $BRANCH || exit 1
fi

test $(du $FILE1F | cut -f1) = "0" && cleanup "$FUNC not found in $2"
test $(du $FILE2F | cut -f1) = "0" && cleanup "$FUNC not found in $3"

#diff $FILE1F $FILE2F
#vimdiff $FILE1F $FILE2F
echo vimdiff $FILE1F $FILE2F
