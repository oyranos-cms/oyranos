project(grantlee_extensions)

SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_AUTOMOC ON)

FIND_PACKAGE(Grantlee5 COMPONENTS Templates REQUIRED)
FIND_PACKAGE(Qt5       COMPONENTS Core      REQUIRED)

ADD_LIBRARY(gsoc_extensions MODULE
  custom_plugin_library.cpp
  string_filters.cpp
)

SET_PROPERTY(TARGET gsoc_extensions PROPERTY EXPORT_NAME gsoc_extensions)
TARGET_LINK_LIBRARIES  ( gsoc_extensions Grantlee5::Templates )
TARGET_COMPILE_FEATURES( gsoc_extensions PRIVATE cxx_auto_type )
GRANTLEE_ADJUST_PLUGIN_NAME( gsoc_extensions )
