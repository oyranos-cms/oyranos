#!/bin/bash

mkdir -p grantlee
cd grantlee
if [ -f grantlee-0.1.7.tar.gz ]; then
  echo ""
else
  wget http://downloads.grantlee.org/grantlee-0.1.7.tar.gz
fi
tar xzf grantlee-0.1.7.tar.gz
cd grantlee-0.1.7
mkdir build
cd build
mkdir -p ../grantlee
cmake -DCMAKE_INSTALL_PREFIX=../.. ..
make -j 4 && make install
cd ../../..
echo `pwd`
#cmake -DGrantlee_DIR=grantlee/lib/grantlee/ .
#make

echo `pwd`
