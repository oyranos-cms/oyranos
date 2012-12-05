#
# Target installation folders
#

set (TARGET_INCLUDE_FOLDER
		"oyranos"
		CACHE STRING
		"Optional folder below system include folder to install include files."
    )

set (TARGET_CMAKE_FOLDER
		"share/cmake-${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}/Modules"
		CACHE STRING
		"The folder below system prefix where to install cmake files."
    )

set (TARGET_PLUGIN_FOLDER "oyranos"
		CACHE STRING
		"Optional folder below system library folder where to install oyranos plugins. LIB_SUFFIX is honored."
    )

set (TARGET_PKGCONFIG_FOLDER
		"pkgconfig"
		CACHE STRING
		"The folder below system library folder where to install pkgconfig files. LIB_SUFFIX is honored."
    )

set (TARGET_DOCUMENTATION_FOLDER
		"share/doc/oyranos-api"
		CACHE STRING
		"The folder below system prefix where to install api documentation files."
    )


#
# Misc.
#

set (LIB_SUFFIX ""
		CACHE STRING
		"Optional suffix to use on lib folders (e.g. 64 for lib64)"
    )
#cmake settings for use by find_package
configure_file (
		"${CMAKE_CURRENT_SOURCE_DIR}/FindOyranos.cmake.in"
		"${CMAKE_CURRENT_BINARY_DIR}/FindOyranos.cmake"
		@ONLY
	)

install ( FILES
		"${CMAKE_CURRENT_BINARY_DIR}/FindOyranos.cmake"
		DESTINATION ${TARGET_CMAKE_FOLDER}
	)

