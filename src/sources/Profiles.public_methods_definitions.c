oyProfiles_s * oy_profile_list_cache_ = 0;

int oyLowerStrcmpWrap (const void * a_, const void * b_)
{
  const char * a = *(const char **)a_,
             * b = *(const char **)b_;
#ifdef HAVE_POSIX
  return strcasecmp(a,b);
#else
  return strcmp(a,b);
#endif
}

/** Function oyProfiles_Create
 *  @memberof oyProfiles_s
 *  @brief   get a list of installed profiles
 *
 *  @param[in]     patterns            a list properties, e.g. classes;
 *                                     Only matching profiles are selected.
 *                                     If NULL, all profiles are accepted.
 *  @param         flags               
 *                                     - see oyProfile_FromFile()
 *                                     - ::OY_ALLOW_DUPLICATES - do not filter
 *                                     duplicate profile ID's
 *  @param         object              the optional object
 *  @return                            the found and selected profiles
 *
 *  @verbatim
    // Put all ICC Display Class profiles in "profiles"
    icSignature profile_class = icSigDisplayClass;
    oyProfile_s * pattern = 0;
    oyProfiles_s * patterns = oyProfiles_New( 0 ),
                 * profiles = 0;
    uint32_t icc_profile_flags = oyICCProfileSelectionFlagsFromOptions( 
                                      OY_CMM_STD, "//" OY_TYPE_STD "/icc_color",
                                                                     NULL, 0 );

    pattern = oyProfile_FromSignature( profile_class, oySIGNATURE_CLASS, 0 );
    oyProfiles_MoveIn( patterns, &pattern, -1 );

    profiles = oyProfiles_Create( patterns, icc_profile_flags, 0 );
    oyProfiles_Release( &patterns );@endverbatim
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/08/31
 *  @since   2008/06/20 (Oyranos: 0.1.8)
 */
OYAPI oyProfiles_s * OYEXPORT
                 oyProfiles_Create   ( oyProfiles_s      * patterns,
                                       uint32_t            flags,
                                       oyObject_s          object)
{
  oyProfiles_s * s = oyProfiles_New( object ),
               * tmps = oyProfiles_New( object );
  int error = 0;

  oyProfile_s * tmp = 0, * pattern = 0;
  char  ** names = 0;
#if !defined(HAVE_POSIX)
  char *t;
#endif
  uint32_t names_n = 0, i = 0, j = 0, n = 0,
           patterns_n = oyProfiles_Count(patterns);
  int sorts = 0;
  const char ** sort = NULL;

  error = !s;

  if(error <= 0)
  {
    names = oyProfileListGet_ ( NULL, flags, &names_n );

    if(!(flags & OY_ALLOW_DUPLICATES))
      flags |= OY_COMPUTE;

    if(oyProfiles_Count( oy_profile_list_cache_ ) != names_n)
    {
      sorts = names_n;
      sort = oyAllocateFunc_(sorts*sizeof(const char*)*2);
      for(i = 0; i < names_n; ++i)
      {
        if(names[i])
        {
          if(oyStrcmp_(names[i], OY_PROFILE_NONE) != 0)
          {
            tmp = oyProfile_FromFile( names[i], OY_NO_CACHE_WRITE | flags, 0 );
#if !defined(HAVE_POSIX)
            t = 0;
            oyStringAdd_(&t, oyProfile_GetText(tmp, oyNAME_DESCRIPTION), oyAllocateFunc_, 0);
            n = strlen(t);
            /* the following upper caseing is portable,
             * still strcasecmp() might be faster? */
            for(j = 0; j < n; ++j)
              if(isalpha(t[j]))
                t[j] = tolower(t[j]);
            sort[i*2] = t;
#else
            sort[i*2] = oyProfile_GetText(tmp, oyNAME_DESCRIPTION);
#endif
            sort[i*2+1] = names[i];
          }
        }
      }
      qsort( sort, sorts, sizeof(char**)*2, oyLowerStrcmpWrap );
      for(i = 0; i < sorts; ++i)
      {
        tmp = oyProfile_FromFile( sort[i*2+1], OY_NO_CACHE_WRITE | flags, 0 );
        oyProfiles_MoveIn(tmps, &tmp, -1);
#if !defined(HAVE_POSIX)
        t = (char*)sort[i*2];
        oyFree_m_(t);
#endif
      }
      oyProfiles_Release(&oy_profile_list_cache_);
      oy_profile_list_cache_ = tmps;
      oyFree_m_(sort);
    }

    n = oyProfiles_Count( oy_profile_list_cache_ );
    if(oyProfiles_Count( oy_profile_list_cache_ ) != names_n)
      WARNc2_S("updated oy_profile_list_cache_ differs: %d %d",n, names_n);
    oyStringListRelease_( &names, names_n, oyDeAllocateFunc_ ); names_n = 0;

    for(i = 0; i < n; ++i)
    {
        int good = 1;

        tmp = oyProfiles_Get( oy_profile_list_cache_, i );

        if(patterns_n > 0)
        {
          good = 0;
          for(j = 0; j < patterns_n; ++j)
          {
            if(tmp)
              pattern = oyProfiles_Get(patterns, j);

            if(oyProfile_Match_( (oyProfile_s_*)pattern, (oyProfile_s_*)tmp ))
              good = 1;

            oyProfile_Release( &pattern );

            if(good) break;
          }
        }

        if(good &&
           !(flags & OY_ALLOW_DUPLICATES))
        {
          int count = oyProfiles_Count( s ),
              j;
          for(j = 0; j < count; ++j)
          {
            oyProfile_s * b = oyProfiles_Get( s, j );
            if(oyProfile_Equal(b, tmp))
              good = 0;
            oyProfile_Release( &b );
          }
        }

        if(good)
          oyProfiles_MoveIn( s, (oyProfile_s**)&tmp, -1);

        oyProfile_Release( &tmp );
    }
  }

  return s;
}

