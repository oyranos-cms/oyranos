  char             cmm[8];             /*!< ICC signature, eg 'lcms' */
  char           * backend_version;    /*!< non translatable, eg "v1.17" */
  /** translated, e.g. "name": "lcms" "little cms" "A CMM with 100k ..."
   *  supported should be "name", "copyright" and "manufacturer".
   *  Optional is "help".
   */
  oyCMMGetText_f   getText;
  char          ** texts;              /**< zero terminated list for getText */

  int              oy_compatibility;   /*!< last supported Oyranos CMM API : OYRANOS_VERSION */
  oyCMMapi_s     * api;                /**< must be casted to a according API, zero terminated list */

  oyIcon_s       * icon;               /*!< module GUI icon */

  oyCMMinfoInit_f  init;      /**< should be called before any access to members of oyCMMinfo_s */
  oyCMMinfoReset_f reset;     /**< should be called after last access to members of oyCMMinfo_s */
