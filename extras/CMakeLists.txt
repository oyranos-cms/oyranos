CONFIGURE_FILE (
		"${CMAKE_CURRENT_SOURCE_DIR}/oyranos-config.in"
		"${CMAKE_BINARY_DIR}/oyranos-config"
		@ONLY
       )

INSTALL( PROGRAMS "${CMAKE_BINARY_DIR}/oyranos-config" DESTINATION bin )
INSTALL( PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/oyranos-profile-install" DESTINATION bin )
