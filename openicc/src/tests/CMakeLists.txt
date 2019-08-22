LINK_DIRECTORIES( ${LINK_DIRECTORIES} "${CMAKE_BINARY_DIR}/src/liboyranos/" ${ELEKTRA_LIBRARY_DIRS} )
ADD_EXECUTABLE( test-openicc "${CMAKE_CURRENT_SOURCE_DIR}/test.c" )
TARGET_LINK_LIBRARIES( test-openicc OpenICC )
ADD_EXECUTABLE( test-openicc-static "${CMAKE_CURRENT_SOURCE_DIR}/test.c" )
TARGET_LINK_LIBRARIES( test-openicc-static openicc-static )
ADD_CUSTOM_TARGET(check
        "echo"
        DEPENDS test_openicc
        COMMENT "Test libOpenICC" VERBATIM )

FIND_PROGRAM(VALGRIND_EXECUTABLE valgrind)
IF( VALGRIND_EXECUTABLE )
  SET( MEMCHECK ${VALGRIND_EXECUTABLE} --leak-check=full )
ENDIF( VALGRIND_EXECUTABLE )
ADD_CUSTOM_COMMAND( OUTPUT test_openicc
                    DEPENDS test-openicc-static
                    COMMAND OI_LOCALEDIR=${CMAKE_BINARY_DIR}/locale OI_DEBUG=1 ${MEMCHECK} ./test-openicc-static )

IF(USE_GCOV)
  MESSAGE("testing coverage using gcc's gcov; try target: coverage")
  INCLUDE(CodeCoverage)
  SETUP_TARGET_FOR_COVERAGE(
				coverage  # Name for custom target.
				test-openicc-static # Name of the test driver executable that runs the tests.
									# NOTE! This should always have a ZERO as exit code
									# otherwise the coverage generation will not complete.
				coverage            # Name of output directory.
			)
ENDIF(USE_GCOV)


