FIND_PACKAGE(Oyranos REQUIRED)

ENABLE_TESTING()

# Test for liboyranos_core
LINK_DIRECTORIES( "${CMAKE_BINARY_DIR}/src/liboyranos_core/" )
ADD_EXECUTABLE( test_core "${CMAKE_CURRENT_SOURCE_DIR}/test_core.c" )
TARGET_LINK_LIBRARIES(
   test_core
   oyranos_core ${EXTRA_LIBS}
)
ADD_CUSTOM_TARGET(test-core
	"./test_core"
        DEPENDS test_core
        COMMENT "Test basic linking against liboyranos_core." VERBATIM )
