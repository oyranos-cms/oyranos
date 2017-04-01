# Test for liboyranos_core
LINK_DIRECTORIES( ${LINK_DIRECTORIES} "${CMAKE_BINARY_DIR}/src/liboyranos_core/" )
IF(ENABLE_SHARED_LIBS)
  ADD_EXECUTABLE( test_core "${CMAKE_CURRENT_SOURCE_DIR}/test_core.c" )
  TARGET_LINK_LIBRARIES( test_core OyranosCore )
  ADD_CUSTOM_TARGET(test-core
	COMMAND "${CMAKE_CURRENT_BINARY_DIR}/test_core"
        DEPENDS test_core
        COMMENT "Test basic linking against liboyranos_core." VERBATIM )
  SET( TEST2_DEPS ${TEST2_DEPS} test-core )
ENDIF(ENABLE_SHARED_LIBS)

IF(ENABLE_STATIC_LIBS)
  ADD_EXECUTABLE( test_core-static "${CMAKE_CURRENT_SOURCE_DIR}/test_core.c" )
  TARGET_LINK_LIBRARIES( test_core-static oyranos_core-static )
  ADD_CUSTOM_TARGET(test-core-static
	COMMAND "${CMAKE_CURRENT_BINARY_DIR}/test_core-static"
        DEPENDS test_core-static
        COMMENT "Test basic linking against liboyranos_core." VERBATIM )
  SET( TEST2_DEPS ${TEST2_DEPS} test-core-static )
ENDIF(ENABLE_STATIC_LIBS)


LINK_DIRECTORIES( ${LINK_DIRECTORIES} "${CMAKE_BINARY_DIR}/src/liboyranos_object/" )
IF(ENABLE_SHARED_LIBS)
  ADD_EXECUTABLE( test_object "${CMAKE_CURRENT_SOURCE_DIR}/test_object.c" )
  TARGET_LINK_LIBRARIES( test_object OyranosObject )
  ADD_CUSTOM_TARGET(test-object
	COMMAND "${CMAKE_CURRENT_BINARY_DIR}/test_object"
        DEPENDS test_object test_core
        COMMENT "Test basic linking against liboyranos_object." VERBATIM )
  SET( TEST2_DEPS ${TEST2_DEPS} test-object )
ENDIF(ENABLE_SHARED_LIBS)

IF(ENABLE_STATIC_LIBS)
  ADD_EXECUTABLE( test_object-static "${CMAKE_CURRENT_SOURCE_DIR}/test_object.c" )
  TARGET_LINK_LIBRARIES( test_object-static oyranos_object-static )
  ADD_CUSTOM_TARGET(test-object-static
	COMMAND "${CMAKE_CURRENT_BINARY_DIR}/test_object-static"
        DEPENDS test_object-static test-core-static
        COMMENT "Test basic linking against liboyranos_object." VERBATIM )
  SET( TEST2_DEPS ${TEST2_DEPS} test-object-static )
ENDIF(ENABLE_STATIC_LIBS)


LINK_DIRECTORIES( ${LINK_DIRECTORIES} "${CMAKE_BINARY_DIR}/src/liboyranos_modules/" )
IF(ENABLE_SHARED_LIBS)
  ADD_EXECUTABLE( test_modules "${CMAKE_CURRENT_SOURCE_DIR}/test_modules.c" )
  TARGET_LINK_LIBRARIES( test_modules OyranosModules )
  ADD_CUSTOM_TARGET(test-modules
	COMMAND "${CMAKE_CURRENT_BINARY_DIR}/test_modules"
        DEPENDS test_modules test_object
        COMMENT "Test basic linking against liboyranos_modules." VERBATIM )
  SET( TEST2_DEPS ${TEST2_DEPS} test_modules )
ENDIF(ENABLE_SHARED_LIBS)


LINK_DIRECTORIES( ${LINK_DIRECTORIES} "${CMAKE_BINARY_DIR}/src/liboyranos_config/" )
IF(ENABLE_SHARED_LIBS)
  ADD_EXECUTABLE( test_config "${CMAKE_CURRENT_SOURCE_DIR}/test_config.c" )
  TARGET_LINK_LIBRARIES( test_config OyranosConfig )
  ADD_CUSTOM_TARGET(test-config
	COMMAND "${CMAKE_CURRENT_BINARY_DIR}/test_config"
        DEPENDS test_config test_modules
        COMMENT "Test basic linking against liboyranos_config." VERBATIM )
  SET( TEST2_DEPS ${TEST2_DEPS} test_config )
ENDIF(ENABLE_SHARED_LIBS)


LINK_DIRECTORIES( ${LINK_DIRECTORIES} "${CMAKE_BINARY_DIR}/src/liboyranos/" )
IF(ENABLE_SHARED_LIBS)
  ADD_EXECUTABLE( test2
                "${CMAKE_CURRENT_SOURCE_DIR}/test2.cpp"
                "${CMAKE_SOURCE_DIR}/src/examples/image_display/oyranos_display_helpers.c"
                )
  TARGET_LINK_LIBRARIES( test2 Oyranos )
  SET( TEST2 test2 )
  SET( TEST2_DEPS ${TEST2_DEPS} test2 )
ENDIF(ENABLE_SHARED_LIBS)

IF(ENABLE_STATIC_LIBS)
  ADD_EXECUTABLE( test2-static
                "${CMAKE_CURRENT_SOURCE_DIR}/test2.cpp"
                "${CMAKE_SOURCE_DIR}/src/examples/image_display/oyranos_display_helpers.c"
                )
  TARGET_LINK_LIBRARIES( test2-static ${PACKAGE_NAME}-static )
  SET( TEST2_STATIC test2-static )
  SET( TEST2_DEPS ${TEST2_DEPS} test2-static )
ENDIF(ENABLE_STATIC_LIBS)
ADD_CUSTOM_TARGET(test-2
	"echo"
	DEPENDS test-oyranos2 ${TEST2} ${TEST2_STATIC}
        COMMENT "Test liboyranos extensive." VERBATIM )
ADD_CUSTOM_COMMAND( OUTPUT test-oyranos2
                    DEPENDS ${TEST2_DEPS}
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
IF(ENABLE_SHARED_LIBS)
  ADD_EXECUTABLE( test_oyranos "${CMAKE_CURRENT_SOURCE_DIR}/test_oyranos.c" )
  TARGET_LINK_LIBRARIES( test_oyranos Oyranos )
  ADD_CUSTOM_TARGET(check
	"echo"
        DEPENDS test-oyranos test2
        COMMENT "Test liboyranos." VERBATIM )
  SET( TEST2_DEPS ${TEST2_DEPS} test_oyranos )
ENDIF(ENABLE_SHARED_LIBS)

IF(ENABLE_STATIC_LIBS)
  ADD_EXECUTABLE( test_oyranos-static "${CMAKE_CURRENT_SOURCE_DIR}/test_oyranos.c" )
  TARGET_LINK_LIBRARIES( test_oyranos-static ${PACKAGE_NAME}-static )
  ADD_CUSTOM_TARGET(check-static
	"echo"
        DEPENDS test-oyranos test2-static
        COMMENT "Test liboyranos." VERBATIM )
  SET( TEST2_DEPS ${TEST2_DEPS} test_oyranos-static )
ENDIF(ENABLE_STATIC_LIBS)

ADD_CUSTOM_COMMAND( OUTPUT test-oyranos
                    DEPENDS ${TEST2_DEPS}
                    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/RunTest.cmake )

