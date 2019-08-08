SET( OY_LINGUAS_TOOL de_DE.UTF8 )
MACRO(OY_DOCUMENT_TOOL)
  ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_SOURCE_DIR}/doc/man/${TOOL}.1
        COMMAND PATH=. LD_LIBRARY_PATH=${LD_LIBRARY_PATH} OY_MODULE_PATH=${CMAKE_BINARY_DIR}/src/modules XDG_DATA_HOME=${CMAKE_SOURCE_DIR} LANG=en DISPLAY="man_page" ${TOOL} -X man > ${CMAKE_SOURCE_DIR}/doc/man/${TOOL}.1
        DEPENDS ${TOOL} ${TOOL_SRC}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )
  ADD_CUSTOM_TARGET( ${TOOL}.1 ALL
        DEPENDS ${CMAKE_SOURCE_DIR}/doc/man/${TOOL}.1
#        COMMENT "Extract manual page to ${TOOL}.1"
  )
  STRING(REGEX REPLACE "-" "" TOOL_MD ${TOOL})
  MESSAGE( "${TOOL_MD}: ${CMAKE_SOURCE_DIR}/doc/md/${TOOL_MD}.md" )
  ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_SOURCE_DIR}/doc/md/${TOOL_MD}.md
        COMMAND PATH=. LD_LIBRARY_PATH=${LD_LIBRARY_PATH} OY_MODULE_PATH=${CMAKE_BINARY_DIR}/src/modules XDG_DATA_HOME=${CMAKE_SOURCE_DIR} LANG=en DISPLAY="man_page" ${TOOL} -X markdown > ${CMAKE_SOURCE_DIR}/doc/md/${TOOL_MD}.md
        DEPENDS ${TOOL} ${TOOL_SRC}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )
  ADD_CUSTOM_TARGET( ${TOOL_MD}.md ALL
        DEPENDS ${CMAKE_SOURCE_DIR}/doc/md/${TOOL_MD}.md
#        COMMENT "Extract markdown formated manual page to ${TOOL_MD}.md"
  )
  IF( GETTEXT_FOUND )
    # add translations
#    FOREACH( _lang ${OY_LINGUAS_TOOL} )
    SET( _lang de_DE.UTF8 )
    SET( short_lang de )
      ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_SOURCE_DIR}/doc/man/${short_lang}/${TOOL}.1
        COMMAND mkdir -p "${CMAKE_SOURCE_DIR}/doc/man/${short_lang}/"
        COMMAND cp -a ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LOCALEDIR}/${short_lang}/LC_MESSAGES/openicc.mo ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LOCALEDIR}/${short_lang}/LC_MESSAGES/oyjl.mo ${CMAKE_BINARY_DIR}/locale/${short_lang}/LC_MESSAGES/ || echo nothing to copy
        COMMAND PATH=. LD_LIBRARY_PATH=${LD_LIBRARY_PATH} LOCPATH=${CMAKE_BINARY_DIR}/locale OY_MODULE_PATH=${CMAKE_BINARY_DIR}/src/modules XDG_DATA_HOME=${CMAKE_SOURCE_DIR} LANG=${_lang} DISPLAY="man_page" ${TOOL} -X man > ${CMAKE_SOURCE_DIR}/doc/man/${short_lang}/${TOOL}.1
        DEPENDS ${TOOL} ${TOOL_SRC}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      )
      ADD_CUSTOM_TARGET( ${TOOL}-${short_lang}.1 ALL
        DEPENDS ${CMAKE_SOURCE_DIR}/doc/man/${short_lang}/${TOOL}.1
#        COMMENT "Extract ${_lang} manual page to ${CMAKE_SOURCE_DIR}/doc/man/${short_lang}/${TOOL}.1"
      )

      ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_SOURCE_DIR}/doc/md/${TOOL_MD}${short_lang}.md
        COMMAND PATH=. LD_LIBRARY_PATH=${LD_LIBRARY_PATH} LOCPATH=${CMAKE_BINARY_DIR}/locale OY_MODULE_PATH=${CMAKE_BINARY_DIR}/src/modules XDG_DATA_HOME=${CMAKE_SOURCE_DIR} LANG=${_lang} DISPLAY="man_page" ${TOOL} -X markdown > ${CMAKE_SOURCE_DIR}/doc/md/${TOOL_MD}${short_lang}.md
        DEPENDS ${TOOL} ${TOOL_SRC}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      )
      ADD_CUSTOM_TARGET( ${TOOL_MD}${short_lang}.md ALL
        DEPENDS ${CMAKE_SOURCE_DIR}/doc/md/${TOOL_MD}${short_lang}.md
#        COMMENT "Extract markdown formated ${_lang} manual page to ${TOOL_MD}${short_lang}.md"
      )
