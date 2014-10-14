# - Find LCMS 2
# Find the LCMS (Little Color Management System) library version 2 
# and includes and
# This module defines
#  LCMS2_INCLUDE_DIR, where to find lcms2.h
#  LCMS2_LIBRARIES, the libraries needed to use LCMS2.
#  LCMS2_DOT_VERSION, The version number of the LCMS2 library, e.g. "2.1"
#  LCMS2_VERSION, Similar to LCMS2_DOT_VERSION, but without the dots, e.g. "22"
#  LCMS2_FOUND, If false, do not try to use LCMS2.
#
# The minimum required version of LCMS 2 can be specified using the
# standard syntax, e.g. find_package(LCMS2 2.4)

# Copyright (c) 2008, Adrian Page, <adrian@pagenet.plus.com>
# Copyright (c) 2009, Cyrille Berger, <cberger@cberger.net>
# Copyright (c) 2012-2014, Kai-Uwe Behrmann, <ku.b@gmx.de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


# use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls
find_package(PkgConfig)
pkg_check_modules(LCMS2 lcms2)
set(LCMS2_DEFINITIONS ${LCMS2_CFLAGS_OTHER})


find_path(LCMS2_INCLUDE_DIR lcms2.h
   HINTS
   ${LCMS2_INCLUDEDIR}
   ${LCMS2_INCLUDE_DIRS}
   PATH_SUFFIXES lcms2 liblcms2
)

if(NOT LCMS2_FOUND)
  find_library(LCMS2_LIBRARIES NAMES lcms2 liblcms2 liblcms2-2 lcms-2 liblcms-2
   HINTS
   ${LCMS2_LIBDIR}
   ${LCMS2_LIBRARY_DIRS}
   #PATH_SUFFIXES lcms2
  )
endif(NOT LCMS2_FOUND)

# Store the LCMS2 version number in the cache, so we don't have to search everytime again
if(LCMS2_INCLUDE_DIR  AND NOT  LCMS2_VERSION)
   file(READ ${LCMS2_INCLUDE_DIR}/lcms2.h LCMS2_VERSION_CONTENT)
   string(REGEX MATCH "#define LCMS2_VERSION[ ]*[0-9]*\n" LCMS2_VERSION_MATCH ${LCMS2_VERSION_CONTENT})
   if(LCMS2_VERSION_MATCH)
      string(REGEX REPLACE "#define LCMS2_VERSION[ ]*([0-9]*)\n" "\\1" _LCMS2_VERSION ${LCMS2_VERSION_MATCH})
      string(SUBSTRING ${_LCMS2_VERSION} 0 1 LCMS2_MAJOR_VERSION)
      string(SUBSTRING ${_LCMS2_VERSION} 1 2 LCMS2_MINOR_VERSION)
   endif(LCMS2_VERSION_MATCH)
   set(LCMS2_VERSION "${LCMS2_MAJOR_VERSION}${LCMS2_MINOR_VERSION}" CACHE STRING "Version number of lcms2" FORCE)
   set(LCMS2_DOT_VERSION "${LCMS2_MAJOR_VERSION}.${LCMS2_MINOR_VERSION}" CACHE STRING "Version number of lcms2 split into components" FORCE)
endif(LCMS2_INCLUDE_DIR  AND NOT  LCMS2_VERSION)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LCMS2 REQUIRED_VARS LCMS2_LIBRARIES LCMS2_INCLUDE_DIR
                                 )

IF(LCMS2_FOUND)
  SET( HAVE_LCMS2 TRUE )
ENDIF(LCMS2_FOUND)

mark_as_advanced(LCMS2_INCLUDE_DIR LCMS2_LIBRARIES LCMS2_VERSION)

