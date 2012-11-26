#!/bin/bash
version=0.2.0

mkdir -p grantlee
cd grantlee
if [ -f grantlee-$version.tar.gz ]; then
  echo ""
else
  wget http://downloads.grantlee.org/grantlee-$version.tar.gz
fi
tar xzf grantlee-$version.tar.gz
cd grantlee-$version
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
