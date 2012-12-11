# example filter module
SET( OY_MODULE dFil )
SET( OY_MODULE_LIB ${PACKAGE_NAME}_${OY_MODULE}${OY_MODULE_NAME} )
ADD_LIBRARY( ${OY_MODULE_LIB}
             SHARED ${PACKAGE_NAME}_cmm_${OY_MODULE}.c )
TARGET_LINK_LIBRARIES ( ${OY_MODULE_LIB} oyranos ${EXTRA_LIBS} )
#INSTALL (TARGETS ${OY_MODULE_LIB} DESTINATION lib${LIB_SUFFIX}/${OY_CMMSUBPATH})

# example device module
SET( OY_MODULE dDev )
SET( OY_MODULE_LIB ${PACKAGE_NAME}_${OY_MODULE}${OY_MODULE_NAME} )
ADD_LIBRARY( ${OY_MODULE_LIB}
             SHARED ${PACKAGE_NAME}_cmm_${OY_MODULE}.c )
TARGET_LINK_LIBRARIES ( ${OY_MODULE_LIB} oyranos ${EXTRA_LIBS} )
#INSTALL (TARGETS ${OY_MODULE_LIB} DESTINATION lib${LIB_SUFFIX}/${OY_CMMSUBPATH})

# ICC profile hash calculation
ADD_EXECUTABLE( oymd5icc oymd5icc.c )
TARGET_LINK_LIBRARIES( oymd5icc oyranos ${EXTRA_LIBS} )
#INSTALL (TARGETS oymd5icc DESTINATION bin)

# Device listing
ADD_EXECUTABLE( oyranos-test-device "${CMAKE_CURRENT_SOURCE_DIR}/devices/test_device.c" )
TARGET_LINK_LIBRARIES( oyranos-test-device oyranos ${EXTRA_LIBS} )
#INSTALL (TARGETS oyranos-test-device DESTINATION bin)

# CamerRaw file handling
ADD_EXECUTABLE( oyranos-file "${CMAKE_CURRENT_SOURCE_DIR}/libraw/oyranos_file.cpp" )
TARGET_LINK_LIBRARIES( oyranos-file oyranos ${EXTRA_LIBS} )
#INSTALL (TARGETS oyranos-file DESTINATION bin)

# Taxi DB tool for central hosted ICC profile distribution
ADD_EXECUTABLE( taxi taxi.c )
TARGET_LINK_LIBRARIES( taxi oyranos ${EXTRA_LIBS} )
#INSTALL (TARGETS taxi DESTINATION bin)

# primitive picture montage with ICC support
IF( CAIRO_FOUND )
  ADD_EXECUTABLE( image2pdf image2pdf.c )
  TARGET_LINK_LIBRARIES ( image2pdf oyranos ${EXTRA_LIBS}
                 ${CAIRO_LIBRARIES} )
  #INSTALL (TARGETS oyranos-profile-graph DESTINATION bin)
ENDIF()

# simple image viewer
IF( FLTK_FOUND )
  ADD_EXECUTABLE( image-display 
                  "${CMAKE_CURRENT_SOURCE_DIR}/image_display/image_display.cpp"
                  "${CMAKE_CURRENT_SOURCE_DIR}/image_display/GLee.c"
                )
  TARGET_LINK_LIBRARIES ( image-display oyranos ${EXTRA_LIBS}
                 ${FLTK_LIBRARIES} )
  #INSTALL (TARGETS image-display DESTINATION bin)
ENDIF()

ADD_SUBDIRECTORY( oforms )
