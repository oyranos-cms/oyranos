INCLUDE_DIRECTORIES(
                    "${CMAKE_CURRENT_SOURCE_DIR}"
                    "${CMAKE_CURRENT_SOURCE_DIR}/include"
                    "${CMAKE_BINARY_DIR}/src/include"
                    "${CMAKE_CURRENT_SOURCE_DIR}/libopenicc"
)

# if our static code lands inside a shared lib, we need -fPIC
IF(UNIX)
  IF(CMAKE_COMPILER_IS_GNUCC)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC")
    # coverage testing
    IF(USE_GCOV)
      SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
      SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
      SET(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -g -O0 -fprofile-arcs -ftest-coverage")
    ENDIF(USE_GCOV)
  ENDIF(CMAKE_COMPILER_IS_GNUCC)
ENDIF(UNIX)

# dependencies
INCLUDE(CheckLibraryExists)
INCLUDE(CheckIncludeFile)
CHECK_INCLUDE_FILE(dirent.h HAVE_DIRENT_H)
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
CHECK_INCLUDE_FILE(glob.h HAVE_GLOB_H)
CHECK_LIBRARY_EXISTS( c glob "glob.h" HAVE_GLOB )
CHECK_LIBRARY_EXISTS( c issetugid "unistd.h" HAVE_ISSETUGID )
CHECK_LIBRARY_EXISTS( c getauxval "sys/auxv.h" HAVE_GETAUXVAL )

CHECK_INCLUDE_FILE(langinfo.h HAVE_LANGINFO_H)
CHECK_INCLUDE_FILE(locale.h HAVE_LOCALE_H)

CHECK_INCLUDE_FILE(libintl.h HAVE_LIBINTL_H)
FIND_LIBRARY( LIBINTL_LIBRARIES NAMES intl libintl libintl-8 )
IF(LIBINTL_LIBRARIES)
  SET( EXTRA_LIBS ${EXTRA_LIBS} ${LIBINTL_LIBRARIES} )
  SET( EXTRA_LIBS_CORE ${EXTRA_LIBS_CORE} ${LIBINTL_LIBRARIES} )
ELSE(LIBINTL_LIBRARIES)
  MESSAGE( "-- libintl not found" )
ENDIF(LIBINTL_LIBRARIES)

CHECK_LIBRARY_EXISTS( iconv libiconv_open "iconv.h" HAVE_ICONV )
IF( HAVE_ICONV )
  FIND_LIBRARY( LIBICONV_LIBRARIES NAMES libiconv-2 libiconv iconv
        )
  IF(LIBICONV_LIBRARIES)
    SET( ICONV_LIBS ${LIBICONV_LIBRARIES} )
  ELSE(LIBICONV_LIBRARIES)
    SET( ICONV_LIBS iconv )
  ENDIF(LIBICONV_LIBRARIES)
  SET( EXTRA_LIBS ${EXTRA_LIBS} ${ICONV_LIBS} )
  SET( EXTRA_LIBS_CORE ${EXTRA_LIBS_CORE} ${ICONV_LIBS} )
ELSE( HAVE_ICONV )
  CHECK_LIBRARY_EXISTS( iconv iconv_open "iconv.h" HAVE_ICONV_UNIX )
  IF( HAVE_ICONV_UNIX )
    SET( EXTRA_LIBS ${EXTRA_LIBS} iconv )
    SET( EXTRA_LIBS_CORE ${EXTRA_LIBS_CORE} iconv )
    SET( HAVE_ICONV TRUE )
  ENDIF( HAVE_ICONV_UNIX )
ENDIF( HAVE_ICONV )


CHECK_LIBRARY_EXISTS(m pow "math.h" HAVE_M)
IF(HAVE_M)
  SET( EXTRA_LIBS ${EXTRA_LIBS} m )
  SET( EXTRA_LIBS_CORE ${EXTRA_LIBS_CORE} m )
ENDIF(HAVE_M)

CHECK_LIBRARY_EXISTS(c malloc "stdio.h" HAVE_C)
IF(HAVE_C)
  SET( EXTRA_LIBS ${EXTRA_LIBS} c )
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


FIND_PACKAGE( GetText )
MESSAGE( "GetText: ${GETTEXT_FOUND}" )
IF( GETTEXT_FOUND )
  SET( USE_GETTEXT TRUE )
  SET( OI_LOCALEDIR "${CMAKE_INSTALL_FULL_LOCALEDIR}" )

  SET( LINGUAS ${CMAKE_SOURCE_DIR}/po/cs.po
               ${CMAKE_SOURCE_DIR}/po/de.po
               ${CMAKE_SOURCE_DIR}/po/eo.po
               ${CMAKE_SOURCE_DIR}/po/eu.po
               ${CMAKE_SOURCE_DIR}/po/fr.po
               ${CMAKE_SOURCE_DIR}/po/ru.po
     )
  FOREACH( lang ${LINGUAS} )
    GET_FILENAME_COMPONENT(_absFile ${lang} ABSOLUTE)
    GET_FILENAME_COMPONENT(_abs_PATH ${_absFile} PATH)
    GET_FILENAME_COMPONENT(_lang ${_absFile} NAME_WE)
    SET( OI_LINGUAS ${OI_LINGUAS} ${_lang} )
    SET( OI_LINGUAS_GMO_FILES ${OI_LINGUAS_GMO_FILES} " ${CMAKE_BINARY_DIR}/src/${_lang}.gmo" )
  ENDFOREACH( lang )
  SET( OI_LINGUAS_FULL cs_CZ de_DE eo eu fr_FR ru_RU )
  SET( OI_LINGUAS ${OI_LINGUAS} PARENT_SCOPE )
  SET( OI_LINGUAS_FULL ${OI_LINGUAS_FULL} PARENT_SCOPE )
  SET( OI_LINGUAS_PO_FILES ${LINGUAS} PARENT_SCOPE )
  SET( OI_LINGUAS_GMO_FILES ${OI_LINGUAS_GMO_FILES} PARENT_SCOPE )
ENDIF()

# debug
GET_PROPERTY(inc_dirs DIRECTORY PROPERTY INCLUDE_DIRECTORIES)
#MESSAGE( "-- INCLUDE_DIRECTORIES: ${inc_dirs}" )


ADD_SUBDIRECTORY( include )

ADD_SUBDIRECTORY( libopenicc )

IF( ENABLE_TESTS_${PROJECT_UP_NAME} )
    ADD_SUBDIRECTORY( tests )
ENDIF()

IF( ENABLE_TOOLS_${PROJECT_UP_NAME} )
    ADD_SUBDIRECTORY( tools )
ENDIF()

ADD_SUBDIRECTORY( examples )


# versions for libraries
SET_TARGET_PROPERTIES(   OpenICC
    PROPERTIES VERSION   ${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_MICRO_VERSION}
               SOVERSION ${${PROJECT_NAME}_MAJOR_VERSION}
                     )

SET( DOC_SOURCES
	doxymentation.dox
  openicc*.md
	${CFILES_OBJECT_CONFIG_PUBLIC}
	${CHEADERS_OBJECT_CONFIG_PUBLIC}
        PARENT_SCOPE
   )


# i18n
IF( GETTEXT_FOUND )
  SET(_potFile ${CMAKE_CURRENT_SOURCE_DIR}/../po/${PROJECT_DOWN_NAME}.pot)
  #SET( _xgettext_option_list --language=C --keyword=_ --keyword=N_ --keyword=C_:1c,2 --keyword=NC_:1c,2 -s --package-name=${PROJECT_DOWN_NAME} --package-version=${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION} )
  SET( _xgettext_option_list --add-comments --keyword=gettext --flag=gettext:1:pass-c-format --keyword=_ --flag=_:1:pass-c-format --keyword=N_ --flag=N_:1:pass-c-format  --copyright-holder='Kai-Uwe Behrmann'  --msgid-bugs-address='ku.b@gmx.de' --from-code=utf-8 --package-name=${PROJECT_DOWN_NAME} --package-version=${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION} )
  SET( XGETTEXT_CMD xgettext )
  FILE( GLOB SRC_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
        libopenicc/*.c
        tools/openicc_device.c
        tools/openicc_json.c
      )

  ADD_CUSTOM_COMMAND(OUTPUT ${_potFile}
    COMMAND ${XGETTEXT_CMD} ${_xgettext_option_list} -o ${_potFile} ${SRC_FILES}
    DEPENDS ${SRC_FILES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Extract translatable messages to ${_potFile}"
  )

  SET( LINGUAS ../po/cs.po ../po/de.po ../po/eo.po ../po/eu.po  ../po/fr.po ../po/ru.po )
  # compile translations always to have them ready for installation
  SET( GETTEXT_TRANSLATIONS_TARGET_PREFIX ${PROJECT_DOWN_NAME}_ )
  GETTEXT_CREATE_TRANSLATIONS( ${_potFile} ALL ${LINGUAS} )

  ADD_CUSTOM_COMMAND( OUTPUT ${PROJECT_DOWN_NAME}_gmo-cp
    COMMAND for lang in ${OI_LINGUAS}\; do mkdir -p "${CMAKE_BINARY_DIR}/locale/$$lang/LC_MESSAGES/" \; cp "${CMAKE_CURRENT_BINARY_DIR}/$$lang.gmo" "${CMAKE_BINARY_DIR}/locale/$$lang/LC_MESSAGES/${PROJECT_DOWN_NAME}.mo" \; done
    COMMENT "put ${PROJECT_DOWN_NAME}.mo files into ${CMAKE_BINARY_DIR}/locale/lang/LC_MESSAGES/ path for testing with LOCDIR"
                    )
  ADD_CUSTOM_TARGET( ${PROJECT_DOWN_NAME}_gmo ALL
                     DEPENDS ${GETTEXT_TRANSLATIONS_TARGET_PREFIX}translations ${PROJECT_DOWN_NAME}_gmo-cp  )
ENDIF()

