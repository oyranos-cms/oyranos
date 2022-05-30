/** @brief manage complex oyNamedColor_s inside Oyranos
 *  @memberof oyNamedColor_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 *
 *  @param[in]     chan                pointer to channel data with a number of elements specified in sig or channels_n, optional
 *  @param[in]     blob                CGATS or other reference data, optional
 *  @param[in]     blob_len            length of the data blob
 *  @param[in]     ref                 possibly a ICC profile
 *  @param         object              the optional object
 * 
 */
oyNamedColor_s *   oyNamedColor_Create ( const double    * chan,
                                         const char      * blob,
                                         int               blob_len,
                                         oyProfile_s     * ref,
                                         oyObject_s        object )
{
  int n = 0;
  oyNamedColor_s_ * s = (oyNamedColor_s_*) oyNamedColor_New( object );
  int error = !s;

  if(!s) return NULL;

  s->profile_  = oyProfile_Copy( ref, 0 );
  if(!s->profile_) return NULL;

  n = oyProfile_GetChannelsCount( s->profile_ );
  if(n)
    oyStruct_AllocHelper_m_( s->channels_, double, n, s, goto cleanC );
  oyCopyColor( chan, &s->channels_[0], 1, ref, n );
  oyCopyColor( 0, &s->XYZ_[0], 1, 0, 0 );

  if(error <= 0 && blob && blob_len)
  {

    s->blob_ = s->oy_->allocateFunc_( blob_len );
    if(!s->blob_) goto cleanC;

    if(error <= 0)
      error = !memcpy( s->blob_, blob, blob_len );

    if(error <= 0)
      s->blob_len_ = blob_len;
  }

  return (oyNamedColor_s*) s;

cleanC:
  oyNamedColor_Release( (oyNamedColor_s**)&s );
  return NULL;
}

/** Function: oyNamedColor_CreateWithName
 *  @memberof oyNamedColor_s
 *  @brief   create a oyNamedColor_s struct
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 december 2007 (API 0.1.8)
 *
 *  @param[in]  nick        very short, only a few letters
 *  @param[in]  name        of the color
 *  @param[in]  description description
 *  @param[in]  chan        pointer to channel data with a number of elements specified in sig or channels_n
 *  @param[in]  XYZ         reference values
 *  @param[in]  blob        CGATS or other reference data
 *  @param[in]  blob_len    length of the data blob
 *  @param[in]  profile_ref profile
 *  @param[in]  object      memory management
 *  @return                 a oyNamedColor_s
 * 
 */
oyNamedColor_s *   oyNamedColor_CreateWithName (
                                       const char        * nick,
                                       const char        * name,
                                       const char        * description,
                                       const double      * chan,
                                       const double      * XYZ,
                                       const char        * blob,
                                       int                 blob_len,
                                       oyProfile_s       * profile_ref,
                                       oyObject_s          object )
{
  int error = 0;
  oyNamedColor_s_ * s = NULL;
  oyObject_s    s_obj = oyObject_NewFrom( object, "oyNamedColor_s" );

  error = !s_obj;

  if(error <= 0)
  {
    error = oyObject_SetNames( s_obj, nick, name, description );
    if(error)
      WARNc1_S("oyObject_SetNames/ \"%s\" ) failed", oyNoEmptyName_m_(nick));
  }

  if(!error)
  {
    s = (oyNamedColor_s_*) oyNamedColor_Create( chan, blob, blob_len, profile_ref, s_obj );
    error =!s;
  }

  oyObject_Release( &s_obj );

  if(error <= 0 && XYZ)
    oyCopyColor( XYZ, s->XYZ_, 1, 0, 0 );

  return (oyNamedColor_s*) s;
}


/*  @return                 pointer to D50 Lab doubles with L 0.0...1.0 a/b -1.27...1.27 */

/** Function: oyNamedColor_GetSpaceRef
 *  @memberof oyNamedColor_s
 *  @brief   get a color space reference
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 december 2007 (API 0.1.8)
 */