#    ENDFOREACH( _lang )
  ENDIF( GETTEXT_FOUND )
ENDMACRO(OY_DOCUMENT_TOOL)

SET( LD_LIBRARY_PATH "${CMAKE_BINARY_DIR}:${CMAKE_BINARY_DIR}/src/liboyranos:${CMAKE_BINARY_DIR}/src/liboyranos_config:${CMAKE_BINARY_DIR}/src/liboyranos_modules:${CMAKE_BINARY_DIR}/src/liboyranos_object:${CMAKE_BINARY_DIR}/src/liboyranos_core" )
# ICC profiles listing
SET( TOOL "oyranos-profiles" )
SET( TOOL_SRC "${CMAKE_CURRENT_SOURCE_DIR}/oyranos_profiles.c" )
ADD_EXECUTABLE( ${TOOL} ${TOOL_SRC} )
TARGET_LINK_LIBRARIES( ${TOOL} ${TARGET_LIB} ${EXTRA_LIBS} )
INSTALL (TARGETS ${TOOL} DESTINATION ${CMAKE_INSTALL_BINDIR})
OY_DOCUMENT_TOOL()

# ICC profile information and manipulations
ADD_EXECUTABLE( oyranos-profile "${CMAKE_CURRENT_SOURCE_DIR}/oyranos_profile_tool.c" )
TARGET_LINK_LIBRARIES( oyranos-profile ${TARGET_LIB} ${EXTRA_LIBS} )
INSTALL (TARGETS oyranos-profile DESTINATION ${CMAKE_INSTALL_BINDIR})

# Oyranos settings and config DB
ADD_EXECUTABLE( oyranos-config "${CMAKE_CURRENT_SOURCE_DIR}/oyranos-config.c" )
TARGET_LINK_LIBRARIES( oyranos-config ${TARGET_LIB} ${EXTRA_LIBS} ${DBUS_LIBS} )
INSTALL (TARGETS oyranos-config DESTINATION ${CMAKE_INSTALL_BINDIR})

# Oyranos policy / grouping for easy switching, export, import of default settings
ADD_EXECUTABLE( oyranos-policy "${CMAKE_CURRENT_SOURCE_DIR}/oyranos_policy.c" )
TARGET_LINK_LIBRARIES( oyranos-policy ${TARGET_LIB} ${EXTRA_LIBS} )
INSTALL (TARGETS oyranos-policy DESTINATION ${CMAKE_INSTALL_BINDIR})

# automatic monitor profile selection, information, Taxi DB, EDID fallback
ADD_EXECUTABLE( oyranos-monitor "${CMAKE_CURRENT_SOURCE_DIR}/oyranos_gamma.c" )
TARGET_LINK_LIBRARIES ( oyranos-monitor ${TARGET_LIB} ${EXTRA_LIBS}
               ${XCM_LIBRARIES} ${X11_EXTRA_LIBS} )
INSTALL (TARGETS oyranos-monitor DESTINATION ${CMAKE_INSTALL_BINDIR})

# Oyranos monitor white point handling
SET( TOOL "oyranos-monitor-white-point" )
SET( TOOL_SRC "${CMAKE_CURRENT_SOURCE_DIR}/oyranos-monitor-white-point.c" )
ADD_EXECUTABLE( ${TOOL} ${TOOL_SRC} )
TARGET_LINK_LIBRARIES( ${TOOL} ${TARGET_LIB} ${EXTRA_LIBS} ${DBUS_LIBS} )
INSTALL (TARGETS ${TOOL} DESTINATION ${CMAKE_INSTALL_BINDIR})
OY_DOCUMENT_TOOL()

# Device listing
ADD_EXECUTABLE( oyranos-device "${CMAKE_CURRENT_SOURCE_DIR}/oyranos-device.c" )
TARGET_LINK_LIBRARIES( oyranos-device ${TARGET_LIB} ${EXTRA_LIBS} )
# still beta quality; needs more testing and man page
#INSTALL (TARGETS oyranos-device DESTINATION ${CMAKE_INSTALL_BINDIR})

# 2D grapher for ICC profiles
IF( CAIRO_FOUND )
  SET( TOOL "oyranos-profile-graph" )
  SET( TOOL_SRC "${CMAKE_CURRENT_SOURCE_DIR}/oyranos_profile_graph.c" )
  ADD_EXECUTABLE( ${TOOL} ${TOOL_SRC} )
  TARGET_LINK_LIBRARIES ( ${TOOL} ${TARGET_LIB} ${EXTRA_LIBS}
                 ${CAIRO_LIBRARIES} )
  INSTALL (TARGETS ${TOOL} DESTINATION ${CMAKE_INSTALL_BINDIR})
  OY_DOCUMENT_TOOL()
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
