SET( CFILES_OBJECT_MODULES_PUBLIC
	${GENDIR}oyCMMapi_s.c
	${GENDIR}oyCMMapi4_s.c
	${GENDIR}oyCMMapi7_s.c
	${GENDIR}oyCMMapis_s.c
	${GENDIR}oyCMMapiFilter_s.c
	${GENDIR}oyCMMapiFilters_s.c
	${GENDIR}oyCMMui_s.c
	${GENDIR}oyConnector_s.c
	${GENDIR}oyFilterCore_s.c
	${GENDIR}oyFilterGraph_s.c
	${GENDIR}oyFilterNode_s.c
	${GENDIR}oyFilterNodes_s.c
	${GENDIR}oyFilterSocket_s.c
	${GENDIR}oyFilterPlug_s.c
	${GENDIR}oyFilterPlugs_s.c
	${GENDIR}${PACKAGE_NAME}_module.c
	${PACKAGE_NAME}_threads.c
   )
SET( CFILES_OBJECT_MODULES_PUBLIC ${CFILES_OBJECT_MODULES_PUBLIC} PARENT_SCOPE )
SET( CFILES_OBJECT_MODULES
	${CFILES_OBJECT_MODULES_PUBLIC}
	${GENDIR}oyCMMapi_s_.c
	${GENDIR}oyCMMapi3_s.c
	${GENDIR}oyCMMapi3_s_.c
	${GENDIR}oyCMMapi4_s_.c
	${GENDIR}oyCMMapi5_s.c
	${GENDIR}oyCMMapi5_s_.c
	${GENDIR}oyCMMapi6_s.c
	${GENDIR}oyCMMapi6_s_.c
	${GENDIR}oyCMMapi7_s_.c
	${GENDIR}oyCMMapi8_s.c
	${GENDIR}oyCMMapi8_s_.c
	${GENDIR}oyCMMapi9_s.c
	${GENDIR}oyCMMapi9_s_.c
	${GENDIR}oyCMMapi10_s.c
	${GENDIR}oyCMMapi10_s_.c
	${GENDIR}oyCMMapis_s_.c
	${GENDIR}oyCMMapiFilter_s_.c
	${GENDIR}oyCMMapiFilters_s_.c
	${GENDIR}oyCMMinfo_s.c
	${GENDIR}oyCMMinfo_s_.c
	${GENDIR}oyCMMobjectType_s.c
	${GENDIR}oyCMMobjectType_s_.c
	${GENDIR}oyCMMui_s_.c
	${GENDIR}oyConnector_s_.c
	${GENDIR}oyFilterCore_s_.c
	${GENDIR}oyFilterGraph_s_.c
	${GENDIR}oyFilterNode_s_.c
	${GENDIR}oyFilterNodes_s_.c
	${GENDIR}oyFilterSocket_s_.c
	${GENDIR}oyFilterPlug_s_.c
	${GENDIR}oyFilterPlugs_s_.c
	${PACKAGE_NAME}_forms.c
   )
SET( CHEADERS_OBJECT_MODULES_PUBLIC
	${GENDIR}oyCMMapi_s.h
	${GENDIR}oyCMMapi4_s.h
	${GENDIR}oyCMMapi7_s.h
	${GENDIR}oyCMMapis_s.h
	${GENDIR}oyCMMapiFilter_s.h
	${GENDIR}oyCMMapiFilters_s.h
	${GENDIR}oyCMMui_s.h
	${GENDIR}oyConnector_s.h
	${GENDIR}oyFilterCore_s.h
	${GENDIR}oyFilterGraph_s.h
	${GENDIR}oyFilterNode_s.h
	${GENDIR}oyFilterNodes_s.h
	${GENDIR}oyFilterSocket_s.h
	${GENDIR}oyFilterPlug_s.h
	${GENDIR}oyFilterPlugs_s.h
	${GENDIR}${PACKAGE_NAME}_module.h
        ../include/oyCMM_s.h
        ../include/${PACKAGE_NAME}_threads.h
   )
