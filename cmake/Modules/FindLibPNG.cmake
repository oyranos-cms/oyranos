# - Try to find LibPNG
# Once done this will define
#
#  LIBPNG_FOUND - system has LibPNG
#  LIBPNG_INCLUDE_DIR - the LibPNG include directory
#  LIBPNG_LIBRARIES - Link these to use LibPNG

# Copyright (c) 2006, Alexander Neundorf, <neundorf@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# This file is based on cmake-2.6/Modules/FindBZip2.cmake
# Copyright (c) 2010, Yiannis Belias, <jonnyb@hol.gr>
# modify for libpng
# Copyright (c) 2012, Kai-Uwe Behrmann, <ku.b@gmx.de>

# use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls
find_package(PkgConfig)
pkg_check_modules(LIBPNG libpng)
set(LIBPNG_DEFINITIONS ${LIBPNG_CFLAGS_OTHER})

IF (LIBPNG_INCLUDE_DIR AND LIBPNG_LIBRARIES)
    SET(LibPNG_FIND_QUIETLY TRUE)
ENDIF (LIBPNG_INCLUDE_DIR AND LIBPNG_LIBRARIES)

FIND_PATH(LIBPNG_INCLUDE_DIR png.h )

FIND_LIBRARY(LIBPNG_LIBRARIES NAMES png )

# handle the QUIETLY and REQUIRED arguments and set LIBPNG_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibPNG DEFAULT_MSG LIBPNG_LIBRARIES LIBPNG_INCLUDE_DIR)

IF(LIBPNG_FOUND)
  SET( HAVE_LIBPNG TRUE )
ENDIF(LIBPNG_FOUND)

MARK_AS_ADVANCED(LIBPNG_INCLUDE_DIR LIBPNG_LIBRARIES)
