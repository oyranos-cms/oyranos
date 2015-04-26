# --- dependencies ---

# --- variables ---
SET( OY_MODULE_NAME "_cmm_module" )

# --- normal modules ---

ADD_SUBDIRECTORY( color/modules )

# --- meta modules ---

ADD_SUBDIRECTORY( oyranos-meta )

SET( OY_MODULE_LIBRARIES ${OY_MODULE_LIBRARIES} PARENT_SCOPE )
