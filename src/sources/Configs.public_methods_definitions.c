#include "oyranos_sentinel.h"

/** Function  oyConfigs_FromDomain
 *  @memberof oyConfigs_s
 *  @brief    Send a request to a configuration module
 *
 *  A empty options argument should send out an warning message, which
 *  contains intructions on how to talk with the module.
 *  Otherwise the calls are pure convention and depend on the usage
 *  and agreement of the partners.
 *
 *  For the convention to call to color devices
 *  @see oyX1Configs_FromPatternUsage().
 *
 *  @param[in]     registration_domain                     the module to call to
 *  @param[in]     options                                 options to specify the calling into modules
 *                                                         messages are bound to this object
 *  @param[out]    configs                                 the returned configurations
 *  @param[in]     object                                  a optional user object
 *  @return                                                0 - good, 1 <= error, -1 >= issues,
 *                                                         look for messages
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/16 (Oyranos: 0.1.10)
 *  @date    2009/01/19
 */
OYAPI int  OYEXPORT
               oyConfigs_FromDomain  ( const char        * registration_domain,
                                       oyOptions_s       * options,
                                       oyConfigs_s      ** configs,
                                       oyObject_s          object )
{
  oyConfigs_s * s = 0;
  oyConfig_s * config = 0;
  int error = !registration_domain,
      l_error = 0;
  oyCMMapi8_s_ * cmm_api8 = 0;
  int i, n;

  oyExportStart_(EXPORT_CHECK_NO);

  /**
   *  1. first we search for oyCMMapi8_s complex config support matching to our
   *     registration_domain
   *  2. if we find a module, we ask for the options
   *  3. add the options to the config (in the module)
   */
  if(error <= 0)
  {
    cmm_api8 = (oyCMMapi8_s_*) oyCMMsGetFilterApi_( 0, registration_domain,
                                                   oyOBJECT_CMM_API8_S );
    error = !cmm_api8;
  }

  if(error <= 0)
    error = !cmm_api8->oyConfigs_FromPattern;

  if(error <= 0)
    error = cmm_api8->oyConfigs_FromPattern( registration_domain, options, &s );

  if(error <= 0)
  {
    n = oyConfigs_Count( s );
    for(i = 0; i < n && error <= 0; ++i)
    {
      config = oyConfigs_Get( s, i );

      l_error = oyOptions_SetSource( oyConfigPriv_m(config)->backend_core,
                                     oyOPTIONSOURCE_FILTER); OY_ERR
      l_error = oyOptions_SetSource( oyConfigPriv_m(config)->data,
                                     oyOPTIONSOURCE_FILTER ); OY_ERR

      oyConfig_Release( &config );
    }
  }

  if(error <= 0 && configs)
  {
    *configs = s;
    s = 0;
  } else
    oyConfigs_Release( &s );

  oyExportEnd_();
  return error;
}

/** Function  oyConfigs_FromDeviceClass
 *  @memberof oyConfigs_s
 *  @brief    Ask a module for device informations or other direct calls
 *
 *  @param[in]     device_type     the device type ::oyFILTER_REG_TYPE,
 *                                     defaults to OY_TYPE_STD (optional)
 *  @param[in]     device_class    the device class, e.g. "monitor",
 *                                     ::oyFILTER_REG_APPLICATION
 *  @param[in]     options             options to pass to the module, for zero
 *                                     the usage instructions are requested,
 *                                     a option "device_name" can be used 
 *                                     as filter
 *  @param[out]    devices         the devices
 *  @param[in]     object              the optional object
 *  @return                            0 - good, >= 1 - error
 *
 *  @verbatim
    // pass empty options to the module to get a usage message
    oyOptions_s * options = 0;
    int error = oyConfigs_FromDeviceClass( OY_TYPE_STD, "monitor",
                                              options, 0, 0 );
    @endverbatim
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/28 (Oyranos: 0.1.10)
 *  @date    2009/01/30
 */
