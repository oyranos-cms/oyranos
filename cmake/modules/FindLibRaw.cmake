# - Try to find LibRaw
# Once done this will define
#
#  LIBRAW_FOUND - system has LibRaw
#  LIBRAW_INCLUDE_DIR - the LibRaw include directory
#  LIBRAW_LIBRARIES - Link these to use LibRaw

# Copyright (c) 2006, Alexander Neundorf, <neundorf@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# This file is based on cmake-2.6/Modules/FindBZip2.cmake
# Copyright (c) 2010, Yiannis Belias, <jonnyb@hol.gr>

IF (LIBRAW_INCLUDE_DIR AND LIBRAW_LIBRARIES)
    SET(LibRaw_FIND_QUIETLY TRUE)
ENDIF (LIBRAW_INCLUDE_DIR AND LIBRAW_LIBRARIES)

FIND_PATH(LIBRAW_INCLUDE_DIR libraw/libraw.h )

FIND_LIBRARY(LIBRAW_LIBRARIES NAMES raw raw_r )

# handle the QUIETLY and REQUIRED arguments and set LIBRAW_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LibRaw DEFAULT_MSG LIBRAW_LIBRARIES LIBRAW_INCLUDE_DIR)
MESSAGE( "LIBRAW_LIBRARIES=" ${LIBRAW_LIBRARIES} )
MARK_AS_ADVANCED(LIBRAW_INCLUDE_DIR LIBRAW_LIBRARIES)
