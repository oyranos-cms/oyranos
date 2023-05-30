find_package(PkgConfig)
pkg_check_modules(XFIXES xfixes)


FIND_PATH(XFIXES_INCLUDE_DIRS Xfixes.h
     	HINTS
     	${XFIXES_INCLUDEDIR}
     	${XFIXES_INCLUDE_DIR}
     	PATH_SUFFIXES X11 X11/extensions
	)

FIND_LIBRARY(XFIXES_LIBRARY_DIRS NAMES Xfixes)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Xfixes
  REQUIRED_VARS XFIXES_LIBRARY_DIRS
	)

if (XFIXES_FOUND)
    set(HAVE_XFIXES TRUE)
    if (NOT Xfixes_FIND_QUIETLY)
        message(STATUS "Found XFIXES: ${XFIXES_LIBRARY_DIRS} ${XFIXES_INCLUDE_DIRS}")
    endif (NOT Xfixes_FIND_QUIETLY)
else (XFIXES_FOUND)
    if (NOT Xfixes_FIND_QUIETLY)
        message(STATUS "Xfixes was NOT found.")
    endif (NOT Xfixes_FIND_QUIETLY)
    if (Xfixes_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find Xfixes")
    endif (Xfixes_FIND_REQUIRED)
endif (XFIXES_FOUND)
