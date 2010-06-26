PROJECT(gsoc_2010_tests)

FIND_PACKAGE(Qt4 COMPONENTS QtCore QtTest REQUIRED)

INCLUDE(${QT_USE_FILE})

INCLUDE_DIRECTORIES(
  ${PROJECT_BINARY_DIR}
)


#Setup MOC
set( oyTEST_MOC_HEADERS oyTest.h )
qt4_wrap_cpp( oyTEST_MOC_SOURCES ${oyTEST_MOC_HEADERS} )

# Tests for the auto-generated oyranos object system
ADD_EXECUTABLE( oyTest "oyTest.cc" ${oyTEST_MOC_SOURCES} )
TARGET_LINK_LIBRARIES(
   oyTest
   ${QT_LIBRARIES}
   ${oyranos_object_LIBRARIES}
)

# A Qt Regular Expression testing helper program
ADD_EXECUTABLE( qregexp "regexp.cc" )
TARGET_LINK_LIBRARIES(
   qregexp
   ${QT_LIBRARIES}
)
