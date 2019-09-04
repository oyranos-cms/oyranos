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
 *  The returned struct must be casted to a according API. 
 *  oyCMMapi_s forms a zero terminated list accesable through oyCMMapi_s::oyCMMapi_GetNext().
 *  @see oyCMMinfo_SetApi()
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

/** Function  oyCMMinfo_SetApi
 *  @memberof oyCMMinfo_s
 *  @brief    Set the first api
 *
 *  Set a zero terminated list, which will be visible by the module loader.
 *  You can do this inside the oyCMMinfo_s init function setable by
 *  oyCMMinfo_SetInitF().
 *  Check if oyCMMinfo_s has already an api set 
 *  through oyCMMinfo_GetApi(). Append more oyCMMapi_s 's through oyCMMapi_s::oyCMMapi_SetNext().
 *
 *  @version  Oyranos: 0.9.5
 *  @date     2013/06/11
 *  @since    2013/06/11 (Oyranos: 0.9.5)
 */
OYAPI void  OYEXPORT
                   oyCMMinfo_SetApi  ( oyCMMinfo_s       * info,
                                       oyCMMapi_s        * first_api )
{
  ((oyCMMinfo_s_*)info)->api = first_api;
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
  return ((oyCMMinfo_s_*)info)->icon;
}

/** Function  oyCMMinfo_GetInitF
 *  @memberof oyCMMinfo_s
 *  @brief    Get a init function
 *
 *  Get a init function for the actual oyCMMinfo_s.
 *  @see oyCMMinfo_SetInitF
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
/** Function  oyCMMinfo_SetInitF
 *  @memberof oyCMMinfo_s
 *  @brief    Set a init function
 *
 *  Set a init function for the actual oyCMMinfo_s.
 *  @see oyCMMinfo_SetApi() oyCMMinfo_GetInitF()
 *
 *  @version  Oyranos: 0.9.5
 *  @date     2013/06/11
 *  @since    2013/06/11 (Oyranos: 0.9.5)
 */
OYAPI void  OYEXPORT
                   oyCMMinfo_SetInitF( oyCMMinfo_s       * info,
                                       oyCMMinfoInit_f     init )
{
  ((oyCMMinfo_s_*)info)->init = init;
}

/** Function  oyCMMinfo_GetResetF
 *  @memberof oyCMMinfo_s
 *  @brief    Get a reset function
 *
 *  Get a reset function for the actual oyCMMinfo_s.
 *  @see oyCMMinfo_SetResetF
 *
 *  @version  Oyranos: 0.9.7
 *  @date     2019/09/03
 *  @since    2013/06/10 (Oyranos: 0.9.5)
 */
OYAPI oyCMMinfoReset_f  OYEXPORT
                   oyCMMinfo_GetResetF(oyCMMinfo_s       * info )
{
  return ((oyCMMinfo_s_*)info)->reset;
}
/** Function  oyCMMinfo_SetResetF
 *  @memberof oyCMMinfo_s
 *  @brief    Set a reset function
 *
 *  Set a reset function for the actual oyCMMinfo_s.
 *  @see oyCMMinfo_SetApi() oyCMMinfo_GetResetF()
 *
 *  @version  Oyranos: 0.9.7
 *  @date     2019/09/03
 *  @since    2013/06/10 (Oyranos: 0.9.5)
 */
OYAPI void  OYEXPORT
                   oyCMMinfo_SetResetF(oyCMMinfo_s       * info,
                                       oyCMMinfoReset_f    reset )
{
  ((oyCMMinfo_s_*)info)->reset = reset;
}
