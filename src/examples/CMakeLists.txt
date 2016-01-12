# example filter module
SET( OY_MODULE dFil )
SET( OY_MODULE_LIB ${PACKAGE_NAME}_${OY_MODULE}${OY_MODULE_NAME} )
ADD_LIBRARY( ${OY_MODULE_LIB}
             SHARED ${PACKAGE_NAME}_cmm_${OY_MODULE}.c )
TARGET_LINK_LIBRARIES ( ${OY_MODULE_LIB} ${PROJECT_NAME} ${EXTRA_LIBS} )
#INSTALL (TARGETS ${OY_MODULE_LIB} DESTINATION lib${LIB_SUFFIX}/${OY_CMMSUBPATH})

# example device module
SET( OY_MODULE dDev )
SET( OY_MODULE_LIB ${PACKAGE_NAME}_${OY_MODULE}${OY_MODULE_NAME} )
ADD_LIBRARY( ${OY_MODULE_LIB}
             SHARED ${PACKAGE_NAME}_cmm_${OY_MODULE}.c )
TARGET_LINK_LIBRARIES ( ${OY_MODULE_LIB} ${PROJECT_NAME} ${EXTRA_LIBS} )
#INSTALL (TARGETS ${OY_MODULE_LIB} DESTINATION lib${LIB_SUFFIX}/${OY_CMMSUBPATH})

# ICC profile hash calculation
ADD_EXECUTABLE( oymd5icc oymd5icc.c )
TARGET_LINK_LIBRARIES( oymd5icc ${PROJECT_NAME} ${EXTRA_LIBS} )

# Device listing
ADD_EXECUTABLE( oyranos-test-device "${CMAKE_CURRENT_SOURCE_DIR}/devices/test_device.c" )
TARGET_LINK_LIBRARIES( oyranos-test-device ${PROJECT_NAME} ${EXTRA_LIBS} )
#INSTALL (TARGETS oyranos-test-device DESTINATION bin)

# CamerRaw file handling
ADD_EXECUTABLE( oyranos-file "${CMAKE_CURRENT_SOURCE_DIR}/libraw/oyranos_file.cpp" )
TARGET_LINK_LIBRARIES( oyranos-file ${PROJECT_NAME} ${EXTRA_LIBS} )
#INSTALL (TARGETS oyranos-file DESTINATION bin)

# Taxi DB tool for central hosted ICC profile distribution
ADD_EXECUTABLE( taxi taxi.c )
TARGET_LINK_LIBRARIES( taxi ${PROJECT_NAME} ${EXTRA_LIBS} )
#INSTALL (TARGETS taxi DESTINATION bin)

# primitive picture montage with ICC support
IF( CAIRO_FOUND )
  ADD_EXECUTABLE( image2pdf image2pdf.c )
  TARGET_LINK_LIBRARIES ( image2pdf ${PROJECT_NAME} ${EXTRA_LIBS}
                 ${CAIRO_LIBRARIES} )
ENDIF()

# simple image viewer
IF( FLTK_FOUND )
  # OS X: fltk-1.3.3 with Cocoa is assumed
  # Remove incompatible settings.
  IF(${CMAKE_CXX_FLAGS} MATCHES "-arch i386")
    STRING(REGEX REPLACE
      "-arch i386(.+)" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}"
    )
  ENDIF()

  IF(${CMAKE_EXE_LINKER_FLAGS} MATCHES "-arch i386")
    STRING(REGEX REPLACE
      "-arch i386(.+)" "" CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}"
    )
  ENDIF()

  ADD_EXECUTABLE( oyranos-image-display 
                  "${CMAKE_CURRENT_SOURCE_DIR}/image_display/image_display.cpp"
                  "${CMAKE_CURRENT_SOURCE_DIR}/image_display/GLee.c"
                )
  SET_PROPERTY(TARGET oyranos-image-display PROPERTY OSX_ARCHITECTURES x86_64)
  SET_PROPERTY(TARGET oyranos-image-display PROPERTY OSX_ARCHITECTURES_DEBUG x86_64)
  TARGET_LINK_LIBRARIES ( oyranos-image-display ${PROJECT_NAME} ${EXTRA_LIBS}
                 ${FLTK_LIBRARIES} ${XRANDR_LIBRARIES} )
  INSTALL (TARGETS oyranos-image-display DESTINATION bin)
  INSTALL( FILES ${CMAKE_CURRENT_SOURCE_DIR}/image_display/oyranos-image-display.desktop DESTINATION share/applications )
  INSTALL( FILES ${CMAKE_CURRENT_SOURCE_DIR}/image_display/oyranos-image-display.1 DESTINATION share/man/man1 )

  ADD_EXECUTABLE( oyranos-config-fltk
                  oyranos_config_fltk.cxx
                  ../../FL/Flmm_Tabs.cxx
                )
  SET_PROPERTY(TARGET oyranos-config-fltk PROPERTY OSX_ARCHITECTURES x86_64)
  SET_PROPERTY(TARGET oyranos-config-fltk PROPERTY OSX_ARCHITECTURES_DEBUG x86_64)
  TARGET_LINK_LIBRARIES ( oyranos-config-fltk ${PROJECT_NAME} ${EXTRA_LIBS}
                 ${FLTK_LIBRARIES} ${XRANDR_LIBRARIES} )
  INSTALL (TARGETS oyranos-config-fltk DESTINATION bin)
ENDIF()


ADD_SUBDIRECTORY( oforms )

# oFORMS for a Oyranos CLI renderer and parser
ADD_EXECUTABLE( oyranos-xforms oforms/oyranos_xforms.c )
TARGET_LINK_LIBRARIES( oyranos-xforms
                  ${OY_OFORMS_LIB} ${PROJECT_NAME} ${EXTRA_LIBS} )
INSTALL (TARGETS oyranos-xforms DESTINATION bin)

# oFORMS for a Oyranos FLTK renderer and parser
IF( FLTK_FOUND )
  ADD_EXECUTABLE( oyranos-xforms-fltk
                  oforms/oyranos_forms_fltk.cxx
                  oforms/oyranos_widgets_fltk.cxx
                  oforms/oyranos_xforms_fltk.cxx
                )
  TARGET_LINK_LIBRARIES ( oyranos-xforms-fltk ${PROJECT_NAME} ${EXTRA_LIBS}
                  ${OY_OFORMS_LIB} ${FLTK_LIBRARIES} ${XRANDR_LIBRARIES} )
  INSTALL (TARGETS oyranos-xforms-fltk DESTINATION bin)
ENDIF()

# oFORMS for a Oyranos module generation
ADD_EXECUTABLE( oyranos-xforms-modules oforms/oyranos_xforms_modules.c )
TARGET_LINK_LIBRARIES( oyranos-xforms-modules
             ${PROJECT_NAME} ${OY_OFORMS_LIB} ${EXTRA_LIBS} )
INSTALL (TARGETS oyranos-xforms-modules DESTINATION bin)

