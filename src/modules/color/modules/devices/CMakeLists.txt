# --- dependencies ---

# --- variables ---
IF(USE_GETTEXT)
  SET_SOURCE_FILES_PROPERTIES( ${PACKAGE_NAME}_monitor.c PROPERTIES COMPILE_DEFINITIONS "USE_GETTEXT=1" )
  SET_SOURCE_FILES_PROPERTIES( ${PACKAGE_NAME}_monitor_x11.c PROPERTIES COMPILE_DEFINITIONS "USE_GETTEXT=1" )
ENDIF()

# --- helpers ---
MACRO(OY_HANDLE_DEVICE_JSON)
  IF(ENABLE_STATIC_LIBS_${PROJECT_UP_NAME})
    IF(UNIX)
      SET( GJ_${OY_MODULE} generate-rank-map-${OY_MODULE} )
      ADD_CUSTOM_TARGET( generate-rank-map-${OY_MODULE}
        COMMAND cp ${CMAKE_SOURCE_DIR}/color/rank-map/${DEVICE_JSON} .
	COMMAND xxd -i ${DEVICE_JSON} | sed "s/};/  , 0x00 };/" > ${CMAKE_SOURCE_DIR}/src/include_private/${DEVICE_JSON}.h
        COMMAND rm ${DEVICE_JSON}
	COMMENT "generate static compiled in ${OY_MODULE} rank map from JSON." VERBATIM )
    ENDIF(UNIX)
  ENDIF(ENABLE_STATIC_LIBS_${PROJECT_UP_NAME})
  INSTALL( FILES       ${CMAKE_SOURCE_DIR}/color/rank-map/${DEVICE_JSON}
           DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/color/rank-map/
         )
ENDMACRO(OY_HANDLE_DEVICE_JSON)

# --- normal modules ---
IF( HAVE_X11 )
  SET( OY_MODULE oyX1 )
  SET( MODULE_EXTRA_LIBS
               ${XINERAMA_LIBRARIES} ${XRANDR_LIBRARIES} ${XCM_LIBRARIES}
               ${XFIXES_LIBRARIES} ${XXF86VM_LIBRARIES} ${X11_LIBRARIES} )
  SET( OY_MODULE_LIB ${PACKAGE_NAME}_${OY_MODULE}${OY_MODULE_NAME} )
  SET( MODULE_DEFS ${MODULE_DEFS} "${OY_MODULE}=${OY_MODULE}" )
  IF(HAVE_XFIXES)
    SET( BASE_DEFS ${BASE_DEFS} "HAVE_XFIXES=1")
  ENDIF()
  IF(HAVE_XCM)
    SET( BASE_DEFS ${BASE_DEFS} "HAVE_XCM=1")
  ENDIF()
  IF(HAVE_XRANDR)
    SET( BASE_DEFS ${BASE_DEFS} "HAVE_XRANDR=1")
  ENDIF()
  IF(HAVE_XINERAMA)
    SET( BASE_DEFS ${BASE_DEFS} "HAVE_XINERAMA=1")
  ENDIF()
  IF(HAVE_XXF86VM)
    SET( BASE_DEFS ${BASE_DEFS} "HAVE_XXF86VM=1")
  ENDIF()
  IF(HAVE_XEXT)
    SET( BASE_DEFS ${BASE_DEFS} "HAVE_XEXT=1")
  ENDIF()
  IF(HAVE_X11)
    SET( BASE_DEFS ${BASE_DEFS} "HAVE_X11=1")
  ENDIF()
  SET( BASE_DEFS ${BASE_DEFS} "PRINT_TIME=fprintf( stderr, \"%s \", oyjlPrintTime(OYJL_BRACKETS, oyjlGREEN) )")
  SET( BASE_DEFS ${BASE_DEFS} "PRINT_TIME_INCLUDE=<oyjl.h>")
  SET_SOURCE_FILES_PROPERTIES( ${PACKAGE_NAME}_monitor_x11.c PROPERTIES COMPILE_DEFINITIONS "${BASE_DEFS}" )
  SET( SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE_NAME}_cmm_disp.c
               ${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE_NAME}_monitor.c
               ${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE_NAME}_monitor_x11.c
               ${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE_NAME}_monitor_x11_extend.c
     )
  OY_CREATE_MODULE(ENABLE_SHARED_LIBS_${PROJECT_UP_NAME} ENABLE_STATIC_LIBS_${PROJECT_UP_NAME})

  SET( DEVICE_JSON config.icc_profile.monitor.${OY_MODULE}.qarz.json )
  OY_HANDLE_DEVICE_JSON()
ENDIF( HAVE_X11 )

