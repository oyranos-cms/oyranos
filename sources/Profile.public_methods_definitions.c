/** Function  oyProfile_FromStd
 *  @memberof oyProfile_s
 *  @brief    Create from default colour space settings
 *
 *  @param[in]    type           default colour space
 *  @param[in]    object         the optional base
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
OYAPI oyProfile_s * OYEXPORT
oyProfile_FromStd     ( oyPROFILE_e       type,
                        oyObject_s        object)
{
  oyProfile_s_ * s = 0;
  char * name = 0;
  oyAlloc_f allocateFunc = 0;
  int pos = type - oyDEFAULT_PROFILE_START;

  if(!oy_profile_s_std_cache_)
  {
    int len = sizeof(oyProfile_s*) *
                            (oyDEFAULT_PROFILE_END - oyDEFAULT_PROFILE_START);
    oy_profile_s_std_cache_ = oyAllocateFunc_( len );
    memset( oy_profile_s_std_cache_, 0, len );
  }

  if(oyDEFAULT_PROFILE_START < type && type < oyDEFAULT_PROFILE_END)
    if(oy_profile_s_std_cache_[pos])
      return oyProfile_Copy( oy_profile_s_std_cache_[pos], 0 );

  if(object)
    allocateFunc = object->allocateFunc_;

  if(type)
    name = oyGetDefaultProfileName ( type, allocateFunc );

  s = oyProfile_FromFile_( name, 0, object );

  if(s)
    s->use_default_ = type;

  if(oyDEFAULT_PROFILE_START < type && type < oyDEFAULT_PROFILE_END)
    oy_profile_s_std_cache_[pos] = oyProfile_Copy( (oyProfile_s*)s, 0 );

  oyProfile_GetID( (oyProfile_s*)s );

  return (oyProfile_s*)s;
}

/** Function  oyProfile_FromFile
 *  @memberof oyProfile_s
 *  @brief    Create from file
 *
 *  Supported are profiles with absolute path names, profiles in OpenICC 
 *  profile paths and profiles relative to the current working path. 
 *  Search will occure in this order.
 *
 *  @param[in]    name           profile file name
 *  @param[in]    flags          for future extension
 *  @param[in]    object         the optional base
 *
 *  flags supports OY_NO_CACHE_READ and OY_NO_CACHE_WRITE to disable cache
 *  reading and writing. The cache flags are useful for one time profiles or
 *  scanning large numbers of profiles.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/11/0 (Oyranos: 0.1.9)
 *  @date    2010/05/18
 */
OYAPI oyProfile_s * OYEXPORT
oyProfile_FromFile            ( const char      * name,
                                uint32_t          flags,
                                oyObject_s        object)
{
  oyProfile_s_ * s = 0;

  s = oyProfile_FromFile_( name, flags, object );

  oyProfile_GetID( (oyProfile_s*)s );

  return (oyProfile_s*)s;
}

/** Function  oyProfile_FromMem
 *  @memberof oyProfile_s
 *  @brief    Create from in memory blob
 *
 *  @param[in]    size           buffer size
 *  @param[in]    block          pointer to memory containing a profile
 *  @param[in]    flags          for future use
 *  @param[in]    object         the optional base
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
OYAPI oyProfile_s * OYEXPORT
oyProfile_FromMem             ( size_t            size,
                                const oyPointer   block,
                                uint32_t          flags,
                                oyObject_s        object)
{
  oyProfile_s_ * s = 0;
  int error = 0;
  oyPointer block_ = 0;
  size_t size_ = 0;

  if(block && size)
  {
    oyAllocHelper_m_( block_, char, size, object ? object->allocateFunc_:0,
                      error = 1 );

    if(!error)
    {
      size_ = size;
      error = !memcpy( block_, block, size );
    }
  }

  s = oyProfile_FromMemMove_( size_, &block_, flags, object );

  oyProfile_GetID( (oyProfile_s*)s );

  return (oyProfile_s*)s;
}

/** Function  oyProfile_FromSignature
 *  @memberof oyProfile_s
 *  @brief    Create a fractional profile from signature
 *
 *  @param[in]    sig            signature
 *  @param[in]    type           type of signature to set
 *  @param[in]    object         the optional base
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/06/20
 *  @since   2007/06/20 (Oyranos: 0.1.8)
 */
OYAPI oyProfile_s * OYEXPORT
             oyProfile_FromSignature(  icSignature         sig,
                                       oySIGNATURE_TYPE_e  type,
                                       oyObject_s          object )
{
  oyProfile_s_ * s = oyProfile_New_( object );
  int error = !s;

  if(error <= 0)
    error = oyProfile_SetSignature( (oyProfile_s*)s, sig, type );

  return (oyProfile_s*)s;
}

/** Function  oyProfile_FromMD5
 *  @memberof oyProfile_s
 *  @brief    Look up a profile from it's md5 hash sum
 *
 *  @param[in]    md5            hash sum
 *  @param[in]    object         the optional base
 *  @return                      a profile
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/03/20 (Oyranos: 0.1.10)
 *  @date    2009/03/20
 */
