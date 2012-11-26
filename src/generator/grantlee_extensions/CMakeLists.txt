project(grantlee_extensions)

INCLUDE(${Grantlee_USE_FILE})

grantlee_add_plugin(gsoc_extensions
  custom_plugin_library

  FILTERS
    string_filters
)
