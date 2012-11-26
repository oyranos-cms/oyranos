configure_file (
		"${CMAKE_CURRENT_SOURCE_DIR}/oyranos_version.h.in"
		"${CMAKE_BINARY_DIR}/src/include/oyranos_version.h"
		@ONLY
	)

#now add all source files of this folder
file (GLOB SRC_FILES *.c)

set (SOURCES ${SRC_FILES} ${HDR_FILES})

add_library(oyranos_core SHARED ${SRC_FILES})

install (TARGETS oyranos_core DESTINATION lib${LIB_SUFFIX})

#if you want the same name for those 3 libraries, here is how to do it:
#http://www.itk.org/Wiki/CMake_FAQ#How_do_I_make_my_shared_and_static_libraries_have_the_same_root_name.2C_but_different_suffixes.3F