OYAPI oyProfile_s * OYEXPORT
                   oyProfile_FromMD5(  uint32_t          * md5,
                                       oyObject_s          object )
{
  oyProfile_s * s = 0, * tmp = 0;
  int error = !md5,
      equal = 0;
  char ** names = 0;
  uint32_t count = 0, i = 0;

  if(error)
    return 0;

  if(error <= 0)
  {
    names = /*(const char**)*/ oyProfileListGet_ ( NULL, &count );

    for(i = 0; i < count; ++i)
    {
      if(names[i])
      {
        if(oyStrcmp_(names[i], OY_PROFILE_NONE) != 0)
          tmp = oyProfile_FromFile( names[i], 0, 0 );

        if(tmp->oy_->hash_ptr_)
          equal = memcmp( md5, tmp->oy_->hash_ptr_, OY_HASH_SIZE );
        else
          equal = 1;
        if(equal == 0)
          {
            s = tmp;
            break;
          }

          oyProfile_Release( &tmp );
        }
      }

      oyStringListRelease_( &names, count, oyDeAllocateFunc_ );
  }

  return s;
}

/** Function  oyProfile_GetChannelsCount
 *  @memberof oyProfile_s
 *  @brief    Number of channels in a colour space
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
OYAPI int OYEXPORT
oyProfile_GetChannelsCount( oyProfile_s * profile )
{
  oyProfile_s_ * s = (oyProfile_s_*)profile;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(s->channels_n_)
    return s->channels_n_;

  s->channels_n_ = oyICCColourSpaceGetChannelCount( s->sig_ );

  return s->channels_n_;
}

/** Function  oyProfile_GetSignature
 *  @memberof oyProfile_s
 *  @brief    Get ICC colour space signature
 *
 *  \verbatim
    // show some profile properties
    oyProfile_s * p = ...; // get from somewhere
    icSignature vs = oyValueUInt32( oyProfile_GetSignature(p,oySIGNATURE_VERSION) );      
    char * v = (char*)&vs;
    printf("  created %d.%d.%d %d:%d:%d\n", 
             oyProfile_GetSignature(p,oySIGNATURE_DATETIME_YEAR),
             oyProfile_GetSignature(p,oySIGNATURE_DATETIME_MONTH),
             oyProfile_GetSignature(p,oySIGNATURE_DATETIME_DAY),
             oyProfile_GetSignature(p,oySIGNATURE_DATETIME_HOURS),
             oyProfile_GetSignature(p,oySIGNATURE_DATETIME_MINUTES),
             oyProfile_GetSignature(p,oySIGNATURE_DATETIME_SECONDS)
          );
    printf("  pcs: %s  colour space: %s version: %d.%d.%d\n", 
          oyICCColourSpaceGetName( (icColorSpaceSignature)
                         oyProfile_GetSignature(p,oySIGNATURE_PCS) ),
          oyICCColourSpaceGetName( (icColorSpaceSignature)
                         oyProfile_GetSignature(p,oySIGNATURE_COLOUR_SPACE) ),
          (int)v[0], (int)v[1]/16, (int)v[1]%16
          );
    \endverbatim
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
OYAPI icSignature OYEXPORT
oyProfile_GetSignature ( oyProfile_s * profile,
                         oySIGNATURE_TYPE_e type )
{
  icHeader *h = 0;
  icSignature sig = 0;
  oyProfile_s_ * s = (oyProfile_s_*)profile;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(s->sig_ && type == oySIGNATURE_COLOUR_SPACE)
    return s->sig_;

  if(!s->block_)
  {
    if(type == oySIGNATURE_COLOUR_SPACE)
      sig = s->sig_ = icSigXYZData;
    return sig;
  }

  h = (icHeader*) s->block_;

  switch(type)
  {
  case oySIGNATURE_COLOUR_SPACE:       /* colour space */
       sig = s->sig_ = oyValueCSpaceSig( h->colorSpace ); break;
  case oySIGNATURE_PCS:                /* profile connection space */
       sig = oyValueCSpaceSig( h->pcs ); break;
  case oySIGNATURE_SIZE:               /* internal stored size */
       sig = oyValueUInt32( h->size ); break;
  case oySIGNATURE_CMM:                /* prefered CMM */
       sig = oyValueUInt32( h->cmmId ); break;
  case oySIGNATURE_VERSION:            /* version */
       sig = oyValueUInt32( h->version ); break;
  case oySIGNATURE_CLASS:              /* usage class 'mntr' ... */
       sig = oyValueUInt32( h->deviceClass ); break;
  case oySIGNATURE_MAGIC:              /* magic; ICC: 'acsp' */
       sig = oyValueUInt32( h->magic ); break;
  case oySIGNATURE_PLATFORM:           /* operating system */
       sig = oyValueUInt32( h->platform ); break;
  case oySIGNATURE_OPTIONS:            /* various ICC header flags */
       sig = oyValueUInt32( h->flags ); break;
  case oySIGNATURE_MANUFACTURER:       /* device manufacturer */
       sig = oyValueUInt32( h->manufacturer ); break;
  case oySIGNATURE_MODEL:              /* device modell */
       sig = oyValueUInt32( h->model ); break;
  case oySIGNATURE_INTENT:             /* seldom used profile claimed intent*/
       sig = oyValueUInt32( h->renderingIntent ); break;
  case oySIGNATURE_CREATOR:            /* profile creator ID */
       sig = oyValueUInt32( h->creator ); break;
  case oySIGNATURE_DATETIME_YEAR:      /* creation time in UTC */
       sig = oyValueUInt16( h->date.year ); break;
  case oySIGNATURE_DATETIME_MONTH:     /* creation time in UTC */
       sig = oyValueUInt16( h->date.month ); break;
  case oySIGNATURE_DATETIME_DAY:       /* creation time in UTC */
       sig = oyValueUInt16( h->date.day ); break;
  case oySIGNATURE_DATETIME_HOURS:     /* creation time in UTC */
       sig = oyValueUInt16( h->date.hours ); break;
  case oySIGNATURE_DATETIME_MINUTES:   /* creation time in UTC */
       sig = oyValueUInt16( h->date.minutes ); break;
  case oySIGNATURE_DATETIME_SECONDS:   /* creation time in UTC */
       sig = oyValueUInt16( h->date.seconds ); break;
  case oySIGNATURE_MAX: break;
  }

  return sig;
}

