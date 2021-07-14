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
                    "${CMAKE_SOURCE_DIR}/extras/icc"
                    "${GENDIR}"
)

# dependencies
INCLUDE(CheckLibraryExists)
INCLUDE(CheckIncludeFile)
CHECK_INCLUDE_FILE(dirent.h HAVE_DIRENT_H)
# The second dirent.h test catches on Android
FIND_PATH( DIRENT_INCLUDE_DIR dirent.h
           HINTS /mingw ${INCLUDE_DIRECTORIES}
           PATH_SUFFIXES include
	  )
IF(DIRENT_INCLUDE_DIR)
   SET( HAVE_DIRENT_H TRUE )
   MESSAGE( "-- dirent.h: " ${DIRENT_INCLUDE_DIR} )
ELSE(DIRENT_INCLUDE_DIR)
  MESSAGE( "-- dirent.h not found" )
ENDIF(DIRENT_INCLUDE_DIR)
CHECK_INCLUDE_FILE(langinfo.h HAVE_LANGINFO_H)

CHECK_INCLUDE_FILE(libintl.h HAVE_LIBINTL_H)
FIND_LIBRARY( LIBINTL_LIBRARIES NAMES intl libintl libintl-8 )
IF(LIBINTL_LIBRARIES)
  SET( EXTRA_LIBS_CORE ${EXTRA_LIBS_CORE} ${LIBINTL_LIBRARIES} )
ELSE(LIBINTL_LIBRARIES)
  MESSAGE( "-- libintl not found" )
ENDIF(LIBINTL_LIBRARIES)

CHECK_INCLUDE_FILE( iconv.h HAVE_ICONV_H )
CHECK_LIBRARY_EXISTS( iconv libiconv_open "iconv.h" HAVE_ICONV )
IF( HAVE_ICONV )
  FIND_LIBRARY( LIBICONV_LIBRARIES NAMES libiconv-2 libiconv iconv )
  IF(LIBICONV_LIBRARIES)
    SET( ICONV_LIBS ${LIBICONV_LIBRARIES} )
  ELSE(LIBICONV_LIBRARIES)
    SET( ICONV_LIBS iconv )
  ENDIF(LIBICONV_LIBRARIES)
  SET( EXTRA_LIBS_CORE ${EXTRA_LIBS_CORE} ${ICONV_LIBS} )
ELSE( HAVE_ICONV )
  CHECK_LIBRARY_EXISTS( iconv iconv_open "iconv.h" HAVE_ICONV_UNIX )
  IF( HAVE_ICONV_UNIX )
    SET( EXTRA_LIBS_CORE ${EXTRA_LIBS_CORE} iconv )
    SET( HAVE_ICONV TRUE )
  ENDIF( HAVE_ICONV_UNIX )
ENDIF( HAVE_ICONV )


CHECK_LIBRARY_EXISTS(m pow "math.h" HAVE_M)
IF(HAVE_M)
  FIND_LIBRARY( LIBM_LIBRARIES NAMES m )
  IF(LIBM_LIBRARIES)
    SET( EXTRA_LIBS_CORE ${EXTRA_LIBS_CORE} ${LIBM_LIBRARIES} )
  ENDIF(LIBM_LIBRARIES)
  MESSAGE( "-- m: ${LIBC_LIBRARIES}" )
ELSE(HAVE_M)
  MESSAGE( "-- m not found" )
ENDIF(HAVE_M)

CHECK_LIBRARY_EXISTS(dl dlopen "dlfcn.h" HAVE_DL)
IF(HAVE_DL)
  SET( EXTRA_LIBS_MODULES ${EXTRA_LIBS_MODULES} dl )
ELSE(HAVE_DL)
  FIND_PATH(DL_INCLUDE_DIR dlfcn.h)
  IF(DL_INCLUDE_DIR)
    SET( HAVE_DL 1 )
  ENDIF(DL_INCLUDE_DIR)
ENDIF(HAVE_DL)

