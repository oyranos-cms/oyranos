CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET( SOURCES
	SAX.c 
	entities.c 
	encoding.c 
	error.c 
	parserInternals.c 
	parser.c 
	tree.c 
	hash.c
	list.c 
	xmlIO.c 
	xmlmemory.c 
	uri.c 
	valid.c 
	xpath.c 
	globals.c 
	threads.c 
	xmlstring.c 
	buf.c 
	dict.c 
	SAX2.c 
	chvalid.c
	)
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/libxml )
SET(EXTRA_LIBS m)
ADD_LIBRARY( xml2-static STATIC ${SOURCES} )
TARGET_LINK_LIBRARIES (
   xml2-static
   ${EXTRA_LIBS} )