/** Function  oyProfile_SetSignature
 *  @memberof oyProfile_s
 *  @brief    Set signature
 *
 *  @param[in]     profile             the profile
 *  @param[in]     sig                 signature
 *  @param[in]     type                type of signature to set
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/06/20
 *  @since   2007/06/20 (Oyranos: 0.1.8)
 */
OYAPI int OYEXPORT
             oyProfile_SetSignature (  oyProfile_s       * profile,
                                       icSignature         sig,
                                       oySIGNATURE_TYPE_e  type )
{
  oyProfile_s_ * s = profile;
  int error = !s;
  oyPointer block_ = 0;
  size_t size_ = 128;
  icHeader *h = 0;

  if(!s)
    return 1;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 1 )

  if(error <= 0 && type == oySIGNATURE_COLOUR_SPACE)
  {
    if(sig)
      s->sig_ = sig;
    else
      s->sig_ = icSigXYZData;
    return error;
  }

  if(error <= 0 && !s->block_)
  {
    oyAllocHelper_m_( block_, char, size_, s->oy_ ? s->oy_->allocateFunc_:0,
                      error = 1 );

    if(error <= 0)
    {
      s->block_ = block_;
      s->size_ = size_;
    }
  }

  if(error <= 0)
    h = (icHeader*) s->block_;

  if(error <= 0)
  switch(type)
  {
  case oySIGNATURE_COLOUR_SPACE:       /* colour space */
       h->colorSpace = oyValueCSpaceSig( s->sig_ ); break;
  case oySIGNATURE_PCS:                /* profile connection space */
       h->pcs = oyValueCSpaceSig( sig ); break;
  case oySIGNATURE_SIZE:               /* internal stored size */
       h->size = oyValueUInt32( sig ); break;
  case oySIGNATURE_CMM:                /* prefered CMM */
       h->cmmId = oyValueUInt32( sig ); break;
  case oySIGNATURE_VERSION:            /* version */
       h->version = oyValueUInt32( sig ); break;
  case oySIGNATURE_CLASS:              /* usage class 'mntr' ... */
       h->deviceClass = oyValueUInt32( sig ); break;
  case oySIGNATURE_MAGIC:              /* magic; ICC: 'acsp' */
       h->magic = oyValueUInt32( sig ); break;
  case oySIGNATURE_PLATFORM:           /* operating system */
       h->platform = oyValueUInt32( sig ); break;
  case oySIGNATURE_OPTIONS:            /* various ICC header flags */
       h->flags = oyValueUInt32( sig ); break;
  case oySIGNATURE_MANUFACTURER:       /* device manufacturer */
       h->manufacturer = oyValueUInt32( sig ); break;
  case oySIGNATURE_MODEL:              /* device modell */
       h->model = oyValueUInt32( sig ); break;
  case oySIGNATURE_INTENT:             /* seldom used profile claimed intent*/
       h->renderingIntent = oyValueUInt32( sig ); break;
  case oySIGNATURE_CREATOR:            /* profile creator ID */
       h->creator = oyValueUInt32( sig ); break;
  case oySIGNATURE_DATETIME_YEAR:      /* creation time in UTC */
       h->date.year = oyValueUInt16( sig ); break;
  case oySIGNATURE_DATETIME_MONTH:     /* creation time in UTC */
       h->date.month = oyValueUInt16( sig ); break;
  case oySIGNATURE_DATETIME_DAY:       /* creation time in UTC */
       h->date.day = oyValueUInt16( sig ); break;
  case oySIGNATURE_DATETIME_HOURS:     /* creation time in UTC */
       h->date.hours = oyValueUInt16( sig ); break;
  case oySIGNATURE_DATETIME_MINUTES:   /* creation time in UTC */
       h->date.minutes = oyValueUInt16( sig ); break;
  case oySIGNATURE_DATETIME_SECONDS:   /* creation time in UTC */
       h->date.seconds = oyValueUInt16( sig ); break;
  case oySIGNATURE_MAX: break;
  }

  return error;
}