FIND_PATH(LTDL_INCLUDE_DIR ltdl.h)
IF(LTDL_INCLUDE_DIR)
  SET( HAVE_LTDL_H 1 )
  CHECK_LIBRARY_EXISTS(ltdl dlopen "dlfcn.h" HAVE_LTDL)
  IF( HAVE_LTDL )
    SET( EXTRA_LIBS_MODULES ${EXTRA_LIBS_MODULES} ltdl )
  ENDIF( HAVE_LTDL )
ENDIF(LTDL_INCLUDE_DIR)

IF(NOT HAVE_DL AND NOT HAVE_LTDL)
  MESSAGE( SEND_ERROR "need one of libdl with dlfcn.h or libltdl with ltdl.h" )
ENDIF(NOT HAVE_DL AND NOT HAVE_LTDL)

CHECK_LIBRARY_EXISTS(c malloc "stdio.h" HAVE_C)
IF(HAVE_C)
  FIND_LIBRARY( LIBC_LIBRARIES NAMES c )
  IF(LIBC_LIBRARIES)
    SET( EXTRA_LIBS_CORE ${EXTRA_LIBS_CORE} ${LIBC_LIBRARIES} )
  ENDIF(LIBC_LIBRARIES)
  MESSAGE( "-- c: ${LIBC_LIBRARIES}" )
ELSE(HAVE_C)
  MESSAGE( "-- c not found" )
ENDIF(HAVE_C)


FIND_PATH( STDINT_INCLUDE_DIR stdint.h
           HINTS /mingw ${INCLUDE_DIRECTORIES}
           PATH_SUFFIXES include
	  )
IF(STDINT_INCLUDE_DIR)
  SET( HAVE_STDINT_H 1 )
  MESSAGE( "-- stdint.h: " ${STDINT_INCLUDE_DIR} )
ELSE(STDINT_INCLUDE_DIR)
  MESSAGE( "-- stdint.h not found" )
ENDIF(STDINT_INCLUDE_DIR)

FIND_PATH( INTTYPES_INCLUDE_DIR inttypes.h
           HINTS /mingw ${INCLUDE_DIRECTORIES}
           PATH_SUFFIXES include
	  )
IF(INTTYPES_INCLUDE_DIR)
  SET( HAVE_INTTYPES_H 1 )
  MESSAGE( "-- inttypes.h: " ${INTTYPES_INCLUDE_DIR} )
ELSE(INTTYPES_INCLUDE_DIR)
  MESSAGE( "-- inttypes.h not found" )
ENDIF(INTTYPES_INCLUDE_DIR)

# A GNU extension for debugging the call stack.
CHECK_LIBRARY_EXISTS( c backtrace "execinfo.h" HAVE_BACKTRACE )

FIND_PACKAGE( LCMS )
IF( HAVE_LCMS )
  INCLUDE_DIRECTORIES( ${LCMS_INCLUDE_DIR} )
  LINK_DIRECTORIES( ${LCMS_LIBRARY_DIRS} )
  SET( LCMS_I18N lcms-i18n.c )
ENDIF()

FIND_PACKAGE( LCMS2 )
IF( HAVE_LCMS2 )
  INCLUDE_DIRECTORIES( ${LCMS2_INCLUDE_DIR} )
  LINK_DIRECTORIES( ${LCMS2_LIBRARY_DIRS} )
  SET( LCM2_I18N lcm2-i18n.c )
ENDIF()

IF(NOT LCMS_FOUND AND NOT LCMS2_FOUND )
  MESSAGE( SEND_ERROR "Need at least one little CMS installation. lcms2 is prefered; Download http://www.littlecms.com" )
ENDIF()

IF(ENABLE_OPENMP)
  FIND_PACKAGE( OpenMP )
  IF(OPENMP_FOUND)
    SET( HAVE_OPENMP 1 )
  ENDIF(OPENMP_FOUND)
ENDIF(ENABLE_OPENMP)


