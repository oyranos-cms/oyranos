#oFORMS basics
SET( OY_OFORMS_LIB ${PACKAGE_NAME}_forms_cmd_line )
ADD_LIBRARY( ${OY_OFORMS_LIB}
             STATIC ${OY_OFORMS_LIB}.c )
TARGET_LINK_LIBRARIES ( ${OY_OFORMS_LIB} oyranos ${EXTRA_LIBS} )

# oFORMS for a Oyranos CML renderer and parser
ADD_EXECUTABLE( ../oyranos-xforms oyranos_xforms.c )
TARGET_LINK_LIBRARIES( ../oyranos-xforms
             oyranos ${OY_OFORMS_LIB} ${EXTRA_LIBS} )
#INSTALL (TARGETS oyranos-xforms DESTINATION bin)

# oFORMS for a Oyranos FLTK renderer and parser
IF( FLTK_FOUND )
  ADD_EXECUTABLE( ../oyranos-xforms-fltk
                  oyranos_forms_fltk.cxx
                  oyranos_widgets_fltk.cxx
                  oyranos_xforms_fltk.cxx
                )
  TARGET_LINK_LIBRARIES ( ../oyranos-xforms-fltk oyranos ${EXTRA_LIBS}
                 ${FLTK_LIBRARIES} )
  #INSTALL (TARGETS oyranos-xforms-fltk DESTINATION bin)
ENDIF()

# oFORMS for a Oyranos module generation
ADD_EXECUTABLE( ../oyranos-xforms-modules oyranos_xforms_modules.c )
TARGET_LINK_LIBRARIES( ../oyranos-xforms-modules
             oyranos ${OY_OFORMS_LIB} ${EXTRA_LIBS} )
INSTALL (TARGETS ../oyranos-xforms-modules DESTINATION bin)
INSTALL (FILES ./oyranos-xforms-modules.1 DESTINATION share/man/man1 )