/** Function  oyProfile_SetChannelNames
 *  @memberof oyProfile_s
 *  @brief    Set channel names
 *
 *  The function should be used to specify extra channels or unusual colour
 *  layouts like CMYKRB. The number of elements in names_chan should fit to the
 *  channels count or to the colour space signature.
 *
 *  You can let single entries empty if they are understandable by the
 *  colour space signature. Oyranos will set them for you on request.
 *
 *  @param[in]     profile             profile
 *  @param[in]     names_chan          pointer to channel names 
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
OYAPI void OYEXPORT
oyProfile_SetChannelNames            ( oyProfile_s       * profile,
                                       oyObject_s        * names_chan )
{
  oyProfile_s_ * s = (oyProfile_s_*)profile;
  int n = oyProfile_GetChannelsCount( profile );
  int error = !s;

  if(error)
    return;

  oyCheckType__m( oyOBJECT_PROFILE_S, return )

  if(names_chan && n)
  {
    int i = 0;
    s->names_chan_ = s->oy_->allocateFunc_( (n + 1 ) * sizeof(oyObject_s) );
    s->names_chan_[ n ] = NULL;
    for( ; i < n; ++i )
      if(names_chan[i])
        s->names_chan_[i] = oyObject_Copy( names_chan[i] );
  }
}

/** Function  oyProfile_GetChannelNames
 *  @memberof oyProfile_s
 *  @brief    Get channel names
 *
 *  @param[in]     profile             the profile
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
OYAPI const oyObject_s * OYEXPORT
oyProfile_GetChannelNames           ( oyProfile_s   * profile )
{
  oyProfile_s_ * s = (oyProfile_s_*)profile;
  int n = oyProfile_GetChannelsCount( profile );
  int error = 0;
  icColorSpaceSignature sig = oyProfile_GetSignature( profile, oySIGNATURE_COLOUR_SPACE );

  if(!profile)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(!s->names_chan_ && n)
  {
    int i = 0;
    s->names_chan_ = s->oy_->allocateFunc_( (n + 1 ) * sizeof(oyObject_s) );
    if(!s->names_chan_)
      error = 1;
    if(error <= 0)
    {
      s->names_chan_[ n ] = NULL;
      for( ; i < n; ++i )
      {
        s->names_chan_[i] = oyObject_NewFrom( s->oy_ );
        if(!s->names_chan_[i])
          error = 1;
        else
          error = oyObject_SetNames( s->names_chan_[i],
                    oyICCColourSpaceGetChannelName ( sig, i, oyNAME_NICK ),
                    oyICCColourSpaceGetChannelName ( sig, i, oyNAME_NAME ),
                    oyICCColourSpaceGetChannelName ( sig, i, oyNAME_DESCRIPTION )
                      );
      }
    }
  }

  if(error <= 0 && s->names_chan_)
    return (const oyObject_s*) s->names_chan_;
  else
    return 0;
}

/** Function  oyProfile_GetChannelName
 *  @memberof oyProfile_s
 *  @brief    Get a channels name
 *
 *  A convinience function to get a single name with a certain type.
 *
 *  @param[in] profile  address of a Oyranos named colour structure
 *  @param[in] pos      position of channel 
 *  @param[in] type     sort of text 
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
OYAPI const char   * OYEXPORT
oyProfile_GetChannelName           ( oyProfile_s   * profile,
                                     int                pos,
                                     oyNAME_e           type )
{
  oyProfile_s_ * s = (oyProfile_s_*)profile;
  int n = oyProfile_GetChannelsCount( (oyProfile_s*)s );
  const oyChar * text = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if( 0 <= pos && pos < n )
    return "-";

  if(!s->names_chan_)
    oyProfile_GetChannelNames ( (oyProfile_s*)s );

  if(s->names_chan_ && s->names_chan_[pos])
    text = oyObject_GetName( s->names_chan_[pos], type );

  return text;
}

/** Function  oyProfile_GetID
 *  @memberof oyProfile_s
 *  @brief    Get unique name
 *
 *  the returned string is identical to oyNAME_ID
 *
 *  @since Oyranos: version 0.1.8
 *  @date  26 november 2007 (API 0.1.8)
 */
OYAPI const oyChar* OYEXPORT
                   oyProfile_GetID   ( oyProfile_s       * profile )
{
  oyProfile_s_ * s = (oyProfile_s_*)profile;
  int error = !s;
  const oyChar * text = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(error <= 0)
    text = oyObject_GetName( s->oy_, oyNAME_NICK );

  if(error <= 0 && !text)
  {
    char * temp = 0;
    int found = 0;

    oyAllocHelper_m_( temp, oyChar, 1024, 0, error = 1 );

    /* A short number represents a default profile,
     * The length should not exceed OY_HASH_SIZE.
     */
    if(s->use_default_ && !found && error <= 0)
    {
      oySprintf_(temp, "%d", s->use_default_);
      if(oyStrlen_(temp) < OY_HASH_SIZE)
        found = 1;
    }

    /* Do we have a file_name_? */
    if(s->file_name_ && !found && error <= 0)
    {
      oySprintf_(temp, "%s", s->file_name_);
      if(temp[0])
        found = 1;
    }

    /* Do we have a hash_? */
    if(!found && error <= 0)
    {
      if(!oyProfile_Hashed_((oyProfile_s*)s))
        error = oyProfile_GetHash_( s );

      if(error <= 0)
      {
        uint32_t * i = (uint32_t*)s->oy_->hash_ptr_;
        if(i)
          oySprintf_(temp, "%x%x%x%x", i[0], i[1], i[2], i[3]);
        else
          oySprintf_(temp, "                " );
        if(temp[0])
          found = 1;
      }
    }

    if(error <= 0 && !found)
      error = 1;

    if(error <= 0)
      error = oyObject_SetName( s->oy_, temp, oyNAME_NICK );

    oyFree_m_( temp );

    if(error <= 0)
      text = oyObject_GetName( s->oy_, oyNAME_NICK );
  }

  return text;
}