SET( CHEADERS_OBJECT_MODULES_PUBLIC ${CHEADERS_OBJECT_MODULES_PUBLIC} PARENT_SCOPE )
SET( CHEADERS_OBJECT_MODULES
	${GENDIR}oyCMMapi3_s.h
	${GENDIR}oyCMMapi5_s.h
	${GENDIR}oyCMMapi6_s.h
	${GENDIR}oyCMMapi8_s.h
	${GENDIR}oyCMMapi9_s.h
	${GENDIR}oyCMMapi10_s.h
	${GENDIR}oyCMMinfo_s.h
	${GENDIR}oyCMMobjectType_s.h
	${GENDIR}oyCMMapi_s_.h
	${GENDIR}oyCMMapi3_s_.h
	${GENDIR}oyCMMapi4_s_.h
	${GENDIR}oyCMMapi5_s_.h
	${GENDIR}oyCMMapi6_s_.h
	${GENDIR}oyCMMapi7_s_.h
	${GENDIR}oyCMMapi8_s_.h
	${GENDIR}oyCMMapi9_s_.h
	${GENDIR}oyCMMapi10_s_.h
	${GENDIR}oyCMMapis_s_.h
	${GENDIR}oyCMMapiFilter_s_.h
	${GENDIR}oyCMMapiFilters_s_.h
	${GENDIR}oyCMMinfo_s_.h
	${GENDIR}oyCMMobjectType_s_.h
	${GENDIR}oyCMMui_s_.h
	${GENDIR}oyConnector_s_.h
	${GENDIR}oyFilterCore_s_.h
	${GENDIR}oyFilterGraph_s_.h
	${GENDIR}oyFilterNode_s_.h
	${GENDIR}oyFilterNodes_s_.h
	${GENDIR}oyFilterPlug_s_.h
	${GENDIR}oyFilterPlugs_s_.h
	${GENDIR}oyFilterSocket_s_.h
	${GENDIR}${PACKAGE_NAME}_module_internal.h
	${CHEADERS_OBJECT_MODULES_PUBLIC}
   )


SET( SOURCES ${CFILES_OBJECT_MODULES} ${CHEADERS_OBJECT_MODULES} )

# SET( EXTRA_LIBS ${EXTRA_LIBS} ${LIBXML2_LIBRARIES} )
ADD_LIBRARY( ${PROJECT_NAME}Modules SHARED ${SOURCES} )
TARGET_LINK_LIBRARIES (
   ${PROJECT_NAME}Modules
   ${PROJECT_NAME}Object ${PROJECT_NAME}Core ${EXTRA_LIBS} )
ADD_LIBRARY( oyranos_modules-static STATIC ${SOURCES} )
TARGET_LINK_LIBRARIES (
   oyranos_modules-static
   oyranos_object-static oyranos_core-static ${EXTRA_LIBS} )

# OS X Framework
SET_TARGET_PROPERTIES( ${PROJECT_NAME}Modules PROPERTIES
  FRAMEWORK TRUE
  FRAMEWORK_VERSION ${OYRANOS_VERSION}
  PRIVATE_HEADER ""
  PUBLIC_HEADER "${CHEADERS_OBJECT_MODULES_PUBLIC}"
  RESOURCE ""
#  INSTALL_NAME_DIR "@executable_path/../../../../${FRAMEWORK_DIR}"
#  DEBUG_POSTFIX -d
)

INSTALL( TARGETS ${PROJECT_NAME}Modules oyranos_modules-static
         RUNTIME DESTINATION bin
         FRAMEWORK DESTINATION ${FRAMEWORK_DIR}
         LIBRARY DESTINATION ${LIB_DIR}
         ARCHIVE DESTINATION lib${LIB_SUFFIX}
         PUBLIC_HEADER DESTINATION include/${PACKAGE_NAME}
       )