oyProfile_s *     oyNamedColor_GetSpaceRef ( oyNamedColor_s  * color )
{
  oyNamedColor_s_ * s = (oyNamedColor_s_*) color;
  if(!color)
    return 0;

  oyProfile_Copy( s->profile_, 0 );
  return s->profile_;
}


/** Function: oyNamedColor_SetChannels
 *  @memberof oyNamedColor_s
 *  @brief   set color channels
 *
 *
 *  @param[in]     color              Oyranos color struct pointer
 *  @param[in]     channels            pointer to channel data
 *  @param[in]     flags               reserved
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
void   oyNamedColor_SetChannels      ( oyNamedColor_s    * color,
                                       const double      * channels,
                                       uint32_t            flags OY_UNUSED )
{
  int i, n;
  oyNamedColor_s_ * s = (oyNamedColor_s_*) color;
  if(!color)
    return;

  n = oyProfile_GetChannelsCount( s->profile_ );
  if(channels)
    for(i = 0; i < n; ++i)
      s->channels_[i] = channels[i];
}

/**
 *  @internal
 *  Function: oyColorConvert_
 *  @memberof oyNamedColor_s
 *  @brief   convert colors
 *
 *  The options are passed to oyConversion_CreateBasicPixels();
 *  The function does the lookups for the profiles and the modules contexts
 *  in the Oyranos cache on the fly. The allocated oyImage_s and
 *  oyConversion_s structures are not cheap as they are not cached.
 *
 *  @version Oyranos: 0.1.11
 *  @since   2007/12/23 (Oyranos: 0.1.8)
 *  @date    2010/09/10
 */
int  oyColorConvert_  ( oyProfile_s       * p_in,
                        oyProfile_s       * p_out,
                        oyPointer           buf_in,
                        oyPointer           buf_out,
                        oyDATATYPE_e        buf_type_in,
                        oyDATATYPE_e        buf_type_out,
                        oyOptions_s       * options,
                        int                 count )
{
  oyImage_s * in  = NULL,
            * out = NULL;
  oyConversion_s * conv = NULL;
  int error = 0;

  in    = oyImage_Create( count, 1,
                         buf_in ,
                         oyChannels_m(oyProfile_GetChannelsCount(p_in)) |
                          oyDataType_m(buf_type_in),
                         p_in,
                         0 );
  out   = oyImage_Create( count, 1,
                         buf_out ,
                         oyChannels_m(oyProfile_GetChannelsCount(p_out)) |
                          oyDataType_m(buf_type_out),
                         p_out,
                         0 );

  conv   = oyConversion_CreateBasicPixels( in,out, options, 0 );
  error  = oyConversion_RunPixels( conv, 0 );

  oyConversion_Release( &conv );
  oyImage_Release( &in );
  oyImage_Release( &out );

  return error;
}

/** Function: oyNamedColor_GetColor
 *  @memberof oyNamedColor_s
 *  @brief   convert a named color to a standard color space
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 december 2007 (API 0.1.8)
 */
int          oyNamedColor_GetColor   ( oyNamedColor_s    * color,
                                       oyProfile_s       * profile,
                                       oyPointer           buf,
                                       oyDATATYPE_e        buf_type,
                                       uint32_t            flags,
                                       oyOptions_s       * options )
{
  int error = !color || !profile || !buf;
  oyNamedColor_s_ * s = (oyNamedColor_s_*) color;
  oyProfile_s * p_in = 0;

  if(!error)
    p_in = s->profile_;

  /* XYZ has priority */
  if(error <= 0 &&
     s->XYZ_[0] != -1 && s->XYZ_[1] != -1 && s->XYZ_[2] != -1)
  {
    p_in = oyProfile_FromStd ( oyEDITING_XYZ, flags, NULL );
 
    error = oyColorConvert_( p_in, profile,
                              s->XYZ_, buf,
                              oyDOUBLE, buf_type, options, 1);

    oyProfile_Release ( &p_in );

  } else if(error <= 0)
    error = oyColorConvert_( p_in, profile,
                              s->channels_, buf,
                              oyDOUBLE, buf_type, options, 1);

  return error;
}

