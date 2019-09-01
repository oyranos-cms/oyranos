# Device listing
ADD_EXECUTABLE( openicc-device "${CMAKE_CURRENT_SOURCE_DIR}/openicc_device.c" )
TARGET_LINK_LIBRARIES( openicc-device OpenICC ${EXTRA_LIBS} )

IF( ENABLE_INSTALL_${PROJECT_UP_NAME} )
  INSTALL (TARGETS openicc-device DESTINATION ${CMAKE_INSTALL_BINDIR})
  IF( NOT HAVE_OYJL )
    INCLUDE( "${CMAKE_BINARY_DIR}/oyjl/cmake/OyjlConfig.cmake" )
    UNSET( HAVE_OYJL )
  ENDIF()
  OYJL_DOCUMENT_TOOL( openicc-device "${CMAKE_CURRENT_SOURCE_DIR}/openicc_device.c" "de_DE.UTF8" "${CMAKE_SOURCE_DIR}/docs" )
ENDIF( ENABLE_INSTALL_${PROJECT_UP_NAME} )