/** Function  oyProfile_Equal
 *  @memberof oyProfile_s
 *  @brief    Check if two profiles are equal by their hash sum
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/02/03 (Oyranos: 0.1.8)
 *  @date    2009/04/16
 */
OYAPI int OYEXPORT
                   oyProfile_Equal   ( oyProfile_s       * profileA,
                                       oyProfile_s       * profileB )
{
  int equal = 0;

  if(profileA && profileB)
    equal = oyObject_HashEqual( profileA->oy_, profileB->oy_ );

  return equal;
}

/** Function  oyProfile_GetText
 *  @memberof oyProfile_s
 *  @brief    Get a presentable name
 *
 *  The type argument should select the following string in return: \n
 *  - oyNAME_NAME - a readable XML element
 *  - oyNAME_NICK - the hash ID
 *  - oyNAME_DESCRIPTION - profile internal name (icSigProfileDescriptionTag)
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/11/26 (Oyranos: 0.1.8)
 *  @date    2008/06/23
 */
OYAPI const oyChar* OYEXPORT
                   oyProfile_GetText ( oyProfile_s       * profile,
                                       oyNAME_e            type )
{
  const char * text = 0;
  char ** texts = 0;
  int32_t texts_n = 0;
  oyProfileTag_s * tag = 0;
  oyProfile_s_ * s = (oyProfile_s_*)profile;
  int error = !s;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(error <= 0 && type == oyNAME_NICK)
    text = oyProfile_GetID( (oyProfile_s*)s );

  if(error <= 0 && !text)
    if(type <= oyNAME_DESCRIPTION)
      text = oyObject_GetName( s->oy_, type );

  if(error <= 0 && !(text && text[0]))
  {
    char * temp = 0,
         * tmp2 = 0;
    int found = 0;

    oyAllocHelper_m_( temp, char, 1024, 0, error = 1 );

    /* Ask the CMM? */
    if(!found && error <= 0 &&
       type == oyNAME_DESCRIPTION)
    {
      {
        tag = oyProfile_GetTagById( (oyProfile_s*)s, icSigProfileDescriptionTag );
        texts = oyProfileTag_GetText( tag, &texts_n, "", 0,0,0);

        if(texts_n && texts[0] && texts[0][0])
        {
          memcpy(temp, texts[0], oyStrlen_(texts[0]));
          temp[oyStrlen_(texts[0])] = 0;
          found = 1;

          oyStringListRelease_( &texts, texts_n, tag->oy_->deallocateFunc_ );
        } else
          /* we try to get something as oyNAME_NAME */
        if(s->file_name_ && s->file_name_[0])
        {
          size_t len = oyStrlen_(s->file_name_);
          if(strrchr(s->file_name_,'/'))
          {
            tmp2 = oyStrrchr_(s->file_name_,'/')+1;
            len = oyStrlen_( tmp2 );
            memcpy( temp, tmp2, len );
          } else
            memcpy( temp, s->file_name_, len );
          temp[len] = 0;
          found = 1;
        }
      }
    }

    if(type == oyNAME_NAME)
    {
      uint32_t * i = (uint32_t*)s->oy_->hash_ptr_;
      char * file_name = oyProfile_GetFileName_r( s, oyAllocateFunc_ );

      if(oyProfile_Hashed_(s))
        error = oyProfile_GetHash_( s );

      if(s->use_default_ && error <= 0)
        oyWidgetTitleGet( (oyWIDGET_e)s->use_default_, 0, &text, 0, 0 );

      oySprintf_( temp, "<profile use_default=\"%s\" file_name=\"%s\" hash=\"",
             oyNoEmptyName_m_(text),
             oyNoEmptyName_m_(file_name) );
      if(i)
        oySprintf_( &temp[oyStrlen_(temp)], "%x%x%x%x\" />",
             i[0], i[1], i[2], i[3] );
      else
        oySprintf_( &temp[oyStrlen_(temp)], "                \" />" );

      if(file_name) free(file_name); file_name = 0;
      found = 1;
    }

    if(!found)
    {
      text = oyProfile_GetID( (oyProfile_s*)s );
      if(text[0])
        found = 1;
    }

    /* last rescue */
    if(!found && oyProfile_Hashed_(s))
      error = oyProfile_GetHash_( s );

    if(!found && error <= 0)
    {
      uint32_t * i = (uint32_t*)s->oy_->hash_ptr_;
      oySprintf_(temp, "%x%x%x%x", i[0], i[1], i[2], i[3]);
      if(temp[0])
        found = 1;
    }

    if(error <= 0 && !found)
      error = 1;

    if(error <= 0)
      error = oyObject_SetName( s->oy_, temp, type );

    oyFree_m_( temp );

    if(error <= 0)
      text = oyObject_GetName( s->oy_, type );
  }

  return text;
}

