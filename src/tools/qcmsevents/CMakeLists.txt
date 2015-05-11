# X Color Management event observer
IF( HAVE_QT AND UNIX AND NOT APPLE )
  IF(QT4_FOUND)
    INCLUDE(   ${QT_USE_FILE}   )
  ENDIF()

  SET( qcmsevents_SRCS
       qcmsevents.cpp )
  SET( qcmsevents_MOC_HEADERS
       qcmsevents.h )
  SET( qcmsevents_RCCS
       qcmsevents.qrc )
  IF(QT4_FOUND)
    QT4_ADD_RESOURCES( qcmsevents_RCC_SRCS ${qcmsevents_RCCS} )
    QT4_WRAP_CPP( qcmsevents_SRCS ${qcmsevents_MOC_HEADERS} )
  ENDIF()
  IF(Qt5Widgets_FOUND)
    QT5_ADD_RESOURCES( qcmsevents_RCC_SRCS ${qcmsevents_RCCS} )
    SET(CMAKE_AUTOMOC ON)
    INCLUDE_DIRECTORIES( ${Qt5Widgets_INCLUDE_DIRS} )
  ENDIF()

  ADD_EXECUTABLE( qcmsevents ${qcmsevents_SRCS} ${qcmsevents_RCC_SRCS} )
  TARGET_LINK_LIBRARIES ( qcmsevents Oyranos ${EXTRA_LIBS}
                 ${QT_LIBRARIES} ${XCM_LIBRARIES} )
  IF(Qt5Widgets_FOUND)
    TARGET_LINK_LIBRARIES ( qcmsevents Oyranos ${EXTRA_LIBS}
                 Qt5::Widgets Qt5::X11Extras ${XCM_LIBRARIES} )
  ENDIF()
  INSTALL( TARGETS qcmsevents
           RUNTIME DESTINATION bin )
  INSTALL( FILES qcmsevents-applet.desktop DESTINATION share/applications )
  INSTALL( FILES qcmsevents.svg DESTINATION share/pixmaps )
ENDIF()

