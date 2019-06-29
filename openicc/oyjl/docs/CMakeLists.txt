IF(ENABLE_DOCU_${PROJECT_UP_NAME})
  SET( DOC_SOURCES ${DOC_SOURCES}
        ${CHEADERS_OYJL_PUBLIC}
        ${CMAKE_SOURCE_DIR}/oyjl_args.c
        ${CMAKE_SOURCE_DIR}/oyjl_args_tree.c
        ${CMAKE_SOURCE_DIR}/oyjl_core.c
        ${CMAKE_SOURCE_DIR}/oyjl_tree.c
        ${CMAKE_SOURCE_DIR}/oyjl_yajl.c
        ${CMAKE_CURRENT_SOURCE_DIR}/doxymentation.dox
     )
ENDIF(ENABLE_DOCU_${PROJECT_UP_NAME})

CONFIGURE_FILE(
                "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in"
                "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.dox"
                @ONLY
       )
CONFIGURE_FILE(
                "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.files.in"
                "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.files"
                @ONLY
       )

FIND_PACKAGE( Doxygen )
IF( DOXYGEN_FOUND )

  ADD_CUSTOM_COMMAND( OUTPUT html
	  COMMAND cat ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.files | grep ^FILE_PATTERNS | sed 's%\;% %g' >> Doxyfile.dox \; doxygen Doxyfile.dox \; cp ${CMAKE_SOURCE_DIR}/oyjl-args-qml/images/logo-sw.svg html/ \; cp ${CMAKE_CURRENT_SOURCE_DIR}/customdoxygen.css html/
                    )

  ADD_CUSTOM_TARGET( docu ALL
                     DEPENDS html )

  INSTALL( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION ${CMAKE_INSTALL_DOCDIR} )
ENDIF()
