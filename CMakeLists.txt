cmake_minimum_required (VERSION 2.6)

project (Oyranos)

set (OYRANOS_VERSION_MAJOR 0)
set (OYRANOS_VERSION_MINOR 9)
set (OYRANOS_VERSION_MICRO 2)

set (SO_VERSION 1)


#additional modules for loading libraries
set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
	if (NOT FORCE_IN_SOURCE_BUILD)
		message(FATAL_ERROR
				"In-source builds are not permitted.\n"
				"Make a separate folder for building:\n"
				"    mkdir build && cd build && cmake ..\n"
				"Before that, remove the files already created:\n"
				"    rm -rf CMakeCache.txt CMakeFiles\n"
				"If you really know what you are doing\n"
				"(will overwrite original files!) use:\n"
				"    cmake -DFORCE_IN_SOURCE_BUILD=ON\n"
			)
	endif (NOT FORCE_IN_SOURCE_BUILD)
endif(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)

if (NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif (NOT CMAKE_BUILD_TYPE)


#some basic and important variables
set (DOMAIN oyranos.org) #needed by doxygen
set (REVERSE_DOMAIN org.oyranos)

set (OYRANOS_VERSION "${OYRANOS_VERSION_MAJOR}.${OYRANOS_VERSION_MINOR}.${OYRANOS_VERSION_MICRO}")
set (OYRANOS_VERSION_MAJORMINOR "${OYRANOS_VERSION_MAJOR}.${OYRANOS_VERSION_MINOR}")


#now add all directories
add_subdirectory (src)
add_subdirectory (cmake)


ADD_CUSTOM_TARGET(uninstall
	"${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/OyranosUninstall.cmake")

ADD_CUSTOM_TARGET(dist
		COMMAND git archive --prefix=oyranos-${OYRANOS_VERSION}/ --format=tar HEAD | bzip2 > ${CMAKE_CURRENT_BINARY_DIR}/oyranos-${OYRANOS_VERSION}.tar.bz2 && echo "git:       git://www.oyranos.org/git/oyranos" > ${CMAKE_CURRENT_BINARY_DIR}/oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum && echo "git sha1:  " | tr -d "\\n" >> ${CMAKE_CURRENT_BINARY_DIR}/oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum && git rev-parse HEAD >> ${CMAKE_CURRENT_BINARY_DIR}/oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum && cd ${CMAKE_CURRENT_BINARY_DIR}/ && tar xjf oyranos-${OYRANOS_VERSION}.tar.bz2 && cp -ar ${CMAKE_SOURCE_DIR}/elektra-0.7.1/ oyranos-${OYRANOS_VERSION} && tar cf - oyranos-${OYRANOS_VERSION} | bzip2 > oyranos-${OYRANOS_VERSION}.tar.bz2 && echo  "package:   http://sourceforge.net/projects/oyranos/files/Oyranos/Oyranos%20${OYRANOS_VERSION_MAJORMINOR}/oyranos-${OYRANOS_VERSION}.tar.bz2" >> oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum && echo  "size:      " | tr -d "\\n" >> oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum && du -b oyranos-${OYRANOS_VERSION}.tar.bz2 | sed s/[[:space:]][[:print:]]*// >> oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum && echo  "md5sum:    " | tr -d "\\n" >> oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum  && md5sum oyranos-${OYRANOS_VERSION}.tar.bz2 >> oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum && echo  "sha1sum:   " | tr -d "\\n" >> oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum && sha1sum oyranos-${OYRANOS_VERSION}.tar.bz2 >> oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum && echo  "sha256sum: " | tr -d "\\n" >> oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum && sha256sum oyranos-${OYRANOS_VERSION}.tar.bz2 >> oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum
		COMMENT Make a oyranos source package by using git
		VERBATIM
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
		)

