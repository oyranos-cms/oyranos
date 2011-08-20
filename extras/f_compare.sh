#!/bin/bash

function usage() {
  echo "Usage: $0 [-h|-s] <function> [branch:]<file1> [branch:]<file2>"
  echo
  echo -e "-h\tShow this help message"
  echo -e "-s\tShow just the diff output"
  echo

  exit 0
}

function cleanup() {
  echo $1
  rm -f $FILE1F $FILE2F

  exit 1
}

case $1 in
  -h|"")
    usage ;;
  -s)
    DIFF=1
    FUNC=$2;
    IN1=$3;
    IN2=$4 ;;
  *)
    DIFF=0
    FUNC=$1;
    IN1=$2;
    IN2=$3 ;;
esac

FILE1=${IN1#*:};
FILE2=${IN2#*:};

AWK=$(dirname $0)/f_compare.awk

FILE1F="$(basename ${FILE1})-${FUNC}.c"
FILE2F="$(basename ${FILE2})-${FUNC}.c"

AWKTMP=/tmp/f_compare-tmp.awk

BRANCH=$(git branch|grep '*'|cut -f2 -d' ')
echo "Current branch: $BRANCH"

if echo $IN1|grep -q : ; then
  BRANCH1=$(echo $IN1|cut -f1 -d:)
else
  BRANCH1=$BRANCH
fi
if echo $IN2|grep -q : ; then
  BRANCH2=$(echo $IN2|cut -f1 -d:)
else
  BRANCH2=$BRANCH
fi
echo "$FILE1 -> $BRANCH1"
echo "$FILE2 -> $BRANCH2"

SHOW1=$BRANCH1:$FILE1
SHOW2=$BRANCH2:$FILE2

cat $AWK | sed "s/__FUNC__/$FUNC/" > $AWKTMP

git show $SHOW1 | awk -f $AWKTMP > $FILE1F
git show $SHOW2 | awk -f $AWKTMP > $FILE2F

test $(du $FILE1F | cut -f1) = "0" && cleanup "$FUNC not found in $IN1"
test $(du $FILE2F | cut -f1) = "0" && cleanup "$FUNC not found in $IN2"

if [ $DIFF = 1 ]; then
  echo "############## diff: ############## "
  diff $FILE1F $FILE2F
  echo "##############       ############## "
else
  vimdiff $FILE1F $FILE2F
fi

echo vimdiff $FILE1F $FILE2F