FIND_PACKAGE( Cups )
FIND_PACKAGE( CUPS )
IF(HAVE_CUPS)
  INCLUDE_DIRECTORIES( ${CUPS_INCLUDE_DIR} )
  LINK_DIRECTORIES( ${CUPS_LIBRARIES} )
  IF( NOT CUPS_LIBRARIES )
    SET( CUPS_LIBRARIES ${CUPS_LIBRARY} )
  ENDIF( NOT CUPS_LIBRARIES )
ENDIF(HAVE_CUPS)

FIND_PACKAGE( Sane )
IF(SANE_FOUND)
  INCLUDE_DIRECTORIES( ${SANE_INCLUDE_DIR} )
  LINK_DIRECTORIES( ${SANE_LIBRARIES} )
ENDIF(SANE_FOUND)

FIND_PACKAGE( Cairo )
IF(HAVE_CAIRO)
  INCLUDE_DIRECTORIES( ${CAIRO_INCLUDE_DIR}/cairo )
  INCLUDE_DIRECTORIES( ${CAIRO_INCLUDE_DIR} )
  LINK_DIRECTORIES( ${CAIRO_LIBRARY_DIRS} )
ENDIF(HAVE_CAIRO)

CHECK_INCLUDE_FILE( OpenGL/glu.h HAVE_GLU_H )
FIND_PATH( HAVE_GLU_H glu.h
           HINTS /mingw ${INCLUDE_DIRECTORIES}
           PATH_SUFFIXES include
	  )

IF(NOT ${CMAKE_SYSTEM_NAME} MATCHES Android AND ENABLE_FLTK)
  FIND_PACKAGE( FLTK )
  IF(FLTK_FOUND)
    SET( HAVE_FLTK 1 )
    SET( HAVE_FLTK ${HAVE_FLTK} PARENT_SCOPE )
    INCLUDE_DIRECTORIES( ${FLTK_INCLUDE_DIR} )
    LINK_DIRECTORIES( ${FLTK_LIBRARIES} )
  ENDIF(FLTK_FOUND)
ENDIF()

FIND_PACKAGE( LibRaw )
IF( HAVE_LIBRAW )
#  TRY_COMPILE(
#    HAVE_LIBRAW_REALLY
#    ${CMAKE_BINARY_DIR}/src/tests
#    ${CMAKE_SOURCE_DIR}/src/tests/libraw_test.cxx
#    CMAKE_FLAGS "-DCMAKE_CXX_FLAGS=-I${LIBRAW_INCLUDE_DIR} -DCMAKE_EXE_LINKER_FLAGS=${LIBRAW_LIBRARY_DIRS}"
#    COMPILE_DEFINITIONS -shared
#    OUTPUT_VARIABLE TRY_COMPILE_OUTPUT
#    )
  INCLUDE_DIRECTORIES( ${LIBRAW_INCLUDE_DIR} )
  LINK_DIRECTORIES( ${LIBRAW_LIBRARY_DIRS} )
  TRY_COMPILE(
    HAVE_LIBRAW_PLUGIN
    ${CMAKE_BINARY_DIR}/src/tests/libraw_test
    ${CMAKE_SOURCE_DIR}/src/tests/libraw_test
    libraw_test
    CMAKE_FLAGS "-DCMAKE_CXX_FLAGS=-I${LIBRAW_INCLUDE_DIR} -DCMAKE_EXE_LINKER_FLAGS=${LIBRAW_LIBRARY_DIRS}"
    OUTPUT_VARIABLE TRY_COMPILE_OUTPUT
    )
  MESSAGE( "-- HAVE_LIBRAW_PLUGIN: ${HAVE_LIBRAW_PLUGIN}" )
ENDIF()

FIND_PACKAGE( Exiv2 )
IF(HAVE_EXIV2)
  INCLUDE_DIRECTORIES( ${EXIV2_INCLUDE_DIRS} )
  LINK_DIRECTORIES( ${EXIV2_LIBRARY_DIRS} )
ENDIF(HAVE_EXIV2)

