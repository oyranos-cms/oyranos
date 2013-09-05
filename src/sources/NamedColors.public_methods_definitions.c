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
void   oyNamedColor_SetPrefix        ( oyNamedColors_s   * colors,
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
void   oyNamedColor_SetSuffix        ( oyNamedColors_s   * colors,
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
const char * oyNamedColor_GetPrefix  ( oyNamedColors_s   * colors )
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
const char * oyNamedColor_GetSuffix  ( oyNamedColors_s   * colors )
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
 *  The function is not reentrant.
 *
 *  @param[in]     colors              Oyranos colors struct pointer
 *  @param[in]     pos                 nth color in list
 *  @return                            string
 *
 *  @version Oyranos: 0.9.5
 *  @date    2013/09/03
 *  @since   2013/09/03 (Oyranos: 0.9.5)
 */
const char * oyNamedColors_GetColorName (
                                       oyNamedColors_s   * colors,
                                       int                 pos )
{
  oyNamedColors_s_ * s = (oyNamedColors_s_*) colors;
  oyNamedColor_s * c;

  if(!colors)
    return 0;

  oyCheckType__m( oyOBJECT_NAMED_COLORS_S, return 0 )

  c = (oyNamedColor_s*) oyStructList_GetRefType( s->list_, pos,
                                                 oyOBJECT_NAMED_COLOR_S );

  if(s->single_color_name)
    oyObject_GetDeAlloc( s->oy_ )( &s->single_color_name );

  oyStringAddPrintf_( &s->single_color_name,
                      oyObject_GetAlloc( s->oy_ ),
                      oyObject_GetDeAlloc( s->oy_ ),
                      "%s%s%s",
                      oyNoEmptyString_m_(s->prefix),
                      oyNoEmptyString_m_(
                        oyNamedColor_GetName( c,
                                              oyNAME_NAME,0) ),
                      oyNoEmptyString_m_(s->suffix)
                    );

  oyNamedColor_Release( &c );

  return s->single_color_name;
}
