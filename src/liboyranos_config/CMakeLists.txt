SET( CFILES_OBJECT_CONFIG_GEN_PUBLIC
	${GENDIR}oyConfig_s.c
	${GENDIR}oyConfigs_s.c
	${GENDIR}oyProfile_s.c
	${GENDIR}oyProfiles_s.c
	${GENDIR}oyProfileTag_s.c
	${GENDIR}${PACKAGE_NAME}_devices.c
	${GENDIR}${PACKAGE_NAME}_devices_internal.c
   )
SET( CFILES_OBJECT_CONFIG_GEN_PUBLIC ${CFILES_OBJECT_CONFIG_GEN_PUBLIC} PARENT_SCOPE )
SET( CFILES_OBJECT_CONFIG_GEN
	${CFILES_OBJECT_CONFIG_GEN_PUBLIC}
	${GENDIR}oyConfig_s_.c
	${GENDIR}oyConfigs_s_.c
	${GENDIR}oyProfile_s_.c
	${GENDIR}oyProfiles_s_.c
	${GENDIR}oyProfileTag_s_.c
   )
SET( CFILES_OBJECT_CONFIG_PUBLIC
	${PACKAGE_NAME}.c
	${PACKAGE_NAME}_colour.c
	${PACKAGE_NAME}_icc.c
   )
SET( CFILES_OBJECT_CONFIG_PUBLIC ${CFILES_OBJECT_CONFIG_PUBLIC} PARENT_SCOPE )
SET( CFILES_OBJECT_CONFIG
	${CFILES_OBJECT_CONFIG_PUBLIC}
	${PACKAGE_NAME}_check.c
	${PACKAGE_NAME}_elektra.c
	${PACKAGE_NAME}_io.c
	${PACKAGE_NAME}_widgets.c
	${PACKAGE_NAME}_sentinel.c
	${PACKAGE_NAME}_texts.c
	${PACKAGE_NAME}_xml.c
        ../oyjl/oyjl_tree_parse.c
	${CFILES_OBJECT_CONFIG_GEN}
   )
SET( CHEADERS_OBJECT_CONFIG_PUBLIC
	${GENDIR}oyConfig_s.h
	${GENDIR}oyConfigs_s.h
	${GENDIR}oyProfile_s.h
	${GENDIR}oyProfiles_s.h
	${GENDIR}oyProfileTag_s.h
	${GENDIR}${PACKAGE_NAME}_devices.h
   )
SET( CHEADERS_OBJECT_CONFIG_PUBLIC ${CHEADERS_OBJECT_CONFIG_PUBLIC} PARENT_SCOPE )
SET( CHEADERS_OBJECT_CONFIG
	${GENDIR}oyConfig_s_.h
	${GENDIR}oyConfigs_s_.h
	${GENDIR}oyProfile_s_.h
	${GENDIR}oyProfiles_s_.h
	${GENDIR}oyProfileTag_s_.h
	${GENDIR}${PACKAGE_NAME}_devices_internal.h
	${CHEADERS_OBJECT_CONFIG_PUBLIC}
   )


SET( SOURCES ${CFILES_OBJECT_CONFIG} ${CHEADERS_OBJECT_CONFIG} )

SET( EXTRA_LIBS ${EXTRA_LIBS} ${ELEKTRA_LIBRARIES} ${YAJL_LIBRARIES} )
ADD_LIBRARY( oyranos_config SHARED ${SOURCES} )
TARGET_LINK_LIBRARIES (
   oyranos_config
   oyranos_modules oyranos_object oyranos_core ${EXTRA_LIBS} )
ADD_LIBRARY( oyranos_config-static STATIC ${SOURCES} )
TARGET_LINK_LIBRARIES (
   oyranos_config-static
   oyranos_modules-static oyranos_object-static oyranos_core-static ${EXTRA_LIBS} )

INSTALL (TARGETS oyranos_config oyranos_config-static DESTINATION lib${LIB_SUFFIX})
INSTALL (FILES ${CHEADERS_OBJECT_CONFIG_PUBLIC} DESTINATION include/${PACKAGE_NAME})


