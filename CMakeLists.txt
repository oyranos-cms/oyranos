CMAKE_MINIMUM_REQUIRED (VERSION 2.6.4)

PROJECT (Oyranos)

SET (OYRANOS_VERSION_MAJOR 0)
SET (OYRANOS_VERSION_MINOR 9)
SET (OYRANOS_VERSION_MICRO 6)

SET (OYRANOS_DEVEL_MONTH 03)
SET (OYRANOS_DEVEL_YEAR 2014)

SET (PACKAGE_NAME "oyranos")
SET (PACKAGE_DESCRIPTION "Oyranos is a Color Management System (CMS) on operating system level. It allows to match predictably input device colors to output device colors across supporting applications. One goal is to make color management useful for all users in a automated fashion and regardless of any technical knowledge.")

SET( OY_CMMSUBPATH     "color/modules" )
SET( OY_METASUBPATH    "${PACKAGE_NAME}" )

#additional cmake modules for loading libraries
SET (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

IF (NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
ENDIF (NOT CMAKE_BUILD_TYPE)


# some basic and important variables
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


# detect top level projects
FIND_PACKAGE( Yajl )
IF( HAVE_YAJL )
  INCLUDE_DIRECTORIES( ${YAJL_INCLUDE_DIRS} )
  LINK_DIRECTORIES( ${YAJL_LIBRARY_DIRS} )
  INCLUDE( CheckIncludeFiles )
  CHECK_INCLUDE_FILES( yajl/yajl_version.h HAVE_YAJL_VERSION_H )
  # yajl2 should come with yajl_version.h, the included yajl is version 1.0.x
  IF( NOT HAVE_YAJL_VERSION_H )
    INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/yajl/ )
  ENDIF( NOT HAVE_YAJL_VERSION_H )
ELSE( HAVE_YAJL )
  IF(CMAKE_VERSION VERSION_GREATER 2.8.0)
    include(ExternalProject)
    ExternalProject_Add( yajl
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/yajl
    CONFIGURE_COMMAND cmake ${CMAKE_CURRENT_SOURCE_DIR}/yajl
    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/yajl
    BUILD_COMMAND ${MAKE}
    # skip installation
    INSTALL_COMMAND echo )
    INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/yajl )
    LINK_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR}/yajl )
    SET( YAJL_LIBRARIES yajl-static )
  ENDIF()
  
ENDIF( HAVE_YAJL )

FIND_PACKAGE( Elektra )
IF(HAVE_ELEKTRA)
  SET( ELEKTRA_SPEC "Requires: elektra
BuildRequires: elektra-devel" )
  INCLUDE_DIRECTORIES( ${ELEKTRA_INCLUDE_DIRS} )
  LINK_DIRECTORIES( ${ELEKTRA_LIBRARY_DIRS} )
  STRING(REPLACE ";" " " ELEKTRA_PC_LDFLAGS "${ELEKTRA_STATIC_LDFLAGS}")
ELSE(HAVE_ELEKTRA)
  SET( ELEKTRA_VERSION elektra-0.7.1 )
  MESSAGE( "-- Using internal Elektra: ${ELEKTRA_VERSION}" )
  INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/${ELEKTRA_VERSION}/src/include
       ${CMAKE_BINARY_DIR}/${ELEKTRA_VERSION}/src/include )
  SET( ELEKTRA_LIBRARIES_STATIC
       ${CMAKE_BINARY_DIR}/${ELEKTRA_VERSION}/src/libelektra/libelektra_a-kdb.o
       ${CMAKE_BINARY_DIR}/${ELEKTRA_VERSION}/src/libelektra/libelektra_a-kdbhandle.o
       ${CMAKE_BINARY_DIR}/${ELEKTRA_VERSION}/src/libelektra/libelektra_a-key.o
       ${CMAKE_BINARY_DIR}/${ELEKTRA_VERSION}/src/libelektra/libelektra_a-keyset.o
       ${CMAKE_BINARY_DIR}/${ELEKTRA_VERSION}/src/libelektra/libelektra_a-trie.o
       ${CMAKE_BINARY_DIR}/${ELEKTRA_VERSION}/src/libelektra/libelektra_a-kdbhighlevel.o
       ${CMAKE_BINARY_DIR}/${ELEKTRA_VERSION}/src/libelektra/libelektra_a-kdbcapability.o
       ${CMAKE_BINARY_DIR}/${ELEKTRA_VERSION}/src/libelektra/libelektra_a-keyhelpers.o
       ${CMAKE_BINARY_DIR}/${ELEKTRA_VERSION}/src/libelektra/libelektra_a-keymeta.o
       ${CMAKE_BINARY_DIR}/${ELEKTRA_VERSION}/src/libelektra/libelektra_a-keyname.o
       ${CMAKE_BINARY_DIR}/${ELEKTRA_VERSION}/src/libelektra/libelektra_a-keytest.o
       ${CMAKE_BINARY_DIR}/${ELEKTRA_VERSION}/src/libelektra/libelektra_a-keyvalue.o
       ${CMAKE_BINARY_DIR}/${ELEKTRA_VERSION}/src/libelektra/libelektra_a-serialize.o
       ${CMAKE_BINARY_DIR}/${ELEKTRA_VERSION}/src/libelektra/libelektra_a-split.o
       ${CMAKE_BINARY_DIR}/${ELEKTRA_VERSION}/src/libelektra/libelektra_a-exported_symbols.o
       ${CMAKE_BINARY_DIR}/${ELEKTRA_VERSION}/src/libloader/libloader_static_a-kdbLibLoader.o
       ${CMAKE_BINARY_DIR}/${ELEKTRA_VERSION}/src/libhelper/libhelper_static_a-internal.o
       ${CMAKE_BINARY_DIR}/${ELEKTRA_VERSION}/src/libhelper/libhelper_static_a-helper.o
       ${CMAKE_BINARY_DIR}/${ELEKTRA_VERSION}/src/backends/filesys/libelektra_filesys_a-filesys.o
       ${CMAKE_BINARY_DIR}/${ELEKTRA_VERSION}/src/libelektratools/libelektratools_a-kdbtools.o
       ${CMAKE_BINARY_DIR}/${ELEKTRA_VERSION}/src/libelektratools/libelektratools_a-stream.o
     )
  SET( ELEKTRA_LIBRARIES 
       ${LIBXML2_LIBRARIES}
     )
  IF(CMAKE_VERSION VERSION_GREATER 2.8.0)
    include(ExternalProject)
    ExternalProject_Add( ${ELEKTRA_VERSION}
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${ELEKTRA_VERSION}
    # GCC flags (-fPIC) will fail on Windows, without we get relocation R_X86_64_32 errors
    CONFIGURE_COMMAND CFLAGS=-fPIC ${CMAKE_CURRENT_SOURCE_DIR}/${ELEKTRA_VERSION}/configure --enable-static --enable-shared=false
    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${ELEKTRA_VERSION}
    BUILD_COMMAND ${MAKE}
    # skip installation
    INSTALL_COMMAND echo )
  ENDIF()
