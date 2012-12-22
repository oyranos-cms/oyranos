SET (GENDIR "${CMAKE_SOURCE_DIR}/src/API_generated/" )

INCLUDE_DIRECTORIES(
                    "${CMAKE_SOURCE_DIR}"
                    "${CMAKE_SOURCE_DIR}/src"
                    "${CMAKE_SOURCE_DIR}/src/include"
                    "${CMAKE_BINARY_DIR}/src/include"
                    "${CMAKE_SOURCE_DIR}/src/include_private"
                    "${CMAKE_BINARY_DIR}/src/include_private"
                    "${CMAKE_SOURCE_DIR}/src/liboyranos_core"
                    "${CMAKE_SOURCE_DIR}/src/liboyranos_object"
                    "${CMAKE_SOURCE_DIR}/src/liboyranos_modules"
                    "${CMAKE_SOURCE_DIR}/src/liboyranos_config"
                    "${CMAKE_SOURCE_DIR}/src/liboyranos"
                    "${GENDIR}"
)

# if our static code lands inside a shared lib, we need -fPIC
IF(UNIX)
  IF(CMAKE_COMPILER_IS_GNUCC)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
  ENDIF(CMAKE_COMPILER_IS_GNUCC)
ENDIF(UNIX)

# dependencies
INCLUDE(CheckLibraryExists)

CHECK_LIBRARY_EXISTS(m pow "math.h" HAVE_M)
IF(HAVE_M)
  SET( EXTRA_LIBS ${EXTRA_LIBS} m )
  SET( EXTRA_LIBS_CORE m )
ENDIF(HAVE_M)

CHECK_LIBRARY_EXISTS(dl dlopen "dlfcn.h" HAVE_DL)
IF(HAVE_DL)
  SET( EXTRA_LIBS ${EXTRA_LIBS} dl )
ENDIF(HAVE_DL)

CHECK_LIBRARY_EXISTS(ltdl lt_dlopen "ltdl.h" HAVE_LTDL)
IF(HAVE_LTDL)
  SET( EXTRA_LIBS ${EXTRA_LIBS} ltdl )
ENDIF(HAVE_LTDL)

CHECK_LIBRARY_EXISTS(c malloc "stdio.h" HAVE_C)
IF(HAVE_C)
  SET( EXTRA_LIBS ${EXTRA_LIBS} c )
ENDIF(HAVE_C)

FIND_PACKAGE( LCMS )
INCLUDE_DIRECTORIES( ${LCMS_INCLUDE_DIRS} )
LINK_DIRECTORIES( ${LCMS_LIBRARY_DIRS} )

FIND_PACKAGE( LCMS2 )
INCLUDE_DIRECTORIES( ${LCMS2_INCLUDE_DIRS} )
LINK_DIRECTORIES( ${LCMS2_LIBRARY_DIRS} )

IF(NOT( LCMS_FOUND OR LCMS2_FOUND ))
  MESSAGE( SEND_ERROR "Need at least one little CMS installation. lcms2 is prefered; Download http://www.littlecms.com" )
ENDIF()

FIND_PACKAGE( OpenMP )
INCLUDE_DIRECTORIES( ${OPENMP_INCLUDE_DIRS} )
LINK_DIRECTORIES( ${OPENMP_LIBRARY_DIRS} )
SET( EXTRA_LIBS ${EXTRA_LIBS} ${OPENMP_LIBRARIES} )
IF(OPENMP_FOUND)
  SET( HAVE_OPENMP 1 )
  SET( CMAKE_C_FLAGS_CORE "${CMAKE_C_FLAGS}" )
  SET( CMAKE_CXX_FLAGS_CORE "${CMAKE_CXX_FLAGS}" )
  SET( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}" )
  SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}" )
ENDIF(OPENMP_FOUND)

FIND_PACKAGE( Xcm REQUIRED )
INCLUDE_DIRECTORIES( ${XCM_INCLUDE_DIRS} )
LINK_DIRECTORIES( ${XCM_LIBRARY_DIRS} )

FIND_PACKAGE( X11 )
INCLUDE_DIRECTORIES( ${X11_INCLUDE_DIRS} )
LINK_DIRECTORIES( ${X11_LIBRARY_DIRS} )
IF(X11_FOUND)
  SET( HAVE_X11 1 )
ENDIF(X11_FOUND)

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

FIND_PACKAGE( CUPS )
INCLUDE_DIRECTORIES( ${CUPS_INCLUDE_DIRS} )
LINK_DIRECTORIES( ${CUPS_LIBRARY_DIRS} )

