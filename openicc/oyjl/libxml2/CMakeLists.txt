CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET( SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/SAX.c 
	${CMAKE_CURRENT_SOURCE_DIR}/entities.c 
	${CMAKE_CURRENT_SOURCE_DIR}/encoding.c 
	${CMAKE_CURRENT_SOURCE_DIR}/error.c 
	${CMAKE_CURRENT_SOURCE_DIR}/parserInternals.c 
	${CMAKE_CURRENT_SOURCE_DIR}/parser.c 
	${CMAKE_CURRENT_SOURCE_DIR}/tree.c 
	${CMAKE_CURRENT_SOURCE_DIR}/hash.c
	${CMAKE_CURRENT_SOURCE_DIR}/list.c 
	${CMAKE_CURRENT_SOURCE_DIR}/xmlIO.c 
	${CMAKE_CURRENT_SOURCE_DIR}/xmlmemory.c 
	${CMAKE_CURRENT_SOURCE_DIR}/uri.c 
	${CMAKE_CURRENT_SOURCE_DIR}/valid.c 
	${CMAKE_CURRENT_SOURCE_DIR}/xpath.c 
	${CMAKE_CURRENT_SOURCE_DIR}/globals.c 
	${CMAKE_CURRENT_SOURCE_DIR}/threads.c 
	${CMAKE_CURRENT_SOURCE_DIR}/xmlstring.c 
	${CMAKE_CURRENT_SOURCE_DIR}/buf.c 
	${CMAKE_CURRENT_SOURCE_DIR}/dict.c 
	${CMAKE_CURRENT_SOURCE_DIR}/SAX2.c 
	${CMAKE_CURRENT_SOURCE_DIR}/chvalid.c
	)
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/libxml )
SET(EXTRA_LIBS ${EXTRA_LIBS} m)
ADD_LIBRARY( xml2-static STATIC ${SOURCES} )
SET( SOURCES_STATIC ${SOURCES_STATIC} ${SOURCES} PARENT_SCOPE )
TARGET_LINK_LIBRARIES (
   xml2-static
   ${EXTRA_LIBS} )