/** Function  oyProfile_GetMem
 *  @memberof oyProfile_s
 *  @brief    Get the ICC profile in memory
 *
 *  The prefered memory comes from the unmodified original memory.
 *  Otherwise a previously modified tag list is serialised into memory.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/12/20 (Oyranos: 0.1.8)
 *  @date    2010/04/16
 */
OYAPI oyPointer OYEXPORT
                   oyProfile_GetMem  ( oyProfile_s       * profile,
                                       size_t            * size,
                                       uint32_t            flag,
                                       oyAlloc_f           allocateFunc )
{
  oyPointer block = 0;
  oyProfile_s_ * s = (oyProfile_s_*)profile;
  int error = !s;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(s)
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );

  if(error <= 0 && s->type_ == oyOBJECT_PROFILE_S)
  {
    if(s->size_ && s->block_ && !s->tags_modified_)
    {

      block = oyAllocateWrapFunc_( s->size_, allocateFunc );
      error = !block;
      if(error <= 0)
        error = !memcpy( block, s->block_, s->size_ );
      if(error <= 0 && size)
        *size = s->size_;

    } else
    if( oyStructList_Count( s->tags_ ))
    {
      block = oyProfile_TagsToMem_ ( s, size, allocateFunc );
    }
  }

  if(s)
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return block;
}

/** Function  oyProfile_GetTagByPos
 *  @memberof oyProfile_s
 *  @brief    Get a profile tag
 *
 *  @param[in]     profile             the profile
 *  @param[in]     pos                 header + tag position
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/02/06
 *  @since   2008/02/06 (Oyranos: 0.1.8)
 */
OYAPI oyProfileTag_s * OYEXPORT
                 oyProfile_GetTagByPos(oyProfile_s       * profile,
                                       int                 pos )
{
  oyProfileTag_s * tag = 0;
  oyProfile_s_ * s = (oyProfile_s_*)profile;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(s)
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );

  tag = oyProfile_GetTagByPos_( s, pos );

  if(s)
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return tag;
}

/** Function  oyProfile_GetTagById
 *  @memberof oyProfile_s
 *  @brief    Get a profile tag by its tag signature
 *
 *  @param[in]     profile             the profile
 *  @param[in]     id                  icTagSignature
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2 january 2008 (API 0.1.8)
 */
OYAPI oyProfileTag_s * OYEXPORT
                 oyProfile_GetTagById( oyProfile_s       * profile,
                                       icTagSignature      id )
{
  oyProfile_s_ * s = (oyProfile_s_*)profile;
  int error = !s;
  oyProfileTag_s * tag = 0,
                 * tmp = 0;
  int i = 0, n = 0;
  icTagSignature tag_id_ = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(error <= 0)
    n = oyProfile_GetTagCount_( s );

  if(error <= 0 && n)
  {
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );
    for(i = 0; i < n; ++i)
    {
      tmp = oyProfile_GetTagByPos_( s, i );
      tag_id_ = 0;

      if(tmp)
        tag_id_ = oyProfileTagPriv_m(tmp)->use;

      if(tag_id_ == id)
      {
        tag = tmp; tmp = 0;
        break;
      } else
        oyProfileTag_Release( &tmp );
    }
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );
  }

  return tag;
}

/** Function  oyProfile_GetTagCount
 *  @memberof oyProfile_s
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/01/01 (Oyranos: 0.1.8)
 *  @date    2009/12/29
 */
OYAPI int OYEXPORT
                   oyProfile_GetTagCount( oyProfile_s    * profile )
{
  int n = 0;
  oyProfile_s_ * s = (oyProfile_s_*)profile;
  int error = !s;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(error <= 0 && !s->tags_)
    error = 1;

  if(error <= 0)
    n = oyStructList_Count( s->tags_ );

  if(error <= 0 && !n)
  {
    oyProfileTag_s * tag = 0;
    if(s)
      oyObject_Lock( s->oy_, __FILE__, __LINE__ );

    tag = oyProfile_GetTagByPos_ ( s, 0 );
    oyProfileTag_Release( &tag );
    n = oyStructList_Count( s->tags_ );

    if(s)
      oyObject_UnLock( s->oy_, __FILE__, __LINE__ );
  }

  return n;
}

/** Function  oyProfile_TagMoveIn
 *  @memberof oyProfile_s
 *  @brief    Add a tag to a profile
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/02/01 (Oyranos: 0.1.8)
 *  @date    2009/12/29
 */
