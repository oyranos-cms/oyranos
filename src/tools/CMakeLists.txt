# ICC profiles listing
ADD_EXECUTABLE( oyranos-profiles "${CMAKE_CURRENT_SOURCE_DIR}/oyranos_profiles.c" )
TARGET_LINK_LIBRARIES( oyranos-profiles ${PROJECT_NAME} ${EXTRA_LIBS} )
INSTALL (TARGETS oyranos-profiles DESTINATION ${CMAKE_INSTALL_BINDIR})

# ICC profile information and manipulations
ADD_EXECUTABLE( oyranos-profile "${CMAKE_CURRENT_SOURCE_DIR}/oyranos_profile_tool.c" )
TARGET_LINK_LIBRARIES( oyranos-profile ${PROJECT_NAME} ${EXTRA_LIBS} )
INSTALL (TARGETS oyranos-profile DESTINATION ${CMAKE_INSTALL_BINDIR})

# Oyranos policy / grouping for easy switching, export, import of default settings
ADD_EXECUTABLE( oyranos-policy "${CMAKE_CURRENT_SOURCE_DIR}/oyranos_policy.c" )
TARGET_LINK_LIBRARIES( oyranos-policy ${PROJECT_NAME} ${EXTRA_LIBS} )
INSTALL (TARGETS oyranos-policy DESTINATION ${CMAKE_INSTALL_BINDIR})

# automatic monitor profile selection, information, Taxi DB, EDID fallback
ADD_EXECUTABLE( oyranos-monitor "${CMAKE_CURRENT_SOURCE_DIR}/oyranos_gamma.c" )
TARGET_LINK_LIBRARIES ( oyranos-monitor ${PROJECT_NAME} ${EXTRA_LIBS}
               ${XINERAMA_LIBRARIES} ${XRANDR_LIBRARIES} ${XCM_LIBRARIES}
               ${XFIXES_LIBRARIES} ${X11_LIBRARIES} )
INSTALL (TARGETS oyranos-monitor DESTINATION ${CMAKE_INSTALL_BINDIR})

# Device listing
ADD_EXECUTABLE( oyranos-device "${CMAKE_CURRENT_SOURCE_DIR}/oyranos-device.c" )
TARGET_LINK_LIBRARIES( oyranos-device ${PROJECT_NAME} ${EXTRA_LIBS} )
# still beta quality; needs more testing and man page
#INSTALL (TARGETS oyranos-device DESTINATION ${CMAKE_INSTALL_BINDIR})

# 2D grapher for ICC profiles
IF( CAIRO_FOUND )
  ADD_EXECUTABLE( oyranos-profile-graph "${CMAKE_CURRENT_SOURCE_DIR}/oyranos_profile_graph.c" )
  TARGET_LINK_LIBRARIES ( oyranos-profile-graph ${PROJECT_NAME} ${EXTRA_LIBS}
                 ${CAIRO_LIBRARIES} )
  INSTALL (TARGETS oyranos-profile-graph DESTINATION ${CMAKE_INSTALL_BINDIR})
ENDIF()

# color conversion
ADD_EXECUTABLE( oyranos-icc "${CMAKE_CURRENT_SOURCE_DIR}/oyranos_convert.c" )
TARGET_LINK_LIBRARIES( oyranos-icc ${PROJECT_NAME} ${EXTRA_LIBS} oyranos_forms_cmd_line-static )
INSTALL (TARGETS oyranos-icc DESTINATION ${CMAKE_INSTALL_BINDIR})

# compatibility tools
IF( ${PLATFORM} MATCHES Linux )
  ADD_EXECUTABLE( oyranos-compat-gnome "${CMAKE_CURRENT_SOURCE_DIR}/oyranos-compat-gnome.c" )
  TARGET_LINK_LIBRARIES( oyranos-compat-gnome dl )
  INSTALL (TARGETS oyranos-compat-gnome DESTINATION ${CMAKE_INSTALL_BINDIR})
ENDIF()

ADD_SUBDIRECTORY (qcmsevents)
