# X Color Management event observer
IF( QT_FOUND AND UNIX AND NOT APPLE )
  INCLUDE( ${QT_USE_FILE} )

  SET( qcmsevents_SRCS
       qcmsevents.cpp )
  SET( qcmsevents_MOC_HEADERS
       qcmsevents.h )
  SET( qcmsevents_RCCS
       qcmsevents.qrc )
  QT4_ADD_RESOURCES( qcmsevents_RCC_SRCS ${qcmsevents_RCCS} )
  QT4_WRAP_CPP( qcmsevents_SRCS ${qcmsevents_MOC_HEADERS} )

  ADD_EXECUTABLE( qcmsevents ${qcmsevents_SRCS} ${qcmsevents_RCC_SRCS} )
  TARGET_LINK_LIBRARIES ( qcmsevents oyranos ${EXTRA_LIBS}
                 ${QT_LIBRARIES} ${XCM_LIBRARIES} )
  INSTALL (TARGETS qcmsevents DESTINATION bin)
  INSTALL (FILES qcmsevents-applet.desktop DESTINATION share/applications)
  INSTALL (FILES qcmsevents.svg DESTINATION share/icons)
ENDIF()

