# - Try to find Cairo
# Once done this will define
#
#  CAIRO_FOUND - system has Cairo
#  CAIRO_INCLUDE_DIR - the Cairo include directory
#  CAIRO_LIBRARIES - Link these to use Cairo

# Copyright (c) 2006, Alexander Neundorf, <neundorf@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# This file is based on cmake-2.6/Modules/FindBZip2.cmake
# Copyright (c) 2010, Yiannis Belias, <jonnyb@hol.gr>
# modify for Cairo
# Copyright (c) 2012, Kai-Uwe Behrmann, <ku.b@gmx.de>

# use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls
if(NOT WIN32)
   find_package(PkgConfig)
   pkg_check_modules(CAIRO cairo)
   set(CAIRO_DEFINITIONS ${CAIRO_CFLAGS_OTHER})
endif(NOT WIN32)

IF (CAIRO_INCLUDE_DIR AND CAIRO_LIBRARIES)
    SET(CAIRO_FIND_QUIETLY TRUE)
ENDIF (CAIRO_INCLUDE_DIR AND CAIRO_LIBRARIES)

FIND_PATH(CAIRO_INCLUDE_DIR cairo/cairo.h ONLY_CMAKE_FIND_ROOT_PATH)

FIND_LIBRARY(CAIRO_LIBRARIES NAMES cairo )

# handle the QUIETLY and REQUIRED arguments and set CAIRO_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CAIRO DEFAULT_MSG CAIRO_LIBRARIES CAIRO_INCLUDE_DIR)

IF(CAIRO_FOUND)
  SET( HAVE_CAIRO TRUE )
ENDIF(CAIRO_FOUND)

MARK_AS_ADVANCED(CAIRO_INCLUDE_DIR CAIRO_LIBRARIES)