FIND_PACKAGE( LibPNG )
IF(HAVE_LIBPNG)
  INCLUDE_DIRECTORIES( ${LIBPNG_INCLUDE_DIRS} )
  IF( LIBPNG_INCLUDEDIR )
    SET( LIBPNG_INCLUDE "${LIBPNG_INCLUDEDIR}/png.h" )
  ELSE( LIBPNG_INCLUDEDIR )
    SET( LIBPNG_INCLUDE <png.h> )
  ENDIF( LIBPNG_INCLUDEDIR )
  LINK_DIRECTORIES( ${LIBPNG_LIBRARY_DIRS} )
ENDIF()

FIND_PACKAGE( JPEG )
IF( JPEG_FOUND )
  FIND_PATH(JPEG_INCLDIR jpeglib.h)
  FIND_PACKAGE_HANDLE_STANDARD_ARGS(JPEG DEFAULT_MSG JPEG_INCLDIR)
ENDIF( JPEG_FOUND )
IF( JPEG_FOUND )
  INCLUDE_DIRECTORIES( ${JPEG_INCLUDE_DIR} )
  SET( HAVE_JPEG 1 )
ENDIF( JPEG_FOUND )

FIND_PACKAGE( TIFF )
FIND_PATH(TIFF_INCLDIR tiff.h)
IF( TIFF_FOUND )
  INCLUDE_DIRECTORIES( ${TIFF_INCLUDE_DIR} )
  SET( HAVE_TIFF 1 )
ENDIF( TIFF_FOUND )

INCLUDE(FindThreads)
IF(CMAKE_HAVE_THREADS_LIBRARY)
   SET(THREADS_FOUND TRUE)
ELSE()
   SET(THREADS_FOUND FALSE)
ENDIF(CMAKE_HAVE_THREADS_LIBRARY)

IF(CMAKE_USE_PTHREADS_INIT)
   SET(HAVE_PTHREAD 1)
   SET(PTHREADS_FOUND TRUE)
   SET(THREAD_LIBRARIES "")
   FIND_LIBRARY( THREAD_LIBRARIES NAMES pthread pthreads PATH
                 DOC "detect pthread library" )
ELSE()
   SET(HAVE_PTHREAD 0)
   SET(PTHREADS_FOUND FALSE)
ENDIF(CMAKE_USE_PTHREADS_INIT)
MESSAGE( "THREADS_FOUND: ${THREADS_FOUND}" )
MESSAGE( "PTHREADS_FOUND: ${PTHREADS_FOUND}" )
IF(UNIX)
  IF(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_CLANG)
    CHECK_LIBRARY_EXISTS( pthread pthread_create "" PTHREAD_LIB )
    IF( PTHREAD_LIB )
      SET( THREAD_LIBRARIES "-lpthread" )
    ENDIF( PTHREAD_LIB )
    CHECK_LIBRARY_EXISTS( pthreads pthread_create "" PTHREADS_LIB )
    IF( PTHREADS_LIB )
      SET( THREAD_LIBRARIES "-lpthreads" )
    ENDIF( PTHREADS_LIB )
  ENDIF(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_CLANG)
ENDIF(UNIX)
MESSAGE( "THREAD_LIBRARIES: ${THREAD_LIBRARIES}" )

FIND_PACKAGE( GetText )
MESSAGE( "GetText: ${GETTEXT_FOUND}" )
IF( GETTEXT_FOUND AND HAVE_LIBINTL_H )
  SET( USE_GETTEXT TRUE )
  SET( USE_GETTEXT ${USE_GETTEXT} PARENT_SCOPE )

  SET( LINGUAS ${CMAKE_SOURCE_DIR}/po/cs.po
               ${CMAKE_SOURCE_DIR}/po/de.po
               ${CMAKE_SOURCE_DIR}/po/eo.po
               ${CMAKE_SOURCE_DIR}/po/eu.po
               ${CMAKE_SOURCE_DIR}/po/fr.po
               ${CMAKE_SOURCE_DIR}/po/ru.po
     )
  FOREACH( oy_lang ${LINGUAS} )
    GET_FILENAME_COMPONENT(_absFile ${oy_lang} ABSOLUTE)
    GET_FILENAME_COMPONENT(_abs_PATH ${_absFile} PATH)
    GET_FILENAME_COMPONENT(_lang ${_absFile} NAME_WE)
    SET( OY_LINGUAS ${OY_LINGUAS} ${_lang} )
    SET( OY_LINGUAS_GMO_FILES ${OY_LINGUAS_GMO_FILES} " ${CMAKE_BINARY_DIR}/src/${_lang}.gmo" )
  ENDFOREACH( oy_lang )
  SET( OY_LINGUAS_FULL cs_CZ de_DE eo eu fr_FR ru_RU )
  SET( OY_LINGUAS ${OY_LINGUAS} PARENT_SCOPE )
  SET( OY_LINGUAS_FULL ${OY_LINGUAS_FULL} PARENT_SCOPE )
  SET( OY_LINGUAS_PO_FILES ${LINGUAS} PARENT_SCOPE )
  SET( OY_LINGUAS_GMO_FILES ${OY_LINGUAS_GMO_FILES} PARENT_SCOPE )
