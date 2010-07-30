project(grantlee_extensions)

INCLUDE(${Grantlee_USE_FILE})

grantlee_add_plugin(grantlee_gsoc_extensions
  custom_plugin_library

  FILTERS
    string_filters
)
