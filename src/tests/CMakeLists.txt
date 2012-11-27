# Test for liboyranos_core
LINK_DIRECTORIES( "${CMAKE_BINARY_DIR}/src/liboyranos_core/" )
ADD_EXECUTABLE( test_core "${CMAKE_CURRENT_SOURCE_DIR}/test_core.c" )
TARGET_LINK_LIBRARIES( test_core oyranos_core )
ADD_CUSTOM_TARGET(test-core
	"./test_core"
        DEPENDS test_core
        COMMENT "Test basic linking against liboyranos_core." VERBATIM )

LINK_DIRECTORIES( "${CMAKE_BINARY_DIR}/src/liboyranos_object/" )
ADD_EXECUTABLE( test_object "${CMAKE_CURRENT_SOURCE_DIR}/test_object.c" )
TARGET_LINK_LIBRARIES( test_object oyranos_object )
ADD_CUSTOM_TARGET(test-object
	"./test_object"
        DEPENDS test_object
        COMMENT "Test basic linking against liboyranos_object." VERBATIM )
