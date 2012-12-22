CONFIGURE_FILE (
		"${CMAKE_CURRENT_SOURCE_DIR}/oyranos_version.h.in"
		"${CMAKE_BINARY_DIR}/src/include/oyranos_version.h"
		@ONLY
	)

SET( CFILES_CORE_PUBLIC
	${PACKAGE_NAME}_core.c
	${PACKAGE_NAME}_debug.c
	${PACKAGE_NAME}_i18n.c
   )
SET( CFILES_CORE_PUBLIC ${CFILES_CORE_PUBLIC} PARENT_SCOPE )
SET( CFILES_CORE
	${CFILES_CORE_PUBLIC}
	${PACKAGE_NAME}_helper.c
	${PACKAGE_NAME}_io_core.c
	${PACKAGE_NAME}_misc.c
	${PACKAGE_NAME}_sentinel_core.c
	${PACKAGE_NAME}_string.c
	md5.c
	lookup3.c
   )

SET( SOURCES ${CFILES_CORE} )

SET( CMAKE_C_FLAGS "${CMAKE_C_FLAGS_CORE}" )
SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_CORE}" )

ADD_LIBRARY( oyranos_core SHARED ${CFILES_CORE})
TARGET_LINK_LIBRARIES ( oyranos_core ${EXTRA_LIBS_CORE} )
ADD_LIBRARY( oyranos_core-static STATIC ${CFILES_CORE})
TARGET_LINK_LIBRARIES ( oyranos_core-static ${EXTRA_LIBS_CORE} )

INSTALL( TARGETS oyranos_core oyranos_core-static DESTINATION lib${LIB_SUFFIX} )
INSTALL (FILES "${CMAKE_BINARY_DIR}/src/include/oyranos_version.h" DESTINATION include/${PACKAGE_NAME})