FIND_PACKAGE( Cairo )
INCLUDE_DIRECTORIES( ${CAIRO_INCLUDE_DIRS} )
LINK_DIRECTORIES( ${CAIRO_LIBRARY_DIRS} )

FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui)
INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIRS} )
LINK_DIRECTORIES( ${QT_LIBRARY_DIRS} )
IF(QT_FOUND)
  SET( HAVE_QT 1 )
ENDIF(QT_FOUND)

FIND_PACKAGE( Fltk )
INCLUDE_DIRECTORIES( ${FLTK_INCLUDE_DIRS} )
LINK_DIRECTORIES( ${FLTK_LIBRARY_DIRS} )
IF(FLTK_FOUND)
  SET( HAVE_FLTK 1 )
ENDIF(FLTK_FOUND)

FIND_PACKAGE(LibXml2 REQUIRED)
INCLUDE_DIRECTORIES( ${LIBXML2_INCLUDE_DIR} )
LINK_DIRECTORIES( ${LIBXML2_LIBRARY_DIRS} )
IF(LIBXML2_FOUND)
  SET( HAVE_LIBXML2 1 )
  # we need xml2 for internal elektra
  SET( LIBXML2_LIBRARIES ${LIBXML2_LIBRARIES} PARENT_SCOPE)
ENDIF(LIBXML2_FOUND)

FIND_PACKAGE( Yajl REQUIRED )
INCLUDE_DIRECTORIES( ${YAJL_INCLUDE_DIRS} )
LINK_DIRECTORIES( ${YAJL_LIBRARY_DIRS} )
IF( HAVE_YAJL )
  INCLUDE( CheckIncludeFiles )
  CHECK_INCLUDE_FILES( yajl/yajl_version.h HAVE_YAJL_VERSION_H )
  # yajl2 should come with yajl_version.h, the included yajl is version 1.0.x
  IF( NOT HAVE_YAJL_VERSION_H )
    INCLUDE_DIRECTORIES( ${CMAKE_SOURCE_DIR}/yajl/ )
  ENDIF( NOT HAVE_YAJL_VERSION_H )
ENDIF( HAVE_YAJL )

FIND_PACKAGE( LibRaw )
INCLUDE_DIRECTORIES( ${LIBRAW_INCLUDE_DIR} )
LINK_DIRECTORIES( ${LIBRAW_LIBRARY_DIRS} )
IF( HAVE_LIBRAW )
#  TRY_COMPILE(
#    HAVE_LIBRAW_REALLY
#    ${CMAKE_BINARY_DIR}/src/tests
#    ${CMAKE_SOURCE_DIR}/src/tests/libraw_test.cxx
#    CMAKE_FLAGS "-DCMAKE_CXX_FLAGS=-I${LIBRAW_INCLUDE_DIR} -DCMAKE_EXE_LINKER_FLAGS=${LIBRAW_LIBRARY_DIRS}"
#    COMPILE_DEFINITIONS -shared
#    OUTPUT_VARIABLE TRY_COMPILE_OUTPUT
#    )
  TRY_COMPILE(
    HAVE_LIBRAW_PLUGIN
    ${CMAKE_BINARY_DIR}/src/tests/libraw_test
    ${CMAKE_SOURCE_DIR}/src/tests/libraw_test
    libraw_test
    CMAKE_FLAGS "-DCMAKE_CXX_FLAGS=-I${LIBRAW_INCLUDE_DIR} -DCMAKE_EXE_LINKER_FLAGS=${LIBRAW_LIBRARY_DIRS}"
    OUTPUT_VARIABLE TRY_COMPILE_OUTPUT
    )
  MESSAGE( "HAVE_LIBRAW_PLUGIN: ${HAVE_LIBRAW_PLUGIN}" )
ENDIF()

FIND_PACKAGE( Exif2 )
INCLUDE_DIRECTORIES( ${EXIV2_INCLUDE_DIRS} )
LINK_DIRECTORIES( ${EXIV2_LIBRARY_DIRS} )

FIND_PACKAGE( LibPNG REQUIRED )
INCLUDE_DIRECTORIES( ${LIBPNG_INCLUDE_DIRS} )
LINK_DIRECTORIES( ${LIBPNG_LIBRARY_DIRS} )

FIND_PACKAGE( GetText )

