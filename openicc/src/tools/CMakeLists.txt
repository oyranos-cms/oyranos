# Device listing
ADD_EXECUTABLE( openicc-device "${CMAKE_CURRENT_SOURCE_DIR}/openicc_device.c" )
TARGET_LINK_LIBRARIES( openicc-device OpenICC ${EXTRA_LIBS} )

# JSON helper
ADD_EXECUTABLE( oi-json "${CMAKE_CURRENT_SOURCE_DIR}/openicc_json.c" )
TARGET_LINK_LIBRARIES( oi-json OpenICC ${EXTRA_LIBS} )

IF( ENABLE_INSTALL_${PROJECT_UP_NAME} )
  INSTALL (TARGETS openicc-device DESTINATION ${CMAKE_INSTALL_BINDIR})
  INSTALL (TARGETS oi-json DESTINATION ${CMAKE_INSTALL_BINDIR})
  ADD_CUSTOM_TARGET( openicc-device.md ALL
    COMMAND PATH=. LANG=en LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/src/libopenicc openicc-device -X markdown > ${CMAKE_CURRENT_SOURCE_DIR}/openicc-device.md
    DEPENDS openicc-device
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Extract markdown formated manual page from tool"
  )
  ADD_CUSTOM_TARGET( openicc-device-de.md ALL
    COMMAND PATH=. LANG=de_DE.UTF8 OI_LOCALEDIR=${CMAKE_BINARY_DIR}/locale openicc-device -X markdown > ${CMAKE_CURRENT_SOURCE_DIR}/openicc-device-de.md
    DEPENDS openicc-device
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Extrahiere markdown formatiertes Handbuch von Werkzeug"
  )
ENDIF( ENABLE_INSTALL_${PROJECT_UP_NAME} )

IF(ENABLE_QT)
  ADD_SUBDIRECTORY( openicc-json-qml )
ENDIF(ENABLE_QT)
