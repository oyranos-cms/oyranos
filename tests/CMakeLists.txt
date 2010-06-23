PROJECT(gsoc_2010_tests)

INCLUDE(${QT_USE_FILE})

# ... Application sources etc.
ADD_EXECUTABLE( qregexp "regexp.cc" )
TARGET_LINK_LIBRARIES(
   qregexp
   ${QT_LIBRARIES}
)
