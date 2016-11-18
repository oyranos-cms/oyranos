find_package(PkgConfig)
pkg_check_modules(XMU xmu)


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