ENDIF( GETTEXT_FOUND AND HAVE_LIBINTL_H )

# debug
IF(CMAKE_CROSSCOMPILING)
  MESSAGE( "-- CMAKE_CROSSCOMPILING: ${CMAKE_CROSSCOMPILING}" )
  GET_PROPERTY(inc_dirs DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
  MESSAGE( "-- INCLUDE_DIRECTORIES:  ${inc_dirs}" )
  GET_PROPERTY(link_dirs DIRECTORY PROPERTY LINK_DIRECTORIES)
  MESSAGE( "-- LINK_DIRECTORIES:     ${link_dirs}" )
ENDIF()

SET( FRAMEWORK_DIR "Library/Frameworks" )
SET( FRAMEWORK_DIR ${FRAMEWORK_DIR} PARENT_SCOPE )
IF(APPLE)
  SET( LIB_DIR ${FRAMEWORK_DIR} )
  SET( GCC_LINK_PATH_FLAG "-F " PARENT_SCOPE )
  SET( GCC_LINK_LIB_FLAG "-framework " PARENT_SCOPE )
  # each framework header directory is named here
  # otherwise user would need to include the framework name before the header name,
  # which would not be consistent with other platforms
  SET( OYRANOS_INCLUDE_FLAGS "-I${CMAKE_INSTALL_PREFIX}/${FRAMEWORK_DIR}/Oyranos.framework/Headers -I${CMAKE_INSTALL_PREFIX}/${FRAMEWORK_DIR}/OyranosConfig.framework/Headers -I${CMAKE_INSTALL_PREFIX}/${FRAMEWORK_DIR}/OyranosModules.framework/Headers -I${CMAKE_INSTALL_PREFIX}/${FRAMEWORK_DIR}/OyranosObject.framework/Headers -I${CMAKE_INSTALL_PREFIX}/${FRAMEWORK_DIR}/OyranosCore.framework/Headers" PARENT_SCOPE )
ELSE(APPLE)
  SET( LIB_DIR "${CMAKE_INSTALL_LIBDIR}" )
  SET( GCC_LINK_PATH_FLAG "-L" PARENT_SCOPE )
  SET( GCC_LINK_LIB_FLAG "-l" PARENT_SCOPE )
  SET( OYRANOS_INCLUDE_FLAGS "-I${CMAKE_INSTALL_FULL_INCLUDEDIR}/${PACKAGE_NAME}" PARENT_SCOPE )
ENDIF(APPLE)
SET( LIB_DIR ${LIB_DIR} PARENT_SCOPE )

IF(ENABLE_SHARED_LIBS_${PROJECT_UP_NAME})
  SET( TARGET_LIB ${PROJECT_NAME} )
ELSE(ENABLE_SHARED_LIBS_${PROJECT_UP_NAME})
  SET( TARGET_LIB ${PACKAGE_NAME}-static )
ENDIF(ENABLE_SHARED_LIBS_${PROJECT_UP_NAME})

ADD_SUBDIRECTORY( include )

ADD_SUBDIRECTORY( include_private )

ADD_SUBDIRECTORY( liboyranos_core )

ADD_SUBDIRECTORY( liboyranos_object )

IF(OPENMP_FOUND)
    INCLUDE_DIRECTORIES( ${OPENMP_INCLUDE_DIRS} )
    LINK_DIRECTORIES( ${OPENMP_LIBRARY_DIRS} )
    SET( EXTRA_LIBS ${EXTRA_LIBS} ${OPENMP_LIBRARIES} )
    SET( CMAKE_C_FLAGS_CORE "${CMAKE_C_FLAGS}" )
    SET( CMAKE_CXX_FLAGS_CORE "${CMAKE_CXX_FLAGS}" )
    SET( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}" )
    SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}" )
