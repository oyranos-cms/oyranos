/** Function  oyCMMinfo_GetCMM
 *  @memberof oyCMMinfo_s
 *  @brief    Get CMM signature, e.g. "lcms"
 *
 *  It contains only four bytes
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/14
 *  @since    2012/09/14 (Oyranos: 0.5.0)
 */
OYAPI const char *  OYEXPORT
                   oyCMMinfo_GetCMM  ( oyCMMinfo_s       * info )
{
  return ((oyCMMinfo_s_*)info)->cmm;
}
/** Function  oyCMMinfo_GetVersion
 *  @memberof oyCMMinfo_s
 *  @brief    Get Backend Version
 *
 *  non translatable, eg "v1.17"
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/14
 *  @since    2012/09/14 (Oyranos: 0.5.0)
 */
OYAPI const char *  OYEXPORT
                   oyCMMinfo_GetVersion
                                     ( oyCMMinfo_s       * info )
{
  return ((oyCMMinfo_s_*)info)->backend_version;
}
/** Function  oyCMMinfo_GetTextF
 *  @memberof oyCMMinfo_s
 *  @brief    Get oyCMMGetText_f function
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/14
 *  @since    2012/09/14 (Oyranos: 0.5.0)
 */
OYAPI oyCMMGetText_f  OYEXPORT
                   oyCMMinfo_GetTextF( oyCMMinfo_s       * info )
{
  return ((oyCMMinfo_s_*)info)->getText;
}
/** Function  oyCMMinfo_GetTexts
 *  @memberof oyCMMinfo_s
 *  @brief    Get keywords for oyCMMGetText_f function
 *
 *  zero terminated list for getText
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/14
 *  @since    2012/09/14 (Oyranos: 0.5.0)
 */
OYAPI const char **  OYEXPORT
                   oyCMMinfo_GetTexts( oyCMMinfo_s       * info )
{
  return (const char **)((oyCMMinfo_s_*)info)->texts;
}
/** Function  oyCMMinfo_GetComp
 *  @memberof oyCMMinfo_s
 *  @brief    Get Oyranos compatibility
 *
 *  last supported Oyranos CMM API : OYRANOS_VERSION
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/14
 *  @since    2012/09/14 (Oyranos: 0.5.0)
 */
OYAPI int  OYEXPORT
                   oyCMMinfo_GetCompatibility
                                     ( oyCMMinfo_s       * info )
{
  return ((oyCMMinfo_s_*)info)->oy_compatibility;
}
/** Function  oyCMMinfo_GetApi
 *  @memberof oyCMMinfo_s
 *  @brief    Get the first api
 *
 *  must be casted to a according API, zero terminated list
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/14
 *  @since    2012/09/14 (Oyranos: 0.5.0)
 */
OYAPI oyCMMapi_s * OYEXPORT
                   oyCMMinfo_GetApi  ( oyCMMinfo_s       * info )
{
  return ((oyCMMinfo_s_*)info)->api;
}

/** Function  oyCMMinfo_GetIcon
 *  @memberof oyCMMinfo_s
 *  @brief    Get icon
 *
 *  zero terminated list of a icon pyramid
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/14
 *  @since    2012/09/14 (Oyranos: 0.5.0)
 */
OYAPI oyIcon_s * OYEXPORT
                   oyCMMinfo_GetIcon ( oyCMMinfo_s       * info )
{
  return &((oyCMMinfo_s_*)info)->icon;
}

/** Function  oyCMMinfo_GetInitF
 *  @memberof oyCMMinfo_s
 *  @brief    Get icon
 *
 *  zero terminated list of a icon pyramid
 *
 *  @version  Oyranos: 0.9.5
 *  @date     2013/06/10
 *  @since    2013/06/10 (Oyranos: 0.9.5)
 */
OYAPI oyCMMinfoInit_f  OYEXPORT
                   oyCMMinfo_GetInitF( oyCMMinfo_s       * info )
{
  return ((oyCMMinfo_s_*)info)->init;
}
