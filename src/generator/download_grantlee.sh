#!/bin/bash
version=5.1.0
packet_file=grantlee-$version.tar.gz
loc=http://downloads.grantlee.org/

mkdir -p grantlee
cd grantlee
if [ -f grantlee-$version.tar.gz ]; then
  echo ""
else
  which curl && curl -L $loc$packet_file -o $packet_file || wget $loc$packet_file
fi
tar xzf grantlee-$version.tar.gz
cd grantlee-$version
mkdir build
cd build
mkdir -p ../grantlee
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS_DEBUG=-std=c++11 -DCMAKE_INSTALL_PREFIX=../.. ..
make -j 4 && make install
cd ../../..
echo `pwd`
#cmake -DGrantlee_DIR=grantlee/lib/grantlee/ .
#make

echo `pwd`
