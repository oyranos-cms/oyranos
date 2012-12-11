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

IF (NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
ENDIF (NOT CMAKE_BUILD_TYPE)


#some basic and important variables
SET( DOMAIN oyranos.org ) #needed by doxygen
SET( REVERSE_DOMAIN org.oyranos )

SET( OYRANOS_VERSION "${OYRANOS_VERSION_MAJOR}.${OYRANOS_VERSION_MINOR}.${OYRANOS_VERSION_MICRO}" )
SET( OYRANOS_VERSION_MAJORMINOR "${OYRANOS_VERSION_MAJOR}.${OYRANOS_VERSION_MINOR}" )

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
ADD_SUBDIRECTORY( settings )
ADD_SUBDIRECTORY( doc )



ADD_CUSTOM_TARGET( uninstall
	"${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/OyranosUninstall.cmake")

ADD_CUSTOM_TARGET( dist
		COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/ && git archive --prefix=oyranos-${OYRANOS_VERSION}/ --format=tar HEAD > ${CMAKE_CURRENT_BINARY_DIR}/oyranos-${OYRANOS_VERSION}.tar 
                COMMAND echo "git:       git clone git://www.oyranos.org/git/oyranos" > ${CMAKE_CURRENT_BINARY_DIR}/oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum
                COMMAND echo "git sha1:  " | tr -d "\\n" >> ${CMAKE_CURRENT_BINARY_DIR}/oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum
                COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/ && git rev-parse HEAD >> ${CMAKE_CURRENT_BINARY_DIR}/oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum 
                COMMAND rm -rf oyranos-${OYRANOS_VERSION} && tar xf oyranos-${OYRANOS_VERSION}.tar 
                COMMAND cp -ar ${CMAKE_SOURCE_DIR}/elektra-0.7.1/ oyranos-${OYRANOS_VERSION} && tar cf - oyranos-${OYRANOS_VERSION} | bzip2 > oyranos-${OYRANOS_VERSION}.tar.bz2
                COMMAND echo  "package:   http://sourceforge.net/projects/oyranos/files/Oyranos/Oyranos%20${OYRANOS_VERSION_MAJORMINOR}/oyranos-${OYRANOS_VERSION}.tar.bz2" >> oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum
                COMMAND echo  "size:      " | tr -d "\\n" >> oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum
                COMMAND du -b oyranos-${OYRANOS_VERSION}.tar.bz2 | sed s/[[:space:]][[:print:]]*// >> oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum 
                COMMAND echo  "md5sum:    " | tr -d "\\n" >> oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum
                COMMAND cd ${CMAKE_CURRENT_BINARY_DIR}/ && md5sum oyranos-${OYRANOS_VERSION}.tar.bz2 >> oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum
                COMMAND echo  "sha1sum:   " | tr -d "\\n" >> oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum
                COMMAND sha1sum oyranos-${OYRANOS_VERSION}.tar.bz2 >> oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum
                COMMAND echo  "sha256sum: " | tr -d "\\n" >> oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum
                COMMAND sha256sum oyranos-${OYRANOS_VERSION}.tar.bz2 >> oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum
		COMMENT Make a oyranos source package by using git
		VERBATIM
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
		)
EXEC_PROGRAM (  rpmbuild 
                ARGS --showrc | awk '/^build arch/ {print $$4}'
                OUTPUT_VARIABLE RPMARCH
             )
SET( PWD ENV{PWD} )
ADD_CUSTOM_TARGET( rpm
		COMMAND rm -rf ${PACKAGE_NAME}-${OYRANOS_VERSION}/
		COMMAND tar xjf ${PACKAGE_NAME}-${OYRANOS_VERSION}.tar.bz2
                COMMAND mkdir -p rpmdir/BUILD rpmdir/SPECS rpmdir/SOURCES rpmdir/SRPMS rpmdir/RPMS/${RPMARCH}
                COMMAND cp -f ${PACKAGE_NAME}-${OYRANOS_VERSION}.tar.bz2 rpmdir/SOURCES
                COMMAND rpmbuild --clean -ba extras/${PACKAGE_NAME}.spec --define "_topdir ${CMAKE_CURRENT_BINARY_DIR}/rpmdir"
                DEPENDS dist
		COMMENT Make a oyranos RPM package by using git
		VERBATIM
		)

