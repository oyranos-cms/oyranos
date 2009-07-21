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

##################TARGETS##################

#raw2tiff##################
ADD_EXECUTABLE( raw2tiff "RAW.cc" "raw2tiff.cc")
TARGET_LINK_LIBRARIES( raw2tiff ${LIBRAW_L_LIBS} "exiv2" "tiff" )
#scan##################
ADD_EXECUTABLE( scan "scan.c")
TARGET_LINK_LIBRARIES( scan ${SANE_LIBS} "tiff" )

#oytest##################
EXECUTE_PROCESS(	COMMAND oyranos-config --ldflags
						COMMAND tr '\n' " "
						OUTPUT_VARIABLE OY_LDFLAGS )
EXECUTE_PROCESS(	COMMAND oyranos-config --cflags
						COMMAND tr '\n' " "
						OUTPUT_VARIABLE OY_CFLAGS )
ADD_EXECUTABLE( oytest "oyTest.cc")
SET_TARGET_PROPERTIES( oytest	PROPERTIES
										COMPILE_FLAGS "${OY_CFLAGS}"
										LINK_FLAGS "${OY_LDFLAGS}" )
#oyranos backends##################
ADD_LIBRARY( oyranos_SANE_cmm_module SHARED oyranos_cmm_SANE.c )
TARGET_LINK_LIBRARIES( oyranos_SANE_cmm_module ${SANE_LIBS} )
ADD_LIBRARY( oyranos_oyRE_cmm_module SHARED oyranos_cmm_oyRE.cc )
TARGET_LINK_LIBRARIES( oyranos_oyRE_cmm_module "exiv2" )

add_subdirectory( doc )

##################INSTALLATION##################
SET( CMAKE_INSTALL_PREFIX ${LOCAL} )
INSTALL( TARGETS raw2tiff scan
	RUNTIME
	DESTINATION "bin/" )
INSTALL( TARGETS oyranos_SANE_cmm_module oyranos_oyRE_cmm_module
	DESTINATION "lib/colour/moduls/" )
