LINK_DIRECTORIES( ${LINK_DIRECTORIES} ${YAJL_LIBRARY_DIRS} )

SET( CFILES_OBJECT_IMAGING_PUBLIC
	${GENDIR}oyArray2d_s.c
	${GENDIR}oyConnectorImaging_s.c
	${GENDIR}oyConversion_s.c
	${GENDIR}oyNamedColor_s.c
	${GENDIR}oyNamedColors_s.c
	${GENDIR}oyImage_s.c
	${GENDIR}oyPixelAccess_s.c
	${GENDIR}${PACKAGE_NAME}_conversion.c
	${GENDIR}${PACKAGE_NAME}_image.c
   )
SET( CFILES_OBJECT_IMAGING_PUBLIC ${CFILES_OBJECT_IMAGING_PUBLIC} PARENT_SCOPE )
SET( CFILES_OBJECT_IMAGING
	${CFILES_OBJECT_IMAGING_PUBLIC}
	${GENDIR}oyArray2d_s_.c
	${GENDIR}oyConnectorImaging_s_.c
	${GENDIR}oyConversion_s_.c
	${GENDIR}oyNamedColor_s_.c
	${GENDIR}oyNamedColors_s_.c
	${GENDIR}oyImage_s_.c
	${GENDIR}oyPixelAccess_s_.c
	${PACKAGE_NAME}_alpha.c
	${PACKAGE_NAME}_cmm.c
   )
SET( CHEADERS_OBJECT_IMAGING_PUBLIC
	${GENDIR}oyArray2d_s.h
	${GENDIR}oyConnectorImaging_s.h
	${GENDIR}oyConversion_s.h
	${GENDIR}oyNamedColor_s.h
	${GENDIR}oyNamedColors_s.h
	${GENDIR}oyImage_s.h
	${GENDIR}oyPixelAccess_s.h
	${GENDIR}${PACKAGE_NAME}_conversion.h
	${GENDIR}${PACKAGE_NAME}_image.h
	${GENDIR}${PACKAGE_NAME}_profile.h
   )
SET( CHEADERS_OBJECT_IMAGING_PUBLIC ${CHEADERS_OBJECT_IMAGING_PUBLIC} PARENT_SCOPE )
SET( CHEADERS_OBJECT_IMAGING
	${GENDIR}oyArray2d_s_.h
	${GENDIR}oyConnectorImaging_s_.h
	${GENDIR}oyConversion_s_.h
	${GENDIR}oyNamedColor_s_.h
	${GENDIR}oyNamedColors_s_.h
	${GENDIR}oyImage_s_.h
	${GENDIR}oyPixelAccess_s_.h
	${GENDIR}${PACKAGE_NAME}_conversion_internal.h
	${GENDIR}${PACKAGE_NAME}_image_internal.h
	${CHEADERS_OBJECT_IMAGING_PUBLIC}
   )

SET( SOURCES ${CFILES_OBJECT_IMAGING} ${CHEADERS_OBJECT_IMAGING} )

#SET( EXTRA_LIBS ${EXTRA_LIBS} ${YAJL_LIBRARIES} )

FILE( GLOB icc_sub_dirs ${OY_ICC_DIR}/* )
FOREACH(icc_sub_dir ${icc_sub_dirs} )
  FILE( GLOB profiles ${icc_sub_dir}/* )
    GET_FILENAME_COMPONENT(group ${icc_sub_dir} NAME_WE)
  SET( OYRANOS_ALL_ICC ${OYRANOS_ALL_ICC} ${profiles} )
  SET_PROPERTY( SOURCE ${profiles}
                PROPERTY MACOSX_PACKAGE_LOCATION Resources/share/color/icc/${group} )
ENDFOREACH(icc_sub_dir)
MESSAGE( "OYRANOS_ALL_ICC ${OYRANOS_ALL_ICC}" )

ADD_LIBRARY( ${PROJECT_NAME} SHARED ${SOURCES} ${CHEADERS_OBJECT_IMAGING_PUBLIC} ${RANK_MAPS_PUBLIC} ${SETTINGS_PUBLIC} ${OYRANOS_ALL_ICC} )
TARGET_LINK_LIBRARIES (
   ${PROJECT_NAME}
   ${PROJECT_NAME}Config ${PROJECT_NAME}Modules ${PROJECT_NAME}Object ${PROJECT_NAME}Core ${EXTRA_LIBS} )
ADD_LIBRARY( oyranos-static STATIC ${SOURCES} ${CHEADERS_OBJECT_IMAGING_PUBLIC} )
TARGET_LINK_LIBRARIES (
   oyranos-static
   oyranos_config-static oyranos_modules-static oyranos_object-static oyranos_core-static ${EXTRA_LIBS} )

# OS X Framework
FOREACH( policy ${SETTINGS_PUBLIC} )
  SET_PROPERTY( SOURCE ${policy}
                PROPERTY MACOSX_PACKAGE_LOCATION Resources/share/color/settings
              )
ENDFOREACH( policy )
FOREACH( rankmap ${RANK_MAPS_PUBLIC} )
  SET_PROPERTY( SOURCE ${rankmap}
                PROPERTY MACOSX_PACKAGE_LOCATION Resources/share/color/rank-map
              )
ENDFOREACH( rankmap )

SET_TARGET_PROPERTIES( ${PROJECT_NAME} PROPERTIES
  FRAMEWORK TRUE
  FRAMEWORK_VERSION ${OYRANOS_VERSION}
  PRIVATE_HEADER ""
  PUBLIC_HEADER "${CHEADERS_OBJECT_IMAGING_PUBLIC}"
  RESOURCE ""
#  INSTALL_NAME_DIR "@executable_path/../../../../${FRAMEWORK_DIR}"
#  DEBUG_POSTFIX -d
)

INSTALL( TARGETS       ${PROJECT_NAME} oyranos-static
         RUNTIME       DESTINATION bin
         FRAMEWORK     DESTINATION ${FRAMEWORK_DIR}
         LIBRARY       DESTINATION ${LIB_DIR}
         ARCHIVE       DESTINATION lib${LIB_SUFFIX}
         PUBLIC_HEADER DESTINATION include/${PACKAGE_NAME}
         RESOURCE      DESTINATION share/color/settings
       )

# install for all but app bundles
INSTALL( FILES       ${SETTINGS_PUBLIC}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/share/color/settings
       )

