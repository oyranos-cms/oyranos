#cmake settings for use by find_package
CONFIGURE_FILE (
          "${CMAKE_CURRENT_SOURCE_DIR}/OyranosConfig.cmake.in"
          "${CMAKE_CURRENT_BINARY_DIR}/OyranosConfig.cmake"
          @ONLY
        )

INSTALL ( FILES
          "${CMAKE_CURRENT_BINARY_DIR}/OyranosConfig.cmake"
          DESTINATION ${CMAKE_MODULES_INSTALL_DIR}
          COMPONENT dev
        )


INSTALL( PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/oyranos-profile-install" DESTINATION ${CMAKE_INSTALL_BINDIR} )
INSTALL( PROGRAMS "${CMAKE_CURRENT_SOURCE_DIR}/oyranos-monitor-daemon" DESTINATION ${CMAKE_INSTALL_BINDIR} )
INSTALL( FILES oyranos-profile-install.desktop DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications)
INSTALL( FILES icons/oyranos_logo.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pixmaps )
# KDE has problems rendering SVG blur
#INSTALL( FILES icons/oyranos_logo.svg DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/icons/hicolor/scalable/apps )
INSTALL( FILES icons/oyNM-logo.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pixmaps )
INSTALL( FILES icons/lcms_logo2.png DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pixmaps )

# needs to go to the autostart directory - hack hack hack
MESSAGE( "XDG_CONFIG_DIR: ${XDG_CONFIG_DIR}" )
IF( XDG_CONFIG_DIR )
  MESSAGE( "Will install oyranos-monitor-setup.desktop into ${XDG_CONFIG_DIR}/autostart" )
  INSTALL( FILES oyranos-monitor-setup.desktop OPTIONAL DESTINATION ${XDG_CONFIG_DIR}/autostart )
ELSE()
  MESSAGE( "Will install oyranos-monitor-setup.desktop into ${CMAKE_INSTALL_FULL_SYSCONFDIR}/xdg/autostart. Change with -DXDG_CONFIG_DIR=<path> ." )
  INSTALL( FILES oyranos-monitor-setup.desktop OPTIONAL DESTINATION ${CMAKE_INSTALL_FULL_SYSCONFDIR}/xdg/autostart )
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
		DESTINATION ${CMAKE_INSTALL_LIBDIR}/${TARGET_PKGCONFIG_FOLDER}
       )

