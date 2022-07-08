IF(ENABLE_DOCU_${PROJECT_UP_NAME})
  SET( DOC_SOURCES ${DOC_SOURCES}
        ${CHEADERS_OYJL_PUBLIC}
        ${CMAKE_SOURCE_DIR}/oyjl_args.c
        ${CMAKE_SOURCE_DIR}/oyjl_args_tree.c
        ${CMAKE_SOURCE_DIR}/oyjl_core.c
        ${CMAKE_SOURCE_DIR}/oyjl_tree.c
        ${CMAKE_SOURCE_DIR}/oyjl_yajl.c
        ${CFILES_OYJL_CORE_PUBLIC}
        ${CFILES_OYJL_YAJL_PUBLIC}
        ${CFILES_OYJL_EXTRA}
        ${CMAKE_CURRENT_SOURCE_DIR}/doxymentation.dox
        ${CMAKE_CURRENT_SOURCE_DIR}/md/*.md
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

FILE( GLOB MAN1_PAGES_${PROJECT_UP_NAME} man/*.1 )
FILE( GLOB MAN1DE_PAGES_${PROJECT_UP_NAME} man/de/*.1 )
FILE( GLOB MAN3_PAGES_${PROJECT_UP_NAME} man/*.3 )
INSTALL( FILES ${MAN1_PAGES_${PROJECT_UP_NAME}} DESTINATION ${CMAKE_INSTALL_MANDIR}/man1 )
INSTALL( FILES ${MAN1DE_PAGES_${PROJECT_UP_NAME}} DESTINATION ${CMAKE_INSTALL_MANDIR}/de/man1 )
INSTALL( FILES ${MAN3_PAGES_${PROJECT_UP_NAME}} DESTINATION ${CMAKE_INSTALL_MANDIR}/man3 )

FIND_PACKAGE( Doxygen )
IF( DOXYGEN_FOUND )

  ADD_CUSTOM_COMMAND( OUTPUT html
	  COMMAND cat ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.files | grep ^FILE_PATTERNS | sed 's%\;% %g' >> Doxyfile.dox \; doxygen Doxyfile.dox \; cp ${CMAKE_SOURCE_DIR}/oyjl-args-qml/images/logo-sw.svg html/ \; cp ${CMAKE_CURRENT_SOURCE_DIR}/customdoxygen.css html/\; cp ${CMAKE_CURRENT_SOURCE_DIR}/layout.css html/
                    )

  ADD_CUSTOM_TARGET( docu ALL
                     DEPENDS html )

  INSTALL( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION ${CMAKE_INSTALL_DOCDIR} )
ENDIF()
