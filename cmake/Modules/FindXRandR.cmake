find_package(PkgConfig)
pkg_check_modules(XRANDR xrandr)


if (XRANDR_FOUND)
    set(HAVE_XRANDR TRUE)
    if (NOT XRandR_FIND_QUIETLY)
        message(STATUS "Found XRANDR: ${XRANDR_LIBRARY_DIRS} ${XRANDR_INCLUDE_DIRS}")
    endif (NOT XRandR_FIND_QUIETLY)
else (XRANDR_FOUND)
    if (NOT XRandR_FIND_QUIETLY)
        message(STATUS "XRandR was NOT found.")
    endif (NOT XRandR_FIND_QUIETLY)
    if (XRandR_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find XRandR")
    endif (XRandR_FIND_REQUIRED)
endif (XRANDR_FOUND)

