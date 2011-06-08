/** Function  oyProfiles_Create
 *  @memberof oyProfiles_s
 *  @brief    Get a list of installed profiles
 *
 *  @param[in]     patterns            a list properties, e.g. classes
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/20 (Oyranos: 0.1.8)
 *  @date    2008/06/20
 */
OYAPI oyProfiles_s * OYEXPORT
                 oyProfiles_Create( oyProfiles_s   * patterns,
                                    oyObject_s       object)
{
  oyProfiles_s * s = 0;
  int error = 0;

  oyProfile_s * tmp = 0, * pattern = 0;
  char  ** names = 0, * full_name = 0;
  oyPointer block = 0;
  uint32_t names_n = 0, i = 0, j = 0, n = 0,
           patterns_n = oyProfiles_Count(patterns);
  size_t   size = 128;

  s = oyProfiles_New( object );
  error = !s;

  if(error <= 0)
  {
    names = oyProfileListGet_ ( NULL, &names_n );

    for(j = 0; j < patterns_n; ++j)
    {
      pattern = oyProfiles_Get(patterns, j);

      if(oyProfilePriv_m(pattern)->size_ > 132)
        size = 0;

      oyProfile_Release( &pattern );
    }

    if(oyProfiles_Count( oy_profile_list_cache_ ) != names_n)
    {
      for(i = 0; i < names_n; ++i)
      {
        if(names[i])
        {
          if(oyStrcmp_(names[i], OY_PROFILE_NONE) != 0)
          {
            if(size && 0)
            { /* TODO short readings */
              full_name = oyFindProfile_(names[i]);
              block = oyReadFileToMem_ (full_name, &size, oyAllocateFunc_);
              tmp = oyProfile_FromMemMove_( size, &block, 0, &error, 0 );
            }
            else
            {
              tmp = oyProfile_FromFile( names[i], OY_NO_CACHE_WRITE, 0 );
              oy_profile_list_cache_ = oyProfiles_MoveIn(oy_profile_list_cache_,
                                                         &tmp, -1);
              error = !oy_profile_list_cache_;
            }
          }
        }
      }
    }

    n = oyProfiles_Count( oy_profile_list_cache_ );
    if(oyProfiles_Count( oy_profile_list_cache_ ) != names_n)
      WARNc2_S("updated oy_profile_list_cache_ differs: %d %d",n, names_n);
    oyStringListRelease_( &names, names_n, oyDeAllocateFunc_ ); names_n = 0;

    for(i = 0; i < n; ++i)
    {
        tmp = oyProfiles_Get( oy_profile_list_cache_, i );

        if(patterns_n > 0)
        {
          for(j = 0; j < patterns_n; ++j)
          {
            if(tmp)
              pattern = oyProfiles_Get(patterns, j);

            if(oyProfile_Match_( (oyProfile_s_*)pattern, (oyProfile_s_*)tmp ))
            {
              s = oyProfiles_MoveIn( s, &tmp, -1);
              error = !s;
              break;
            }

            oyProfile_Release( &pattern );
          }

        } else {

          s = oyProfiles_MoveIn( s, &tmp, -1);
          error = !s;
        }

        oyProfile_Release( &tmp );
    }
  }

  return s;
}

/** Function  oyProfiles_ForStd
 *  @memberof oyProfiles_s
 *  @brief    Get a list of installed profiles
 *
 *  Allow for a special case with oyDEFAULT_PROFILE_START in the colour_space
 *  argument, to select all possible standard colour profiles, e.g. for 
 *  typical colour conversions.
 *
 *  oyASSUMED_WEB will result in exactly one profile added as long as it is
 *  available in the file paths.
 *
 *  @param[in]     std_profile_class  standard profile class, e.g. oyEDITING_RGB
 *  @param[out]    current             get the colour_space profile position
 *  @param         object              a optional object
 *  @return                            the profile list
 *
 *  @par Example - get all standard RGB profiles:
 *  @verbatim
    oyPROFILE_e type = oyEDITING_RGB;
    int current = 0,
        size, i;
    oyProfile_s * temp_prof = 0;
    oyProfiles_s * iccs = 0;

    iccs = oyProfiles_ForStd( type, &current, 0 );

    size = oyProfiles_Count(iccs);
    for( i = 0; i < size; ++i)
    {
      temp_prof = oyProfiles_Get( iccs, i );
      printf("%s %d: \"%s\" %s\n", i == current ? "*":" ", i,
             oyProfile_GetText( temp_prof, oyNAME_DESCRIPTION ),
             oyProfile_GetFileName(temp_prof, -1));
      oyProfile_Release( &temp_prof );
    } @endverbatim
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/25 (Oyranos: 0.1.8)
 *  @date    2008/08/06
 */
