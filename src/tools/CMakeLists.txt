# ICC profiles listing
LINK_DIRECTORIES( ${LINK_DIRECTORIES} "${CMAKE_BINARY_DIR}/src/liboyranos" )
ADD_EXECUTABLE( oyranos-profiles "${CMAKE_CURRENT_SOURCE_DIR}/oyranos_profiles.c" )
TARGET_LINK_LIBRARIES( oyranos-profiles oyranos )
INSTALL (TARGETS oyranos-profiles DESTINATION bin)

# ICC profile information and manipulations
LINK_DIRECTORIES( ${LINK_DIRECTORIES} "${CMAKE_BINARY_DIR}/src/liboyranos" )
ADD_EXECUTABLE( oyranos-profile "${CMAKE_CURRENT_SOURCE_DIR}/oyranos_profile.c" )
TARGET_LINK_LIBRARIES( oyranos-profile oyranos )
INSTALL (TARGETS oyranos-profile DESTINATION bin)

# Oyranos policy / grouping for easy switching, export, import of default settings
LINK_DIRECTORIES( ${LINK_DIRECTORIES} "${CMAKE_BINARY_DIR}/src/liboyranos" )
ADD_EXECUTABLE( oyranos-policy "${CMAKE_CURRENT_SOURCE_DIR}/oyranos_policy.c" )
TARGET_LINK_LIBRARIES( oyranos-policy oyranos )
INSTALL (TARGETS oyranos-policy DESTINATION bin)

# automatic monitor profile selection, information, Taxi DB, EDID fallback
LINK_DIRECTORIES( ${LINK_DIRECTORIES} "${CMAKE_BINARY_DIR}/src/liboyranos" )
ADD_EXECUTABLE( oyranos-monitor "${CMAKE_CURRENT_SOURCE_DIR}/oyranos_gamma.c" )
TARGET_LINK_LIBRARIES ( oyranos-monitor oyranos ${EXTRA_LIBS}
               ${XINERAMA_LIBRARIES} ${XRANDR_LIBRARIES} ${XCM_LIBRARIES}
               ${XFIXES_LIBRARIES} ${X11_LIBRARIES} )
INSTALL (TARGETS oyranos-monitor DESTINATION bin)
