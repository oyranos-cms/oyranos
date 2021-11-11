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

SET( MAN1_PAGES
     man/openicc-device.1
   )
SET( MAN1DE_PAGES
     man/de/openicc-device.1
   )

INSTALL( FILES ${MAN1_PAGES} DESTINATION ${CMAKE_INSTALL_MANDIR}/man1 )
INSTALL( FILES ${MAN1DE_PAGES} DESTINATION ${CMAKE_INSTALL_MANDIR}/de/man1 )

FIND_PACKAGE( Doxygen )
IF( DOXYGEN_FOUND )

  ADD_CUSTOM_COMMAND( OUTPUT html
	  COMMAND cat ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.files | grep ^FILE_PATTERNS | sed 's%\;% %g' >> Doxyfile.dox \; doxygen Doxyfile.dox \; cp ${CMAKE_SOURCE_DIR}/extras/openicc_logo.png html/ \;
                    )

  ADD_CUSTOM_TARGET( docu ALL
                     DEPENDS html )

  INSTALL( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION ${CMAKE_INSTALL_DOCDIR} )
ENDIF()

