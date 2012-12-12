ADD_CUSTOM_COMMAND( OUTPUT html
	COMMAND test -n 'which doxygen' && ( cat ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.files | grep ^FILE_PATTERNS | sed 's%\;/home/kuwe/programme/CMS/Oyranos/Entwickeln/src/API_generated/% %g \; s%\;% %g' >> Doxyfile \; doxygen Doxyfile \; cp ${CMAKE_SOURCE_DIR}/extras/icons/${PACKAGE_NAME}_logo.png html/ \; cp ${CMAKE_CURRENT_SOURCE_DIR}/doxygen.css html/ \; doxygen ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile_alpha \; cp ${CMAKE_SOURCE_DIR}/extras/icons/${PACKAGE_NAME}_logo.png doc_alpha/html/) || echo "Will not create Documentation because doxygen is missing"
                  )

ADD_CUSTOM_TARGET( docu ALL
                   DEPENDS html )

CONFIGURE_FILE(
                "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in"
                "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
                @ONLY
       )
CONFIGURE_FILE(
                "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.files.in"
                "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.files"
                @ONLY
       )
CONFIGURE_FILE(
                "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile_alpha.in"
                "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile_alpha"
                @ONLY
       )

SET( MAN1_PAGES
     man/oyranos-monitor.1
     man/oyranos-monitor-daemon.1
     man/oyranos-policy.1
     man/oyranos-profile.1
     man/oyranos-profile-graph.1
     man/oyranos-profile-install.1
     man/oyranos-profiles.1
     man/qcmsevents.1
   )
SET( MAN3_PAGES
     man/oyranos.3
     man/oyranos-config.3
   )

INSTALL( FILES ${MAN1_PAGES} DESTINATION share/man/man1 )
INSTALL( FILES ${MAN3_PAGES} DESTINATION share/man/man3 )
INSTALL( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/doc/${PACKAGE_NAME} )
