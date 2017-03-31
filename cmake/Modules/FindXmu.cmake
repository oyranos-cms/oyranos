# Redistribution and use is allowed according to the terms of the BSD license.
# Copyright (c) 2012-2014, Kai-Uwe Behrmann, <ku.b@gmx.de>

find_package(PkgConfig)
pkg_check_modules(XMU xmu)

FIND_PATH(XMU_INCLUDE_DIR Xmu.h
     	HINTS
     	${XMU_INCLUDEDIR}
     	${XMU_INCLUDE_DIRS}
     	PATH_SUFFIXES X11 X11/Xmu
	)

FIND_LIBRARY(XMU_LIBRARY_DIR NAMES Xmu)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Xmu
	REQUIRED_VARS XMU_LIBRARY_DIR XMU_INCLUDE_DIR
	)

if (XMU_FOUND)
    set(HAVE_XMU TRUE)
    if (NOT Xmu_FIND_QUIETLY)
        message(STATUS "Found XMU: ${XMU_LIBRARY_DIRS} ${XMU_INCLUDE_DIRS}")
    endif (NOT Xmu_FIND_QUIETLY)
else (XMU_FOUND)
    if (NOT Xmu_FIND_QUIETLY)
        message(STATUS "Xmu was NOT found.")
    endif (NOT Xmu_FIND_QUIETLY)
    if (Xmu_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find Xmu")
    endif (Xmu_FIND_REQUIRED)
endif (XMU_FOUND)

