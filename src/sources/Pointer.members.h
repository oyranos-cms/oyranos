  char               * lib_name;       /*!< the CMM */
  char               * func_name;      /*!< optional the CMM's function name */
  char               * id;             /*!< identification string */
  oyPointer            ptr;            /*!< a CMM's data pointer */
  int                  size;           /**< -1 && 0 - unknown, > 1 size of ptr*/
  char               * resource;       /**< the resource type, e.g. oyCMM_PROFILE, oyCMM_COLOUR_CONVERSION */
  oyPointer_release_f  ptrRelease;     /*!< CMM's deallocation function */
  int                  ref;            /**< Oyranos reference counter */
