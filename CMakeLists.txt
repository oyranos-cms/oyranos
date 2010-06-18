PROJECT(gsoc_2010)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

FIND_PACKAGE(Qt4 COMPONENTS QtCore REQUIRED)
FIND_PACKAGE(Grantlee REQUIRED)

INCLUDE(${QT_USE_FILE})

INCLUDE_DIRECTORIES(
  ${Grantlee_INCLUDE_DIR}
  ${PROJECT_BINARY_DIR}
)

CONFIGURE_FILE(
  ${PROJECT_SOURCE_DIR}/grantlee_paths.h.cmake
  ${PROJECT_BINARY_DIR}/grantlee_paths.h
)

#Setup MOC
set( MOC_HEADERS ClassInfo.h )
qt4_wrap_cpp( MOC_SOURCES ${MOC_HEADERS} )

# ... Application sources etc.
ADD_EXECUTABLE( oyAPIGenerator "main.cc" "ClassTemplates.cc" "ClassInfo.cc" ${MOC_TEST_SOURCES} )
TARGET_LINK_LIBRARIES(
   oyAPIGenerator
   ${QT_LIBRARIES}
   ${Grantlee_CORE_LIBRARIES}
)
