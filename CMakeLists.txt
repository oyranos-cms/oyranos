CMAKE_MINIMUM_REQUIRED (VERSION 2.6.2)

PROJECT (Oyranos)

SET (OYRANOS_VERSION_MAJOR 0)
SET (OYRANOS_VERSION_MINOR 9)
SET (OYRANOS_VERSION_MICRO 6)

SET (OYRANOS_DEVEL_MONTH 8)
SET (OYRANOS_DEVEL_YEAR 2016)

SET (PACKAGE_NAME "oyranos")
SET (PACKAGE_DESCRIPTION "Oyranos is a Color Management System (CMS) on operating system level. It allows to match predictably input device colors to output device colors across supporting applications. One goal is to make color management useful for all users in a automated fashion and regardless of any technical knowledge.")

SET( OY_CMMSUBPATH     "color/modules" )
SET( OY_METASUBPATH    "${PACKAGE_NAME}-meta" )
SET( OY_ICC_DIR        "${CMAKE_INSTALL_PREFIX}/share/color/icc" )

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

IF( CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]" )
  MESSAGE( "-- Info build \"${CMAKE_BUILD_TYPE}\"" )
  SET( DEBUG 1 )
ENDIF()

FIND_PROGRAM(CMAKE_AR_TMP NAMES ar)

IF(CMAKE_C_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  SET(CMAKE_COMPILER_IS_CLANG 1)
ENDIF()

# if our static code lands inside a shared lib, we need -fPIC
IF(UNIX)
  IF(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_CLANG)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
    # coverage testing
    IF(USE_GCOV AND CMAKE_COMPILER_IS_GNUCC)
      SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
      SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
      SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
    ENDIF(USE_GCOV AND CMAKE_COMPILER_IS_GNUCC)
  ENDIF(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_CLANG)
ENDIF(UNIX)


# detect top level projects
IF(USE_SYSTEM_YAJL)
  FIND_PACKAGE(Yajl REQUIRED)
ELSE()
  FIND_PACKAGE(Yajl)
ENDIF()
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
    #include(ExternalProject)
    #ExternalProject_Add( yajl
    #SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/yajl
    #CONFIGURE_COMMAND cmake ${CMAKE_CURRENT_SOURCE_DIR}/yajl
    #BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/yajl
    #BUILD_COMMAND ${MAKE}
    # skip installation
    #INSTALL_COMMAND echo )
    ADD_SUBDIRECTORY( yajl )
    INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/yajl )
    LINK_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR}/yajl )
    SET( YAJL_LIBRARIES yajl-static )
  ENDIF()
ENDIF( HAVE_YAJL )

IF(USE_SYSTEM_ELEKTRA)
  FIND_PACKAGE(Elektra REQUIRED)
ELSE()
  FIND_PACKAGE(Elektra)
ENDIF()
IF(HAVE_ELEKTRA)
  SET( ELEKTRA_SPEC "Requires: elektra
BuildRequires: elektra-devel" )
  INCLUDE_DIRECTORIES( ${ELEKTRA_INCLUDE_DIRS} )
  LINK_DIRECTORIES( ${ELEKTRA_LIBRARY_DIRS} )
  STRING(REPLACE ";" " " ELEKTRA_PC_LDFLAGS "${ELEKTRA_STATIC_LDFLAGS}")
ENDIF(HAVE_ELEKTRA)

IF(USE_SYSTEM_OPENICC)
  FIND_PACKAGE(OpenIcc REQUIRED)
ELSE()
  FIND_PACKAGE(OpenIcc)
ENDIF()
IF( HAVE_OPENICC )
  INCLUDE_DIRECTORIES( ${OPENICC_INCLUDE_DIR} )
  LINK_DIRECTORIES( ${OPENICC_LIBRARY_DIRS} )
ELSE( HAVE_OPENICC )
  IF(CMAKE_VERSION VERSION_GREATER 2.8.0)
    ADD_SUBDIRECTORY( openicc )
    SET( HAVE_OPENICC TRUE  )
    INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/openicc/include )
    LINK_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR}/openicc/libopenicc )
    SET( OPENICC_LIBRARY openicc-static )
  ENDIF()