OYAPI oyProfiles_s * OYEXPORT
                 oyProfiles_ForStd   ( oyPROFILE_e         std_profile_class,
                                       int               * current,
                                       oyObject_s          object)
{
  oyPROFILE_e type = std_profile_class;
    char * default_p = 0;
    int i, val = -1;

    char  * temp = 0,
          * text = 0;
    uint32_t size = 0;
    oyProfiles_s * iccs = 0, * patterns = 0;
    oyProfile_s * profile = 0, * temp_prof = 0;
    icSignature csp;

    if(type == oyASSUMED_WEB)
    {
      profile = oyProfile_FromStd( type, object );
      iccs = oyProfiles_New( object );
      if(current)
      {
        if(profile)
          *current          = 0;
        else
          *current          = -1;
      }
      oyProfiles_MoveIn( iccs, &profile, 0 );
      return iccs;
    }

    if(type == oyEDITING_XYZ ||
       type == oyASSUMED_XYZ ||
       type == oyEDITING_LAB ||
       type == oyASSUMED_LAB ||
       type == oyEDITING_RGB ||
       type == oyASSUMED_RGB ||
       type == oyEDITING_CMYK ||
       type == oyASSUMED_CMYK ||
       type == oyPROFILE_PROOF ||
       type == oyEDITING_GRAY ||
       type == oyASSUMED_GRAY)
      default_p = oyGetDefaultProfileName( (oyPROFILE_e)type, oyAllocateFunc_);

    /* prepare the patterns according to the profile type */
    if(type == oyEDITING_XYZ ||
       type == oyASSUMED_XYZ)
    {
      csp = icSigXYZData;

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigColorSpaceClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigInputClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigDisplayClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );
    }
    if(type == oyEDITING_LAB ||
       type == oyASSUMED_LAB)
    {
      csp = icSigLabData;

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigColorSpaceClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigInputClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigDisplayClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );
    }
    if(type == oyEDITING_RGB ||
       type == oyASSUMED_RGB)
    {
      csp = icSigRgbData;

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigColorSpaceClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigInputClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigDisplayClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );
    }
    /* support typical output Rgb device for cinema and print proofing */
    if(type == oyPROFILE_PROOF)
    {
      csp = icSigRgbData;

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigOutputClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigDisplayClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );
    }
    if(type == oyEDITING_CMYK ||
       type == oyASSUMED_CMYK ||
       type == oyPROFILE_PROOF)
    {
      csp = icSigCmykData;

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigOutputClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigInputClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigDisplayClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigColorSpaceClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );
    }
    if(type == oyEDITING_GRAY ||
       type == oyASSUMED_GRAY)
    {
      csp = icSigGrayData;

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigInputClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigDisplayClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOUR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigColorSpaceClass, oySIGNATURE_CLASS);
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );
    }
    if(type == oyDEFAULT_PROFILE_START)
    {
      profile = oyProfile_FromSignature( icSigColorSpaceClass,
                                         oySIGNATURE_CLASS, 0 );
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );
      profile = oyProfile_FromSignature( icSigInputClass,
                                         oySIGNATURE_CLASS, 0 );
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );
      profile = oyProfile_FromSignature( icSigOutputClass,
                                         oySIGNATURE_CLASS, 0 );
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );
      profile = oyProfile_FromSignature( icSigDisplayClass,
                                         oySIGNATURE_CLASS, 0 );
      patterns = oyProfiles_MoveIn( patterns, &profile, -1 );
    }

    /* get the profile list */
    iccs = oyProfiles_Create( patterns, 0 );

    /* detect the default profile position in our list */
    size = oyProfiles_Count(iccs);
    if(default_p)
    for( i = 0; i < size; ++i)
    {
      temp_prof = oyProfiles_Get( iccs, i );
      text = oyStringCopy_( oyProfile_GetFileName(temp_prof, -1),
                            oyAllocateFunc_ );
      temp = oyStrrchr_( text, '/' );
      if(temp)
        ++temp;
      else
        temp = text;

      if(oyStrstr_( temp, default_p) &&
         oyStrlen_( temp ) == oyStrlen_(default_p))
      {
        val = i;
        break;
      }

      oyProfile_Release( &temp_prof );
      oyDeAllocateFunc_( text );
    }

    if(current)
      *current          = val;

    if(default_p)
      oyFree_m_( default_p );
    oyProfiles_Release( &patterns );

  return iccs;
}

/** Function  oyProfiles_MoveIn
 *  @memberof oyProfiles_s
 *  @brief    Move a profile inside the profiles list.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 november 2007 (API 0.1.8)
 */
oyProfiles_s* oyProfiles_MoveIn      ( oyProfiles_s      * list,
                                       oyProfile_s      ** obj,
                                       int                 pos )
{
  int error = 0;
  oyProfiles_s_ * s = (oyProfiles_s_*)list;

  if(s)
    oyCheckType__m( oyOBJECT_PROFILES_S, return 0 )

  if(obj && *obj && (*obj)->type_ == oyOBJECT_PROFILE_S)
  {
    if(!list)
      list = oyProfiles_New(0);

    if(list && s->list_)
        error = oyStructList_MoveIn( s->list_, (oyStruct_s**) obj, pos,
                                     OY_OBSERVE_AS_WELL );
  }

  return list;
}

