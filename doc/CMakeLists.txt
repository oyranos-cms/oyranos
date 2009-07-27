#add_custom_command( 	TARGET oyranos_oyRE_cmm_module
#							PRE_BUILD
#							COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/mkdoc.sh ARGS oyRE_help.html )
#add_custom_command( 	TARGET oyranos_SANE_cmm_module
#							PRE_BUILD
#							COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/mkdoc.sh ARGS SANE_help.html )
#message( "Hello!" )

# With these rules they always get built, even if *.html are up to date...
EXECUTE_PROCESS( COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/mkdoc.sh ${CMAKE_CURRENT_SOURCE_DIR}/oyRE_help.html )
EXECUTE_PROCESS( COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/mkdoc.sh ${CMAKE_CURRENT_SOURCE_DIR}/SANE_help.html )
