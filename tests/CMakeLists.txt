PROJECT(gsoc_2010_tests)

SET( AUTO_SRC_DIR "${PROJECT_BINARY_DIR}/../API_generated/" )
SET( oyTEST_HEADERS "${AUTO_SRC_DIR}/oyTest.h" )

FIND_PACKAGE(Qt4 COMPONENTS QtCore QtTest REQUIRED)

INCLUDE(${QT_USE_FILE})

INCLUDE_DIRECTORIES(
  ${AUTO_SRC_DIR}
)

#Setup MOC
qt4_wrap_cpp( oyTEST_MOC_SOURCES ${oyTEST_HEADERS} )

# Tests for the auto-generated oyranos object system
ADD_EXECUTABLE( oyTest "${AUTO_SRC_DIR}/oyTest.cc" ${oyTEST_MOC_SOURCES} )
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
