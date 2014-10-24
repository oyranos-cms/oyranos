  /** check options for validy and correct */
  oyCMMFilter_ValidateOptions_f    oyCMMFilter_ValidateOptions;
  oyWidgetEvent_f              oyWidget_Event;     /**< handle widget events */

  const char     * options;            /**< default options */
  oyCMMuiGet_f     oyCMMuiGet;         /**< xml ui elements for filter options*/
  /**< XML namespace to describe the used data_types
   *   e.g. 'oy="http://www.oyranos.org/2009/oyranos"'
   */
  const char     * xml_namespace;

  oyCMMobjectType_s ** object_types;   /**< zero terminated list of types */

  oyCMMGetText_f   getText;            /**< describe selectors in UI */
  const char    ** texts;              /**< zero terminated categories for getText, e.g. {"///GPU","///CPU","//color",0} */

  oyConversion_Correct_f oyConversion_Correct; /**< check a graph */
  /** registration pattern which are supported by oyConversion_Correct \n
   *
   *  e.g. for imaging this could be "//imaging"
   */
  const char     * pattern;

  /** basic key which is used in the Oyranos DB together with "context",
   *  "renderer", "context_fallback" and "renderer_fallback" \n
   *
   *  e.g. for CMMs this could be OY_CMM_STD
   */
  const char     * key_base;

  /** get registration of fallback module for this pattern */
  oyCMMGetFallback_f oyCMMGetFallback;

  /** get name from module registration */
  oyCMMRegistrationToName_f oyCMMRegistrationToName;

  /** get the default pattern for a module group */
  oyCMMGetDefaultPattern_f oyCMMGetDefaultPattern;
