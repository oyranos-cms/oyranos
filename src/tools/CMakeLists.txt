SET( LANGS
     cs_CZ.UTF8
     de_DE.UTF8 )
SET( OYJL_DIR ":${CMAKE_BINARY_DIR}/openicc/oyjl" )
MACRO(OY_DOCUMENT_TOOL _tool _tool_src _langs)
  IF( CMAKE_CROSSCOMPILING )
      MESSAGE( "-- Skipping ${_tool} docu: cross compiling; Hint: build first for local march to generate docu + i18n" )
  ELSE( CMAKE_CROSSCOMPILING )
    ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_SOURCE_DIR}/doc/man/${_tool}.1
        COMMAND PATH=. LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:$$LD_LIBRARY_PATH OY_MODULE_SKIP_FIX_PATH=1 OY_MODULE_PATH=${CMAKE_BINARY_DIR}/src/modules XDG_DATA_HOME=${CMAKE_SOURCE_DIR} LANG=C DISPLAY="man_page" ${_tool} -X man > ${CMAKE_SOURCE_DIR}/doc/man/${_tool}.1
        DEPENDS ${_tool} ${_tool_src}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    ADD_CUSTOM_TARGET( ${_tool}.1 ALL
        DEPENDS ${CMAKE_SOURCE_DIR}/doc/man/${_tool}.1
#        COMMENT "Extract manual page to ${_tool}.1"
    )
    STRING(REGEX REPLACE "-" "" _tool_MD ${_tool})
    MESSAGE( "${_tool_MD}: ${CMAKE_SOURCE_DIR}/doc/md/${_tool_MD}.md" )
    ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_SOURCE_DIR}/doc/md/${_tool_MD}.md
        COMMAND PATH=. LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:$$LD_LIBRARY_PATH OY_MODULE_SKIP_FIX_PATH=1 OY_MODULE_PATH=${CMAKE_BINARY_DIR}/src/modules XDG_DATA_HOME=${CMAKE_SOURCE_DIR} LANG=C DISPLAY="man_page" ${_tool} -X markdown > ${CMAKE_SOURCE_DIR}/doc/md/${_tool_MD}.md
        DEPENDS ${_tool} ${_tool_src}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    ADD_CUSTOM_TARGET( ${_tool_MD}.md ALL
        DEPENDS ${CMAKE_SOURCE_DIR}/doc/md/${_tool_MD}.md
#        COMMENT "Extract markdown formated manual page to ${_tool_MD}.md"
    )
    IF( USE_GETTEXT )
#      MESSAGE( "add \"${_tool}\": \"${_langs}\"" )
      # add translations
      SET(long_langs "")
      FOREACH( _lang ${_langs} )
        STRING(SUBSTRING ${_lang} 0 2 short_lang)
        STRING(SUBSTRING ${_lang} 0 5 long_lang)
        SET(long_langs ${long_langs} ${long_lang})
