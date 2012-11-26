include_directories(
                    "${CMAKE_SOURCE_DIR}/src/include"
                    "${CMAKE_BINARY_DIR}/src/include"
                    "${CMAKE_SOURCE_DIR}/src/include_private"
                    "${CMAKE_BINARY_DIR}/src/include_private"
                    "${CMAKE_SOURCE_DIR}/src/liboyranos_core"
                    "${CMAKE_SOURCE_DIR}/src/liboyranos_object"
                    "${CMAKE_SOURCE_DIR}/src/liboyranos_object/API_generated"
                    "${CMAKE_SOURCE_DIR}/src/liboyranos_modules"
                    "${CMAKE_SOURCE_DIR}/src/liboyranos_config"
                    "${CMAKE_SOURCE_DIR}/src/liboyranos"
)


include(CheckLibraryExists)
check_library_exists(m pow "math.h" HAVE_M)

if(HAVE_M)
  set( EXTRA_LIBS ${EXTRA_LIBS} m )
endif(HAVE_M)

add_subdirectory (include)

add_subdirectory (include_private)

add_subdirectory (liboyranos_core)

add_subdirectory (tests)