OYAPI int  OYEXPORT oyConfigs_FromDeviceClass (
                                       const char        * device_type,
                                       const char        * device_class,
                                       oyOptions_s       * options,
                                       oyConfigs_s      ** devices,
                                       oyObject_s          object )
{
  int error = !device_class || !device_class[0];
  oyConfig_s * device = 0;
  oyConfigs_s * configs = 0;
  int i, j, j_n;
  uint32_t count = 0,
         * rank_list = 0;
  char ** texts = 0,
        * device_class_registration = 0;
  const char * tmp = 0,
             * device_name = 0;

  if(error > 0)
  {
    WARNc_S( "\n  No device_class argument provided. Give up" );
    return 0;
  }

  /** 1. obtain detailed and expensive device informations */
  if(options)
  {
    options = oyOptions_Copy( options, 0 );
    device_name = oyOptions_FindString( options, "device_name", 0 );
  }

  /** 1.2.1 build a device class registration string */
  if(error <= 0)
  {
    device_class_registration = oyDeviceRegistrationCreate_(
                                          device_type, device_class,
                                          device_name,
                                          device_class_registration );
    error = !device_class_registration;
  }

  /** 1.2.2 get all device class module names */
  if(error <= 0)
    error = oyConfigDomainList  ( device_class_registration, &texts, &count,
                                  &rank_list, 0 );

  oyFree_m_(device_class_registration);

  if(devices && !*devices)
    *devices = oyConfigs_New( object );

  /** 1.3 ask each module */
  for( i = 0; i < count; ++i )
  {
    const char * registration_domain = texts[i];

    /** 1.3.1 call into module */
    error = oyConfigs_FromDomain( registration_domain, options, &configs,
                                  object);

    if(devices && *devices)
      j_n = oyConfigs_Count( configs );
    else
      j_n = 0;
    for( j = 0; j < j_n; ++j )
    {
      
      device = oyConfigs_Get( configs, j );

      device_class_registration = oyDeviceRegistrationCreate_(
                                          device_type, device_class,
                                          NULL, NULL );
      if(oyConfigPriv_m(device)->registration)
        oyStruct_GetDeAllocator((oyStruct_s*)device)(oyConfigPriv_m(device)->registration);
      oyConfigPriv_m(device)->registration = device_class_registration;
      device_class_registration = NULL;

      if(device_name)
      {
        /** 1.3.1.1 Compare the device_name with the device_name option
         *          and collect the matching devices. */
        tmp = oyConfig_FindString( device, "device_name", 0 );
        if(tmp && oyStrcmp_( tmp, device_name ) == 0)
          oyConfigs_MoveIn( *devices, &device, -1 );
      } else
        /** 1.3.1.2 ... or collect all device configurations */
        oyConfigs_MoveIn( *devices, &device, -1 );

      oyConfig_Release( &device );
    }

    oyConfigs_Release( &configs );
  }

  if(devices)
    j_n = oyConfigs_Count( *devices );
  else
    j_n = 0;

  for( j = 0; j < j_n; ++j )
  {
    device = oyConfigs_Get( *devices, j );

    /** The basic call on how to obtain the configuration is added here as
     *  the objects name. "properties" and "list" are known. */
    if(oyOptions_FindString( options, "command", "properties" ) ||
       oyOptions_FindString( options, "oyNAME_DESCRIPTION", 0 ))
      oyObject_SetName( device->oy_, "properties", oyNAME_NAME );
    else if(oyOptions_FindString( options, "list", 0 ))
      oyObject_SetName( device->oy_, "list", oyNAME_NAME );

    oyConfig_Release( &device );
  }

  oyOptions_Release( &options );

  return error;
}

