# - Find ELEKTRA
# This module defines
#  ELEKTRA_INCLUDE_DIR, where to find kdb.h
#  ELEKTRA_LIBRARIES, the libraries needed to use ELEKTRA.
#  ELEKTRA_DOT_VERSION, The version number of the ELEKTRA library, e.g. "0.8"
#  ELEKTRA_VERSION, Similar to ELEKTRA_DOT_VERSION, but without the dots, e.g. "22"
#  ELEKTRA_FOUND, If false, do not try to use ELEKTRA.
#
# The minimum required version of ELEKTRA can be specified using the
# standard syntax, e.g. find_package(ELEKTRA 0.8)

# Copyright (c) 2008, Adrian Page, <adrian@pagenet.plus.com>
# Copyright (c) 2009, Cyrille Berger, <cberger@cberger.net>
# Copyright (c) 2012-2014, Kai-Uwe Behrmann, <ku.b@gmx.de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


# use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls
find_package(PkgConfig)
pkg_check_modules(ELEKTRA elektra)
set(ELEKTRA_DEFINITIONS ${ELEKTRA_CFLAGS_OTHER})


find_path(ELEKTRA_INCLUDE_DIR kdb.h
   HINTS
   ${ELEKTRA_INCLUDEDIR}
   ${ELEKTRA_INCLUDE_DIRS}
   PATH_SUFFIXES elektra
   ONLY_CMAKE_FIND_ROOT_PATH
 )

if(NOT ELEKTRA_FOUND)
  find_library(ELEKTRA_LIBRARIES NAMES elektra-core elektra
   HINTS
   ${ELEKTRA_LIBDIR}
   ${ELEKTRA_LIBRARY_DIRS}
   ONLY_CMAKE_FIND_ROOT_PATH
  )
endif(NOT ELEKTRA_FOUND)

# Store the ELEKTRA version number in the cache, so we don't have to search everytime again
if(ELEKTRA_INCLUDE_DIR  AND NOT  ELEKTRA_VERSION)
   file(READ ${ELEKTRA_INCLUDE_DIR}/kdb.h ELEKTRA_VERSION_CONTENT)
   string(REGEX MATCH "#define ELEKTRA_VERSION[ ]*[0-9]*\n" ELEKTRA_VERSION_MATCH ${ELEKTRA_VERSION_CONTENT})
   if(ELEKTRA_VERSION_MATCH)
      string(REGEX REPLACE "#define ELEKTRA_VERSION[ ]*([0-9]*)\n" "\\1" _ELEKTRA_VERSION ${ELEKTRA_VERSION_MATCH})
      string(SUBSTRING ${_ELEKTRA_VERSION} 0 1 ELEKTRA_MAJOR_VERSION)
      string(SUBSTRING ${_ELEKTRA_VERSION} 1 2 ELEKTRA_MINOR_VERSION)
   endif(ELEKTRA_VERSION_MATCH)
   set(ELEKTRA_VERSION "${ELEKTRA_MAJOR_VERSION}${ELEKTRA_MINOR_VERSION}" CACHE STRING "Version number of kdb" FORCE)
   set(ELEKTRA_DOT_VERSION "${ELEKTRA_MAJOR_VERSION}.${ELEKTRA_MINOR_VERSION}" CACHE STRING "Version number of kdb split into components" FORCE)
endif(ELEKTRA_INCLUDE_DIR  AND NOT  ELEKTRA_VERSION)
find_package(PkgConfig)

FIND_PATH(ELEKTRA_INCLUDE_DIRS NAMES elektra/kdb.h
    	ONLY_CMAKE_FIND_ROOT_PATH
	)

FIND_LIBRARY(ELEKTRA_LIBRARIES NAMES elektra elektra-core
	ONLY_CMAKE_FIND_ROOT_PATH
	)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ELEKTRA
	REQUIRED_VARS ELEKTRA_LIBRARIES ELEKTRA_INCLUDE_DIR
	)

if (ELEKTRA_FOUND)

  # query pkg-config asking for Elektra >= 0.7.0
  EXEC_PROGRAM(${PKGCONFIG_EXECUTABLE} ARGS --atleast-version=0.7.0 elektra RETURN_VALUE _return_VALUE OUTPUT_VARIABLE _pkgconfigDevNull )

    set(HAVE_ELEKTRA TRUE)
    if (NOT ELEKTRA_FIND_QUIETLY)
	    message(STATUS "Found ELEKTRA: ${ELEKTRA_LIBRARIES} ${ELEKTRA_INCLUDE_DIRS}")
    endif (NOT ELEKTRA_FIND_QUIETLY)
else (ELEKTRA_FOUND)
    if (NOT ELEKTRA_FIND_QUIETLY)
        message(STATUS "Elektra was NOT found.")
    endif (NOT ELEKTRA_FIND_QUIETLY)
    if (ELEKTRA_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find Elektra")
    endif (ELEKTRA_FIND_REQUIRED)
endif (ELEKTRA_FOUND)