ENDIF( HAVE_OPENICC )

FIND_PACKAGE( X11 )
IF(X11_FOUND)
  INCLUDE_DIRECTORIES( ${X11_INCLUDE_DIRS} )
  LINK_DIRECTORIES( ${X11_LIBRARY_DIRS} )
  SET( HAVE_X11 1 )

  FIND_PACKAGE( Xfixes )
  INCLUDE_DIRECTORIES( ${XFIXES_INCLUDE_DIRS} )
  LINK_DIRECTORIES( ${XFIXES_LIBRARY_DIRS} )

  FIND_PACKAGE( XRandR )
  INCLUDE_DIRECTORIES( ${XRANDR_INCLUDE_DIRS} )
  LINK_DIRECTORIES( ${XRANDR_LIBRARY_DIRS} )

  FIND_PACKAGE( Xinerama )
  INCLUDE_DIRECTORIES( ${XINERAMA_INCLUDE_DIRS} )
  LINK_DIRECTORIES( ${XINERAMA_LIBRARY_DIRS} )

  FIND_PACKAGE( Xxf86vm )
  INCLUDE_DIRECTORIES( ${XXF86VM_INCLUDE_DIRS} )
  LINK_DIRECTORIES( ${XXF86VM_LIBRARY_DIRS} )

  FIND_PACKAGE( Xmu )
  INCLUDE_DIRECTORIES( ${XMU_INCLUDE_DIRS} )
  LINK_DIRECTORIES( ${XMU_LIBRARY_DIRS} )
ENDIF(X11_FOUND)

IF(USE_SYSTEM_LIBXCM)
  FIND_PACKAGE(Xcm REQUIRED)
ELSE()
  FIND_PACKAGE(Xcm)
ENDIF()
IF( HAVE_XCM )
  INCLUDE_DIRECTORIES( ${XCM_INCLUDE_DIRS} )
  LINK_DIRECTORIES( ${XCM_LIBRARIES} )
  LINK_DIRECTORIES( ${XCM_LIBRARY_DIRS} )
ELSE( HAVE_XCM )
  IF(CMAKE_VERSION VERSION_GREATER 2.8.0)
    ADD_SUBDIRECTORY( libxcm )
    SET( HAVE_XCM TRUE  )
    INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/libxcm/include )
    LINK_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR}/libxcm/src )
    LINK_DIRECTORIES( ${XCM_LIBRARY_DIRS} )
    SET( XCM_LIBRARIES xcm-static )
  ENDIF()
ENDIF( HAVE_XCM )

#now add all directories
ADD_SUBDIRECTORY( color )
ADD_SUBDIRECTORY( src )
ADD_SUBDIRECTORY( cmake )
ADD_SUBDIRECTORY( extras )
ADD_SUBDIRECTORY( doc )

CONFIGURE_FILE(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/OyranosUninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake/OyranosUninstall.cmake"
    IMMEDIATE @ONLY)

ADD_CUSTOM_TARGET( uninstall
	"${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/OyranosUninstall.cmake")

ADD_CUSTOM_TARGET( dist
		COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/ && git archive --prefix=oyranos-${OYRANOS_VERSION}/ --format=tar HEAD > ${CMAKE_CURRENT_BINARY_DIR}/oyranos-${OYRANOS_VERSION}.tar 
                COMMAND echo "git:       git clone git://guthub.com/oyranos-cms/oyranos.git" > ${CMAKE_CURRENT_BINARY_DIR}/oyranos-${OYRANOS_VERSION}.tar.bz2.hashsum
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
                COMMAND cp -f ${CMAKE_SOURCE_DIR}/extras/oyranos-rpmlintrc rpmdir/SOURCES
                COMMAND rpmbuild --nodeps --clean -ba extras/${PACKAGE_NAME}.spec --define "_topdir ${CMAKE_CURRENT_BINARY_DIR}/rpmdir"
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
		COMMAND cd ${PACKAGE_NAME}-${OYRANOS_VERSION}/ && debuild -d
                DEPENDS debsrc
		COMMENT Make a oyranos DEB package
		VERBATIM
		)

