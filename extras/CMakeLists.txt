CONFIGURE_FILE(
		"${CMAKE_CURRENT_SOURCE_DIR}/oyranos-config.in"
		"${CMAKE_BINARY_DIR}/oyranos-config"
		@ONLY
       )

INSTALL( PROGRAMS "${CMAKE_BINARY_DIR}/oyranos-config" DESTINATION bin )
INSTALL( PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/oyranos-profile-install" DESTINATION bin )
INSTALL( PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/oyranos-monitor-daemon" DESTINATION bin )
INSTALL( FILES oyranos-profile-install.desktop DESTINATION share/applications)
INSTALL( FILES icons/oyranos_logo.png DESTINATION share/pixmaps )
INSTALL( FILES icons/lcms_logo2.png DESTINATION share/pixmaps )

# needs to go to the autostart directory - hack hack hack
MESSAGE( "XDG_CONFIG_DIR: ${XDG_CONFIG_DIR}" )
IF( XDG_CONFIG_DIR )
  MESSAGE( "Will install oyranos-monitor-setup.desktop into ${XDG_CONFIG_DIR}/autostart" )
  INSTALL( FILES oyranos-monitor-setup.desktop OPTIONAL DESTINATION ${XDG_CONFIG_DIR}/autostart )
ELSE()
  MESSAGE( "Will install oyranos-monitor-setup.desktop into /etc/xdg/autostart. Change with -DXDG_CONFIG_DIR=<path> ." )
  INSTALL( FILES oyranos-monitor-setup.desktop OPTIONAL DESTINATION /etc/xdg/autostart )
ENDIF()

#pkg-config files
CONFIGURE_FILE(
		"${CMAKE_CURRENT_SOURCE_DIR}/oyranos.pc.in"
		"${CMAKE_CURRENT_BINARY_DIR}/oyranos.pc"
		@ONLY
       )

CONFIGURE_FILE(
		"${CMAKE_CURRENT_SOURCE_DIR}/oyranos.spec.in"
		"${CMAKE_CURRENT_BINARY_DIR}/oyranos.spec"
		@ONLY
       )

INSTALL( FILES
		"${CMAKE_CURRENT_BINARY_DIR}/oyranos.pc"
		DESTINATION lib${LIB_SUFFIX}/${TARGET_PKGCONFIG_FOLDER}
       )

