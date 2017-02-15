# Test for liboyranos_core
LINK_DIRECTORIES( ${LINK_DIRECTORIES} "${CMAKE_BINARY_DIR}/src/liboyranos_core/" )
ADD_EXECUTABLE( test_core "${CMAKE_CURRENT_SOURCE_DIR}/test_core.c" )
ADD_EXECUTABLE( test_core-static "${CMAKE_CURRENT_SOURCE_DIR}/test_core.c" )
TARGET_LINK_LIBRARIES( test_core OyranosCore )
TARGET_LINK_LIBRARIES( test_core-static oyranos_core-static )
ADD_CUSTOM_TARGET(test-core
	COMMAND "${CMAKE_CURRENT_BINARY_DIR}/test_core"
	COMMAND "${CMAKE_CURRENT_BINARY_DIR}/test_core-static"
        DEPENDS test_core test_core-static
        COMMENT "Test basic linking against liboyranos_core." VERBATIM )

LINK_DIRECTORIES( ${LINK_DIRECTORIES} "${CMAKE_BINARY_DIR}/src/liboyranos_object/" )
ADD_EXECUTABLE( test_object "${CMAKE_CURRENT_SOURCE_DIR}/test_object.c" )
TARGET_LINK_LIBRARIES( test_object OyranosObject )
ADD_EXECUTABLE( test_object-static "${CMAKE_CURRENT_SOURCE_DIR}/test_object.c" )
TARGET_LINK_LIBRARIES( test_object-static oyranos_object-static )
ADD_CUSTOM_TARGET(test-object
	COMMAND "${CMAKE_CURRENT_BINARY_DIR}/test_object"
	COMMAND "${CMAKE_CURRENT_BINARY_DIR}/test_object-static"
        DEPENDS test_object test_object-static test_core
        COMMENT "Test basic linking against liboyranos_object." VERBATIM )

LINK_DIRECTORIES( ${LINK_DIRECTORIES} "${CMAKE_BINARY_DIR}/src/liboyranos_modules/" )
ADD_EXECUTABLE( test_modules-static "${CMAKE_CURRENT_SOURCE_DIR}/test_modules.c" )
TARGET_LINK_LIBRARIES( test_modules-static oyranos_modules-static )
ADD_EXECUTABLE( test_modules "${CMAKE_CURRENT_SOURCE_DIR}/test_modules.c" )
TARGET_LINK_LIBRARIES( test_modules OyranosModules )
ADD_CUSTOM_TARGET(test-modules
	COMMAND "${CMAKE_CURRENT_BINARY_DIR}/test_modules"
	COMMAND "${CMAKE_CURRENT_BINARY_DIR}/test_modules-static"
        DEPENDS test_modules test_modules-static test_object
        COMMENT "Test basic linking against liboyranos_modules." VERBATIM )

LINK_DIRECTORIES( ${LINK_DIRECTORIES} "${CMAKE_BINARY_DIR}/src/liboyranos_config/" )
ADD_EXECUTABLE( test_config "${CMAKE_CURRENT_SOURCE_DIR}/test_config.c" )
TARGET_LINK_LIBRARIES( test_config OyranosConfig )
ADD_EXECUTABLE( test_config-static "${CMAKE_CURRENT_SOURCE_DIR}/test_config.c" )
TARGET_LINK_LIBRARIES( test_config-static oyranos_config-static )
ADD_CUSTOM_TARGET(test-config
	COMMAND "${CMAKE_CURRENT_BINARY_DIR}/test_config"
	COMMAND "${CMAKE_CURRENT_BINARY_DIR}/test_config-static"
        DEPENDS test_config test_modules
        COMMENT "Test basic linking against liboyranos_config." VERBATIM )

LINK_DIRECTORIES( ${LINK_DIRECTORIES} "${CMAKE_BINARY_DIR}/src/liboyranos/" )
ADD_EXECUTABLE( test2
                "${CMAKE_CURRENT_SOURCE_DIR}/test2.cpp"
                "${CMAKE_SOURCE_DIR}/src/examples/image_display/oyranos_display_helpers.c"
              )
TARGET_LINK_LIBRARIES( test2 Oyranos )
ADD_EXECUTABLE( test2-static
                "${CMAKE_CURRENT_SOURCE_DIR}/test2.cpp"
                "${CMAKE_SOURCE_DIR}/src/examples/image_display/oyranos_display_helpers.c"
              )
TARGET_LINK_LIBRARIES( test2-static oyranos-static )
ADD_CUSTOM_TARGET(test-2
	"echo"
        DEPENDS test-oyranos2 test_config
        COMMENT "Test liboyranos extensive." VERBATIM )
ADD_CUSTOM_COMMAND( OUTPUT test-oyranos2
                    DEPENDS test2
                    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/RunTest2.cmake )

IF(USE_GCOV)
  MESSAGE("testing coverage using gcc's gcov; try target: coverage")
  INCLUDE(CodeCoverage)
  SETUP_TARGET_FOR_COVERAGE(
				coverage  # Name for custom target.
				test2         # Name of the test driver executable that runs the tests.
									# NOTE! This should always have a ZERO as exit code
									# otherwise the coverage generation will not complete.
				coverage            # Name of output directory.
			)
ENDIF(USE_GCOV)


LINK_DIRECTORIES( ${LINK_DIRECTORIES} "${CMAKE_BINARY_DIR}/src/liboyranos/" )
ADD_EXECUTABLE( test_oyranos "${CMAKE_CURRENT_SOURCE_DIR}/test_oyranos.c" )
TARGET_LINK_LIBRARIES( test_oyranos Oyranos )
ADD_EXECUTABLE( test_oyranos-static "${CMAKE_CURRENT_SOURCE_DIR}/test_oyranos.c" )
TARGET_LINK_LIBRARIES( test_oyranos-static oyranos-static )
ADD_CUSTOM_TARGET(check
	"echo"
        DEPENDS test-oyranos test2 test2-static
        COMMENT "Test liboyranos." VERBATIM )

ADD_CUSTOM_COMMAND( OUTPUT test-oyranos
                    DEPENDS test_oyranos test_oyranos-static test_config test_config-static test_modules-static test_modules test_object-static test_object test_core-static test_core
                    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/RunTest.cmake )
