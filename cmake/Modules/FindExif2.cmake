# - Try to find Exiv2
# Once done this will define
#
#  EXIV2_FOUND - system has Exiv2
#  EXIV2_INCLUDE_DIR - the Exiv2 include directory
#  EXIV2_LIBRARIES - Link these to use Exiv2

# Copyright (c) 2006, Alexander Neundorf, <neundorf@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# This file is based on cmake-2.6/Modules/FindBZip2.cmake
# Copyright (c) 2010, Yiannis Belias, <jonnyb@hol.gr>

# Turned into a Exiv2 detector.
# Copyright (c) 2012, Kai-Uwe Behrmann, <ku.b@gmx.de>

# use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls
if(NOT WIN32)
   find_package(PkgConfig)
   pkg_check_modules(EXIV2 exiv2)
   set(EXIV2_DEFINITIONS ${EXIV2_CFLAGS_OTHER})
endif(NOT WIN32)

IF (EXIV2_INCLUDE_DIR AND EXIV2_LIBRARIES)
    SET(Exiv2_FIND_QUIETLY TRUE)
ENDIF (EXIV2_INCLUDE_DIR AND EXIV2_LIBRARIES)

FIND_LIBRARY(EXIV2_LIBRARIES NAMES exiv2 )

FIND_PATH(EXIV2_INCLUDE_DIR exiv2/exiv2.hpp
   HINTS
   ${PC_EXIV2_INCLUDEDIR}
   ${PC_EXIF2_INCLUDE_DIRS}
   PATH_SUFFIXES exiv2
)


# handle the QUIETLY and REQUIRED arguments and set EXIV2_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Exiv2 DEFAULT_MSG EXIV2_LIBRARIES EXIV2_INCLUDE_DIR)

IF(EXIV2_FOUND)
  SET( HAVE_EXIV2 TRUE )
ENDIF(EXIV2_FOUND)

MARK_AS_ADVANCED(EXIV2_INCLUDE_DIR EXIV2_LIBRARIES)
