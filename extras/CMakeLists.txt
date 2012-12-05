CONFIGURE_FILE(
		"${CMAKE_CURRENT_SOURCE_DIR}/oyranos-config.in"
		"${CMAKE_BINARY_DIR}/oyranos-config"
		@ONLY
       )

INSTALL( PROGRAMS "${CMAKE_BINARY_DIR}/oyranos-config" DESTINATION bin )
INSTALL( PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/oyranos-profile-install" DESTINATION bin )

#pkg-config files
CONFIGURE_FILE(
		"${CMAKE_CURRENT_SOURCE_DIR}/oyranos.pc.in"
		"${CMAKE_CURRENT_BINARY_DIR}/oyranos.pc"
		@ONLY
       )

INSTALL( FILES
		"${CMAKE_CURRENT_BINARY_DIR}/oyranos.pc"
		DESTINATION lib${LIB_SUFFIX}/${TARGET_PKGCONFIG_FOLDER}
       )

