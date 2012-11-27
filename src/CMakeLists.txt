INCLUDE_DIRECTORIES(
                    "${CMAKE_SOURCE_DIR}/src/include"
                    "${CMAKE_BINARY_DIR}/src/include"
                    "${CMAKE_SOURCE_DIR}/src/include_private"
                    "${CMAKE_BINARY_DIR}/src/include_private"
                    "${CMAKE_SOURCE_DIR}/src/liboyranos_core"
                    "${CMAKE_SOURCE_DIR}/src/liboyranos_object"
                    "${CMAKE_SOURCE_DIR}/src/liboyranos_modules"
                    "${CMAKE_SOURCE_DIR}/src/liboyranos_config"
                    "${CMAKE_SOURCE_DIR}/src/liboyranos"
                    "${CMAKE_SOURCE_DIR}/src/API_generated"
)


INCLUDE(CheckLibraryExists)
CHECK_LIBRARY_EXISTS(m pow "math.h" HAVE_M)

IF(HAVE_M)
  SET( EXTRA_LIBS ${EXTRA_LIBS} m )
ENDIF(HAVE_M)

ADD_SUBDIRECTORY (include)

ADD_SUBDIRECTORY (include_private)

ADD_SUBDIRECTORY (liboyranos_core)

ADD_SUBDIRECTORY (tests)

ADD_CUSTOM_COMMAND( OUTPUT generator/oyAPIGenerator
                COMMAND cd generator/ && CMAKE_PREFIX_PATH="${CMAKE_CURRENT_BINARY_DIR}/generator/grantlee/lib/cmake/grantlee/" cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS=-g -Wall ${CMAKE_CURRENT_SOURCE_DIR}/generator && make || echo "Did you run ${CMAKE_CURRENT_SOURCE_DIR}/generator/download_grantlee.sh in ${CMAKE_CURRENT_BINARY_DIR}/generator/ ?"
		)

ADD_CUSTOM_TARGET( generate
		COMMAND cd ${CMAKE_CURRENT_BINARY_DIR}/generator/ && ./oyAPIGenerator ${CMAKE_CURRENT_SOURCE_DIR}/templates:${CMAKE_CURRENT_SOURCE_DIR}/sources:${CMAKE_CURRENT_SOURCE_DIR}/API_generated
		COMMENT "Generate source code from templates"
		VERBATIM
                DEPENDS generator/oyAPIGenerator
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/generator" )

