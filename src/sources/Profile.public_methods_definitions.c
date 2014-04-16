static oyProfile_s_ ** oy_profile_s_std_cache_ = 0;

/** Function  oyProfile_FromStd
 *  @memberof oyProfile_s
 *  @brief    Create from default color space settings
 *
 *  @param[in]    type           default color space
 *  @param[in]    flags          see -> oyProfile_FromFile()
 *  @param[in]    object         the optional base
 *
 *  @version Oyranos: 0.9.6
 *  @since   2007/11/0 (Oyranos: 0.1.9)
 *  @date    2014/04/04
 */
OYAPI oyProfile_s * OYEXPORT
         oyProfile_FromStd           ( oyPROFILE_e         type,
                                       uint32_t            flags,
                                       oyObject_s          object )
{
  oyProfile_s_ * s = 0;
  char * name = 0;
  oyAlloc_f allocateFunc = 0;
  int pos = type - oyDEFAULT_PROFILE_START;

  if(!oy_profile_s_std_cache_)
  {
    int len = sizeof(oyProfile_s_*) *
                            (oyDEFAULT_PROFILE_END - oyDEFAULT_PROFILE_START);
    oy_profile_s_std_cache_ = oyAllocateFunc_( len );
    memset( oy_profile_s_std_cache_, 0, len );
  }

  if(object)
    allocateFunc = object->allocateFunc_;

  if(type)
    name = oyGetDefaultProfileName ( type, allocateFunc );

  if(oyDEFAULT_PROFILE_START < type && type < oyDEFAULT_PROFILE_END)
    if(oy_profile_s_std_cache_[pos] &&
       oy_profile_s_std_cache_[pos]->file_name_ && name &&
       strcmp(oy_profile_s_std_cache_[pos]->file_name_, name) == 0 )
    {
      if(object->deallocateFunc_)
        object->deallocateFunc_( name );
      else
        oyDeAllocateFunc_( name );
      return oyProfile_Copy( (oyProfile_s*)oy_profile_s_std_cache_[pos], 0 );
    }

  s = oyProfile_FromFile_( name, flags, object );

  if(!s)
  {
    /* try some aliases */
    /* START Debian icc-profiles icc-profiles-icc */
    if(strcmp("XYZ.icc",name) == 0)
    {
      s = oyProfile_FromFile_( "LCMSXYZI.ICM", flags, object );
    }
    else if(strcmp("Lab.icc",name) == 0)
    {
      s = oyProfile_FromFile_( "LCMSLABI.ICM", flags, object );
    }
    else if(strcmp("LStar-RGB.icc",name) == 0)
    {
      s = oyProfile_FromFile_( "eciRGB_v2.icc", flags, object );
    }
    else if(strcmp("sRGB.icc",name) == 0)
    {
      s = oyProfile_FromFile_( "sRGB.icm", flags, object );
    }
    else if(strcmp("ISOcoated_v2_bas.ICC",name))
    {
      s = oyProfile_FromFile_( "ISOcoated_v2_eci.icc", flags, object );
      if(!s)
        s = oyProfile_FromFile_( "Fogra27L.icm", flags, object );
    }
    /* END Debian icc-profiles icc-profiles-icc */
  }

  if(s)
    s->use_default_ = type;
  else
  {
    int count = 0, i;
    char * text = 0;
    char ** path_names = oyProfilePathsGet_( &count, oyAllocateFunc_ );
    for(i = 0; i < count; ++i)
    {
      STRING_ADD( text, path_names[i] );
      STRING_ADD( text, "\n" );
    }

    if(strcmp("LCMSXYZI.ICM",name) == 0 ||
       strcmp("LCMSLABI.ICM",name) == 0 ||
       strcmp("LStar-RGB.icc",name) == 0 ||
       strcmp("sRGB.icc",name) == 0 ||
       strcmp("ISOcoated_v2_bas.ICC",name) == 0
      )
    {
      oyMessageFunc_p( oyMSG_ERROR,(oyStruct_s*)object,
                       OY_DBG_FORMAT_"\n\t%s: \"%s\"\n\t%s\n\t%s\n%s", OY_DBG_ARGS_,
                _("Could not open default ICC profile"),name,
                _("You can get them from http://sf.net/projects/openicc"),
                _("install in the OpenIccDirectory icc path"), text );
    } else
      oyMessageFunc_p( oyMSG_ERROR,(oyStruct_s*)object,
                       OY_DBG_FORMAT_"\n\t%s: \"%s\"\n\t%s\n%s", OY_DBG_ARGS_,
                _("Could not open default ICC profile"), name,
                _("install in the OpenIccDirectory icc path"), text );
  }

  if(oyDEFAULT_PROFILE_START < type && type < oyDEFAULT_PROFILE_END)
    oy_profile_s_std_cache_[pos] = (oyProfile_s_*)oyProfile_Copy( (oyProfile_s*)s, 0 );

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
 *  @param[in]    name           profile file name or ICC ID
 *  @param[in]    flags          OY_NO_CACHE_READ, OY_NO_CACHE_WRITE, OY_COMPUTE - compute ID
 *  @param[in]    object         the optional base
 *
 *  flags supports 
 *  - ::OY_NO_CACHE_READ and ::OY_NO_CACHE_WRITE to disable cache
 *  reading and writing. The cache flags are useful for one time profiles or
 *  scanning large numbers of profiles.
 *  - ::OY_COMPUTE lets newly compute ID
 *  - ::OY_ICC_VERSION_2 and ::OY_ICC_VERSION_4 let select version 2 and 4 profiles separately.
 *  - ::OY_SKIP_NON_DEFAULT_PATH ignore profiles outside of default paths
 *
 *  @version Oyranos: 0.9.6
 *  @since   2007/11/0 (Oyranos: 0.1.9)
 *  @date    2014/04/16
 */
OYAPI oyProfile_s * OYEXPORT
oyProfile_FromFile            ( const char      * name,
                                uint32_t          flags,
                                oyObject_s        object)
{
  oyProfile_s_ * s = 0;
  uint32_t md5[4];

  char * fn = oyFindProfile_( name, flags );
  if(fn)
    s = oyProfile_FromFile_( name, flags, object );

  if(fn) oyFree_m_(fn);

  if(flags & OY_COMPUTE)
    oyProfile_GetHash_( s, OY_COMPUTE );

  oyProfile_GetID( (oyProfile_s*)s );

  if(s)
    return (oyProfile_s*)s;

  if(name && strlen(name) == 32)
  {
    sscanf(name, "%08x%08x%08x%08x", &md5[0],&md5[1],&md5[2],&md5[3] );
    s = (oyProfile_s_*) oyProfile_FromMD5( md5, object );
  }

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

  s = oyProfile_FromMemMove_( size_, &block_, flags, &error, object );

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
OYAPI oyProfile_s * OYEXPORT oyProfile_FromMD5(
                                       uint32_t          * md5,
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
    names = /*(const char**)*/ oyProfileListGet_ ( NULL, 0, &count );

    for(i = 0; i < count; ++i)
    {
      if(names[i])
      {
        if(oyStrcmp_(names[i], OY_PROFILE_NONE) != 0)
          /* ICC ID's are not relyable so we recompute it here */
          tmp = oyProfile_FromFile( names[i], OY_COMPUTE, 0 );

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

/** Function oyProfile_FromTaxiDB
 *  @memberof oyProfile_s
 *  @brief   look up a profile of a device from Taxi DB
 *
 *  The function asks the online ICC Taxi DB for a profile. It is therefore
 *  blocking and can cause a serious delay before returning.
 *
 *  The TAXI_id option is expected to come from 
 *  oyConfig_GetBestMatchFromTaxiDB() or oyDevicesFromTaxiDB().
 *
 *  @param[in]     options             - "TAXI_id" shall provide a string
 *                                       for device driver parameter selection
 *  @param[out]    profile             the resulting profile
 *  @return                            error
 *
 *  @version Oyranos: 0.3.3
 *  @since   2012/01/08 (Oyranos: 0.3.3)
 *  @date    2012/01/08
 */
OYAPI oyProfile_s * OYEXPORT oyProfile_FromTaxiDB (
                                       oyOptions_s       * options,
                                       oyObject_s          object )
{
  int error = !options;
  oyProfile_s * p = NULL;
  oyOptions_s * s = options;
  size_t size = 0;
  char * mem = NULL;
  const char * taxi_id = NULL;

  oyCheckType__m( oyOBJECT_OPTIONS_S, return p )

  if(error > 0)
  {
    WARNc_S( "No options provided. Give up." );
    return p;
  }

  taxi_id = oyOptions_FindString( options, "TAXI_id", 0 );

  if(taxi_id)
    mem = oyReadUrlToMemf_( &size, "r", oyAllocateFunc_,
                            "http://icc.opensuse.org/profile/%s/profile.icc",
                            taxi_id );
  else
    WARNc_S("No TAXI_id provided, Do not know what to download.");

  if(mem && size)
  {
    p = oyProfile_FromMem( size, mem, 0, NULL);
    oyFree_m_( mem ); size = 0;
  }

  return p;
}

/** Function oyProfile_Install
 *  @memberof oyProfile_s
 *  @brief   Install a ICC profile
 *
 *  Without options, the ICC profile will be installed into the users 
 *  ICC profile path.
 *
 *  @param[in]     profile             the profile
 *  @param[in]     options             - "path" can provide a string
 *                                       for manual path selection
 *                                     - "device" = "1" - write to device paths
 *  @return                            error
 *                                     - oyOK - success
 *                                     - >= 1  error
 *                                     - oyERROR_USER msg -> wrong or missing
 *                                       argument
 *                                     - oyERROR_DATA_WRITE msg -> path can not
 *                                       be written
 *                                     - oyERROR_DATA_AMBIGUITY msg ->
 *                                       "Profile already installed"
 *                                     - oyCORRUPTED msg -> profile not useable
 *
 *  @version Oyranos: 0.9.1
 *  @date    2012/11/13
 *  @since   2012/01/13 (Oyranos: 0.9.1)
 */
OYAPI int OYEXPORT oyProfile_Install ( oyProfile_s       * profile,
                                       oyOptions_s       * options )
{
  int error = !profile ? oyERROR_USER : 0;
  oyProfile_s * s = profile,
              * p = 0;
  size_t size = 0;
  char * data = 0;
  char ** names = 0;
  uint32_t count = 0, i = 0;
  const char * t = 0,
             * desc = 0;
  char * fn = 0;
  char * pn = 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return oyERROR_USER )

  if(error > 0)
  {
    WARNc_S( "No profile provided for installation. Give up." );
    return error;
  }

  /** 1. construct a profile name */
  desc = oyProfile_GetText( s, oyNAME_DESCRIPTION );

  if(desc && desc[0])
  {
    char * ext = 0;

    /** 1.1 add user profile path name by default or custom from "path" option
     */
    if(oyOptions_FindString( options, "path", 0 ) != NULL)
    {
      STRING_ADD( fn, oyOptions_FindString( options, "path", 0 ) );
      STRING_ADD( fn, OY_SLASH );
    } else
      STRING_ADD( fn, OY_USERCOLORDATA OY_SLASH OY_ICCDIRNAME OY_SLASH );

    /** 1.2 for "device" = "1" option add 
     *      xxx/devices/device_class_description_xxx/ */
    if(oyOptions_FindString( options, "device", "1" ) != NULL)
    {
      STRING_ADD( fn, "devices" OY_SLASH );
      STRING_ADD( fn, oyICCDeviceClassDescription(
                      oyProfile_GetSignature( s, oySIGNATURE_CLASS ) ) );
      STRING_ADD( fn, OY_SLASH );
    }

    /** 1.3 add ".icc" suffix as needed */
    t = strrchr(desc, '.');
    STRING_ADD( pn, desc );
    if(t)
    {
      ++t;
      STRING_ADD( ext, t );
      if(oyStringCaseCmp_(ext,"icc") != 0 &&
         oyStringCaseCmp_(ext,"icm") != 0)
      {
        oyFree_m_(ext);
        ext = 0;
      }
    }
    if(!ext)
      STRING_ADD( pn, ".icc" );
    else
      oyFree_m_(ext);

    STRING_ADD( fn, pn );
  } else
  {
    WARNcc1_S( s, "%s",_("Profile contains no description") );
    return oyCORRUPTED;
  }

  /** 2. check if file or description name exists */
  names = /*(const char**)*/ oyProfileListGet_ ( NULL, 0, &count );
  for(i = 0; i < (int)count; ++i)
  {
    p = oyProfile_FromFile(names[i], 0,0);
    t = oyProfile_GetText(p, oyNAME_DESCRIPTION);

    if(t && oyStringCaseCmp_(desc, t) == 0)
    {
      WARNcc2_S( s, "%s: \"%s\"", _("Profile already installed"), t );
      error = oyERROR_DATA_AMBIGUITY;
    }

    if(names[i] && oyStringCaseCmp_(names[i], fn) == 0)
    {
      WARNcc2_S( s, "%s: \"%s\"", _("Profile already installed"), names[i] );
      error = oyERROR_DATA_AMBIGUITY;
    }

    oyProfile_Release( &p );

    if(error != 0) break;
  }

  oyStringListRelease_( &names, count, oyDeAllocateFunc_ );
  if(error != 0)
    goto install_cleanup;

  p = oyProfile_FromFile( pn, OY_SKIP_NON_DEFAULT_PATH, 0 );
  if(p)
  {
    WARNcc2_S(s, "%s: \"%s\"", _("Profile exists already"), pn);
    error = oyERROR_DATA_AMBIGUITY;
    goto install_cleanup;
  }

  /** 3. open profile */
  data = oyProfile_GetMem( s, &size, 0, oyAllocateFunc_ );
  if(data && size)
  {
    /** 3.1 write profile */
    error = oyProfile_ToFile_( (oyProfile_s_*)s, fn );
    if(error)
    {
      WARNcc2_S(s, "%s: \"%s\"", _("Can not write profile"), fn);
      error = oyERROR_DATA_WRITE;
    } else
    {
      oyProfile_s_ * s_ = (oyProfile_s_*)s;
      if(s_->file_name_)
        oyDeAllocateFunc_( s_->file_name_ );
      s_->file_name_ = oyStringCopy_( fn, s_->oy_->allocateFunc_ );
    }
  } else
  {
    WARNcc1_S( s, "%s",_("Could not open profile") );
    error = oyERROR_DATA_READ;
  }

  install_cleanup:
  if(pn) oyFree_m_(pn);
  if(fn) oyFree_m_(fn);
  if(data) oyFree_m_(data);
  oyProfile_Release( &p );

  return error;
}

/** Function  oyProfile_GetChannelsCount
 *  @memberof oyProfile_s
 *  @brief    Number of channels in a color space
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

  s->channels_n_ = oyICCColorSpaceGetChannelCount( s->sig_ );

  return s->channels_n_;
}

/** Function  oyProfile_GetSignature
 *  @memberof oyProfile_s
 *  @brief    Get ICC color space signature
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
    printf("  pcs: %s  color space: %s version: %d.%d.%d\n", 
          oyICCColorSpaceGetName( (icColorSpaceSignature)
                         oyProfile_GetSignature(p,oySIGNATURE_PCS) ),
          oyICCColorSpaceGetName( (icColorSpaceSignature)
                         oyProfile_GetSignature(p,oySIGNATURE_COLOR_SPACE) ),
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
  icUInt64Number ui64;
  icUInt32Number * two32 = (icUInt32Number *) &ui64;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(s->sig_ && type == oySIGNATURE_COLOR_SPACE)
    return s->sig_;

  if(!s->block_)
  {
    if(type == oySIGNATURE_COLOR_SPACE)
      sig = s->sig_ = icSigXYZData;
    return sig;
  }

  h = (icHeader*) s->block_;

  switch(type)
  {
  case oySIGNATURE_COLOR_SPACE:       /* color space */
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
  case oySIGNATURE_ATTRIBUTES:         /* various ICC header attributes */
       memcpy( ui64, h->attributes, sizeof(ui64));
       sig = (icSignature) two32[0]; break;
  case oySIGNATURE_ATTRIBUTES2:        /* various ICC header attributes */
       memcpy( ui64, h->attributes, sizeof(ui64));
       sig = (icSignature) two32[1]; break;
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
  case oySIGNATURE_ILLUMINANT:         /* creation time in UTC */
       sig = oyValueInt32( h->illuminant.X ); break;
  case oySIGNATURE_ILLUMINANT_Y:       /* creation time in UTC */
       sig = oyValueInt32( h->illuminant.Y ); break;
  case oySIGNATURE_ILLUMINANT_Z:       /* creation time in UTC */
       sig = oyValueInt32( h->illuminant.Z ); break;
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
  oyProfile_s_ * s = (oyProfile_s_*)profile;
  int error = !s;
  oyPointer block_ = 0;
  size_t size_ = 128;
  icHeader *h = 0;
  icUInt32Number * two32;

  if(!s)
    return 1;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 1 )

  if(error <= 0 && type == oySIGNATURE_COLOR_SPACE)
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
  case oySIGNATURE_COLOR_SPACE:       /* color space */
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
  case oySIGNATURE_ATTRIBUTES:         /* various ICC header attributes */
       two32 = (icUInt32Number *) h->attributes;
       two32[0] = oyValueUInt32( sig ); break;
  case oySIGNATURE_ATTRIBUTES2:        /* various ICC header attributes */
       two32 = (icUInt32Number *) h->attributes;
       two32[1] = oyValueUInt32( sig ); break;
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
  case oySIGNATURE_ILLUMINANT:         /* creation time in UTC */
       h->illuminant.X = oyValueInt32( sig ); break;
  case oySIGNATURE_ILLUMINANT_Y:       /* creation time in UTC */
       h->illuminant.Y = oyValueInt32( sig ); break;
  case oySIGNATURE_ILLUMINANT_Z:       /* creation time in UTC */
       h->illuminant.Z = oyValueInt32( sig ); break;
  case oySIGNATURE_MAX: break;
  }

  return error;
}

/** Function  oyProfile_SetChannelNames
 *  @memberof oyProfile_s
 *  @brief    Set channel names
 *
 *  The function should be used to specify extra channels or unusual color
 *  layouts like CMYKRB. The number of elements in names_chan should fit to the
 *  channels count or to the color space signature.
 *
 *  You can let single entries empty if they are understandable by the
 *  color space signature. Oyranos will set them for you on request.
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
  icColorSpaceSignature sig = oyProfile_GetSignature( profile, oySIGNATURE_COLOR_SPACE );

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
                    oyICCColorSpaceGetChannelName ( sig, i, oyNAME_NICK ),
                    oyICCColorSpaceGetChannelName ( sig, i, oyNAME_NAME ),
                    oyICCColorSpaceGetChannelName ( sig, i, oyNAME_DESCRIPTION )
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
 *  @param[in] profile  address of a Oyranos named color structure
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
      if(!oyProfile_Hashed_(s))
        error = oyProfile_GetHash_( s, 0 );

      if(error <= 0)
      {
        uint32_t * i = (uint32_t*)s->oy_->hash_ptr_;
        if(i)
          oySprintf_(temp, "%08x%08x%08x%08x", i[0], i[1], i[2], i[3]);
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
      char * file_name = oyProfile_GetFileName_r( s, 0, oyAllocateFunc_ );

      if(oyProfile_Hashed_(s))
        error = oyProfile_GetHash_( s, 0 );

      if(s->use_default_ && error <= 0)
        oyWidgetTitleGet( (oyWIDGET_e)s->use_default_, 0, &text, 0, 0 );

      oySprintf_( temp, "<profile use_default=\"%s\" file_name=\"%s\" hash=\"",
             oyNoEmptyName_m_(text),
             oyNoEmptyName_m_(file_name) );
      if(i)
        oySprintf_( &temp[oyStrlen_(temp)], "%08x%08x%08x%08x\" />",
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
      error = oyProfile_GetHash_( s, 0 );

    if(!found && error <= 0)
    {
      uint32_t * i = (uint32_t*)s->oy_->hash_ptr_;
      oySprintf_(temp, "%08x%08x%08x%08x", i[0], i[1], i[2], i[3]);
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
  int error = !s,
      i;
  uint32_t md5[4];
  char * data;

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
      s->tags_modified_ = 0;
      s->use_default_ = 0;
      if(s->file_name_)
        profile->oy_->deallocateFunc_( s->file_name_ );
      s->file_name_ = 0;
      if(s->block_ && s->size_)
        profile->oy_->deallocateFunc_( s->block_ );
      s->size_ = 0;
      s->block_ = oyAllocateWrapFunc_( *size,
                                             profile->oy_->allocateFunc_ );
      error = !memcpy( s->block_, block, *size );
      if(error <= 0)
        s->size_ = *size;
      oyObject_SetNames( profile->oy_, 0,0,0 );
      oyProfile_GetText(profile, oyNAME_NICK);
      oyProfile_GetText(profile, oyNAME_NAME);
      oyProfile_GetText(profile, oyNAME_DESCRIPTION);
    }

    /* get actual ICC profile ID */
    oyProfile_GetMD5( profile, OY_COMPUTE, md5 );

    /* Write ICC profile ID into memory */
    for(i = 0; i < 4; ++i)
      md5[i] = oyValueUInt32( md5[i] );
    data = block;
    if(data && (int)*size >= 132)
      memcpy( &data[84], md5, 16 );
  }

  if(s)
    oyObject_UnLock( s->oy_,__FILE__,__LINE__ );

  return block;
}
/** Function  oyProfile_GetSize
 *  @memberof oyProfile_s
 *  @brief    Get the ICC profile in memory size
 *
 *  The prefered memory comes from the unmodified original memory.
 *  Otherwise a previously modified tag list is serialised into memory.
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/10/05
 *  @since    2012/10/05 (Oyranos: 0.5.0)
 */
