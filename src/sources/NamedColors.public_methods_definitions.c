/** @memberof oyNamedColors_s
 *  @brief    set name prefix
 *
 *  @param[in]     colors              Oyranos colors struct pointer
 *  @param[in]     string              prefix string
 *
 *  @version Oyranos: 0.9.5
 *  @date    2013/08/25
 *  @since   2013/08/25 (Oyranos: 0.9.5)
 */
void   oyNamedColors_SetPrefix       ( oyNamedColors_s   * colors,
                                       const char        * string )
{
  oyNamedColors_s_ * s = (oyNamedColors_s_*) colors;
  if(!colors)
    return;

  oyCheckType__m( oyOBJECT_NAMED_COLORS_S, return )

  if(s->prefix)
    oyObject_GetDeAlloc( s->oy_ )( &s->prefix );

  s->prefix = oyStringCopy_( string, oyObject_GetAlloc( s->oy_) );
}

/** @memberof oyNamedColors_s
 *  @brief    set name suffix
 *
 *  @param[in]     colors              Oyranos colors struct pointer
 *  @param[in]     string              suffix string
 *
 *  @version Oyranos: 0.9.5
 *  @date    2013/08/25
 *  @since   2013/08/25 (Oyranos: 0.9.5)
 */
void   oyNamedColors_SetSuffix       ( oyNamedColors_s   * colors,
                                       const char        * string )
{
  oyNamedColors_s_ * s = (oyNamedColors_s_*) colors;
  if(!colors)
    return;

  oyCheckType__m( oyOBJECT_NAMED_COLORS_S, return )

  if(s->suffix)
    oyObject_GetDeAlloc( s->oy_ )( &s->suffix );

  s->suffix = oyStringCopy_( string, oyObject_GetAlloc( s->oy_) );
}

/** @memberof oyNamedColors_s
 *  @brief    get name prefix
 *
 *  @param[in]     colors              Oyranos colors struct pointer
 *  @return                            prefix string
 *
 *  @version Oyranos: 0.9.5
 *  @date    2013/09/03
 *  @since   2013/09/03 (Oyranos: 0.9.5)
 */
const char * oyNamedColors_GetPrefix ( oyNamedColors_s   * colors )
{
  oyNamedColors_s_ * s = (oyNamedColors_s_*) colors;
  if(!colors)
    return 0;

  oyCheckType__m( oyOBJECT_NAMED_COLORS_S, return 0 )

  return s->prefix;
}

/** @memberof oyNamedColors_s
 *  @brief    get name suffix
 *
 *  @param[in]     colors              Oyranos colors struct pointer
 *  @return                            suffix string
 *
 *  @version Oyranos: 0.9.5
 *  @date    2013/09/03
 *  @since   2013/09/03 (Oyranos: 0.9.5)
 */
const char * oyNamedColors_GetSuffix ( oyNamedColors_s   * colors )
{
  oyNamedColors_s_ * s = (oyNamedColors_s_*) colors;
  if(!colors)
    return 0;

  oyCheckType__m( oyOBJECT_NAMED_COLORS_S, return 0 )

  return s->suffix;
}

/** @memberof oyNamedColors_s
 *  @brief    get full length name
 *
 *  The name is constructed from suffix, name and prefix.
 *
 *  @param[in]     colors              Oyranos colors struct pointer
 *  @param[in]     pos                 nth color in list
 *  @return                            string
 *
 *  @version Oyranos: 0.9.5
 *  @date    2013/12/12
 *  @since   2013/09/03 (Oyranos: 0.9.5)
 */
const char * oyNamedColors_GetColorName (
                                       oyNamedColors_s   * colors,
                                       int                 pos )
{
  oyNamedColors_s_ * s = (oyNamedColors_s_*) colors;
  oyNamedColor_s * c;
  const char * text = "----";
  char * txt = NULL;

  if(!colors)
    return 0;

  oyCheckType__m( oyOBJECT_NAMED_COLORS_S, return 0 )

  c = (oyNamedColor_s*) oyStructList_GetRefType( s->list_, pos,
                                                 oyOBJECT_NAMED_COLOR_S );

  oyStringAddPrintf_( &txt,
                      oyObject_GetAlloc( s->oy_ ),
                      oyObject_GetDeAlloc( s->oy_ ),
                      "%s%s%s",
                      oyNoEmptyString_m_(s->prefix),
                      oyNoEmptyString_m_(
                        oyNamedColor_GetName( c,
                                              oyNAME_NICK,0) ),
                      oyNoEmptyString_m_(s->suffix)
                    );
  oyObject_SetName ( c->oy_, txt, oyNAME_NAME );
  oyFree_m_( txt );
  text = oyObject_GetName( s->oy_, oyNAME_NAME );

  oyNamedColor_Release( &c );

  return text;
}