ENDIF(OPENMP_FOUND)

ADD_SUBDIRECTORY( liboyranos_modules )

ADD_SUBDIRECTORY( liboyranos_config )

ADD_SUBDIRECTORY( modules )

ADD_SUBDIRECTORY( liboyranos )

IF(ENABLE_TESTS_${PROJECT_UP_NAME})
  ADD_SUBDIRECTORY( tests )
ENDIF()

IF(ENABLE_TOOLS_${PROJECT_UP_NAME})
  ADD_SUBDIRECTORY( tools )
ENDIF()

IF(ENABLE_EXAMPLES_${PROJECT_UP_NAME})
  ADD_SUBDIRECTORY( examples )
ENDIF()


# versions for libraries
IF(ENABLE_SHARED_LIBS_${PROJECT_UP_NAME})
  SET_TARGET_PROPERTIES( ${PROJECT_NAME}Core ${PROJECT_NAME}Object ${PROJECT_NAME}Modules ${PROJECT_NAME}Config ${PROJECT_NAME}
    PROPERTIES VERSION   ${OYRANOS_VERSION_MAJOR}.${OYRANOS_VERSION_MINOR}.${OYRANOS_VERSION_MICRO}
               SOVERSION ${OYRANOS_VERSION_MAJOR}
                     )
ENDIF(ENABLE_SHARED_LIBS_${PROJECT_UP_NAME})

MESSAGE( "-- EXTRA_LIBS_CORE:          ${EXTRA_LIBS_CORE}" )
SET( EXTRA_LIBS_CORE     ${EXTRA_LIBS_CORE} PARENT_SCOPE)
SET( EXTRA_LIBS_OBJECT   ${EXTRA_LIBS_OBJECT} PARENT_SCOPE)
SET( EXTRA_LIBS_MODULES  ${EXTRA_LIBS_MODULES} PARENT_SCOPE)
SET( EXTRA_LIBS_CONFIG   ${EXTRA_LIBS_CONFIG} PARENT_SCOPE)
SET( EXTRA_LIBS          ${EXTRA_LIBS} PARENT_SCOPE)
SET( OY_MODULE_LIBRARIES ${OY_MODULE_LIBRARIES} PARENT_SCOPE )

MESSAGE( "-- EXTRA_LIBS:          ${EXTRA_LIBS}" )
MESSAGE( "-- OY_MODULE_LIBRARIES: ${OY_MODULE_LIBRARIES}" )

