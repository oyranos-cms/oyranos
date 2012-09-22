/** Function  oyCMMInfo_GetCMM
 *  @memberof oyCMMInfo_s
 *  @brief    Get CMM signature, e.g. "lcms"
 *
 *  It contains only four bytes
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/14
 *  @since    2012/09/14 (Oyranos: 0.5.0)
 */
OYAPI const char *  OYEXPORT
                   oyCMMInfo_GetCMM  ( oyCMMInfo_s       * info )
{
  return ((oyCMMInfo_s_*)info)->cmm;
}
/** Function  oyCMMInfo_GetVersion
 *  @memberof oyCMMInfo_s
 *  @brief    Get Backend Version
 *
 *  non translatable, eg "v1.17"
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/14
 *  @since    2012/09/14 (Oyranos: 0.5.0)
 */
OYAPI const char *  OYEXPORT
                   oyCMMInfo_GetVersion
                                     ( oyCMMInfo_s       * info )
{
  return ((oyCMMInfo_s_*)info)->backend_version;
}
/** Function  oyCMMInfo_GetTextF
 *  @memberof oyCMMInfo_s
 *  @brief    Get oyCMMGetText_f function
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/14
 *  @since    2012/09/14 (Oyranos: 0.5.0)
 */
OYAPI oyCMMGetText_f  OYEXPORT
                   oyCMMInfo_GetTextF( oyCMMInfo_s       * info )
{
  return ((oyCMMInfo_s_*)info)->getText;
}
/** Function  oyCMMInfo_GetTexts
 *  @memberof oyCMMInfo_s
 *  @brief    Get keywords for oyCMMGetText_f function
 *
 *  zero terminated list for getText
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/14
 *  @since    2012/09/14 (Oyranos: 0.5.0)
 */
OYAPI const char **  OYEXPORT
                   oyCMMInfo_GetTexts( oyCMMInfo_s       * info )
{
  return ((oyCMMInfo_s_*)info)->texts;
}
/** Function  oyCMMInfo_GetComp
 *  @memberof oyCMMInfo_s
 *  @brief    Get Oyranos compatibility
 *
 *  last supported Oyranos CMM API : OYRANOS_VERSION
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/14
 *  @since    2012/09/14 (Oyranos: 0.5.0)
 */
OYAPI int  OYEXPORT
                   oyCMMInfo_GetCompatibility
                                     ( oyCMMInfo_s       * info )
{
  return ((oyCMMInfo_s_*)info)->oy_compatibility;
}
/** Function  oyCMMInfo_GetApi
 *  @memberof oyCMMInfo_s
 *  @brief    Get the first api
 *
 *  must be casted to a according API, zero terminated list
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/14
 *  @since    2012/09/14 (Oyranos: 0.5.0)
 */
OYAPI oyCMMapi_s * OYEXPORT
                   oyCMMInfo_GetApi  ( oyCMMInfo_s       * info )
{
  return ((oyCMMInfo_s_*)info)->api;
}

/** Function  oyCMMInfo_GetIcon
 *  @memberof oyCMMInfo_s
 *  @brief    Get icon
 *
 *  zero terminated list of a icon pyramid
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/09/14
 *  @since    2012/09/14 (Oyranos: 0.5.0)
 */
OYAPI oyIcon_s * OYEXPORT
                   oyCMMInfo_GetIcon ( oyCMMInfo_s       * info )
{
  return ((oyCMMInfo_s_*)info)->icon;
}
