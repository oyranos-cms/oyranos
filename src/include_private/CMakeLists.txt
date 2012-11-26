include(CheckIncludeFile)
include(CheckSymbolExists)

find_package(Xcm REQUIRED)

check_symbol_exists(clearenv     "stdlib.h"         HAVE_CLEARENV)
check_symbol_exists(setenv       "stdlib.h"         HAVE_SETENV)
check_symbol_exists(omp_get_num_procs "stdio.h"     HAVE_OPENMP)

check_include_file(ctype.h      HAVE_CTYPE_H)
check_include_file(errno.h      HAVE_ERRNO_H)
check_include_file(libintl.h    USE_GETTEXT)
check_include_file(locale.h     HAVE_LOCALE_H)
check_include_file(stdio.h      HAVE_STDIO_H)
check_include_file(stdlib.h     HAVE_STDLIB_H)
check_include_file(string.h     HAVE_STRING_H)
check_include_file(time.h       HAVE_TIME_H)
check_include_file(unistd.h     HAVE_UNISTD_H)


configure_file (
	"${CMAKE_CURRENT_SOURCE_DIR}/config.log.h.in"
	"${CMAKE_CURRENT_BINARY_DIR}/config.log.h"
		@ONLY
	)

configure_file (
	"${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
	"${CMAKE_CURRENT_BINARY_DIR}/config.h"
		@ONLY
	)

