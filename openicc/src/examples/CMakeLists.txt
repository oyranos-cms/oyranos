# Device listing
ADD_EXECUTABLE( openicc-config-read "${CMAKE_CURRENT_SOURCE_DIR}/openicc_config_read.c" )
TARGET_LINK_LIBRARIES( openicc-config-read OpenICC ${EXTRA_LIBS} )
#INSTALL (TARGETS openicc-config-read DESTINATION bin)

