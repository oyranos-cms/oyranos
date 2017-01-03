# X Color Management event observer
ADD_SUBDIRECTORY( qt4 )

FIND_PACKAGE(Qt5 COMPONENTS Widgets X11Extras Xml Svg)
IF(QT_FOUND OR Qt5Widgets_FOUND)
  SET( HAVE_QT 1 )
  INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIRS} )
  LINK_DIRECTORIES( ${QT_LIBRARY_DIRS} )
ENDIF(QT_FOUND OR Qt5Widgets_FOUND)
IF( HAVE_QT AND UNIX AND NOT APPLE AND Qt5Widgets_FOUND)
  SET( qcmsevents_SRCS
       qcmsevents.cpp )
  SET( qcmsevents_MOC_HEADERS
       qcmsevents.h )
  SET( qcmsevents_RCCS
       qcmsevents.qrc )
  QT5_ADD_RESOURCES( qcmsevents_RCC_SRCS ${qcmsevents_RCCS} )
  SET(CMAKE_AUTOMOC ON)
  INCLUDE_DIRECTORIES( ${Qt5Widgets_INCLUDE_DIRS} )

  ADD_EXECUTABLE( qcmsevents ${qcmsevents_SRCS} ${qcmsevents_RCC_SRCS} )
  TARGET_LINK_LIBRARIES ( qcmsevents Oyranos ${EXTRA_LIBS}
                 ${QT_LIBRARIES} ${XCM_LIBRARIES} )
  FIND_PACKAGE(PkgConfig)
  PKG_CHECK_MODULES(X11 x11)
  TARGET_LINK_LIBRARIES ( qcmsevents Oyranos ${EXTRA_LIBS}
                 Qt5::Widgets Qt5::X11Extras
                 ${XCM_LIBRARIES}
                 ${X11_LIBRARIES} )
  INSTALL( TARGETS qcmsevents
           RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} )
  INSTALL( FILES qcmsevents-applet.desktop DESTINATION ${CMAKE_INSTALL_FULL_DATAROOTDIR}/applications )
  INSTALL( FILES qcmsevents.svg DESTINATION ${CMAKE_INSTALL_FULL_DATAROOTDIR}/pixmaps )
ENDIF()
