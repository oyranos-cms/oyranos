#oFORMS basics
SET( OY_OFORMS_LIB ${PACKAGE_NAME}_forms_cmd_line-static )
SET( OY_OFORMS_LIB ${OY_OFORMS_LIB} PARENT_SCOPE )
ADD_LIBRARY( ${OY_OFORMS_LIB}
             STATIC ${PACKAGE_NAME}_forms_cmd_line.c )
TARGET_LINK_LIBRARIES ( ${OY_OFORMS_LIB} oyranos ${EXTRA_LIBS} )

MESSAGE( "-- OY_OFORMS_LIB: ${OY_OFORMS_LIB}" )

INSTALL (FILES ./oyranos-xforms.1 DESTINATION share/man/man1 )
INSTALL (FILES ./oyranos-xforms-modules.1 DESTINATION share/man/man1 )
INSTALL (FILES ./oyranos-xforms-fltk.1 DESTINATION share/man/man1 )