OYAPI size_t OYEXPORT
                   oyProfile_GetSize ( oyProfile_s       * profile,
                                       uint32_t            flag )
{
  oyProfile_s_ * s = (oyProfile_s_*)profile;
  int error = !s;
  size_t size = 0;
  char * data;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(error <= 0 && s->type_ == oyOBJECT_PROFILE_S)
  {
    if(s->size_ && s->block_ && !s->tags_modified_)
    {
      return s->size_;

    } else
    {
      data = oyProfile_GetMem( profile, &size, flag,
                               profile->oy_->allocateFunc_ );
      if(data && size)
        profile->oy_->deallocateFunc_( data ); data = 0;
    }
  }

  return size;
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
      if(oyProfileTagPriv_m(tag)->use == oyProfileTagPriv_m((*obj))->use)
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
    tag = oyProfileTag_Create( list, signature, tt, 0,OY_MODULE_NICK, 0);
    error = !tag;
  }

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
OYAPI const char * OYEXPORT oyProfile_GetFileName (
                                           oyProfile_s       * profile,
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
      names = /*(const char**)*/ oyProfileListGet_ ( NULL, 0, &count );

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
                oySprintf_(tmp_hash, "%08x%08x%08x%08x", h[0], h[1], h[2], h[3]);
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

      if(!name && hash)
      {
        uint32_t md5[4];
        sscanf(hash, "%08x%08x%08x%08x", &md5[0],&md5[1],&md5[2],&md5[3] );
        tmp = oyProfile_FromMD5( md5, NULL );
        name = oyStringCopy_( oyProfile_GetFileName(tmp, -1), s->oy_->allocateFunc_ );
        oyProfile_Release( &tmp );
      }

      if(hash)
      {
        char * key = oyAllocateFunc_(80);
        txt = oyFindProfile_( name, 0 );
        sprintf( key, "//"OY_TYPE_STD"/profile.icc/psid_%d", dl_pos );
        oyOptions_SetFromText( &s->oy_->handles_,
                               key,
                               txt,
                               OY_CREATE_NEW );
        oyDeAllocateFunc_( txt );
        name = oyOptions_FindString( s->oy_->handles_,
                                     key, 0 );
        oyFree_m_( key );
      } else
      {
        s->file_name_ = oyFindProfile_( name, 0 );
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

/** Function  oyProfile_GetDevice
 *  @memberof oyProfile_s
 *  @brief    Obtain device information from a profile
 *
 *  @verbatim
    oyConfig_s * device = oyConfig_FromRegistration( "//" OY_TYPE_STD "/config", object );
    oyProfile_GetDevice( profile, device ); @endverbatim
 *
 *  @param[in]     profile             the profile
 *  @param[in,out] device              the device description
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/05/22 (Oyranos: 0.1.10)
 *  @date    2010/10/26
 */
OYAPI int OYEXPORT
                   oyProfile_GetDevice ( oyProfile_s     * profile,
                                         oyConfig_s      * device )
{
  int error = !profile, l_error = 0;
  oyProfile_s_ * s = (oyProfile_s_*)profile;
  oyConfig_s_ * device_ = (oyConfig_s_*)device;
  oyProfileTag_s * tag = 0;
  char ** texts = 0;
  int32_t texts_n = 0;
  int i,
      dmnd_found = 0, dmdd_found = 0, serial_found = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(!error)
  {
    tag = oyProfile_GetTagById( (oyProfile_s*)s, icSigMetaDataTag );
    texts = oyProfileTag_GetText( tag, &texts_n, "", 0,0,0);
    if(texts && texts[0] && texts_n > 0)
    {
      for(i = 2; i+1 < texts_n && error <= 0; i+=2)
      {
        if(!texts[i+0])
          continue;

        if(strcmp(texts[i+0],"model") == 0) dmdd_found = 1;
        if(strcmp(texts[i+0],"manufacturer") == 0) dmnd_found = 1;
        if(strcmp(texts[i+0],"serial") == 0) serial_found = 1;

        error = oyOptions_SetRegistrationTextKey_(
                                           (oyOptions_s_*)device_->backend_core,
                                                   device_->registration,
                                                   texts[i+0], texts[i+1] );
      }

      if(!serial_found)
      {
        /* search for a key ending on _serial to strip namespaces */
        for(i = 2; i+1 < texts_n && error <= 0; i+=2)
        {
          int key_len, s_len;

          if(!texts[i+0])
            continue;

          key_len = strlen(texts[i+0]);
          s_len = strlen("serial");

          if(key_len > s_len &&
             strcmp(&texts[i+0][key_len-s_len-1],"_serial") == 0)
          {
            error = oyOptions_SetRegistrationTextKey_(
                                           (oyOptions_s_*)device_->backend_core,
                                                 device_->registration,
                                                 "serial", texts[i+1] );
            DBG_NUM1_S("added serial: %s", texts[i+1]);
            break;
          }
        }
      }
    }
  }

  if(!error)
  {
    tag = oyProfile_GetTagById( (oyProfile_s*)s, icSigDeviceModelDescTag );
    texts = oyProfileTag_GetText( tag, &texts_n, "", 0,0,0);
    if(texts && texts[0] && texts[0][0] && texts_n == 1 && !dmdd_found)
      error = oyOptions_SetRegistrationTextKey_(
                                           (oyOptions_s_*)device_->backend_core,
                                                 device_->registration,
                                                 "model", texts[0] );
    if(texts_n && texts)
      oyStringListRelease_( &texts, texts_n, oyDeAllocateFunc_ );
  }

  if(!error)
  {
    tag = oyProfile_GetTagById( (oyProfile_s*)s, icSigDeviceMfgDescTag );
    texts = oyProfileTag_GetText( tag, &texts_n, "", 0,0,0);
    if(texts && texts[0] && texts[0][0] && texts_n == 1 && !dmnd_found)
      error = oyOptions_SetRegistrationTextKey_(
                                           (oyOptions_s_*)device_->backend_core,
                                                 device_->registration,
                                                 "manufacturer", texts[0] );
    if(texts_n && texts)
      oyStringListRelease_( &texts, texts_n, oyDeAllocateFunc_ );
  }

  l_error = oyOptions_SetSource( device_->backend_core,
                                 oyOPTIONSOURCE_FILTER); OY_ERR

  return error;
}

/** Function  oyProfile_GetMD5
 *  @memberof oyProfile_s
 *  @brief    Get the ICC profile md5 hash sum
 *
 *  The ICC profiles ID is returned. On request it can be recomputed through
 *  the OY_COMPUTE flag. That computed ID will be used internally as a hash
 *  value. The original profile ID can always be obtained through the
 *  OY_FROM_PROFILE flags until writing of the profile.
 *
 *  @param[in,out] profile             the profile
 *  @param[in]     flags               OY_COMPUTE will calculate the hash
 *                                     OY_FROM_PROFILE - original profile ID
 *  @param[out]    md5                 the the ICC md5 based profile ID
 *  @return                            0 - good, 1 >= error, -1 <= issue(s)
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/01/30 (Oyranos: 0.3.0)
 *  @date    2011/04/10
 */
int                oyProfile_GetMD5  ( oyProfile_s       * profile,
                                       int                 flags,
                                       uint32_t          * md5 )
{
  oyProfile_s_ * s = (oyProfile_s_*)profile;
  int error = !s;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(!oyProfile_Hashed_(s) ||
     flags & OY_COMPUTE ||
     s->tags_modified_)
    error = oyProfile_GetHash_( s, OY_COMPUTE );

  if(oyProfile_Hashed_(s))
  {
    if(!(flags & OY_FROM_PROFILE))
      memcpy( md5, s->oy_->hash_ptr_, OY_HASH_SIZE );
    else
    if(s->block_ && s->size_ >= 132)
    {
      int i;
      char * data = s->block_;
      memcpy( md5, &data[84], 16 );
      for(i = 0; i < 4; ++i)
        md5[i] = oyValueUInt32( md5[i] );
    } else
      error = -3;
  }
  else if(error > 0)
    error += 1;
  else
    error = 1;

  return error;
}

/**
 *  Function  oyProfile_AddDevice
 *  @memberof oyProfile_s
 *  @brief    Add device and driver informations to a profile
 *
 *  oyProfile_AddDevice() is for storing device/driver informations in a 
 *  ICC profile. So the profile can be sent over internet and Oyranos, or 
 *  an other CMS, can better match to a device/driver on the new host.
 *  The convention what to place into the ICC profile is dependent on each
 *  device class and its actual driver or driver type.
 *  The meta data is stored in the ICC 'meta' tag of type 'dict'.
 *
 *  @param[in,out] profile             the profile
 *  @param[in]     device              device and driver informations
 *  @param[in]     options             - "key_prefix_required" : prefix
 *                                       accept only key names with the prefix
 *                                       Separation by point '.' is allowed.
 *                                     - "set_device_attributes"="true"
 *                                       will write "manufacturer", "model",
 *                                       "mnft" and "model_id" keys to the
 *                                       appropriate profile tags and fields.
 *
 *  @version Oyranos: 0.3.2
 *  @since   2009/05/18 (Oyranos: 0.1.10)
 *  @date    2011/08/21
 */
#if 0
TODO find a general form. Do we want to support the mluc type or is that better
up to a specialised GUI?
int                oyProfile_AddDevice(oyProfile_s       * profile,
                                       oyConfig_s        * device )
{
  int error = !profile;
  oyProfile_s_ * s = (oyProfile_s_*)profile;
  oyProfileTag_s_ * pddt = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILE_S, return 0 )

  if(error <= 0)
  {
      pddt = (oyProfileTag_s_*)oyProfile_GetTagById( (oyProfile_s*)s, icSigProfileDetailDescriptionTag_ );

      /* icSigProfileDetailDescriptionTag_ */
      if(error <= 0 && !pddt)
      {
        oyStructList_s * list = 0;

        list = oyStructList_New(0);
        error = oyStructList_MoveIn( list, (oyStruct_s**) &device, 0,
                                     OY_OBSERVE_AS_WELL );

        if(error <= 0)
        {
          pddt = (oyProfileTag_s_*)oyProfileTag_Create( list, icSigProfileDetailDescriptionTag_,
                                                        icSigProfileDetailDescriptionTag_,
                                                        0, OY_MODULE_NICK, 0);
          error = !pddt;
        }

        oyStructList_Release( &list );

        if(pddt)
        {
          error = oyProfile_TagMoveIn_( s, &pddt, -1 );
          ++s->tags_modified_;
        }
      }
  }

  return error;
}
#else
int                oyProfile_AddDevice(oyProfile_s       * profile,
                                       oyConfig_s        * device,
                                       oyOptions_s       * options )
{
  int error = 0;
  int i,j, len, size, block_size, pos;

  char ** keys,
       ** values,
        * key,
        * val;
  const char * r;
  void * string = 0;
  const char * key_prefix_required = oyOptions_FindString( options,
                                            "key_prefix_required", 0 );
  const char * key_prefix = oyConfig_FindString( device, "prefix", 0 );
  const char * prefix = 0;
  char ** key_prefix_texts = 0;
  int key_prefix_texts_n = 0;
  int * key_prefix_texts_len = 0;
  char * manufacturer=0, * model=0, *mnft=0, *model_id=0;

  /* get just some device */
  oyOption_s * o = 0;
  oyConfig_s * d = device;

  oyProfile_s * p = profile;
  oyProfileTag_s * dict_tag;
  icDictTagType * dict;
  icNameValueRecord * record;

  int n = 0;
  int count = 0;

  if(key_prefix_required)
    prefix = key_prefix_required;
  else if(key_prefix)
    prefix = key_prefix;

  if(prefix)
  {
    key_prefix_texts = oyStringSplit_( prefix,'.',
                                       &key_prefix_texts_n, oyAllocateFunc_);
    oyAllocHelper_m_( key_prefix_texts_len,int,key_prefix_texts_n, 0, return 1);
    for(j = 0; j < key_prefix_texts_n; ++j)
      key_prefix_texts_len[j] = strlen( key_prefix_texts[j] );

    if(!key_prefix)
    {
      o = oyOption_New( NULL );
      oyOption_SetRegistration( o, "////prefix" );
      oyOption_SetFromText( o, prefix, 0 );
      oyOptions_MoveIn( *oyConfig_GetOptions(d,"backend_core"), &o, 0 );
    }
  }

  /* count valid entries */
  n = oyConfig_Count( d );
  for(i = 0; i < n; ++i)
  {
    char * reg = 0;
    o = oyConfig_Get( d, i );
    r = oyOption_GetRegistration(o);
    reg = oyFilterRegistrationToText( r, oyFILTER_REG_OPTION, oyAllocateFunc_ );
    val = oyOption_GetValueText( o, oyAllocateFunc_ );
    if(val)
    {
      int pass = 1;

      if(prefix)
      {
        int len = strlen( reg );
        if(key_prefix_required)
          pass = 0;
        if(strcmp(reg,"prefix") == 0)
          pass = 1;
        else
        for(j = 0; j < key_prefix_texts_n; ++j)
        {
          if(len >= key_prefix_texts_len[j] &&
             memcmp( key_prefix_texts[j], reg, key_prefix_texts_len[j]) == 0)
            pass = 1;
          if(pass && len > key_prefix_texts_len[j])
          {
            if( strcmp( reg+key_prefix_texts_len[j], "manufacturer") == 0 )
              manufacturer = oyStringCopy_( val, oyAllocateFunc_ );
            if( strcmp( reg+key_prefix_texts_len[j], "model") == 0 )
              model = oyStringCopy_( val, oyAllocateFunc_ );
            if( strcmp( reg+key_prefix_texts_len[j], "mnft") == 0 )
              mnft = oyStringCopy_( val, oyAllocateFunc_ );
            if( strcmp( reg+key_prefix_texts_len[j], "model_id") == 0 )
              model_id = oyStringCopy_( val, oyAllocateFunc_ );
          }
        }
      }

      if(pass)
      {
        DBG_PROG2_S("%s: %s", reg, val );
        ++count;
      }
    }
    if(reg) oyDeAllocateFunc_(reg);
    if(val) oyDeAllocateFunc_(val);
  }

  /* collect data */
  size = 16 /* or 24 or 32*/
         * n + sizeof(icDictTagType),
  block_size = size;
  pos = 0;

  keys = oyAllocateFunc_( 2 * count * sizeof(char*));
  values = oyAllocateFunc_( 2 * count * sizeof(char*));
  for(i = 0; i < n; ++i)
  {
    o = oyConfig_Get( d, i );
    r = oyOption_GetRegistration(o);
    key = oyFilterRegistrationToText( r, oyFILTER_REG_OPTION,
                                             oyAllocateFunc_ );
    val = oyOption_GetValueText( o, oyAllocateFunc_ );
    if(val)
    {
      int pass = 1;

      if(key_prefix_required)
      {
        len = strlen( key );
        pass = 0;
        if(strcmp(key,"prefix") == 0)
          pass = 1;
        else
        for(j = 0; j < key_prefix_texts_n; ++j)
        {
          if(len >= key_prefix_texts_len[j] &&
             memcmp( key_prefix_texts[j], key, key_prefix_texts_len[j]) == 0)
            pass = 1;
        }
      }

      if(pass)
      {
        keys[pos] = key;
        values[pos] = oyStringCopy_(val,oyAllocateFunc_);
        DBG_PROG2_S("%s: %s", key, val );
        len = strlen( key ) * 2;
        len = len + (len%4 ? 4 - len%4 : 0);
        block_size += len;
        len = strlen( val ) * 2;
        len = len + (len%4 ? 4 - len%4 : 0);
        block_size += len;
        block_size += + 2;
        ++pos;
        key = 0;
      }
    }
    if(key) oyDeAllocateFunc_( key ); key = 0;
    if(val) oyDeAllocateFunc_(val);
  }

  dict = calloc(sizeof(char), block_size);
  dict->sig = oyValueUInt32( icSigDictType );
  dict->number = oyValueUInt32( count );
  dict->size = oyValueUInt32( 16 );

  pos = size;
  for(i = 0; i < count; ++i)
  {
    record = (icNameValueRecord*)((char*)dict + sizeof(icDictTagType) + 16 * i);

    len = 0;
    string = NULL;
    error = oyIconvGet( keys[i], &string, &len, "UTF-8", "UTF-16BE",
                        oyAllocateFunc_ );
    record->name_string_offset = oyValueUInt32( pos );
    len = strlen( keys[i] ) * 2;
    len = len + (len%4 ? 4 - len%4 : 0);
    record->name_string_size =  oyValueUInt32( len );
    memcpy(((char*)dict)+pos, string, len );
    oyFree_m_( string );
    pos += len;

    len = 0;
    string = NULL;
    error = oyIconvGet( values[i], &string, &len, "UTF-8", "UTF-16BE", 
                        oyAllocateFunc_ );
    record->value_string_offset =  oyValueUInt32( pos );
    len = strlen( values[i] ) * 2;
    len = len + (len%4 ? 4 - len%4 : 0);
    record->value_string_size =  oyValueUInt32( len );
    memcpy(((char*)dict)+pos, string, len );
    pos += len;
    oyFree_m_( string );
  }

  dict_tag = oyProfileTag_New(NULL);
  error = oyProfileTag_Set( dict_tag, icSigMetaDataTag, icSigDictType,
                            oyOK, block_size, dict );
  if(error <= 0)
    error = oyProfile_TagMoveIn( p, &dict_tag, -1 );

  if(oyOptions_FindString( options, "set_device_attributes", "true" ))
  {
      uint32_t model_idi = 0;
      const char * t = 0;
      char * data;
      size_t size = 0;
      icHeader * header = 0;

      oyProfileTag_s * tag = oyProfile_GetTagByPos( p, 0 );
      char h[5] = {"head"};
      uint32_t * hi = (uint32_t*)&h;
      char *tag_block = 0;

      data = oyProfile_GetMem( p, &size, 0, oyAllocateFunc_ );
      header = (icHeader*) data;
      t = mnft;
      if(t)
        sprintf( (char*)&header->manufacturer, "%s", t );
      t = model_id;
      if(t)
        model_idi = atoi( t );
      model_idi = oyValueUInt32( model_idi );
      memcpy( &header->model, &model_idi, 4 );

      oyAllocHelper_m_( tag_block, char, 132, 0, return 0 );
      error = !memcpy( tag_block, data, 132 );
      error = oyProfileTag_Set( tag, (icTagSignature)*hi,
                                (icTagTypeSignature)*hi,
                                oyOK, 132, tag_block );
      t = manufacturer;
      if(t)
        error = oyProfile_AddTagText( p, icSigDeviceMfgDescTag, t );
      t =  model;
      if(t)
        error = oyProfile_AddTagText( p, icSigDeviceModelDescTag, t );
      if(data && size)
        oyFree_m_( data );
  }

  oyStringListRelease_( &keys, count, oyDeAllocateFunc_ );
  oyStringListRelease_( &values, count, oyDeAllocateFunc_ );
  if(key_prefix_texts_n)
  {
    oyStringListRelease_( &key_prefix_texts, key_prefix_texts_n,
                          oyDeAllocateFunc_ );
    oyDeAllocateFunc_( key_prefix_texts_len );
  }

  if(manufacturer) oyFree_m_( manufacturer );
  if(model) oyFree_m_( model );
  if(model_id) oyFree_m_( model_id );
  if(mnft) oyFree_m_( mnft );

  return error;
}
#endif