#        MESSAGE( "add \"${_tool}\": \"" ${short_lang} "\" from ${_lang} / \"${_langs}\"" )
        ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_SOURCE_DIR}/doc/man/${short_lang}/${_tool}.1
          COMMAND mkdir -p "${CMAKE_SOURCE_DIR}/doc/man/${short_lang}/"
          COMMAND cp -a ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LOCALEDIR}/${short_lang}/LC_MESSAGES/openicc.mo ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LOCALEDIR}/${short_lang}/LC_MESSAGES/oyjl.mo ${CMAKE_BINARY_DIR}/locale/${short_lang}/LC_MESSAGES/ || echo nothing to copy
          COMMAND PATH=. LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:$$LD_LIBRARY_PATH OY_MODULE_SKIP_FIX_PATH=1 LOCPATH=${CMAKE_BINARY_DIR}/locale OY_MODULE_PATH=${CMAKE_BINARY_DIR}/src/modules XDG_DATA_HOME=${CMAKE_SOURCE_DIR} LANG=${_lang} DISPLAY="man_page" ${_tool} -X man > ${CMAKE_SOURCE_DIR}/doc/man/${short_lang}/${_tool}.1
          DEPENDS ${_tool} ${_tool_src}
          WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
        ADD_CUSTOM_TARGET( ${_tool}-${short_lang}.1 ALL
          DEPENDS ${CMAKE_SOURCE_DIR}/doc/man/${short_lang}/${_tool}.1
#        COMMENT "Extract ${_lang} manual page to ${CMAKE_SOURCE_DIR}/doc/man/${short_lang}/${_tool}.1"
        )

        ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_SOURCE_DIR}/doc/md/${_tool_MD}${short_lang}.md
          COMMAND PATH=. LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:$$LD_LIBRARY_PATH OY_MODULE_SKIP_FIX_PATH=1 LOCPATH=${CMAKE_BINARY_DIR}/locale OY_MODULE_PATH=${CMAKE_BINARY_DIR}/src/modules XDG_DATA_HOME=${CMAKE_SOURCE_DIR} LANG=${_lang} DISPLAY="man_page" ${_tool} -X markdown > ${CMAKE_SOURCE_DIR}/doc/md/${_tool_MD}${short_lang}.md
          DEPENDS ${_tool} ${_tool_src}
          WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
        ADD_CUSTOM_TARGET( ${_tool_MD}${short_lang}.md ALL
          DEPENDS ${CMAKE_SOURCE_DIR}/doc/md/${_tool_MD}${short_lang}.md
#        COMMENT "Extract markdown formated ${_lang} manual page to ${_tool_MD}${short_lang}.md"
        )
      ENDFOREACH( _lang ${_langs} )

      STRING(REGEX REPLACE ";" "," oyjl_langs "${long_langs}")
      ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${_tool}.i18n.c
        COMMAND PATH=. LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:$$LD_LIBRARY_PATH OY_MODULE_SKIP_FIX_PATH=1 LOCPATH=${CMAKE_BINARY_DIR}/locale OY_MODULE_PATH=${CMAKE_BINARY_DIR}/src/modules XDG_DATA_HOME=${CMAKE_SOURCE_DIR} LANG="C" DISPLAY="man_page" ${_tool} -X json | PATH=$$PATH${OYJL_DIR} oyjl-translate-bootstrap -a -i - -o ${CMAKE_CURRENT_SOURCE_DIR}/${_tool}.i18n.c -k name,description,help,label,value_name -d "oyjl,oyranos" -p "${CMAKE_BINARY_DIR}/locale/" -l ${oyjl_langs} -t -w=C
          DEPENDS oyjl-translate-bootstrap ${_tool} ${_tool_src}
          WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
      ADD_CUSTOM_TARGET( ${_tool}.i18n.c ALL
          DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${_tool}.i18n.c
#          COMMENT "Extract ${_langs} translations to ${CMAKE_CURRENT_SOURCE_DIR}/${_tool}.i18n.c"
        )
    ENDIF( USE_GETTEXT )
  ENDIF( CMAKE_CROSSCOMPILING )
ENDMACRO(OY_DOCUMENT_TOOL)
MACRO(OY_COMPLETION_TOOL _tool _tool_src)
  IF( CMAKE_CROSSCOMPILING )
      MESSAGE( "-- Skipping ${_tool} completion: cross compiling" )
  ELSE( CMAKE_CROSSCOMPILING )
    ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${_tool}.completion
      COMMAND LANG=C DISPLAY="man_page" PATH=. LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:$$LD_LIBRARY_PATH OY_MODULE_SKIP_FIX_PATH=1 OY_MODULE_PATH=${CMAKE_BINARY_DIR}/src/modules XDG_DATA_HOME=${CMAKE_SOURCE_DIR} ${_tool} -X export | PATH=$$PATH${OYJL_DIR} oyjl-args-bootstrap -i - --completion-bash > ${CMAKE_CURRENT_SOURCE_DIR}/${_tool}.completion
        DEPENDS oyjl-args-bootstrap ${_tool} ${_tool_src}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    ADD_CUSTOM_TARGET( ${_tool}.completion ALL
      DEPENDS  ${CMAKE_CURRENT_SOURCE_DIR}/${_tool}.completion
      COMMENT "Generate bash completion script code ${_tool}.completion"
    )
    INSTALL( FILES ${CMAKE_CURRENT_SOURCE_DIR}/${_tool}.completion DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/bash-completion/completions/ )
  ENDIF( CMAKE_CROSSCOMPILING )
ENDMACRO(OY_COMPLETION_TOOL)