/** Function oyConfigs_SelectSimilars
 *  @memberof oyConfigs_s
 *  @brief   filter similiar configs compared by a pattern
 *
 *  This is a simple convenience function to select from a list existing 
 *  configurations.
 *
 *  @verbatim
    const char * pattern[][2] = {{"device_name",0},
                                 {"manufacturer",0},
                                 {"model",0},
                                 {"serial",0},
                                 {0,0}};
    oyConfigs_s * devices = 0, * filtered = 0;
    int error = oyConfigs_FromDB( registration, &devices, 0 );
    error = oyConfigs_SelectSimilars( devices, pattern, &filtered )
 *  @endverbatim
 *
 *  @param[in]     list                the list
 *  @param[in]     pattern             user supplied zero terminated pattern
 *                                     list; The first element is a mandadory
 *                                     key to meet. The second element is the 
 *                                     optional value. Each 
 *  @param[out]    filtered            the result
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/26 (Oyranos: 0.1.10)
 *  @date    2009/02/26
 */
OYAPI int  OYEXPORT
                 oyConfigs_SelectSimilars (
                                       oyConfigs_s       * list,
                                       const char        * pattern[][2],
                                       oyConfigs_s      ** filtered )
{
  int error = !list;
  int i,j, matches = 0, n, required = 0;
  oyConfigs_s * s = list,
              * result = 0;
  oyConfig_s * device = 0;
  const char * ct = 0;

  if(!s)
    return 0;

  if(s)
    oyCheckType__m( oyOBJECT_CONFIGS_S, return 0 )

  /** 0. setup Elektra */
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  result = oyConfigs_New(0);
                 
  n = oyConfigs_Count( list );
  for(i = 0; i < n; ++i)
  {
    device = oyConfigs_Get( list, i );
    j = 0;
    matches = 0;
    required = 0;

    while(pattern[j][0])
    {
      ct = oyConfig_FindString( device, pattern[j][0], 0);
      if(pattern[j][1])
      {
        ++required;

        if(ct && strcmp(ct,pattern[j][1]) == 0)
          ++matches;

      } else
      {
        ++required;
        ++matches;
      }

      ++j;
    }

    if(required == matches)
      error = oyConfigs_MoveIn( result, &device, -1 );

    oyConfig_Release( &device );
  }

  *filtered = result;

  return error;
}

/** Function  oyConfigs_FromDB
 *  @memberof oyConfigs_s
 *  @brief    Get all oyConfigs_s from DB
 *
 *  @param[in]     registration        the filter
 *  @param[out]    configs             the found configuration list
 *  @param[in]     object              a optional user object
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/23 (Oyranos: 0.1.10)
 *  @date    2010/02/25
 */
OYAPI int OYEXPORT
                 oyConfigs_FromDB    ( const char        * registration,
                                       oyConfigs_s      ** configs,
                                       oyObject_s          object )
{
  oyConfigs_s * s = 0;
  oyConfig_s_ * config = 0;
  oyOption_s * o = 0;
  char ** texts = 0,
       ** key_set_names = 0,
       ** config_key_names = 0;
  uint32_t count = 0,
         * d_rank_list = 0;
  int error = !registration;
  int i, j, k, n = 0, k_n = 0;
  oyCMMapi8_s_ * cmm_api8 = 0;

  /** 0. setup Elektra */
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  if(error <= 0)
  {
    /** 1. get all module names for the registration pattern */
    error = oyConfigDomainList( registration, &texts, &count, &d_rank_list, 0 );
    if(count)
      s = oyConfigs_New( 0 );

    if(error <= 0 && count && texts)
      cmm_api8 = (oyCMMapi8_s_*) oyCMMsGetFilterApi_( 0, texts[0],
                                                     oyOBJECT_CMM_API8_S );

    for(i = 0; i < count; ++i)
    {
      char * key = NULL;
      /** 2. obtain the directory structure for configurations */
      key_set_names = oyDBKeySetGetNames_( texts[i], &n );

      if(error <= 0)
      for(j = 0; j < n; ++j)
      {
        /** 3. obtain all keys from one configuration directory */
        config_key_names = oyDBKeySetGetNames_( key_set_names[j], &k_n );

        config = (oyConfig_s_*)oyConfig_FromRegistration( texts[i], object );
        error = !config;

        for(k = 0; k < k_n; ++k)
        {
          /** 4. create a oyOption_s for each Elektra DB key/value pair */
          if(error <= 0)
            error = oyOption_FromDB( config_key_names[k], &o, object );
          if(error < 0)
            continue;
          if(error <= 0)
            error = oyOptions_Add( config->db, o, -1, 0 );
          else if(error > 0)
          {
            WARNcc1_S( (oyStruct_s*) object, "Could not generate key %s",
                       config_key_names[k] );
            break;
          }
          oyOption_Release( &o );
        }

        /* add information about the data's origin */
        oyStringAddPrintf( &key, oyAllocateFunc_, oyDeAllocateFunc_, "%s/key_set_name",
                           oyConfig_GetRegistration( (oyConfig_s*) config ) );
        error = oyOptions_SetFromText( &config->data, key,
                                       key_set_names[j], OY_CREATE_NEW );

        /* add a rank map to allow for comparisions */
        if(cmm_api8)
          config->rank_map = oyRankMapCopy( cmm_api8->rank_map,
                                            config->oy_->allocateFunc_ );

        oyConfigs_MoveIn( s, (oyConfig_s**)&config, -1 );

        oyFree_m_( key );
      }
    }

    oyStringListRelease_( &texts, count, oyDeAllocateFunc_ );
  }

  if(configs)
    *configs = s;
  else
    oyConfigs_Release( &s );

  oyExportEnd_();
  return error;
}

