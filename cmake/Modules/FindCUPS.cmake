# - Try to find CUPS
# Once done this will define
#
#  CUPS_FOUND - system has CUPS
#  CUPS_INCLUDE_DIR - the CUPS include directory
#  CUPS_LIBRARIES - Link these to use CUPS

# Copyright (c) 2006, Alexander Neundorf, <neundorf@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# This file is based on cmake-2.6/Modules/FindBZip2.cmake
# Copyright (c) 2010, Yiannis Belias, <jonnyb@hol.gr>
# modify for CUPS
# Copyright (c) 2012, Kai-Uwe Behrmann, <ku.b@gmx.de>

# use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls
if(NOT WIN32)
   find_package(PkgConfig)
   pkg_check_modules(CUPS cups)
   set(CUPS_DEFINITIONS ${CUPS_CFLAGS_OTHER})
endif(NOT WIN32)

FIND_PATH(CUPS_INCLUDE_DIR cups/cups.h ONLY_CMAKE_FIND_ROOT_PATH)

FIND_LIBRARY(CUPS_LIBRARY NAMES cups  ONLY_CMAKE_FIND_ROOT_PATH)

# handle the QUIETLY and REQUIRED arguments and set CUPS_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CUPS DEFAULT_MSG CUPS_LIBRARY CUPS_INCLUDE_DIR)

IF(CUPS_FOUND)
  SET( HAVE_CUPS TRUE )
ENDIF(CUPS_FOUND)

MARK_AS_ADVANCED(CUPS_INCLUDE_DIR CUPS_LIBRARIES)
