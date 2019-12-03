PROJECT(gsoc_2010)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET( CMAKE_BUILD_TYPE "Debug" )

OPTION( BUILD_TESTS "Build the test programs" FALSE )

IF(BUILD_TESTS)
  ADD_SUBDIRECTORY(tests)
ENDIF()

FIND_PACKAGE(Grantlee5 COMPONENTS Templates REQUIRED)

INCLUDE_DIRECTORIES(
  ${PROJECT_BINARY_DIR}
  ${PROJECT_SOURCE_DIR}
)

CONFIGURE_FILE(
  ${PROJECT_SOURCE_DIR}/config.h.cmake
  ${PROJECT_BINARY_DIR}/config.h
)

ADD_SUBDIRECTORY(grantlee_extensions)

#Setup MOC
SET(CMAKE_AUTOMOC ON)

# ... Application sources etc.
SET( GeneratorSrc "main.cc" "ClassTemplates.cc" "ClassInfo.cc" "FuncInfo.cc" "ClassGenerator.cc" )
ADD_EXECUTABLE( oyAPIGenerator ${GeneratorSrc} ${MOC_SOURCES} )
TARGET_LINK_LIBRARIES(
   oyAPIGenerator
   Grantlee5::Templates
)