SET( LD_LIBRARY_PATH "${CMAKE_BINARY_DIR}:${CMAKE_BINARY_DIR}/src/liboyranos:${CMAKE_BINARY_DIR}/src/liboyranos_config:${CMAKE_BINARY_DIR}/src/liboyranos_modules:${CMAKE_BINARY_DIR}/src/liboyranos_object:${CMAKE_BINARY_DIR}/src/liboyranos_core" )
# ICC profiles listing
SET( TOOL "oyranos-profiles" )
SET( TOOL_SRC "${CMAKE_CURRENT_SOURCE_DIR}/oyranos_profiles.c" )
ADD_EXECUTABLE( ${TOOL} ${TOOL_SRC} )
IF( NOT HAVE_OYJL )
  SET_TARGET_PROPERTIES( ${TOOL} PROPERTIES COMPILE_DEFINITIONS "NO_OYJL_ARGS_RENDER=1" )
ENDIF()
TARGET_LINK_LIBRARIES( ${TOOL} ${TARGET_LIB} ${EXTRA_LIBS} )
INSTALL (TARGETS ${TOOL} DESTINATION ${CMAKE_INSTALL_BINDIR})
OY_DOCUMENT_TOOL( ${TOOL} ${TOOL_SRC} "${LANGS}" )
OY_COMPLETION_TOOL( ${TOOL} ${TOOL_SRC} )

# ICC profile information and manipulations
SET( TOOL "oyranos-profile" )
SET( TOOL_SRC "${CMAKE_CURRENT_SOURCE_DIR}/oyranos-profile.c" )
ADD_EXECUTABLE( ${TOOL} ${TOOL_SRC} )
IF( NOT HAVE_OYJL )
  SET_TARGET_PROPERTIES( ${TOOL} PROPERTIES COMPILE_DEFINITIONS "NO_OYJL_ARGS_RENDER=1" )
ENDIF()
TARGET_LINK_LIBRARIES( ${TOOL} ${TARGET_LIB} ${EXTRA_LIBS} )
INSTALL (TARGETS ${TOOL} DESTINATION ${CMAKE_INSTALL_BINDIR})
OY_DOCUMENT_TOOL( ${TOOL} ${TOOL_SRC} "${LANGS}" )
OY_COMPLETION_TOOL( ${TOOL} ${TOOL_SRC} )

# Oyranos settings and config DB
SET( TOOL "oyranos-config" )
SET( TOOL_SRC "${CMAKE_CURRENT_SOURCE_DIR}/oyranos-config.c" )
ADD_EXECUTABLE( ${TOOL} ${TOOL_SRC} )
IF( NOT HAVE_OYJL )
  SET_TARGET_PROPERTIES( ${TOOL} PROPERTIES COMPILE_DEFINITIONS "NO_OYJL_ARGS_RENDER=1" )
ENDIF()
TARGET_LINK_LIBRARIES( ${TOOL} ${TARGET_LIB} ${EXTRA_LIBS} ${DBUS_LIBS} )
INSTALL (TARGETS ${TOOL} DESTINATION ${CMAKE_INSTALL_BINDIR})
OY_DOCUMENT_TOOL( ${TOOL} ${TOOL_SRC} "${LANGS}" )
OY_COMPLETION_TOOL( ${TOOL} ${TOOL_SRC} )

# Oyranos policy / grouping for easy switching, export, import of default settings
SET( TOOL "oyranos-policy" )
SET( TOOL_SRC "${CMAKE_CURRENT_SOURCE_DIR}/oyranos-policy.c" )
ADD_EXECUTABLE( ${TOOL} ${TOOL_SRC} )
IF( NOT HAVE_OYJL )
  SET_TARGET_PROPERTIES( ${TOOL} PROPERTIES COMPILE_DEFINITIONS "NO_OYJL_ARGS_RENDER=1" )
ENDIF()
TARGET_LINK_LIBRARIES( ${TOOL} ${TARGET_LIB} ${EXTRA_LIBS} )
INSTALL (TARGETS ${TOOL} DESTINATION ${CMAKE_INSTALL_BINDIR})
OY_DOCUMENT_TOOL( ${TOOL} ${TOOL_SRC} "${LANGS}" )
OY_COMPLETION_TOOL( ${TOOL} ${TOOL_SRC} )

# automatic monitor profile selection, information, Taxi DB, EDID fallback
ADD_EXECUTABLE( oyranos-monitor "${CMAKE_CURRENT_SOURCE_DIR}/oyranos_gamma.c" )
TARGET_LINK_LIBRARIES ( oyranos-monitor ${TARGET_LIB} ${EXTRA_LIBS}
               ${XCM_LIBRARIES} ${X11_EXTRA_LIBS} )
INSTALL (TARGETS oyranos-monitor DESTINATION ${CMAKE_INSTALL_BINDIR})