IF( ${CMAKE_SYSTEM_NAME} MATCHES Darwin )
  SET( OY_MODULE qarz )
  SET( MODULE_EXTRA_LIBS ${EXTRA_LIBS_CS}
               ${XINERAMA_LIBRARIES} ${XRANDR_LIBRARIES} ${XCM_LIBRARIES}
               ${XFIXES_LIBRARIES} ${X11_LIBRARIES} )
  SET( OY_MODULE_LIB ${PACKAGE_NAME}_${OY_MODULE}${OY_MODULE_NAME} )
  SET( MODULE_DEFS ${MODULE_DEFS} "${OY_MODULE}=${OY_MODULE}" )
  INCLUDE_DIRECTORIES ( /Developer/Headers/FlatCarbon )
  FIND_LIBRARY(CARBON_LIBRARY Carbon)
  MARK_AS_ADVANCED (CARBON_LIBRARY)
  SET( EXTRA_LIBS_CS ${EXTRA_LIBS} ${CARBON_LIBRARY} )
  SET( SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE_NAME}_cmm_disp.c
               ${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE_NAME}_monitor.c
               ${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE_NAME}_monitor_cs.c
     )
  OY_CREATE_MODULE(ENABLE_SHARED_LIBS_${PROJECT_UP_NAME} ENABLE_STATIC_LIBS_${PROJECT_UP_NAME})

  SET( DEVICE_JSON config.icc_profile.monitor.oyX1.${OY_MODULE}.json )
  OY_HANDLE_DEVICE_JSON()
ENDIF()

IF( HAVE_LIBRAW_PLUGIN )
  SET( OY_MODULE oyRE )
  SET( MODULE_EXTRA_LIBS ${EXTRA_LIBS} ${LIBRAW_LIBRARIES} ${EXIV2_LIBRARIES} )
  SET( OY_MODULE_LIB ${PACKAGE_NAME}_${OY_MODULE}${OY_MODULE_NAME} )
  SET( SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE_NAME}_cmm_${OY_MODULE}.cpp )
  OY_CREATE_MODULE(ENABLE_SHARED_LIBS_${PROJECT_UP_NAME} ENABLE_STATIC_LIBS_${PROJECT_UP_NAME})

  SET( DEVICE_JSON config.icc_profile.raw-image.${OY_MODULE}.json )
  OY_HANDLE_DEVICE_JSON()
ENDIF()

IF( CUPS_FOUND )
  SET( OY_MODULE CUPS )
  SET( MODULE_EXTRA_LIBS ${CUPS_LIBRARIES} )
  SET( OY_MODULE_LIB ${PACKAGE_NAME}_${OY_MODULE}${OY_MODULE_NAME} )
  SET( SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE_NAME}_cmm_${OY_MODULE}.c )
  OY_CREATE_MODULE(ENABLE_SHARED_LIBS_${PROJECT_UP_NAME} ENABLE_STATIC_LIBS_${PROJECT_UP_NAME})

  SET( DEVICE_JSON config.icc_profile.printer.${OY_MODULE}.json )
  OY_HANDLE_DEVICE_JSON()
ENDIF()

# SANE
IF(SANE_FOUND)
  SET( OY_MODULE SANE )
  SET( MODULE_EXTRA_LIBS ${SANE_LIBRARY} )
  SET( OY_MODULE_LIB ${PACKAGE_NAME}_${OY_MODULE}${OY_MODULE_NAME} )
  SET( SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/${PACKAGE_NAME}_cmm_${OY_MODULE}.c )
  OY_CREATE_MODULE(ENABLE_SHARED_LIBS_${PROJECT_UP_NAME} ENABLE_STATIC_LIBS_${PROJECT_UP_NAME})

  SET( DEVICE_JSON config.icc_profile.scanner.${OY_MODULE}.json )
  OY_HANDLE_DEVICE_JSON()
ENDIF(SANE_FOUND)

IF(UNIX AND OY_MODULE_STATIC)
  ADD_CUSTOM_TARGET( generate-rank-maps-static
    "echo"
    DEPENDS ${GJ_oyX1} ${GJ_qarz} ${GJ_CUPS} ${GJ_oyRE} ${GJ_SANE}
    COMMENT "generate static compiled in rank maps from JSON." VERBATIM )
ENDIF(UNIX AND OY_MODULE_STATIC)

SET( SOURCES_STATIC ${SOURCES_STATIC} PARENT_SCOPE )
SET( DEFS_STATIC ${MODULE_DEFS} PARENT_SCOPE )
SET( OY_MODULE_STATIC ${OY_MODULE_STATIC} PARENT_SCOPE )
SET( OY_MODULE_LIBRARIES ${OY_MODULE_LIBRARIES} PARENT_SCOPE )