SET( DOC_SOURCES
	doxymentation_common
        ${CMAKE_SOURCE_DIR}/src/include/*.h
        ${CMAKE_BINARY_DIR}/src/include/*.h
        oyranos_monitor_hooks.h
	      ${CMAKE_SOURCE_DIR}/src/liboyranos_core/oyranos_helper.c
	${CFILES_CORE_PUBLIC}
	${CHEADERS_CORE_PUBLIC}
	${CFILES_OBJECT_PUBLIC}
	${CHEADERS_OBJECT_PUBLIC}
	${CFILES_OBJECT_MODULES_PUBLIC}
	${CHEADERS_OBJECT_MODULES_PUBLIC}
	${CFILES_OBJECT_CONFIG_PUBLIC}
	${CFILES_OBJECT_CONFIG_GEN_PUBLIC}
	${CHEADERS_OBJECT_CONFIG_PUBLIC}
	${CFILES_OBJECT_IMAGING_PUBLIC}
	${CHEADERS_OBJECT_IMAGING_PUBLIC}
  ${CMAKE_SOURCE_DIR}/doc/Release*.md
	${CMAKE_SOURCE_DIR}/src/modules/color/modules/lcms2/lcm2_profiler.h
	${CMAKE_SOURCE_DIR}/src/modules/color/modules/lcms2/lcm2_profiler.c
	${CMAKE_SOURCE_DIR}/src/modules/color/modules/lcms2/oyranos_cmm_lcm2.c
	${CMAKE_SOURCE_DIR}/src/modules/color/modules/*.c
	${CMAKE_SOURCE_DIR}/src/modules/color/modules/*.cpp
	${CMAKE_SOURCE_DIR}/src/modules/color/modules/*.h
  ${CMAKE_SOURCE_DIR}/src/modules/color/modules/*.md
  ${CMAKE_SOURCE_DIR}/src/modules/color/modules/devices/*.md
	${CMAKE_SOURCE_DIR}/src/modules/color/modules/devices/*.c
	${CMAKE_SOURCE_DIR}/src/modules/color/modules/devices/*.cpp
	${CMAKE_SOURCE_DIR}/src/modules/color/modules/devices/*.h
	${CMAKE_SOURCE_DIR}/src/modules/oyranos-meta/*.c
	${CMAKE_SOURCE_DIR}/src/modules/oyranos-meta/*.h
  ${CMAKE_SOURCE_DIR}/doc/md/oyranos*.md
   )
IF(NOT HAVE_OYJL)
  SET( DOC_SOURCES ${DOC_SOURCES}
        ${CMAKE_SOURCE_DIR}/openicc/oyjl/oyjl.h
        ${CMAKE_SOURCE_DIR}/openicc/oyjl/oyjl_test.h
        ${CMAKE_SOURCE_DIR}/openicc/oyjl/oyjl_test_main.h
        ${CMAKE_SOURCE_DIR}/openicc/oyjl/oyjl_args.c
        ${CMAKE_SOURCE_DIR}/openicc/oyjl/oyjl_args_tree.c
        ${CMAKE_SOURCE_DIR}/openicc/oyjl/oyjl_core.c
        ${CMAKE_SOURCE_DIR}/openicc/oyjl/oyjl_tree.c
        ${CMAKE_SOURCE_DIR}/openicc/oyjl/oyjl_yajl.c
        ${CMAKE_SOURCE_DIR}/doc/md/oyjl*.md
     )
ENDIF(NOT HAVE_OYJL)
SET( DOC_SOURCES ${DOC_SOURCES} PARENT_SCOPE )


# i18n
IF( GETTEXT_FOUND )
  SET(_potFile ${CMAKE_SOURCE_DIR}/po/${PACKAGE_NAME}.pot)
  #SET( _xgettext_option_list --language=C --keyword=_ --keyword=N_ --keyword=C_:1c,2 --keyword=NC_:1c,2 -s --package-name=${PROJECT_NAME} --package-version=${OYRANOS_VERSION_MAJOR}.${OYRANOS_VERSION_MINOR} )
  SET( _xgettext_option_list --add-comments --keyword=gettext --flag=gettext:1:pass-c-format --keyword=_ --flag=_:1:pass-c-format --keyword=N_ --flag=N_:1:pass-c-format  --copyright-holder='Kai-Uwe Behrmann'  --msgid-bugs-address='ku.b@gmx.de' --from-code=utf-8 --package-name=${PROJECT_NAME} --package-version=${OYRANOS_VERSION_MAJOR}.${OYRANOS_VERSION_MINOR} )
  SET( XGETTEXT_CMD xgettext )
  FILE( GLOB ${PROJECT_UP_NAME}_SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        include/*.h
        API_generated/*.c API_generated/*.h
        liboyranos/*.c
        liboyranos_core/*.c
        liboyranos_object/*.c
        liboyranos_modules/*.c
        liboyranos_config/*.c
        modules/oyranos*.c
        modules/color/modules/*.c
        modules/color/modules/devices/*.c
        modules/color/modules/lcms2/oyranos_cmm_lcm2.c
        modules/color/modules/lcms2/lcm2_profiler.*
        modules/color/modules/*.cpp
        modules/color/modules/devices/*.cpp
        tools/*.c tools/qcmsevents/*.cpp tools/qcmsevents/*.cxx
        examples/*.c examples/devices/*.c examples/libraw/*.cpp
        examples/image_display/*.c* examples/image_display/*.h
      )

  # set to be generated source target
  FILE( GLOB ${PROJECT_UP_NAME}_SRC_FILES_EXTRA RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        modules/*-i18n.c
      )
  MESSAGE( "${PROJECT_UP_NAME}_SRC_FILES_EXTRA: ${${PROJECT_UP_NAME}_SRC_FILES_EXTRA}" )

  ADD_CUSTOM_COMMAND(OUTPUT ${_potFile}
    COMMAND ${XGETTEXT_CMD} ${_xgettext_option_list} -o ${_potFile} ${${PROJECT_UP_NAME}_SRC_FILES} ${${PROJECT_UP_NAME}_SRC_FILES_EXTRA}
    DEPENDS ${${PROJECT_UP_NAME}_SRC_FILES} ${LCM2_I18N} ${LCMS_I18N}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Extract translatable messages to ${_potFile}"
  )

  # compile translations always to have them ready for installation
  SET( GETTEXT_TRANSLATIONS_TARGET_PREFIX ${PROJECT_DOWN_NAME}_ )
  GETTEXT_CREATE_TRANSLATIONS( ${_potFile} ALL ${LINGUAS} )

  ADD_CUSTOM_COMMAND( OUTPUT ${PROJECT_DOWN_NAME}_gmo-cp
    COMMAND for oy_lang in ${OY_LINGUAS}\; do mkdir -p "${CMAKE_BINARY_DIR}/locale/$$oy_lang/LC_MESSAGES/" \; cp "${CMAKE_CURRENT_BINARY_DIR}/$$oy_lang.gmo" "${CMAKE_BINARY_DIR}/locale/$$oy_lang/LC_MESSAGES/${PACKAGE_NAME}.mo" \; done
    COMMENT "put ${PROJECT_DOWN_NAME}.mo files into ${CMAKE_BINARY_DIR}/locale/lang/LC_MESSAGES/ path for testing with LOCDIR"
                    )
  ADD_CUSTOM_TARGET( ${PROJECT_DOWN_NAME}_gmo ALL
                     DEPENDS ${GETTEXT_TRANSLATIONS_TARGET_PREFIX}translations ${PROJECT_DOWN_NAME}_gmo-cp  )

ENDIF()


# code generation
FIND_PACKAGE(Grantlee5 COMPONENTS Templates)
ADD_CUSTOM_COMMAND( OUTPUT generator/oyAPIGenerator
		COMMAND test -d ${CMAKE_CURRENT_BINARY_DIR}/generator/ && echo "-- Reusing previous directory" || mkdir ${CMAKE_CURRENT_BINARY_DIR}/generator/
		COMMAND cd generator/ && CMAKE_PREFIX_PATH="${CMAKE_CURRENT_BINARY_DIR}/generator/grantlee/lib/cmake/grantlee/" cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS=-g -Wall ${CMAKE_CURRENT_SOURCE_DIR}/generator && make || echo "Did you run ${CMAKE_CURRENT_SOURCE_DIR}/generator/download_grantlee.sh in ${CMAKE_CURRENT_BINARY_DIR}/generator/ or install grantlee5-devel?"
		)

ADD_CUSTOM_TARGET( generate
		COMMAND cd ${CMAKE_CURRENT_BINARY_DIR}/generator/ && ./oyAPIGenerator ${CMAKE_CURRENT_SOURCE_DIR}/templates:${CMAKE_CURRENT_SOURCE_DIR}/sources:${CMAKE_CURRENT_SOURCE_DIR}/API_generated
		COMMENT "Generate source code from templates"
		VERBATIM
		DEPENDS generator/oyAPIGenerator
		WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/generator" )