OYAPI int OYEXPORT
                   oyProfile_TagMoveIn(oyProfile_s       * profile,
                                       oyProfileTag_s   ** obj,
                                       int                 pos )
{
  oyProfile_s_ * s = (oyProfile_s_*)profile;
  int error = !s, i,n;
  oyProfileTag_s * tag = 0;

  if(!s)
    return error;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 1 )

  if(!(obj && *obj && (*obj)->type_ == oyOBJECT_PROFILE_TAG_S))
    error = 1;

  if(s)
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );


  if(error <= 0)
  {
    /** Initialise tag list. */
    n = oyProfile_GetTagCount_( s );

    /** Avoid double occurencies of tags. */
    for( i = 0; i < n; ++i )
    {
      tag = oyProfile_GetTagByPos_( s, i );
      if(oyProfileTagPriv_m(tag)->use == oyProfileTagPriv((*obj))->use)
      {
        oyProfile_TagReleaseAt_(s, i);
        n = oyProfile_GetTagCount_( s );
      }
      oyProfileTag_Release( &tag );
    }
    error = oyStructList_MoveIn ( s->tags_, (oyStruct_s**)obj, pos,
                                  OY_OBSERVE_AS_WELL );
    ++s->tags_modified_;
  }

  if(s)
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return error;
}

/** Function  oyProfile_TagReleaseAt
 *  @memberof oyProfile_s
 *  @brief    Remove a tag from a profile
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/03/11
 *  @since   2008/03/11 (Oyranos: 0.1.8)
 */
OYAPI int OYEXPORT
                   oyProfile_TagReleaseAt ( oyProfile_s  * profile,
                                       int                 pos )
{
  oyProfile_s_ * s = (oyProfile_s_*)profile;
  int error = !s;

  if(!s)
    return error;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 1 )

  if(!(s && s->type_ == oyOBJECT_PROFILE_S))
    error = 1;

  if(s)
    oyObject_Lock( s->oy_, __FILE__, __LINE__ );

  if(error <= 0)
  {
    error = oyStructList_ReleaseAt ( s->tags_, pos );
    ++s->tags_modified_;
  }

  if(s)
    oyObject_UnLock( s->oy_, __FILE__, __LINE__ );

  return error;
}

/** Function  oyProfile_AddTagText
 *  @memberof oyProfile_s
 *  @brief    Add a text tag
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/10/18
 *  @since   2009/10/18 (Oyranos: 0.1.10)
 */
OYAPI int OYEXPORT
                   oyProfile_AddTagText ( oyProfile_s       * profile,
                                          icSignature         signature,
                                          const char        * text )
{
  oyStructList_s * list = 0;
  oyName_s * name = oyName_new(0);
  int error = 0;
  oyProfileTag_s * tag = 0;
  icTagTypeSignature tt = icSigTextType;
  icSignature vs = oyValueUInt32( oyProfile_GetSignature( profile,
                                                         oySIGNATURE_VERSION) );
  char * v = (char*)&vs;
  int version_A = (int)v[0]/*,
      version_B = (int)v[1]/16,
      version_C =  (int)v[1]%16*/;

  if(version_A <= 3 &&
     (signature == icSigProfileDescriptionTag ||
      signature == icSigDeviceMfgDescTag ||
      signature == icSigDeviceModelDescTag ||
      signature == icSigScreeningDescTag ||
      signature == icSigViewingCondDescTag))
    tt = icSigTextDescriptionType;
  else if(version_A >= 4 &&
     (signature == icSigProfileDescriptionTag ||
      signature == icSigDeviceMfgDescTag ||
      signature == icSigDeviceModelDescTag ||
      signature == icSigCopyrightTag ||
      signature == icSigViewingCondDescTag))
    tt = icSigMultiLocalizedUnicodeType;

  name = oyName_set_ ( name, text, oyNAME_NAME,
                       oyAllocateFunc_, oyDeAllocateFunc_ );
  list = oyStructList_New(0);
  error = oyStructList_MoveIn( list, (oyStruct_s**) &name, 0, 0 );

  if(!error)
  {
    tag = oyProfileTag_Create( list, tt, 0,OY_MODULE_NICK, 0);
    error = !tag;
  }

  if(!error)
    oyProfileTagPriv_m(tag)->use = signature;

  oyStructList_Release( &list );

  if(tag)
    error = oyProfile_TagMoveIn ( profile, &tag, -1 );

  return error;
}

/** Function  oyProfile_GetFileName
 *  @memberof oyProfile_s
 *  @brief    Get the ICC profile location in the filesystem
 *
 *  This function tries to find a profile on disk matching a possibly memory
 *  only profile. In case the profile was previously opened from file or as a
 *  from Oyranos defaults the associated filename will simply be retuned.
 *
 *  @param         profile             the profile
 *  @param         dl_pos              -1, or the position in a device links
 *                                     source chain
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/02/01 (Oyranos: 0.1.8)
 *  @date    2008/02/01
 */