# Oyranos monitor white point handling
SET( TOOL "oyranos-monitor-white-point" )
SET( TOOL_SRC "${CMAKE_CURRENT_SOURCE_DIR}/oyranos-monitor-white-point.c" )
ADD_EXECUTABLE( ${TOOL} ${TOOL_SRC} )
IF( NOT HAVE_OYJL )
  SET_TARGET_PROPERTIES( ${TOOL} PROPERTIES COMPILE_DEFINITIONS "NO_OYJL_ARGS_RENDER=1" )
ENDIF()
TARGET_LINK_LIBRARIES( ${TOOL} ${TARGET_LIB} ${EXTRA_LIBS} ${DBUS_LIBS} )
INSTALL (TARGETS ${TOOL} DESTINATION ${CMAKE_INSTALL_BINDIR})
OY_DOCUMENT_TOOL( ${TOOL} ${TOOL_SRC} "${LANGS}" )
OY_COMPLETION_TOOL( ${TOOL} ${TOOL_SRC} )

# Device listing
SET( TOOL "oyranos-device" )
SET( TOOL_SRC "${CMAKE_CURRENT_SOURCE_DIR}/oyranos-device.c" )
ADD_EXECUTABLE( ${TOOL} ${TOOL_SRC} )
IF( NOT HAVE_OYJL )
  SET_TARGET_PROPERTIES( ${TOOL} PROPERTIES COMPILE_DEFINITIONS "NO_OYJL_ARGS_RENDER=1" )
ENDIF()
TARGET_LINK_LIBRARIES( ${TOOL} ${TARGET_LIB} ${EXTRA_LIBS} )
INSTALL (TARGETS ${TOOL} DESTINATION ${CMAKE_INSTALL_BINDIR})
OY_DOCUMENT_TOOL( ${TOOL} ${TOOL_SRC} "${LANGS}" )
OY_COMPLETION_TOOL( ${TOOL} ${TOOL_SRC} )

# 2D grapher for ICC profiles
IF( CAIRO_FOUND )
  SET( TOOL "oyranos-profile-graph" )
  SET( TOOL_SRC "${CMAKE_CURRENT_SOURCE_DIR}/oyranos_profile_graph.c" )
  ADD_EXECUTABLE( ${TOOL} ${TOOL_SRC} )
IF( NOT HAVE_OYJL )
  SET_TARGET_PROPERTIES( ${TOOL} PROPERTIES COMPILE_DEFINITIONS "NO_OYJL_ARGS_RENDER=1" )
ENDIF()
  TARGET_LINK_LIBRARIES ( ${TOOL} ${TARGET_LIB} ${EXTRA_LIBS}
                 ${CAIRO_LIBRARIES} )
  INSTALL (TARGETS ${TOOL} DESTINATION ${CMAKE_INSTALL_BINDIR})
  OY_DOCUMENT_TOOL( ${TOOL} ${TOOL_SRC} "${LANGS}" )
  OY_COMPLETION_TOOL( ${TOOL} ${TOOL_SRC} )
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
SET( TOOL "oyranos-icc" )
SET( TOOL_SRC "${CMAKE_CURRENT_SOURCE_DIR}/oyranos-convert.c" )
ADD_EXECUTABLE( ${TOOL} ${TOOL_SRC} )
IF( NOT HAVE_OYJL )
  SET_TARGET_PROPERTIES( ${TOOL} PROPERTIES COMPILE_DEFINITIONS "NO_OYJL_ARGS_RENDER=1" )
ENDIF()
TARGET_LINK_LIBRARIES( ${TOOL} ${TARGET_LIB} ${EXTRA_LIBS} )
INSTALL (TARGETS ${TOOL} DESTINATION ${CMAKE_INSTALL_BINDIR})
OY_DOCUMENT_TOOL( ${TOOL} ${TOOL_SRC} "${LANGS}" )
OY_COMPLETION_TOOL( ${TOOL} ${TOOL_SRC} )

# compatibility tools
IF( ${CMAKE_SYSTEM_NAME} MATCHES Linux )
  ADD_EXECUTABLE( oyranos-compat-gnome "${CMAKE_CURRENT_SOURCE_DIR}/oyranos-compat-gnome.c" )
  TARGET_LINK_LIBRARIES( oyranos-compat-gnome dl )
  INSTALL (TARGETS oyranos-compat-gnome DESTINATION ${CMAKE_INSTALL_BINDIR})
ENDIF()

IF(ENABLE_QT)
  ADD_SUBDIRECTORY (qcmsevents)
ENDIF(ENABLE_QT)
