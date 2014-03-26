CONFIGURE_FILE (
		"${CMAKE_CURRENT_SOURCE_DIR}/oyranos_version.h.in"
		"${CMAKE_BINARY_DIR}/src/include/oyranos_version.h"
		@ONLY
	)

SET( CHEADERS_PUBLIC
        icc34.h
	oyranos.h
	oyranos_color.h
	oyranos_config.h
	oyranos_core.h
	oyranos_definitions.h
	oyranos_icc.h
	oyranos_types.h
        PARENT_SCOPE
   )

INSTALL( FILES
        icc34.h
	oyranos.h
	oyranos_color.h
	oyranos_config.h
	oyranos_core.h
	oyranos_definitions.h
	oyranos_icc.h
	oyranos_types.h
	DESTINATION include/${PACKAGE_NAME})
