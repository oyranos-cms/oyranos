CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

PROJECT (Oyranos)

SET (OYRANOS_VERSION_MAJOR 0)
SET (OYRANOS_VERSION_MINOR 9)
SET (OYRANOS_VERSION_MICRO 2)

SET (SO_VERSION 1)

SET (OYRANOS_DEVEL_MONTH 11)
SET (OYRANOS_DEVEL_YEAR 2012)

SET (PACKAGE_NAME "oyranos")
set (PACKAGE_DESCRIPTION "Oyranos is a Colour Management System (CMS) on operating system level. It allows to match predictably input device colours to output device colours across supporting applications. One goal is to make colour management useful for all users in a automated fashion and regardless of any technical knowledge.")

SET( OY_CMMSUBPATH     "colour/modules" )
SET( OY_METASUBPATH    "${PACKAGE_NAME}" )

#additional cmake modules for loading libraries
SET (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

IF(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
	IF (NOT FORCE_IN_SOURCE_BUILD)
		MESSAGE(FATAL_ERROR
				"In-source builds are not permitted.\n"
				"Make a separate folder for building:\n"
				"    mkdir build && cd build && cmake ..\n"
				"Before that, remove the files already created:\n"
				"    rm -rf CMakeCache.txt CMakeFiles\n"
				"If you really know what you are doing\n"
				"(will overwrite original files!) use:\n"
				"    cmake -DFORCE_IN_SOURCE_BUILD=ON\n"
			)
	ENDIF (NOT FORCE_IN_SOURCE_BUILD)
ENDIF(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)

IF (NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
ENDIF (NOT CMAKE_BUILD_TYPE)


#some basic and important variables
SET (DOMAIN oyranos.org) #needed by doxygen
SET (REVERSE_DOMAIN org.oyranos)

SET (OYRANOS_VERSION "${OYRANOS_VERSION_MAJOR}.${OYRANOS_VERSION_MINOR}.${OYRANOS_VERSION_MICRO}")
SET (OYRANOS_VERSION_MAJORMINOR "${OYRANOS_VERSION_MAJOR}.${OYRANOS_VERSION_MINOR}")

IF( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  SET(PLATFORM "OSX")
ELSEIF( ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  SET(PLATFORM "Linux")
ELSE( ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  SET(PLATFORM "${CMAKE_SYSTEM_NAME}")
ENDIF( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
MESSAGE( "-- Found PLATFORM: ${PLATFORM}" )

#now add all directories
ADD_SUBDIRECTORY( src )
ADD_SUBDIRECTORY( cmake )
ADD_SUBDIRECTORY( extras )


ADD_CUSTOM_TARGET(uninstall
	"${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/OyranosUninstall.cmake")

ADD_CUSTOM_TARGET(dist
		COMMAND git archive --prefix=oyranos-${OYRANOS_VERSION}/ --format=tar HEAD | bzip2 > ${CMAKE_CURRENT_BINARY_DIR}/oyranos-${OYRANOS_VERSION}.tar.bz2 && echo "git:       git://www.oyranos.org/git/oyranos" > ${CMAKE_CURRENT_BINARY_DIR}/oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum && echo "git sha1:  " | tr -d "\\n" >> ${CMAKE_CURRENT_BINARY_DIR}/oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum && git rev-parse HEAD >> ${CMAKE_CURRENT_BINARY_DIR}/oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum && cd ${CMAKE_CURRENT_BINARY_DIR}/ && tar xjf oyranos-${OYRANOS_VERSION}.tar.bz2 && cp -ar ${CMAKE_SOURCE_DIR}/elektra-0.7.1/ oyranos-${OYRANOS_VERSION} && tar cf - oyranos-${OYRANOS_VERSION} | bzip2 > oyranos-${OYRANOS_VERSION}.tar.bz2 && echo  "package:   http://sourceforge.net/projects/oyranos/files/Oyranos/Oyranos%20${OYRANOS_VERSION_MAJORMINOR}/oyranos-${OYRANOS_VERSION}.tar.bz2" >> oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum && echo  "size:      " | tr -d "\\n" >> oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum && du -b oyranos-${OYRANOS_VERSION}.tar.bz2 | sed s/[[:space:]][[:print:]]*// >> oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum && echo  "md5sum:    " | tr -d "\\n" >> oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum  && md5sum oyranos-${OYRANOS_VERSION}.tar.bz2 >> oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum && echo  "sha1sum:   " | tr -d "\\n" >> oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum && sha1sum oyranos-${OYRANOS_VERSION}.tar.bz2 >> oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum && echo  "sha256sum: " | tr -d "\\n" >> oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum && sha256sum oyranos-${OYRANOS_VERSION}.tar.bz2 >> oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum
		COMMENT Make a oyranos source package by using git
		VERBATIM
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
		)