/** Function  oyProfiles_ForStd
 *  @memberof oyProfiles_s
 *  @brief    Get a list of installed profiles
 *
 *  Allow for a special case with oyDEFAULT_PROFILE_START in the color_space
 *  argument, to select all possible standard color profiles, e.g. for 
 *  typical color conversions.
 *
 *  oyASSUMED_WEB will result in exactly one profile added as long as it is
 *  available in the file paths.
 *
 *  @param[in]     std_profile_class  standard profile class, e.g. oyEDITING_RGB
 *  @param         flags               see oyProfile_FromFile()
 *  @param[out]    current             get the color_space profile position
 *  @param         object              a optional object
 *  @return                            the profile list
 *
 *  @par Example - get all standard RGB profiles:
 *  @verbatim
    // Get all ICC profiles, which can be used as assumed RGB profile
    oyPROFILE_e type = oyEDITING_RGB;
    int current = 0,
        size, i;
    oyProfile_s * temp_prof = 0;
    oyProfiles_s * iccs = 0;
    uint32_t icc_profile_flags = oyICCProfileSelectionFlagsFromOptions( 
                                      OY_CMM_STD, "//" OY_TYPE_STD "/icc_color",
                                                                     NULL, 0 );
 
    iccs = oyProfiles_ForStd( type, icc_profile_flags, &current, 0 );

    size = oyProfiles_Count(iccs);
    for( i = 0; i < size; ++i)
    {
      temp_prof = oyProfiles_Get( iccs, i );
      // Show the profile internal and file names on the command line
      printf("%s %d: \"%s\" %s\n", i == current ? "*":" ", i,
             oyProfile_GetText( temp_prof, oyNAME_DESCRIPTION ),
             oyProfile_GetFileName(temp_prof, -1));
      oyProfile_Release( &temp_prof );
    } @endverbatim
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/04/04
 *  @since   2008/07/25 (Oyranos: 0.1.8)
 */
OYAPI oyProfiles_s * OYEXPORT
                 oyProfiles_ForStd   ( oyPROFILE_e         std_profile_class,
                                       uint32_t            flags,
                                       int               * current,
                                       oyObject_s          object)
{
  oyPROFILE_e type = std_profile_class;
    char * default_p = 0;
    int i, val = -1;

    char  * temp = 0,
          * text = 0;
    uint32_t size = 0;
    oyProfiles_s * iccs = 0, * patterns = oyProfiles_New( object );
    oyProfile_s * profile = 0, * temp_prof = 0;
    icSignature csp;

    if(type == oyASSUMED_WEB)
    {
      profile = oyProfile_FromStd( type, flags, object );
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

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigColorSpaceClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigInputClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigDisplayClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );
    }
    if(type == oyEDITING_LAB ||
       type == oyASSUMED_LAB)
    {
      csp = icSigLabData;

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigColorSpaceClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigInputClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigDisplayClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );
    }
    if(type == oyEDITING_RGB ||
       type == oyASSUMED_RGB)
    {
      csp = icSigRgbData;

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigColorSpaceClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigInputClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigDisplayClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );
    }
    /* support typical output Rgb device for cinema and print proofing */
    if(type == oyPROFILE_PROOF)
    {
      csp = icSigRgbData;

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigOutputClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigDisplayClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );
    }
    if(type == oyEDITING_CMYK ||
       type == oyASSUMED_CMYK ||
       type == oyPROFILE_PROOF)
    {
      csp = icSigCmykData;

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigOutputClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigInputClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigDisplayClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigColorSpaceClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );
    }
    if(type == oyEDITING_GRAY ||
       type == oyASSUMED_GRAY)
    {
      csp = icSigGrayData;

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigInputClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigDisplayClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );

      profile = oyProfile_FromSignature( csp, oySIGNATURE_COLOR_SPACE, 0 );
      oyProfile_SetSignature( profile, icSigColorSpaceClass, oySIGNATURE_CLASS);
      oyProfiles_MoveIn( patterns, &profile, -1 );
    }
    if(type == oyDEFAULT_PROFILE_START)
    {
      profile = oyProfile_FromSignature( icSigColorSpaceClass,
                                         oySIGNATURE_CLASS, 0 );
      oyProfiles_MoveIn( patterns, &profile, -1 );
      profile = oyProfile_FromSignature( icSigInputClass,
                                         oySIGNATURE_CLASS, 0 );
      oyProfiles_MoveIn( patterns, &profile, -1 );
      profile = oyProfile_FromSignature( icSigOutputClass,
                                         oySIGNATURE_CLASS, 0 );
      oyProfiles_MoveIn( patterns, &profile, -1 );
      profile = oyProfile_FromSignature( icSigDisplayClass,
                                         oySIGNATURE_CLASS, 0 );
      oyProfiles_MoveIn( patterns, &profile, -1 );
    }

    /* get the profile list */
    iccs = oyProfiles_Create( patterns, flags, 0 );

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

