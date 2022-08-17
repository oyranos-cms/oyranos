# example filter module
SET( OY_MODULE dFil )
SET( OY_MODULE_LIB ${PACKAGE_NAME}_${OY_MODULE}${OY_MODULE_NAME} )
ADD_LIBRARY( ${OY_MODULE_LIB}
             SHARED ${PACKAGE_NAME}_cmm_${OY_MODULE}.c )
TARGET_LINK_LIBRARIES ( ${OY_MODULE_LIB} ${TARGET_LIB} ${EXTRA_LIBS} )
#INSTALL (TARGETS ${OY_MODULE_LIB} DESTINATION ${CMAKE_INSTALL_LIBDIR}/${OY_CMMSUBPATH})

# example device module
SET( OY_MODULE dDev )
SET( OY_MODULE_LIB ${PACKAGE_NAME}_${OY_MODULE}${OY_MODULE_NAME} )
ADD_LIBRARY( ${OY_MODULE_LIB}
             SHARED ${PACKAGE_NAME}_cmm_${OY_MODULE}.c )
TARGET_LINK_LIBRARIES ( ${OY_MODULE_LIB} ${TARGET_LIB} ${EXTRA_LIBS} )
#INSTALL (TARGETS ${OY_MODULE_LIB} DESTINATION ${CMAKE_INSTALL_LIBDIR}/${OY_CMMSUBPATH})

# ICC profile hash calculation
ADD_EXECUTABLE( oymd5icc oymd5icc.c )
TARGET_LINK_LIBRARIES( oymd5icc ${TARGET_LIB} ${EXTRA_LIBS} )

# CamerRaw file handling
ADD_EXECUTABLE( oyranos-file "${CMAKE_CURRENT_SOURCE_DIR}/libraw/oyranos_file.cpp" )
TARGET_LINK_LIBRARIES( oyranos-file ${TARGET_LIB} ${EXTRA_LIBS} )
#INSTALL (TARGETS oyranos-file DESTINATION ${CMAKE_INSTALL_BINDIR})

# Taxi DB tool for central hosted ICC profile distribution
ADD_EXECUTABLE( taxi taxi.c )
TARGET_LINK_LIBRARIES( taxi ${TARGET_LIB} ${EXTRA_LIBS} )
#INSTALL (TARGETS taxi DESTINATION ${CMAKE_INSTALL_BINDIR})

# primitive picture montage with ICC support
IF( CAIRO_FOUND )
  ADD_EXECUTABLE( image2pdf image2pdf.c )
  TARGET_LINK_LIBRARIES ( image2pdf ${TARGET_LIB} ${EXTRA_LIBS}
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

  IF(HAVE_GLU_H OR OPENGL_glu_LIBRARY)
    ADD_EXECUTABLE( oyranos-image-display 
                  "${CMAKE_CURRENT_SOURCE_DIR}/image_display/image_display.cpp"
                  "${CMAKE_CURRENT_SOURCE_DIR}/image_display/GLee.c"
                )
    SET_PROPERTY(TARGET oyranos-image-display PROPERTY OSX_ARCHITECTURES x86_64)
    SET_PROPERTY(TARGET oyranos-image-display PROPERTY OSX_ARCHITECTURES_DEBUG x86_64)
    TARGET_LINK_LIBRARIES ( oyranos-image-display ${TARGET_LIB} ${EXTRA_LIBS}
                 ${FLTK_LIBRARIES} ${XRANDR_LIBRARIES} )
    INSTALL (TARGETS oyranos-image-display DESTINATION ${CMAKE_INSTALL_BINDIR})
    INSTALL( FILES ${CMAKE_CURRENT_SOURCE_DIR}/image_display/oyranos-image-display.desktop DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/applications )
    INSTALL( FILES ${CMAKE_CURRENT_SOURCE_DIR}/image_display/oyranos-image-display.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1 )
  ENDIF()

  ADD_EXECUTABLE( oyranos-config-fltk
                  oyranos_config_fltk.cxx
                  ../../FL/Flmm_Tabs.cxx
                )
  SET_PROPERTY(TARGET oyranos-config-fltk PROPERTY OSX_ARCHITECTURES x86_64)
  SET_PROPERTY(TARGET oyranos-config-fltk PROPERTY OSX_ARCHITECTURES_DEBUG x86_64)
  TARGET_LINK_LIBRARIES ( oyranos-config-fltk ${TARGET_LIB} ${EXTRA_LIBS}
                          ${FLTK_LIBRARIES} ${XRANDR_LIBRARIES} ${DBUS_LIBS} )
  INSTALL (TARGETS oyranos-config-fltk DESTINATION ${CMAKE_INSTALL_BINDIR})
ENDIF()

# Tutorial 1
ADD_EXECUTABLE( tutorial1 tutorial1.c )
TARGET_LINK_LIBRARIES( tutorial1
             ${TARGET_LIB} ${EXTRA_LIBS} )
# Tutorial Taxi
ADD_EXECUTABLE( tutorial-taxi tutorial_taxi.c )
TARGET_LINK_LIBRARIES( tutorial-taxi
             ${TARGET_LIB} ${EXTRA_LIBS} )

# Tutorial JSON Options
ADD_EXECUTABLE( tutorial-json-options tutorial_json_options.c )
TARGET_LINK_LIBRARIES( tutorial-json-options
             ${TARGET_LIB} ${EXTRA_LIBS} )

IF( HAVE_LCMS2 )
# Tutorial lcm2_profiler
INCLUDE_DIRECTORIES( ../modules/color/modules/lcms2 )
ADD_EXECUTABLE( tutorial-lcm2-profiler tutorial_lcm2_profiler.c )
TARGET_LINK_LIBRARIES( tutorial-lcm2-profiler
             ${LCMS2_LIBRARIES} ${LIBM_LIBRARIES} )
ENDIF()
