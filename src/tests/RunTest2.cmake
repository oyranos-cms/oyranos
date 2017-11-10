SET( ENV{OY_MODULE_PATH} "${CMAKE_BINARY_DIR}/../../src/modules" )
SET( ENV{XDG_DATA_DIRS} "${CMAKE_CURRENT_LIST_DIR}/../.." )
SET( ENV{OY_LOCALEDIR} "${CMAKE_BINARY_DIR}/../../locale" )
EXECUTE_PROCESS( COMMAND echo "./test_core")
EXECUTE_PROCESS( COMMAND ./test_core)
EXECUTE_PROCESS( COMMAND echo "./test_core-static")
EXECUTE_PROCESS( COMMAND ./test_core-static)
EXECUTE_PROCESS( COMMAND echo "./test_object")
EXECUTE_PROCESS( COMMAND ./test_object)
EXECUTE_PROCESS( COMMAND echo "./test_object-static")
EXECUTE_PROCESS( COMMAND ./test_object-static)
EXECUTE_PROCESS( COMMAND echo OY_MODULE_PATH: "${CMAKE_BINARY_DIR}/../../src/modules" )
EXECUTE_PROCESS( COMMAND ls -lp "${CMAKE_BINARY_DIR}/../../src/modules" )
EXECUTE_PROCESS( COMMAND echo XDG_DATA_DIRS: "${CMAKE_CURRENT_LIST_DIR}/../.." )
EXECUTE_PROCESS( COMMAND ls -lp ${CMAKE_CURRENT_LIST_DIR}/../.. )
EXECUTE_PROCESS( COMMAND echo "./test_modules")
EXECUTE_PROCESS( COMMAND ./test_modules)
EXECUTE_PROCESS( COMMAND echo "./test_config")
EXECUTE_PROCESS( COMMAND ./test_config)
EXECUTE_PROCESS( COMMAND echo "./test2")
EXECUTE_PROCESS( COMMAND ./test2)
EXECUTE_PROCESS( COMMAND echo "./test2-static")
EXECUTE_PROCESS( COMMAND ./test2-static)