/** Function  oyProfiles_ReleaseAt
 *  @memberof oyProfiles_s
 *  @brief    Release profile at position pos.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 november 2007 (API 0.1.8)
 */
int              oyProfiles_ReleaseAt( oyProfiles_s      * list,
                                       int                 pos )
{
  int error = 0;
  oyProfiles_s_ * s = (oyProfiles_s_*)list;

  if(!s)
    return 1;

  oyCheckType__m( oyOBJECT_PROFILES_S, return 1 )

  if(s && s->list_)
    error = oyStructList_ReleaseAt( s->list_, pos );

  return error;
}

/** Function  oyProfiles_Get
 *  @memberof oyProfiles_s
 *  @brief    Get a copy of the profile at position pos.
 *
 *  @param[in] list                    the profile list to use
 *  @param[in] pos                     the position in list
 *  @return                            a copy of the profile owned by the caller
 *
 *  @since Oyranos: version 0.1.8  2007/11/22
 *  @date  20 december 2007 (API 0.1.8)
 */
oyProfile_s *    oyProfiles_Get      ( oyProfiles_s      * list,
                                       int                 pos )
{
  oyProfile_s * obj = 0;
  oyProfiles_s_ * s = (oyProfiles_s_*)list;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILES_S, return 0 )

  if(s && s->list_)
  {
    oyProfile_s * p = (oyProfile_s*) oyStructList_GetType_( s->list_,
                                                 pos, oyOBJECT_PROFILE_S );

    if(p)
      obj = oyProfile_Copy(p, 0);
  }

  return obj;
}

/** Function  oyProfiles_Count
 *  @memberof oyProfiles_s
 *  @brief    Return the number of profiles in list.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 november 2007 (API 0.1.8)
 */
int              oyProfiles_Count ( oyProfiles_s   * list )
{
  int n = 0;
  oyProfiles_s_ * s = (oyProfiles_s_*)list;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_PROFILES_S, return 0 )

  if(s && s->list_)
    n = oyStructList_Count( s->list_ );

  return n;
}

/** Function oyProfiles_DeviceRank
 *  @memberof oyProfiles_s
 *  @brief   sort a profile list according to a given device
 *
 *  Profiles which match the device will placed according to a rank value on 
 *  top of the list followed by the zero ranked profiles.
 *
 *  @verbatim
    oyProfiles_s * p_list = oyProfiles_ForStd( oyASSUMED_RGB, 0,0 );
    int32_t * rank_list = (int32_t*) malloc( oyProfiles_Count(p_list) *
                                             sizeof(int32_t) );
    oyProfiles_DeviceRank( p_list, oy_device, rank_list );
    n = oyProfiles_Count( p_list );
    for(i = 0; i < n; ++i)
    {
      temp_prof = oyProfiles_Get( p_list, i );
      printf("%d %d: \"%s\" %s\n", rank_list[i], i,
             oyProfile_GetText( temp_prof, oyNAME_DESCRIPTION ),
             oyProfile_GetFileName(temp_prof, 0));
      oyProfile_Release( &temp_prof );
    } @endverbatim
 *
 *  @param[in,out] list                the to be sorted profile list
 *  @param[in]     device              filter pattern
 *  @param[in,out] rank_list           list of rank levels for the profile list
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/05/22 (Oyranos: 0.1.10)
 *  @date    2009/05/22
 */
int              oyProfiles_DeviceRank ( oyProfiles_s    * list,
                                       oyConfig_s        * device,
                                       int32_t           * rank_list )
{
  int error = !list || !device || !rank_list;
  oyProfiles_s * s = list;
  int i,n,rank;
  oyProfile_s * p = 0;
  oyConfig_s * p_device = 0;
  oyOptions_s * old_db = 0;

  if(!list)
    return 0;

  if(error)
    return error;

  oyCheckType__m( oyOBJECT_PROFILES_S, return 0 )

  p_device = oyConfig_New( device->registration, 0 );
  n = oyProfiles_Count( list );

  error = !memset( rank_list, 0, sizeof(int32_t) * n );

  /* oyConfig_Compare assumes its options in device->db, so it is filled here.*/
  if(!oyOptions_Count( device->db ))
  {
    old_db = device->db;
    device->db = device->backend_core;
  }

  for(i = 0; i < n; ++i)
  {
    p = oyProfiles_Get( list, i );

    oyProfile_DeviceGet( p, p_device );
    rank = 0;

    error = oyConfig_Compare( p_device, device, &rank );
    rank_list[i] = rank;

    oyOptions_Clear( p_device->backend_core );
    oyProfile_Release( &p );
  }

  if(!error)
    error = oyStructList_Sort( list->list_, rank_list );

  if(old_db)
    device->db = old_db;

  return error;
}


