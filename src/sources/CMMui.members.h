  /** 0: major - should be stable for the live time of a filters UI, \n
      1: minor - mark new features, \n
      2: patch version - correct errors */
  int32_t          version[3];

  /** 0: last major Oyranos version during development time, e.g. 0
   *  1: last minor Oyranos version during development time, e.g. 0
   *  2: last Oyranos patch version during development time, e.g. 10
   */
  int32_t          module_api[3];

  /** check options for validy and correct */
  oyCMMFilter_ValidateOptions_f    oyCMMFilter_ValidateOptions;
  oyWidgetEvent_f                  oyWidget_Event; /**< handle widget events */

  const char     * category;           /**< menu structure */
  const char     * options;            /**< default options as Elektra/XFORMS 
                                        *   compatible XML */
  oyCMMuiGet_f     oyCMMuiGet;         /**< xml ui elements for filter options*/

  oyCMMGetText_f   getText;            /**< translated UI texts */
  /** zero terminated categories for getText,
   *  e.g. {"name","category","help",0}
   *
   *  The "name" texts shall include information about the module.
   *  The "category" gives in oyNAME_NAME a translated version of
   *  oyCMMui_s::category.
   *  The "help" texts should provide general infromations about the module.
   *
   *  The oyNAME_NICK for the several oyCMMui_s::getText() texts is typical not
   *  translated. For "name" the oyNAME_NICK should be the module name. For 
   *  other texts like "help" and "category" oyNAME_NICK makes no sense.
   */
  const char    ** texts;
  oyCMMapiFilter_s*parent;             /**< link to the owner of this structure */