OYAPI const char * OYEXPORT
                   oyProfile_GetFileName ( oyProfile_s       * profile,
                                           int                 dl_pos )
{
  const char * name = 0;
  oyProfile_s * tmp = 0;
  oyProfile_s_ * s = (oyProfile_s_*)profile;
  int error = !s;
  char ** names = 0;
  uint32_t count = 0, i = 0;
  oyProfileTag_s * psid = 0;
  char ** texts = 0;
  int32_t   texts_n = 0;
  char *  hash = 0,
       *  txt = 0;
  char    tmp_hash[34];
  int       dl_n = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(error <= 0)
  {
    if(dl_pos >= 0)
    {
      psid = oyProfile_GetTagById( (oyProfile_s*)s, icSigProfileSequenceIdentifierTag );
      texts = oyProfileTag_GetText( psid, &texts_n, 0,0,0,0);

      if(texts && texts_n > 1+dl_pos*5+2)
      {
        dl_n = 1 + dl_pos*5+2;
        hash = texts[dl_n];
      }
    }


    if(s->file_name_ && !hash)
    {
      name = s->file_name_;
    } else
    {
      names = /*(const char**)*/ oyProfileListGet_ ( NULL, &count );

      for(i = 0; i < count; ++i)
      {
        if(names[i])
        {
          if(oyStrcmp_(names[i], OY_PROFILE_NONE) != 0)
            tmp = oyProfile_FromFile( names[i], OY_NO_CACHE_WRITE, 0 );

          if(tmp)
          {
            if(hash)
            {
              uint32_t * h = (uint32_t*)tmp->oy_->hash_ptr_;
              if(h)
                oySprintf_(tmp_hash, "%x%x%x%x", h[0], h[1], h[2], h[3]);
              else
                oySprintf_(tmp_hash, "                " );
              if(memcmp( hash, tmp_hash, 2*OY_HASH_SIZE ) == 0 )
              {
               name = names[i];
                break;
              }
            } else
            if(oyProfile_Equal( (oyProfile_s*)s, tmp ))
            {
              name = names[i];
              break;
            }

            oyProfile_Release( &tmp );
          }
        }
      }

      if(hash)
      {
        txt = oyFindProfile_( name );
        sprintf( hash, "//imaging/profile.icc/psid_%d", dl_pos );
        oyOptions_SetFromText( &s->oy_->handles_,
                               hash,
                               txt,
                               OY_CREATE_NEW );
        oyDeAllocateFunc_( txt );
        name = oyOptions_FindString( s->oy_->handles_,
                                     hash, 0 );
      } else
      {
        s->file_name_ = oyFindProfile_( name );
        name = oyStringCopy_( s->file_name_, s->oy_->allocateFunc_ );
        if(s->file_name_)
          oyDeAllocateFunc_( s->file_name_ );
        s->file_name_ = (char*)name;
      }

      if(names)
        oyStringListRelease_( &names, count, oyDeAllocateFunc_ );
    }

    if(texts)
      oyStringListRelease_( &texts, texts_n, oyDeAllocateFunc_ );
  }

  return name;
}

/** Function  oyProfile_DeviceGet
 *  @memberof oyProfile_s
 *  @brief    Obtain device informations from a profile
 *
 *  @verbatim
    oyConfig_s * device = oyConfig_New( "//" OY_TYPE_STD "/config", object );
    oyProfile_DeviceGet( profile, device ); @endverbatim
 *
 *  @param[in]     profile             the profile
 *  @param[in,out] device              the device description
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/05/22 (Oyranos: 0.1.10)
 *  @date    2009/05/23
 */
OYAPI int OYEXPORT
                   oyProfile_DeviceGet ( oyProfile_s     * profile,
                                         oyConfig_s      * device )
{
  int error = !profile, l_error = 0;
  oyProfile_s * s = profile;
  oyConfig_s_ * device_ = (oyConfig_s_*)device;
  oyProfileTag_s * tag = 0;
  char ** texts = 0;
  int32_t texts_n = 0;
  int i,
      dmnd_found = 0, dmdd_found = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(!error)
  {
    tag = oyProfile_GetTagById( s, icSigProfileDetailDescriptionTag_ );
    texts = oyProfileTag_GetText( tag, &texts_n, "", 0,0,0);
    if(texts && texts[0] && texts_n > 0)
      for(i = 2; i+1 < texts_n && error <= 0; i+=2)
      {
        if(strcmp(texts[i+0],"model") == 0) dmdd_found = 1;
        if(strcmp(texts[i+0],"manufacturer") == 0) dmnd_found = 1;

        error = oyOptions_SetRegistrationTextKey_( device_->backend_core,
                                                   device_->registration,
                                                   texts[i+0], texts[i+1] );
      }
  }

  if(!error)
  {
    tag = oyProfile_GetTagById( s, icSigDeviceModelDescTag );
    texts = oyProfileTag_GetText( tag, &texts_n, "", 0,0,0);
    if(texts && texts[0] && texts[0][0] && texts_n == 1 && !dmdd_found)
      error = oyOptions_SetRegistrationTextKey_( device_->backend_core,
                                                 device_->registration,
                                                 "model", texts[0] );
    if(texts_n && texts)
      oyStringListRelease_( &texts, texts_n, oyDeAllocateFunc_ );
  }

  if(!error)
  {
    tag = oyProfile_GetTagById( s, icSigDeviceMfgDescTag );
    texts = oyProfileTag_GetText( tag, &texts_n, "", 0,0,0);
    if(texts && texts[0] && texts[0][0] && texts_n == 1 && !dmnd_found)
      error = oyOptions_SetRegistrationTextKey_( device_->backend_core,
                                                 device_->registration,
                                                 "manufacturer", texts[0] );
    if(texts_n && texts)
      oyStringListRelease_( &texts, texts_n, oyDeAllocateFunc_ );
  }

  l_error = oyOptions_SetSource( device_->backend_core,
                                 oyOPTIONSOURCE_FILTER); OY_ERR

  return error;
}
