PROJECT(gsoc_2010)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET( CMAKE_BUILD_TYPE "Debug" )

OPTION( BUILD_TESTS "Build the test programs" FALSE )

IF(BUILD_TESTS)
  ADD_SUBDIRECTORY(tests)
ENDIF()

#FIND_PACKAGE(Qt4 COMPONENTS QtCore REQUIRED)
#INCLUDE(${QT_USE_FILE})
FIND_PACKAGE(Grantlee REQUIRED)

INCLUDE(${Grantlee_USE_FILE})

INCLUDE_DIRECTORIES(
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}
)

CONFIGURE_FILE(
  ${PROJECT_SOURCE_DIR}/grantlee_paths.h.cmake
  ${PROJECT_BINARY_DIR}/grantlee_paths.h
)

ADD_SUBDIRECTORY(grantlee_extensions)

#Setup MOC
set( MOC_HEADERS ClassInfo.h )
qt4_wrap_cpp( MOC_SOURCES ${MOC_HEADERS} )

# ... Application sources etc.
ADD_EXECUTABLE( oyAPIGenerator "main.cc" "ClassTemplates.cc" "ClassInfo.cc" ${MOC_SOURCES} )
TARGET_LINK_LIBRARIES(
   oyAPIGenerator
   ${QT_LIBRARIES}
   ${Grantlee_CORE_LIBRARIES}
)