# debug
get_property(inc_dirs DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
#MESSAGE( "-- INCLUDE_DIRECTORIES: ${inc_dirs}" )


ADD_SUBDIRECTORY( include )

ADD_SUBDIRECTORY( include_private )

ADD_SUBDIRECTORY( liboyranos_core )

ADD_SUBDIRECTORY( liboyranos_object )

ADD_SUBDIRECTORY( liboyranos_modules )

ADD_SUBDIRECTORY( liboyranos_config )

ADD_SUBDIRECTORY( liboyranos )

ADD_SUBDIRECTORY( modules )

ADD_SUBDIRECTORY( tests )

ADD_SUBDIRECTORY( tools )

ADD_SUBDIRECTORY( examples )


# versions for libraries
SET_TARGET_PROPERTIES( oyranos_core oyranos_object oyranos_modules oyranos_config oyranos
    PROPERTIES VERSION   ${OYRANOS_VERSION_MAJOR}.${OYRANOS_VERSION_MINOR}.${OYRANOS_VERSION_MICRO}
               SOVERSION ${OYRANOS_VERSION_MAJOR}
                     )


SET( DOC_SOURCES
	doxymentation doxymentation_common
	${CFILES_CORE_PUBLIC}
	${CHEADERS_PUBLIC}
	${CFILES_OBJECT_PUBLIC}
	${CHEADERS_OBJECT_PUBLIC}
	${CFILES_OBJECT_MODULES_PUBLIC}
	${CHEADERS_OBJECT_MODULES_PUBLIC}
	${CFILES_OBJECT_CONFIG_PUBLIC}
	${CFILES_OBJECT_CONFIG_GEN_PUBLIC}
	${CHEADERS_OBJECT_CONFIG_PUBLIC}
	${CFILES_OBJECT_IMAGING_PUBLIC}
	${CHEADERS_OBJECT_IMAGING_PUBLIC}
        PARENT_SCOPE
   )


# i18n
IF( GETTEXT_FOUND )
  SET(_potFile ${CMAKE_SOURCE_DIR}/po/${PACKAGE_NAME}.pot)
  #SET( _xgettext_option_list --language=C --keyword=_ --keyword=N_ --keyword=C_:1c,2 --keyword=NC_:1c,2 -s --package-name=${PROJECT_NAME} --package-version=${OYRANOS_VERSION_MAJOR}.${OYRANOS_VERSION_MINOR} )
  SET( _xgettext_option_list --keyword=gettext --flag=gettext:1:pass-c-format --keyword=_ --flag=_:1:pass-c-format --keyword=N_ --flag=N_:1:pass-c-format  --copyright-holder='Kai-Uwe Behrmann'  --msgid-bugs-address='ku.b@gmx.de' --from-code=utf-8 --package-name=${PROJECT_NAME} --package-version=${OYRANOS_VERSION_MAJOR}.${OYRANOS_VERSION_MINOR} )
  SET( XGETTEXT_CMD xgettext )
  FILE( GLOB SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        API_generated/*.c API_generated/*.h
        liboyranos/*.c
        liboyranos_core/*.c
        liboyranos_object/*.c
        liboyranos_modules/*.c
        liboyranos_config/*.c
        modules/*.c
        modules/devices/*.c
        modules/*.cpp
        modules/devices/*.cpp
        tools/*.c tools/qcmsevents/*.cpp tools/qcmsevents/*.cxx
      )

  ADD_CUSTOM_COMMAND(OUTPUT ${_potFile}
    COMMAND ${XGETTEXT_CMD} ${_xgettext_option_list} -o ${_potFile} ${SRC_FILES}
    DEPENDS ${SRC_FILES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Extract translatable messages to ${_potFile}"
  )

  SET( LINGUAS ../po/cs.po ../po/de.po ../po/eo.po ../po/eu.po  ../po/fr.po )
  # compile translations always to have them ready for installation
  GETTEXT_CREATE_TRANSLATIONS( ${_potFile} ALL ${LINGUAS} )
ENDIF()


# code generation
ADD_CUSTOM_COMMAND( OUTPUT generator/oyAPIGenerator
                COMMAND cd generator/ && CMAKE_PREFIX_PATH="${CMAKE_CURRENT_BINARY_DIR}/generator/grantlee/lib/cmake/grantlee/" cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS=-g -Wall ${CMAKE_CURRENT_SOURCE_DIR}/generator && make || echo "Did you run ${CMAKE_CURRENT_SOURCE_DIR}/generator/download_grantlee.sh in ${CMAKE_CURRENT_BINARY_DIR}/generator/ ?"
		)

ADD_CUSTOM_TARGET( generate
		COMMAND cd ${CMAKE_CURRENT_BINARY_DIR}/generator/ && ./oyAPIGenerator ${CMAKE_CURRENT_SOURCE_DIR}/templates:${CMAKE_CURRENT_SOURCE_DIR}/sources:${CMAKE_CURRENT_SOURCE_DIR}/API_generated
		COMMENT "Generate source code from templates"
		VERBATIM
                DEPENDS generator/oyAPIGenerator
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/generator" )

