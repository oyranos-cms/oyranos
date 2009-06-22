CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET( LIBRAW_INCLUDE_DIR "/home/john/local/include" )
SET( LIBRAW_LIBRARY_DIR "/home/john/local/lib" )
SET( LIBRAW_LIBS "raw" )

SET( LIBRAW_L_INCLUDE_DIR "/home/john/Code/GSoC/raw/LibRaw-Lite-0.7.2" )
SET( LIBRAW_L_LIBRARY_DIR "/home/john/Code/GSoC/raw/LibRaw-Lite-0.7.2/lib" )
SET( LIBRAW_L_LIBS "raw-lite" )

PROJECT( gsoc )

SET( CMAKE_BUILD_TYPE "Release" )

INCLUDE_DIRECTORIES( ${LIBRAW_L_INCLUDE_DIR} )
LINK_DIRECTORIES( ${LIBRAW_L_LIBRARY_DIR} )

################################
ADD_EXECUTABLE( raw2tiff "RAW.cc" "raw2tiff.cc")
TARGET_LINK_LIBRARIES( raw2tiff ${LIBRAW_L_LIBS} "exiv2" )
################################
#ADD_EXECUTABLE( scan "scan.c")
#TARGET_LINK_LIBRARIES( scan ${SANE_LIBS} )
################################
