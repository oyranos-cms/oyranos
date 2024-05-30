#
# Target installation folders
#

SET (TARGET_INCLUDE_FOLDER
		"${PACKAGE_NAME}"
		CACHE STRING
		"Optional folder below system include folder to install include files."
    )

IF(NOT CMAKE_MODULES_INSTALL_DIR)
  SET (CMAKE_MODULES_INSTALL_DIR
		"${CMAKE_INSTALL_LIBDIR}/cmake/oyranos"
		CACHE STRING
		"The folder where to install cmake files."
    )
ENDIF(NOT CMAKE_MODULES_INSTALL_DIR)

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