/** Function  oyProfiles_DeviceRank
 *  @memberof oyProfiles_s
 *  @brief    Sort a profile list according to a given device
 *
 *  Profiles which match the device will placed according to a rank value on 
 *  top of the list followed by the zero ranked profiles.
 *
 *  @verbatim
    // Get all ICC profiles, which can be used as assumed RGB profile
    uint32_t icc_profile_flags = oyICCProfileSelectionFlagsFromOptions( 
                                      OY_CMM_STD, "//" OY_TYPE_STD "/icc_color",
                                                                     NULL, 0 );
    oyProfiles_s * p_list = oyProfiles_ForStd( oyASSUMED_RGB,
                                               icc_profile_flags, 0,0 );
    int32_t * rank_list = (int32_t*) malloc( oyProfiles_Count(p_list) *
                                             sizeof(int32_t) );
    // Sort the profiles according to eaches match to a given device
    oyProfiles_DeviceRank( p_list, oy_device, rank_list );

    n = oyProfiles_Count( p_list );
    for(i = 0; i < n; ++i)
    {
      temp_prof = oyProfiles_Get( p_list, i );
      // Show the rank value, the profile internal and file names on the command line
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
 *  @version Oyranos: 0..1
 *  @since   2009/05/22 (Oyranos: 0.1.10)
 *  @date    2012/02/01
 */
int              oyProfiles_DeviceRank ( oyProfiles_s    * list,
                                         oyConfig_s      * device,
                                         int32_t         * rank_list )
{
  int error = !list || !device || !rank_list;
  oyProfiles_s_ * s = (oyProfiles_s_*)list;
  int i,n;
  int32_t rank;
  oyProfile_s * p = 0;
  oyConfig_s * p_device = 0;
  oyConfig_s_ * d = (oyConfig_s_*)device;
  oyOptions_s * old_db = 0;

  if(!list)
    return 0;

  if(error)
    return error;

  oyCheckType__m( oyOBJECT_PROFILES_S, return 0 )

  p_device = oyConfig_FromRegistration( d->registration, 0 );
  n = oyProfiles_Count( list );

  error = !memset( rank_list, 0, sizeof(int32_t) * n );

  /* oyConfig_Compare assumes its options in device->db, so it is filled here.*/
  if(!oyOptions_Count( d->db ))
  {
    old_db = d->db;
    d->db = d->backend_core;
  }

  for(i = 0; i < n; ++i)
  {
    p = oyProfiles_Get( list, i );

    oyProfile_GetDevice( p, p_device );
    rank = 0;

    error = oyConfig_Compare( p_device, device, &rank );
    if(oyConfig_FindString( p_device, "OYRANOS_automatic_generated", "1" ) ||
       oyConfig_FindString( p_device, "OPENICC_automatic_generated", "1" ))
    {
      DBG_NUM2_S( "found OPENICC_automatic_generated: %d %s",
                  rank, strrchr(oyProfile_GetFileName(p,-1),'/')+1);
      /* substract serial number and accound for possible wrong model_id */
      if(oyConfig_FindString( p_device, "serial", 0 ))
        rank -= 13;
      else
        rank -= 2;
      DBG_NUM1_S("after serial && OPENICC_automatic_generated: %d", rank);
    }
    rank_list[i] = rank;

    oyOptions_Clear( oyConfigPriv_m(p_device)->backend_core );
    oyProfile_Release( &p );
  }

  if(!error)
    error = oyStructList_Sort( s->list_, rank_list );

  if(old_db)
    d->db = old_db;

  return error;
}
