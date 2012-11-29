# Test for liboyranos_core
LINK_DIRECTORIES( ${LINK_DIRECTORIES} "${CMAKE_BINARY_DIR}/src/liboyranos_core/" )
ADD_EXECUTABLE( test_core "${CMAKE_CURRENT_SOURCE_DIR}/test_core.c" )
TARGET_LINK_LIBRARIES( test_core oyranos_core )
ADD_CUSTOM_TARGET(test-core
	"./test_core"
        DEPENDS test_core
        COMMENT "Test basic linking against liboyranos_core." VERBATIM )

LINK_DIRECTORIES( ${LINK_DIRECTORIES} "${CMAKE_BINARY_DIR}/src/liboyranos_object/" )
ADD_EXECUTABLE( test_object "${CMAKE_CURRENT_SOURCE_DIR}/test_object.c" )
TARGET_LINK_LIBRARIES( test_object oyranos_object )
ADD_CUSTOM_TARGET(test-object
	"./test_object"
        DEPENDS test_object test-core
        COMMENT "Test basic linking against liboyranos_object." VERBATIM )

LINK_DIRECTORIES( ${LINK_DIRECTORIES} "${CMAKE_BINARY_DIR}/src/liboyranos_modules/" )
ADD_EXECUTABLE( test_modules "${CMAKE_CURRENT_SOURCE_DIR}/test_modules.c" )
TARGET_LINK_LIBRARIES( test_modules oyranos_modules )
ADD_CUSTOM_TARGET(test-modules
	"./test_modules"
        DEPENDS test_modules test-object
        COMMENT "Test basic linking against liboyranos_modules." VERBATIM )

LINK_DIRECTORIES( ${LINK_DIRECTORIES} "${CMAKE_BINARY_DIR}/src/liboyranos_config/" ${ELEKTRA_LIBRARY_DIRS} )
ADD_EXECUTABLE( test_config "${CMAKE_CURRENT_SOURCE_DIR}/test_config.c" )
TARGET_LINK_LIBRARIES( test_config oyranos_config )
ADD_CUSTOM_TARGET(test-config
	"./test_config"
        DEPENDS test_config test-modules
        COMMENT "Test basic linking against liboyranos_config." VERBATIM )

LINK_DIRECTORIES( ${LINK_DIRECTORIES} "${CMAKE_BINARY_DIR}/src/liboyranos/" ${ELEKTRA_LIBRARY_DIRS} )
ADD_EXECUTABLE( test2 "${CMAKE_CURRENT_SOURCE_DIR}/test2.cpp" )
TARGET_LINK_LIBRARIES( test2 oyranos )
SET(TEST2 1)
ADD_CUSTOM_TARGET(test-2
	"echo"
        DEPENDS test-oyranos2 test-config
        COMMENT "Test liboyranos extensive." VERBATIM )
ADD_CUSTOM_COMMAND( OUTPUT test-oyranos2
                    DEPENDS test2
                    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/RunTest2.cmake )
SET(TEST2 0)

LINK_DIRECTORIES( ${LINK_DIRECTORIES} "${CMAKE_BINARY_DIR}/src/liboyranos/" ${ELEKTRA_LIBRARY_DIRS} )
ADD_EXECUTABLE( test_oyranos "${CMAKE_CURRENT_SOURCE_DIR}/test_oyranos.c" )
TARGET_LINK_LIBRARIES( test_oyranos oyranos )
ADD_CUSTOM_TARGET(test
	"echo"
        DEPENDS test-oyranos test2
        COMMENT "Test liboyranos." VERBATIM )

ADD_CUSTOM_COMMAND( OUTPUT test-oyranos
                    DEPENDS test_oyranos
                    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/RunTest.cmake )