/** Function  oyConfigs_Modify
 *  @memberof oyConfigs_s
 *  @brief    Ask a module for device informations or other direct calls
 *
 *
 *  @param[in,out] configs             The passed configs first member is used
 *                                     to obtain a registration string and
 *                                     select a appropriate module.
 *                                     Regarding the module the
 *                                     configs need to be homogenous.
 *                                     All configs are passed at once to the
 *                                     module. Mixing configs from different
 *                                     modules is not defined.
 *  @param[in]     options             options to pass to the module; With zero
 *                                     the usage instructions are requested.
 *  @return                            0 - good, >= 1 - error, issue <= -1 
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/08/21 (Oyranos: 0.1.10)
 *  @date    2009/08/25
 */
OYAPI int  OYEXPORT
                 oyConfigs_Modify    ( oyConfigs_s       * configs,
                                       oyOptions_s       * options )
{
  int error = !oyConfigs_Count( configs );
  oyConfig_s_ * config = 0;
  oyConfigs_s * s = configs;
  int i;
  uint32_t count = 0,
         * rank_list = 0;
  char ** texts = 0,
        * registration_domain = 0;
  oyCMMapi8_s_ * cmm_api8 = 0;

  if(error > 0)
  {
    WARNc_S( "\n  No devices provided. Give up" );
    return 0;
  }
  oyCheckType__m( oyOBJECT_CONFIGS_S, return 1 )


  oyExportStart_(EXPORT_CHECK_NO);

  if(error <= 0)
  {
    /** 1.  pick the first device to select a registration */
    config = (oyConfig_s_*)oyConfigs_Get( configs, 0 );
    /** 1.2 get all device class module names from the firsts oyConfig_s
      *     registration */
    error = oyConfigDomainList  ( config->registration, &texts, &count,
                                  &rank_list, 0 );
    oyConfig_Release( (oyConfig_s**)&config );
  }


  /** 2. call each modules oyCMMapi8_s::oyConfigs_Modify */
  for( i = 0; i < count; ++i )
  {
    registration_domain = texts[i];

    if(error <= 0)
    {
      cmm_api8 = (oyCMMapi8_s_*) oyCMMsGetFilterApi_( 0, registration_domain,
                                                     oyOBJECT_CMM_API8_S );
      error = !cmm_api8;
    }

    if(error <= 0)
      error = !cmm_api8->oyConfigs_Modify;

    if(error <= 0)
      error = cmm_api8->oyConfigs_Modify( configs, options );
  }

  oyStringListRelease_( &texts, count, oyDeAllocateFunc_ );
  if(rank_list)
    oyDeAllocateFunc_( rank_list );

  oyExportEnd_();
  return error;
}

