INCLUDE(CheckSymbolExists)

CHECK_SYMBOL_EXISTS(clearenv     "stdlib.h"         HAVE_CLEARENV)
CHECK_SYMBOL_EXISTS(setenv       "stdlib.h"         HAVE_SETENV)
CHECK_SYMBOL_EXISTS(omp_get_num_procs "stdio.h"     HAVE_OPENMP)

CHECK_INCLUDE_FILE(ctype.h      HAVE_CTYPE_H)
CHECK_INCLUDE_FILE(errno.h      HAVE_ERRNO_H)
CHECK_INCLUDE_FILE(locale.h     HAVE_LOCALE_H)
CHECK_INCLUDE_FILE(stdio.h      HAVE_STDIO_H)
CHECK_INCLUDE_FILE(stdlib.h     HAVE_STDLIB_H)
CHECK_INCLUDE_FILE(string.h     HAVE_STRING_H)
CHECK_INCLUDE_FILE(time.h       HAVE_TIME_H)
CHECK_INCLUDE_FILE(unistd.h     HAVE_UNISTD_H)


CONFIGURE_FILE (
	"${CMAKE_CURRENT_SOURCE_DIR}/config.log.h.in"
	"${CMAKE_CURRENT_BINARY_DIR}/config.log.h"
		@ONLY
	)

CONFIGURE_FILE (
	"${CMAKE_CURRENT_SOURCE_DIR}/oyranos_config_internal.h.in"
	"${CMAKE_CURRENT_BINARY_DIR}/oyranos_config_internal.h"
		@ONLY
	)

INSTALL( FILES
	oyranos_alpha.h
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PACKAGE_NAME}/alpha)
