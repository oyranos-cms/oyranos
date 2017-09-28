# ICC profiles listing
ADD_EXECUTABLE( oyranos-profiles "${CMAKE_CURRENT_SOURCE_DIR}/oyranos_profiles.c" )
TARGET_LINK_LIBRARIES( oyranos-profiles ${TARGET_LIB} ${EXTRA_LIBS} )
INSTALL (TARGETS oyranos-profiles DESTINATION ${CMAKE_INSTALL_BINDIR})

# ICC profile information and manipulations
ADD_EXECUTABLE( oyranos-profile "${CMAKE_CURRENT_SOURCE_DIR}/oyranos_profile_tool.c" )
TARGET_LINK_LIBRARIES( oyranos-profile ${TARGET_LIB} ${EXTRA_LIBS} )
INSTALL (TARGETS oyranos-profile DESTINATION ${CMAKE_INSTALL_BINDIR})

# Oyranos policy / grouping for easy switching, export, import of default settings
ADD_EXECUTABLE( oyranos-policy "${CMAKE_CURRENT_SOURCE_DIR}/oyranos_policy.c" )
TARGET_LINK_LIBRARIES( oyranos-policy ${TARGET_LIB} ${EXTRA_LIBS} )
INSTALL (TARGETS oyranos-policy DESTINATION ${CMAKE_INSTALL_BINDIR})

# automatic monitor profile selection, information, Taxi DB, EDID fallback
ADD_EXECUTABLE( oyranos-monitor "${CMAKE_CURRENT_SOURCE_DIR}/oyranos_gamma.c" )
TARGET_LINK_LIBRARIES ( oyranos-monitor ${TARGET_LIB} ${EXTRA_LIBS}
               ${XINERAMA_LIBRARIES} ${XRANDR_LIBRARIES} ${XCM_LIBRARIES}
               ${XFIXES_LIBRARIES} ${X11_LIBRARIES} )
INSTALL (TARGETS oyranos-monitor DESTINATION ${CMAKE_INSTALL_BINDIR})

# Oyranos monitor white point handling
ADD_EXECUTABLE( oyranos-monitor-white-point "${CMAKE_CURRENT_SOURCE_DIR}/oyranos-monitor-white-point.c" )
TARGET_LINK_LIBRARIES( oyranos-monitor-white-point ${TARGET_LIB} ${EXTRA_LIBS} ${DBUS_LIBS} )
INSTALL (TARGETS oyranos-monitor-white-point DESTINATION ${CMAKE_INSTALL_BINDIR})

# Device listing
ADD_EXECUTABLE( oyranos-device "${CMAKE_CURRENT_SOURCE_DIR}/oyranos-device.c" )
TARGET_LINK_LIBRARIES( oyranos-device ${TARGET_LIB} ${EXTRA_LIBS} )
# still beta quality; needs more testing and man page
#INSTALL (TARGETS oyranos-device DESTINATION ${CMAKE_INSTALL_BINDIR})

# 2D grapher for ICC profiles
IF( CAIRO_FOUND )
  ADD_EXECUTABLE( oyranos-profile-graph "${CMAKE_CURRENT_SOURCE_DIR}/oyranos_profile_graph.c" )
  TARGET_LINK_LIBRARIES ( oyranos-profile-graph ${TARGET_LIB} ${EXTRA_LIBS}
                 ${CAIRO_LIBRARIES} )
  INSTALL (TARGETS oyranos-profile-graph DESTINATION ${CMAKE_INSTALL_BINDIR})
ENDIF()

ADD_SUBDIRECTORY( oforms )

# oFORMS for a Oyranos CLI renderer and parser
ADD_EXECUTABLE( oyranos-xforms oforms/oyranos_xforms.c )
TARGET_LINK_LIBRARIES( oyranos-xforms
                  ${OY_OFORMS_LIB} ${TARGET_LIB} ${EXTRA_LIBS} )
INSTALL (TARGETS oyranos-xforms DESTINATION ${CMAKE_INSTALL_BINDIR})

# oFORMS for a Oyranos FLTK renderer and parser
IF( FLTK_FOUND )
  ADD_EXECUTABLE( oyranos-xforms-fltk
                  oforms/oyranos_forms_fltk.cxx
                  oforms/oyranos_widgets_fltk.cxx
                  oforms/oyranos_xforms_fltk.cxx
                )
  TARGET_LINK_LIBRARIES ( oyranos-xforms-fltk ${TARGET_LIB} ${EXTRA_LIBS}
                  ${OY_OFORMS_LIB} ${FLTK_LIBRARIES} ${XRANDR_LIBRARIES} )
  INSTALL (TARGETS oyranos-xforms-fltk DESTINATION ${CMAKE_INSTALL_BINDIR})
ENDIF()

# oFORMS for a Oyranos module generation
ADD_EXECUTABLE( oyranos-xforms-modules oforms/oyranos_xforms_modules.c )
TARGET_LINK_LIBRARIES( oyranos-xforms-modules
             ${TARGET_LIB} ${OY_OFORMS_LIB} ${EXTRA_LIBS} )
INSTALL (TARGETS oyranos-xforms-modules DESTINATION ${CMAKE_INSTALL_BINDIR})

# color conversion
ADD_EXECUTABLE( oyranos-icc "${CMAKE_CURRENT_SOURCE_DIR}/oyranos_convert.c" )
TARGET_LINK_LIBRARIES( oyranos-icc ${TARGET_LIB} ${EXTRA_LIBS} oyranos_forms_cmd_line-static )
INSTALL (TARGETS oyranos-icc DESTINATION ${CMAKE_INSTALL_BINDIR})

# compatibility tools
IF( ${CMAKE_SYSTEM_NAME} MATCHES Linux )
  ADD_EXECUTABLE( oyranos-compat-gnome "${CMAKE_CURRENT_SOURCE_DIR}/oyranos-compat-gnome.c" )
  TARGET_LINK_LIBRARIES( oyranos-compat-gnome dl )
  INSTALL (TARGETS oyranos-compat-gnome DESTINATION ${CMAKE_INSTALL_BINDIR})
ENDIF()

IF(ENABLE_QT)
  ADD_SUBDIRECTORY (qcmsevents)
ENDIF(ENABLE_QT)
