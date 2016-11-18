CMAKE_MINIMUM_REQUIRED (VERSION 2.6.4)

SET( TARGET yajl )

SET( CHEADERS_YAJL_PUBLIC
	yajl/${TARGET}_common.h
	yajl/${TARGET}_gen.h
	yajl/${TARGET}_parse.h
	yajl/${TARGET}_version.h
   )
SET( CHEADERS_YAJL
	${TARGET}_alloc.h
	${TARGET}_buf.h
	${TARGET}_bytestack.h
	${TARGET}_encode.h
	${TARGET}_lex.h
	${TARGET}_parser.h
   )
SET( CFILES_YAJL 
	${TARGET}.c
	${TARGET}_alloc.c
	${TARGET}_buf.c
	${TARGET}_encode.c
	${TARGET}_gen.c
	${TARGET}_lex.c
	${TARGET}_parser.c
	${TARGET}_version.c
   )

SET( DOKU
	TODO
	README
	ChangeLog
	COPYING
   )

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} )

ADD_LIBRARY( yajl-static STATIC ${CFILES_YAJL} )