/** Function  oyConfigDomainList
 *  @memberof oyConfigs_s
 *  @brief    Count and show the global oyConfigs_s suppliers
 *
 *  @verbatim
    uint32_t count = 0,
           * rank_list = 0;
    char ** texts = 0,
          * temp = 0,
         ** attributes = 0,
          * device_class = 0;
    int i,j, attributes_n;

    // get all configuration filters
    oyConfigDomainList("//"OY_TYPE_STD"/config", &texts, &count,&rank_list ,0 );
    for( i = 0; i < count; ++i )
    {
      attributes_n = 0;

      // pick the filters name and remove the common config part
      temp = oyFilterRegistrationToText( texts[i], oyFILTER_REG_APPLICATION,
                                         malloc );
      attributes = oyStringSplit_( temp, '.', &attributes_n, malloc );
      free(temp);
      temp = malloc(1024); temp[0] = 0;
      for(j = 0; j < attributes_n; ++j)
      {
        if(strcmp(attributes[j], "config") == 0)
          continue;

        if(j && temp[0])
          sprintf( &temp[strlen(temp)], "." );
        sprintf( &temp[strlen(temp)], "%s", attributes[j]);
      }

      // The string in temp can be passed as the device_class argument to 
      // oyDevicesGet()
      printf("%d: %s \"%s\"\n", i, texts[i], temp);

      oyStringListRelease_( &attributes, attributes_n, free );
      free (device_class);
      free(temp);
    }
    @endverbatim
 *
 *  @param[in]     registration_pattern a optional filter
 *  @param[out]    list                the list with full filter registrations
 *  @param[out]    count               the list count
 *  @param[out]    rank_list           the rank fitting to list
 *  @param[in]     allocateFunc        the user allocator for list
 *  @return                            0 - good, >= 1 - error, <= -1 unknown
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/01/19
 */
OYAPI int  OYEXPORT
                 oyConfigDomainList  ( const char        * registration_pattern,
                                       char            *** list,
                                       uint32_t          * count,
                                       uint32_t         ** rank_list,
                                       oyAlloc_f           allocateFunc )
{
  oyCMMapiFilters_s * apis = 0;
  oyCMMapiFilter_s_ * api = 0;
  int error = !list || !count;
  char ** reg_lists = 0;
  int i = 0,
      reg_list_n = 0;
  uint32_t apis_n = 0;

  oyExportStart_(EXPORT_CHECK_NO);

  if(error <= 0)
  {
    apis = oyCMMsGetFilterApis_( 0,0, registration_pattern,
                                 oyOBJECT_CMM_API8_S,
                                 oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR,
                                 rank_list, &apis_n);
    error = !apis;
    if(error && oy_debug)
      WARNc1_S( "no modules available for pattern: \"%s\"",
                       oyNoEmptyString_m_(registration_pattern) );
  }

  if(error <= 0)
  {
    if(!allocateFunc)
      allocateFunc = oyAllocateFunc_;

    for(i = 0; i < apis_n; ++i)
    {
      api = (oyCMMapiFilter_s_*)oyCMMapiFilters_Get( apis, i );
      if(rank_list[0][i])
        oyStringListAddStaticString_( &reg_lists, &reg_list_n,
                                      oyNoEmptyString_m_( api->registration ),
                                      oyAllocateFunc_, oyDeAllocateFunc_ );

      if(api->release)
        api->release( (oyStruct_s**)&api );
    }

    if(reg_list_n && reg_lists)
      *list = oyStringListAppend_( (const char**)reg_lists, reg_list_n, 0,0,
                                   &reg_list_n, allocateFunc );

    oyStringListRelease_( &reg_lists, reg_list_n, oyDeAllocateFunc_ );
  }

  if(count)
    *count = reg_list_n;

  oyCMMapiFilters_Release( &apis );

  oyExportEnd_();
  return error;
}
