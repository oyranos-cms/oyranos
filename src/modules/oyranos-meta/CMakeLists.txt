# --- dependencies ---

# --- variables ---

# --- normal modules ---

# --- meta modules ---
SET( OY_MODULE oyIM )
SET( OY_MODULE_LIB ${PACKAGE_NAME}_${OY_MODULE}${OY_MODULE_NAME} )
ADD_LIBRARY( ${OY_MODULE_LIB}
             SHARED
             ${PACKAGE_NAME}_cmm_${OY_MODULE}.c
             ${PACKAGE_NAME}_cmm_${OY_MODULE}_meta_c.c
             ${PACKAGE_NAME}_cmm_${OY_MODULE}_profile.c)
#SET( EXTRA_LIBS ${EXTRA_LIBS} ${YAJL_LIBRARIES} )
TARGET_LINK_LIBRARIES ( ${OY_MODULE_LIB} ${PROJECT_NAME} ${EXTRA_LIBS} )
INSTALL (TARGETS ${OY_MODULE_LIB} DESTINATION lib${LIB_SUFFIX}/${OY_METASUBPATH})

