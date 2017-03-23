#
# Target installation folders
#

SET (TARGET_INCLUDE_FOLDER
		"${PACKAGE_NAME}"
		CACHE STRING
		"Optional folder below system include folder to install include files."
    )

SET (TARGET_CMAKE_FOLDER
		"cmake/${PACKAGE_NAME}"
		CACHE STRING
		"The folder below system prefix where to install cmake files."
    )

SET (TARGET_PLUGIN_FOLDER
		"color/modules"
		CACHE STRING
		"Optional folder below system library folder where to install oyranos plugins."
    )

SET (TARGET_PLUGIN_LOADER_FOLDER
		"${PACKAGE_NAME}-meta"
		CACHE STRING
		"Optional folder below system library folder where to install oyranos plugin loaders."
    )

SET (TARGET_PKGCONFIG_FOLDER
		"pkgconfig"
		CACHE STRING
		"The folder below system library folder where to install pkgconfig files."
    )


