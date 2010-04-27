#!/bin/bash

mkdir -p src
rm -fr src/*

for t in templates/*.template.h; do
   CLASSNAME=`basename $t .template.h`;
   TMPLNAME=`basename $t`
   SRCNAME=src/oy`basename $t|sed 's/.template//g'`
   ./oyAPIGenerator ${CLASSNAME} ${TMPLNAME}  > ${SRCNAME};
done