/** Function: oyNamedColor_SetColorStd
 *  @memberof oyNamedColor_s
 *  @brief   set color channels
 *
 *
 *  @param[in]     color               Oyranos color struct pointer
 *  @param[in]     color_space         Oyranos standard color space
 *  @param[in]     channels            pointer to channel data
 *  @param[in]     channels_type       data type
 *  @param[in]     flags               reserved for future use
 *  @param[in]     options             for filter node creation
 *  @return                            error
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 december 2007 (API 0.1.8)
 */
int               oyNamedColor_SetColorStd ( oyNamedColor_s * color,
                                       oyPROFILE_e         color_space,
                                       oyPointer           channels,
                                       oyDATATYPE_e        channels_type,
                                       uint32_t            flags,
                                       oyOptions_s       * options )
{
  oyNamedColor_s_ * s = (oyNamedColor_s_*) color;
  int error = !s || !color_space || !channels;
  oyProfile_s * p_in = 0;
  oyProfile_s * p_out = 0;

  /* abreviate */
  if(error <= 0 && channels_type == oyDOUBLE)
  {
    if     (color_space == oyEDITING_LAB)
    {
      oyLab2XYZ( (double*)channels, s->XYZ_ );
      return error;

    }
    else if(color_space == oyEDITING_XYZ)
    {
      oyCopyColor( (double*)channels, s->XYZ_, 1, 0, 0 );
      return error;
    }
  }

  if(error <= 0)
  {
    p_in = oyProfile_FromStd ( color_space, flags, NULL );
    error = !p_in;
  }

  /* reset and allocate */
  if(error <= 0)
  {
    int n = oyProfile_GetChannelsCount( p_in );

    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    if(n > oyProfile_GetChannelsCount( s->profile_ ))
    {
      if(s->channels_)
      { deallocateFunc(s->channels_); s->channels_ = 0; }

      s->channels_ = s->oy_->allocateFunc_( n * sizeof(double) );
    }

    error = !memset( s->channels_, 0, sizeof(double) * n );

    s->XYZ_[0] = s->XYZ_[1] = s->XYZ_[2] = -1;

    if(deallocateFunc && s->blob_)
    { deallocateFunc( s->blob_ ); s->blob_ = 0; s->blob_len_ = 0; }
  }

  /* convert */
  if(error <= 0)
  {
    p_out = s->profile_;
    error = oyColorConvert_( p_in, p_out,
                              channels, s->channels_,
                              channels_type , oyDOUBLE, options, 1 );
    p_out = 0;
  }

  if(error <= 0)                               
  {  
    p_out = oyProfile_FromStd( oyEDITING_XYZ, flags, 0 );
    error = oyColorConvert_( p_in, p_out,
                              channels, s->XYZ_,
                              channels_type , oyDOUBLE, options, 1 );
  }

  oyProfile_Release ( &p_in );
  oyProfile_Release ( &p_out );

  return error;
}

/** Function: oyNamedColor_GetChannelsConst
 *  @memberof oyNamedColor_s
 *  @brief   get color channels
 *
 *
 *  @param[in]     color               Oyranos color struct pointer
 *  @param[in]     flags               reserved
 *  @return                            pointer channels
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
const double *    oyNamedColor_GetChannelsConst ( oyNamedColor_s * color,
                                       uint32_t            flags OY_UNUSED )
{
  static double l[3] = {-1.f,-1.f,-1.f};
  oyNamedColor_s_ * s = (oyNamedColor_s_*) color;
  if(color)
    return s->channels_;
  else
    return l;
}

/** Function: oyNamedColor_GetXYZConst
 *  @memberof oyNamedColor_s
 *  @brief   get XYZ channels
 *
 *
 *  @param[in]  color      Oyranos color struct pointer
 *  @return                 pointer XYZ channels
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/02/18
 *  @since   2008/02/18 (Oyranos: 0.1.8)
 */
const double *    oyNamedColor_GetXYZConst      ( oyNamedColor_s * color)
{
  static double l[3] = {-1.f,-1.f,-1.f};
  oyNamedColor_s_ * s = (oyNamedColor_s_*) color;

  if(color)
    return s->XYZ_;
  else
    return l;
}

