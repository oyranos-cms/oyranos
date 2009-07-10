CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET( LOCAL "/home/john/local/" )
SET( LIBRAW_INCLUDE_DIR "/home/john/local/include" )
SET( LIBRAW_LIBRARY_DIR "/home/john/local/lib" )
SET( LIBRAW_LIBS "raw" )

SET( LIBRAW_L_INCLUDE_DIR "/home/john/Code/GSoC/raw/LibRaw-Lite-0.7.2" )
SET( LIBRAW_L_LIBRARY_DIR "/home/john/Code/GSoC/raw/LibRaw-Lite-0.7.2/lib" )
SET( LIBRAW_L_LIBS "raw-lite" )

SET( SANE_INCLUDE_DIR "/home/john/local/include" )
SET( SANE_LIBRARY_DIR "/home/john/local/lib" )
SET( SANE_LIBS "sane" )

PROJECT( gsoc )

SET( CMAKE_BUILD_TYPE "Debug" )

INCLUDE_DIRECTORIES( ${LIBRAW_L_INCLUDE_DIR} ${SANE_INCLUDE_DIR} )
LINK_DIRECTORIES( ${LIBRAW_L_LIBRARY_DIR} ${SANE_LIBRARY_DIR} )

################################
ADD_EXECUTABLE( raw2tiff "RAW.cc" "raw2tiff.cc")
TARGET_LINK_LIBRARIES( raw2tiff ${LIBRAW_L_LIBS} "exiv2" "tiff" )
################################
ADD_EXECUTABLE( scan "scan.c")
TARGET_LINK_LIBRARIES( scan ${SANE_LIBS} "tiff" )
################################
#SET( OYRANOS_INCLUDE "" )
#INCLUDE_DIRECTORIES( ${OYRANOS_INCLUDE} )
ADD_LIBRARY( oyranos_SANE_cmm_module SHARED oyranos_cmm_SANE.c )
TARGET_LINK_LIBRARIES( oyranos_SANE_cmm_module ${SANE_LIBS} )
ADD_LIBRARY( oyranos_oyRE_cmm_module SHARED oyranos_cmm_oyRE.cc )


##########Installation###########
SET( CMAKE_INSTALL_PREFIX ${LOCAL} )
INSTALL( TARGETS raw2tiff scan
	RUNTIME
	DESTINATION "bin/" )
INSTALL( TARGETS oyranos_SANE_cmm_module oyranos_oyRE_cmm_module
	DESTINATION "lib/colour/moduls/" )