ENDIF(HAVE_ELEKTRA)

#now add all directories
ADD_SUBDIRECTORY( src )
ADD_SUBDIRECTORY( cmake )
ADD_SUBDIRECTORY( extras )
ADD_SUBDIRECTORY( settings )
ADD_SUBDIRECTORY( doc )

CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/OyranosUninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/OyranosUninstall.cmake"
    IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET( uninstall
	"${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/OyranosUninstall.cmake")

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
EXECUTE_PROCESS(COMMAND rpmbuild --showrc
                COMMAND awk '/^build arch/ {print $$4}'
                OUTPUT_VARIABLE RPMARCH
                ERROR_VARIABLE RPMBUILD_ERROR
               )
ADD_CUSTOM_TARGET( rpm
		COMMAND test -d ${PACKAGE_NAME}-${OYRANOS_VERSION}/ && rm -rf ${PACKAGE_NAME}-${OYRANOS_VERSION}/
		COMMAND tar xjf ${PACKAGE_NAME}-${OYRANOS_VERSION}.tar.bz2
                COMMAND mkdir -p rpmdir/BUILD rpmdir/SPECS rpmdir/SOURCES rpmdir/SRPMS rpmdir/RPMS/${RPMARCH}
                COMMAND cp -f ${PACKAGE_NAME}-${OYRANOS_VERSION}.tar.bz2 rpmdir/SOURCES
                COMMAND rpmbuild --clean -ba extras/${PACKAGE_NAME}.spec --define "_topdir ${CMAKE_CURRENT_BINARY_DIR}/rpmdir"
                DEPENDS dist
		COMMENT Make a oyranos RPM package
		VERBATIM
		)

ADD_CUSTOM_TARGET( debsrc
		COMMAND test -d ${PACKAGE_NAME}-${OYRANOS_VERSION}/ && rm -rf ${PACKAGE_NAME}-${OYRANOS_VERSION}/
		COMMAND tar xjf ${PACKAGE_NAME}-${OYRANOS_VERSION}.tar.bz2
                COMMAND mv ${PACKAGE_NAME}-${OYRANOS_VERSION}.tar.bz2 ${PACKAGE_NAME}_${OYRANOS_VERSION}.orig.tar.bz2
                COMMAND cp -R ${CMAKE_SOURCE_DIR}/extras/debian ${PACKAGE_NAME}-${OYRANOS_VERSION}/
                COMMAND dpkg-source -b ${PACKAGE_NAME}-${OYRANOS_VERSION}/
                DEPENDS dist
		COMMENT Make a oyranos DEB source package
		VERBATIM
		)
ADD_CUSTOM_TARGET( debian
		COMMAND cd ${PACKAGE_NAME}-${OYRANOS_VERSION}/ && debuild
                DEPENDS debsrc
		COMMENT Make a oyranos DEB package
		VERBATIM
		)