/** Function: oyNamedColor_GetColorStd
 *  @memberof oyNamedColor_s
 *  @brief   convert a named color to a standard color space
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 december 2007 (API 0.1.8)
 */
int      oyNamedColor_GetColorStd    ( oyNamedColor_s    * color,
                                       oyPROFILE_e         color_space,
                                       oyPointer           buf,
                                       oyDATATYPE_e        buf_type,
                                       uint32_t            flags,
                                       oyOptions_s       * options )
{                        
  int ret = 0;
  oyProfile_s * profile;
  oyNamedColor_s_ * s = (oyNamedColor_s_*) color;

  if(!color)
    return 1;

  /* abreviate */
  if(buf_type == oyDOUBLE &&
     s->XYZ_[0] != -1 && s->XYZ_[1] != -1 && s->XYZ_[2] != -1)
  {
    if(color_space == oyEDITING_LAB)
    {
      oyXYZ2Lab( s->XYZ_, (double*)buf );
      return 0;
    }

    if(color_space == oyEDITING_XYZ)
    {
      oyCopyColor( s->XYZ_, (double*)buf, 1, 0, 0 );
      return 0;
    }
  }

  profile = oyProfile_FromStd ( color_space, flags, NULL );
  if(!profile)
    return 1;

  ret = oyNamedColor_GetColor ( color, profile, buf, buf_type, 0, options );
  oyProfile_Release ( &profile );

  return ret;
}

/** Function: oyNamedColor_GetName
 *  @memberof oyNamedColor_s
 *  @brief   get color channels
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 december 2007 (API 0.1.8)
 */
const char *     oyNamedColor_GetName( oyNamedColor_s    * color,
                                       oyNAME_e            type,
                                       uint32_t            flags )
{
  const char * text = 0;
  oyNamedColor_s_ * s = (oyNamedColor_s_*) color;

  if(!s)
    return 0;

  if(!s->oy_)
    return 0;

  text = oyObject_GetName( s->oy_, type );

  if(!text && flags)
  {
    const char * tmp = 0;
    char * txt = 0;
    double l[3];
    int i;
    icSignature sig = oyProfile_GetSignature( s->profile_,
                                              oySIGNATURE_COLOR_SPACE );

    oyAllocHelper_m_( txt, char, 1024, 0, return 0 );
    oyNamedColor_GetColorStd( color, oyEDITING_LAB, l, oyDOUBLE, 0, 0 );

    switch(type)
    {
      case oyNAME_DESCRIPTION:
           tmp = oyObject_GetName( s->oy_, oyNAME_NAME );
           if(!tmp)
             tmp = oyObject_GetName( s->oy_, oyNAME_NICK );

           if(tmp)
             oySprintf_(txt, "%s: CIE*Lab: ", tmp );
           else
             oySprintf_(txt, "CIE*Lab: ");
           for(i = 0; i < 3; ++i)
             oySprintf_( &txt[ oyStrlen_(txt) ], "%.02f ", l[i] );

           tmp = oyICCColorSpaceGetName( sig );
           if(tmp)
             oySprintf_( &txt[ oyStrlen_(txt) ], "; %s:", tmp );

           if( s->channels_ )
           for(i = 0; i < oyICCColorSpaceGetChannelCount( sig ); ++i)
             oySprintf_( &txt[ oyStrlen_(txt) ], "%.02f ", s->channels_[i] );
           break;
      default:
           break;
    }

    oyObject_SetName ( s->oy_, txt, type );

    oyFree_m_( txt );
    
    text = oyObject_GetName( s->oy_, type );
  }

  if(!text && type > oyNAME_NICK)
    text = oyObject_GetName( s->oy_, type - 2 );
  if(!text && type > oyNAME_NAME)
    text = oyObject_GetName( s->oy_, type - 1 );
  if(!text && type < oyNAME_NICK )
    text = oyObject_GetName( s->oy_, type + 2 );
  if(!text && type < oyNAME_DESCRIPTION )
    text = oyObject_GetName( s->oy_, type + 1 );
  if(!text)
    text = _("----");

  return text;
}
