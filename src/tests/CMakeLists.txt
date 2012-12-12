# Test for liboyranos_core
LINK_DIRECTORIES( ${LINK_DIRECTORIES} "${CMAKE_BINARY_DIR}/src/liboyranos_core/" )
ADD_EXECUTABLE( test_core "${CMAKE_CURRENT_SOURCE_DIR}/test_core.c" )
ADD_EXECUTABLE( test_core-static "${CMAKE_CURRENT_SOURCE_DIR}/test_core.c" )
TARGET_LINK_LIBRARIES( test_core oyranos_core )
TARGET_LINK_LIBRARIES( test_core-static oyranos_core-static )
ADD_CUSTOM_TARGET(test-core
	COMMAND "./test_core"
	COMMAND "./test_core-static"
        DEPENDS test_core test_core-static
        COMMENT "Test basic linking against liboyranos_core." VERBATIM )

LINK_DIRECTORIES( ${LINK_DIRECTORIES} "${CMAKE_BINARY_DIR}/src/liboyranos_object/" )
ADD_EXECUTABLE( test_object "${CMAKE_CURRENT_SOURCE_DIR}/test_object.c" )
TARGET_LINK_LIBRARIES( test_object oyranos_object )
ADD_EXECUTABLE( test_object-static "${CMAKE_CURRENT_SOURCE_DIR}/test_object.c" )
TARGET_LINK_LIBRARIES( test_object-static oyranos_object-static )
ADD_CUSTOM_TARGET(test-object
	COMMAND "./test_object"
	COMMAND "./test_object-static"
        DEPENDS test_object test_object-static test-core
        COMMENT "Test basic linking against liboyranos_object." VERBATIM )

LINK_DIRECTORIES( ${LINK_DIRECTORIES} "${CMAKE_BINARY_DIR}/src/liboyranos_modules/" )
ADD_EXECUTABLE( test_modules-static "${CMAKE_CURRENT_SOURCE_DIR}/test_modules.c" )
TARGET_LINK_LIBRARIES( test_modules-static oyranos_modules-static )
ADD_EXECUTABLE( test_modules "${CMAKE_CURRENT_SOURCE_DIR}/test_modules.c" )
TARGET_LINK_LIBRARIES( test_modules oyranos_modules )
ADD_CUSTOM_TARGET(test-modules
	COMMAND "./test_modules"
	COMMAND "./test_modules-static"
        DEPENDS test_modules test_modules-static test-object
        COMMENT "Test basic linking against liboyranos_modules." VERBATIM )

LINK_DIRECTORIES( ${LINK_DIRECTORIES} "${CMAKE_BINARY_DIR}/src/liboyranos_config/" ${ELEKTRA_LIBRARY_DIRS} )
ADD_EXECUTABLE( test_config "${CMAKE_CURRENT_SOURCE_DIR}/test_config.c" )
TARGET_LINK_LIBRARIES( test_config oyranos_config )
ADD_EXECUTABLE( test_config-static "${CMAKE_CURRENT_SOURCE_DIR}/test_config.c" )
TARGET_LINK_LIBRARIES( test_config-static oyranos_config-static )
ADD_CUSTOM_TARGET(test-config
	COMMAND "./test_config"
	COMMAND "./test_config-static"
        DEPENDS test_config test-modules
        COMMENT "Test basic linking against liboyranos_config." VERBATIM )

LINK_DIRECTORIES( ${LINK_DIRECTORIES} "${CMAKE_BINARY_DIR}/src/liboyranos/" ${ELEKTRA_LIBRARY_DIRS} )
ADD_EXECUTABLE( test2 "${CMAKE_CURRENT_SOURCE_DIR}/test2.cpp" )
TARGET_LINK_LIBRARIES( test2 oyranos )
ADD_EXECUTABLE( test2-static "${CMAKE_CURRENT_SOURCE_DIR}/test2.cpp" )
TARGET_LINK_LIBRARIES( test2-static oyranos-static )
ADD_CUSTOM_TARGET(test-2
	"echo"
        DEPENDS test-oyranos2 test-config
        COMMENT "Test liboyranos extensive." VERBATIM )
ADD_CUSTOM_COMMAND( OUTPUT test-oyranos2
                    DEPENDS test2
                    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/RunTest2.cmake )

LINK_DIRECTORIES( ${LINK_DIRECTORIES} "${CMAKE_BINARY_DIR}/src/liboyranos/" ${ELEKTRA_LIBRARY_DIRS} )
ADD_EXECUTABLE( test_oyranos "${CMAKE_CURRENT_SOURCE_DIR}/test_oyranos.c" )
TARGET_LINK_LIBRARIES( test_oyranos oyranos )
ADD_EXECUTABLE( test_oyranos-static "${CMAKE_CURRENT_SOURCE_DIR}/test_oyranos.c" )
TARGET_LINK_LIBRARIES( test_oyranos-static oyranos-static )
ADD_CUSTOM_TARGET(test
	"echo"
        DEPENDS test-oyranos test-config test2
        COMMENT "Test liboyranos." VERBATIM )

ADD_CUSTOM_COMMAND( OUTPUT test-oyranos
                    DEPENDS test_oyranos test_oyranos-static
                    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/RunTest.cmake )
