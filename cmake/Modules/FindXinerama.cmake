find_package(PkgConfig)
pkg_check_modules(XINERAMA xinerama)


if (XINERAMA_FOUND)
    set(HAVE_XINERAMA TRUE)
    if (NOT Xinerama_FIND_QUIETLY)
        message(STATUS "Found XINERAMA: ${XINERAMA_LIBRARY_DIRS} ${XINERAMA_INCLUDE_DIRS}")
    endif (NOT Xinerama_FIND_QUIETLY)
else (XINERAMA_FOUND)
    if (NOT Xinerama_FIND_QUIETLY)
        message(STATUS "Xinerama was NOT found.")
    endif (NOT Xinerama_FIND_QUIETLY)
    if (Xinerama_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find Xinerama")
    endif (Xinerama_FIND_REQUIRED)
endif (XINERAMA_FOUND)

