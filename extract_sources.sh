#!/bin/bash

function loc {
  echo
  echo
  echo "############# Lines Of Code ############# "
  find sources/ -name "*.c" -o -name "*.cc" -o -name "*.txt" -o -name "*.h" | xargs wc -l | grep total | sed 's/ *\([0-9]\+\).*/\1/' | xargs printf "Total %d lines of imported C code in sources/\n"
  find templates/ -name "*.c" -o -name "*.cc" -o -name "*.txt" -o -name "*.h" | xargs wc -l | grep total | sed 's/ *\([0-9]\+\).*/\1/' | xargs printf "Total %d lines of template C/C++ code in templates/\n"
  wc -l *.cc *.h grantlee_extensions/*.h grantlee_extensions/*.cpp | grep total | sed 's/ *\([0-9]\+\).*/\1/' | xargs printf "Total %d lines of C++ code for oyAPIGenerator\n"
  echo                                                                            "+------------------------------------------------------------+"
  wc -l API_generated/* | grep total | sed 's/ *\([0-9]\+\).*/\1/' | xargs printf "Total %d lines of C/C++ auto-generated code in API_generated/\n"
  echo "############# Lines Of Code ############# "
  echo
  echo
}

if [ "$1" = "-l" ]; then
  loc;
  exit 0;
fi

rm -fr API_generated/*

./oyAPIGenerator

loc

cd API_generated/

cmake .
make
