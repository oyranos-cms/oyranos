#/bin/sh -vx

licence='Copyright (c) 2008 Kai-Uwe Behrmann - http://www.opensource.org/licenses/zlib-license.php'
q=h
quality="-q$q -b$q"
gcr="-k z"
ucr="-k x"
inklimit="-l 340"
my_cr="-kp 0 .3 1 1 .5 $inklimit"
cr_weberg="-kp 0 .3 .94 .94 .48 -l300"
cr=$cr_weberg
cr=$5
diagnose=

FOGRA=$1
name="$2 $3 $4"
tmp="`echo $name | sed 's/_/\ /g'`"
name="$tmp"
model=${FOGRA#*/}
model=${model%.*}

cl="colprof -v $quality $cr $diagnose"

echo "$cl -v -D \"$name\" -A \"$cl $model (ArgyllCMS v1.0.3 http://www.argyllcms.com)\" \
        -M \"$model.txt\" -C \"$licence\" $model"

$cl -v -D "$name" -A "$cl $model (ArgyllCMS v1.0.3 http://www.argyllcms.com)" \
        -M "$model.txt" -C "$licence" $model

final="`echo $name | sed 's/\ /_/g'`.icc"
mv $model.icc "$final"
xicclu -g -ia -fb "$final"
