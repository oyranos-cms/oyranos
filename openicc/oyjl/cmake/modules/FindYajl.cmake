# - Try to find YAJL
# Once done this will define
#
#  YAJL_FOUND - system has YAJL
#  YAJL_INCLUDE_DIR - the YAJL include directory
#  YAJL_LIBRARIES - Link these to use YAJL

# Copyright (c) 2006, Alexander Neundorf, <neundorf@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# This file is based on cmake-2.6/Modules/FindBZip2.cmake
# Copyright (c) 2010, Yiannis Belias, <jonnyb@hol.gr>
# modify for YAJL
# Copyright (c) 2012-2020, Kai-Uwe Behrmann, <ku.b@gmx.de>

# use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls

FIND_PACKAGE(PkgConfig QUIET)
pkg_check_modules(PC_YAJL QUIET yajl)
SET(YAJL_DEFINITIONS ${PC_YAJL_CFLAGS_OTHER})

FIND_PATH(YAJL_INCLUDE_DIR NAMES yajl/yajl_common.h HINTS ${PC_YAJL_INCLUDEDIR} ${PC_YAJL_INCLUDE_DIRS} PATH_SUFFIXES yajl)

SET(YAJL_NAMES ${YAJL_NAMES} yajl libyajl yajl-static)
FIND_LIBRARY(YAJL_LIBRARIES NAMES ${YAJL_NAMES} HINTS ${PC_YAJL_LIBDIR} ${PC_YAJL_LIBRARY_DIRS})


# handle the QUIETLY and REQUIRED arguments and set YAJL_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Yajl DEFAULT_MSG YAJL_LIBRARIES YAJL_INCLUDE_DIR)

IF(YAJL_FOUND)
  SET( HAVE_YAJL TRUE )
  SET( YAJL_INCLUDE_DIRS ${YAJL_INCLUDE_DIRS} ${YAJL_INCLUDE_DIR} )
ENDIF(YAJL_FOUND)

MARK_AS_ADVANCED(YAJL_INCLUDE_DIR YAJL_LIBRARIES)
