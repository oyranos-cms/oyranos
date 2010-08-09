/** Function oyConfig_AddDBData
 *  @memberof oyConfig_s
 *  @brief   add a key value pair to a oyConfig_s::db
 *
 *  This functions handles canonical user side settings. The keys added with
 *  this function can later be stored in the DB. A call to oyConfig_GetDB() or
 *  oyConfig_ClearDBData() overwrite the added entries. \n
 *  Modules should add informations to oyConfig_s::data.
 *
 *  @param[in]     config              the configuration
 *  @param[in]     key                 a key name, e.g. "my_key"
 *  @param[in]     value               a value, e.g. "my_value"
 *  @param[in]     flags               see oyOptions_s::oyOptions_SetFromText(.., flags,..)
 *  @return                            0 - good, 1 >= error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/21 (Oyranos: 0.1.10)
 *  @date    2009/02/08
 */
OYAPI int  OYEXPORT
               oyConfig_AddDBData    ( oyConfig_s        * config,
                                       const char        * key,
                                       const char        * value,
                                       uint32_t            flags )
{
  int error = !config || !key;
  char * tmp = 0;
  oyConfig_s_ * s = (oyConfig_s_*)config;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  if(error <= 0)
  {
    STRING_ADD( tmp, s->registration );
    if(tmp[oyStrlen_(tmp)-1] != OY_SLASH_C)
      STRING_ADD( tmp, OY_SLASH );

    if(oyStrrchr_( key, OY_SLASH_C ) != 0)
      STRING_ADD( tmp, oyStrrchr_( key, OY_SLASH_C )+1 );
    else
      STRING_ADD( tmp, key );

    /** We provide basically a wrapper for oyOptions_SetFromText(). */
    error = oyOptions_SetFromText( &s->db, tmp, value, flags );

    oyFree_m_( tmp );
  }

  return error;
}

/** Function  oyConfig_ClearDBData
 *  @memberof oyConfig_s
 *  @brief    Remove all additional data from the oyConfig_s::db object cache
 *
 *  Clear the local DB cache. The function will be called as well from
 *  oyConfig_GetDB().
 *
 *  @param[in]     config              the configuration
 *  @return                            0 - good, 1 >= error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/08 (Oyranos: 0.1.10)
 *  @date    2009/02/08
 */
OYAPI int  OYEXPORT
               oyConfig_ClearDBData    ( oyConfig_s        * config )
{
  int error = !config;
  oyConfig_s_ * config_ = (oyConfig_s_*)config;
  oyConfig_s * s = config;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  if(error <= 0)
  {
    error = oyOptions_Release( &config_->db );
    config_->db = oyOptions_New( config_->oy_ );
  }

  return error;
}

/** Function  oyConfig_GetDB
 *  @brief    Search a configuration in the DB for a configuration from module
 *  @memberof oyConfig_s
 *
 *  @param[in]     device              the to be checked configuration from
 *                                     oyConfigs_FromPattern_f
 *  @param[out]    rank_value          the number of matches between config and
 *                                     pattern, -1 means invalid
 *  @return                            0 - good, >= 1 - error + a message should
 *                                     be sent
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/26 (Oyranos: 0.1.10)
 *  @date    2009/01/26
 */
OYAPI int  OYEXPORT
               oyConfig_GetDB        ( oyConfig_s        * device,
                                       int32_t           * rank_value )
{
  int error = !device;
  int rank = 0, max_rank = 0, i, n;
  oyConfigs_s * configs = 0;
  oyConfig_s * config = 0, * max_config = 0;
  oyConfig_s_ * device_ = (oyConfig_s_*)device;
  oyConfig_s * s = device;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  if(error <= 0)
  {
    error = oyConfigs_FromDB( device_->registration, &configs, 0 );

    n = oyConfigs_Count( configs );

    for( i = 0; i < n; ++i )
    {
      config = oyConfigs_Get( configs, i );

      error = oyConfig_Compare( device, config, &rank );
      DBG_PROG1_S("rank: %d\n", rank);
      if(max_rank < rank)
      {
        max_rank = rank;
        oyConfig_Release( &max_config );
        max_config = oyConfig_Copy( config, 0 );
      }

      oyConfig_Release( &config );
    }
  }

  if(error <= 0 && rank_value)
    *rank_value = max_rank;

  if(error <= 0 && max_config)
  {
    oyOptions_Release( &device_->db );
    device_->db = oyOptions_Copy( oyConfigPriv_m(max_config)->db, 0 );
    oyConfig_Release( &max_config );
  }

  return error;
}

/** Function  oyConfig_SaveToDB
 *  @memberof oyConfig_s
 *  @brief    Store a oyConfig_s in DB
 *
 *  @param[in]     config              the configuration
 *  @return                            0 - good, 1 >= error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/21 (Oyranos: 0.1.10)
 *  @date    2009/01/21
 */
OYAPI int  OYEXPORT
               oyConfig_SaveToDB     ( oyConfig_s        * config )
{
  int error = !config;
  oyOptions_s * opts = 0;
  oyConfig_s * s = config;
  oyConfig_s_ * config_ = (oyConfig_s_*)config;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  DBG_PROG_START

  if(error <= 0)
  {
    opts = oyOptions_New( 0 );
    oyOptions_AppendOpts( opts, config_->db );
    oyOptions_AppendOpts( opts, config_->backend_core );

    error = oyOptions_SaveToDB( opts, config_->registration );

    oyOptions_Release( &opts );
  }

  DBG_PROG_ENDE
  return error;
}

/** Function oyConfig_EraseFromDB
 *  @memberof oyConfig_s
 *  @brief   remove a oyConfig_s from DB
 *
 *  @param[in]     config              the configuration
 *  @return                            0 - good, 1 >= error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/27 (Oyranos: 0.1.10)
 *  @date    2009/01/27
 */
OYAPI int  OYEXPORT
               oyConfig_EraseFromDB  ( oyConfig_s        * config )
{
  int error = !config;
  oyOption_s * o = 0;
  int i;
  char * text = 0,
       * tmp = 0;
  oyConfig_s * s = config;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  if(error <= 0)
  {
    i = 0;
    text = config->registration;
    if(text)
      while( (text = oyStrchr_(++text, OY_SLASH_C)) != 0)
        ++i;

    if(i != 4)
    {
      o = oyOptions_Get( config->db, 0 );
      i = 0;
      text = 0;
      if(o)
        text = o->registration;
      if(text)
        while( (text = oyStrchr_(++text, OY_SLASH_C)) != 0)
          ++i;

      if(i == 5)
      {
        tmp = oyStringCopy_( o->registration, oyAllocateFunc_ );
        text = oyStrrchr_(tmp, OY_SLASH_C);
        text[0] = 0;
        text = tmp;
      }

    }
    else
      text = config->registration;

    error = oyEraseKey_( text );

    if(tmp)
      oyFree_m_( tmp );
    oyOption_Release( &o );
  }

  oyExportEnd_();
  DBG_PROG_ENDE
  return error;
}

/** Function oyConfig_Compare
 *  @brief   check for matching to a given pattern
 *  @memberof oyConfig_s
 *
 *  @param[in]     module_device       the to be checked configuration from
 *                                     oyConfigs_FromPattern_f;
 *                                     Additional allowed are DB configs.
 *  @param[in]     db_pattern          the to be compared configuration from
 *                                     elsewhere
 *  @param[out]    rank_value          the number of matches between config and
 *                                     pattern, -1 means invalid
 *  @return                            0 - good, >= 1 - error + a message should
 *                                     be sent
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/26 (Oyranos: 0.1.10)
 *  @date    2010/02/25
 */
int            oyConfig_Compare      ( oyConfig_s        * module_device,
                                       oyConfig_s        * db_pattern,
                                       int32_t           * rank_value )
{
  int error = !module_device || !db_pattern;
  int domain_n, pattern_n, i, j, k, l,
      rank = 0,
      d_rank = 0,
      has_opt;
  oyOption_s * d = 0,
             * p = 0,
             * check = 0;
  oyOptions_s * dopts = 0; /* device options */
  char * d_opt = 0, * d_val = 0,
       * p_opt = 0, * p_val = 0,
       * check_opt = 0, * check_val = 0;
  oyConfig_s * pattern = db_pattern,
             * device = module_device;
  oyRankPad  * rank_map = 0;
  oyConfig_s * s = module_device;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  if(error <= 0)
  {
    if(device->rank_map)
      rank_map = device->rank_map;
    else
      rank_map = pattern->rank_map;

    domain_n = oyOptions_Count( device->backend_core );
    if(domain_n)
      dopts = device->backend_core;
    else
    {
      domain_n = oyOptions_Count( device->db );
      if(domain_n)
        /* fall back for pure DB contructed oyConfig_s */
        dopts = device->db;
      else
        WARNc1_S("No key/values pairs found in %s", device->registration);
    }

    pattern_n = oyOptions_Count( pattern->db );
    for(i = 0; i < domain_n; ++i)
    {
      d = oyOptions_Get( dopts, i );
      d_opt = oyFilterRegistrationToText( d->registration, oyFILTER_REG_MAX, 0);
      d_val = oyOption_GetValueText( d, oyAllocateFunc_ );
      has_opt = 0;

      /* check for double occurences */
      for(l = 0; l < i; ++l)
      {
        check = oyOptions_Get( dopts, l );
        check_opt = oyFilterRegistrationToText( check->registration,
                                                oyFILTER_REG_MAX, 0);
        if(oyStrcmp_(d_opt, check_opt) == 0)
        {
          check_val = oyOption_GetValueText( check, oyAllocateFunc_ );
          WARNc4_S( "%d %s occured twice with: %s %s", i, d_opt, check_val,
                    check_val ? check_val : "" );
          if(check_val) oyFree_m_( check_val );
        }

        oyOption_Release( &check );
        oyFree_m_( check_opt );
      }

      d_rank = oyConfig_DomainRank( device );
      if(d_rank > 0 && d_val && d_opt)
      for( j = 0; j < pattern_n; ++j )
      {
        p = oyOptions_Get( pattern->db, j );

        p_opt = oyFilterRegistrationToText( p->registration, oyFILTER_REG_MAX,
                                            0 );

        if(p_opt && oyStrstr_(p_opt, d_opt))
        {
          p_val = oyOption_GetValueText( p, oyAllocateFunc_ );
          has_opt = 1;

          /** Option name is equal and and value matches : increase rank value*/
          if(p_val && oyStrstr_( d_val, p_val ))
          {
            if(rank_map)
            {
              k = 0;
              while(rank_map[k].key)
              {
                if(oyStrcmp_(rank_map[k].key, d_opt) == 0)
                {
                  rank += rank_map[k].match_value;
                  break;
                }
                ++k;
              }
            } else
              ++rank;

            oyFree_m_(p_val);
          } else if(rank_map)
          {
            k = 0;
            while(rank_map[k].key)
            {
              if(oyStrcmp_(rank_map[k].key, d_opt) == 0)
              {
                rank += rank_map[k].none_match_value;
                break;
              }
              ++k;
            }
          }
        }
        /*
        rank += oyFilterRegistrationMatch( d->registration, p->registration,
                                           oyOBJECT_CMM_API8_S); */

        oyOption_Release( &p );
        if(p_opt) oyFree_m_( p_opt );
        if(p_val) oyFree_m_( p_val );
      }

      if(!has_opt && rank_map)
      {
        k = 0;
          while(rank_map[k].key)
          {
            if(oyStrcmp_(rank_map[k].key, d_opt) == 0)
            {
              rank += rank_map[k].not_found_value;
              break;
            }
            ++k;
          }
      }

      oyOption_Release( &d );
      oyFree_m_( d_opt );
      oyFree_m_( d_val );
    }
  }

  if(rank_value)
    *rank_value = rank;

  return error;
}

/** Function oyConfig_DomainRank
 *  @brief   check for being recognised by a given module
 *  @memberof oyConfig_s
 *
 *  @param[in]     config              the configuration to be checked
 *                                     wether or not the module can make
 *                                     sense of it and support the data
 *  @return                            0 - indifferent, <= -1 - no fit
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/26 (Oyranos: 0.1.10)
 *  @date    2009/01/26
 */
OYAPI int  OYEXPORT
               oyConfig_DomainRank   ( oyConfig_s        * config )
{
  oyCMMapiFilters_s * apis = 0;
  int error = !config;
  int i = 0,
      rank = 0;
  uint32_t * rank_list = 0,
           max_rank = 0;
  uint32_t apis_n = 0;
  oyCMMapi8_s * cmm_api8 = 0;
  oyConfig_s * s = config;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  oyExportStart_(EXPORT_CHECK_NO);

  if(error <= 0)
  {
    apis = oyCMMsGetFilterApis_( 0, config->registration,
                                 oyOBJECT_CMM_API8_S,
                                 &rank_list, &apis_n);
    error = !apis;
  }

  if(error <= 0)
  {
    for(i = 0; i < apis_n; ++i)
    {
      cmm_api8 = (oyCMMapi8_s*) oyCMMapiFilters_Get( apis, i );

      error = !cmm_api8->oyConfig_Rank;
      if(error <= 0)
      /** Ask the module if it wants later on to accept this configuration. */
        rank = cmm_api8->oyConfig_Rank( config ) * rank_list[i];

      if(max_rank < rank)
        max_rank = rank;

      if(cmm_api8->release)
        cmm_api8->release( (oyStruct_s**)&cmm_api8 );
    }
  }

  oyCMMapiFilters_Release( &apis );

  if(error && max_rank >= 0)
    rank = -1;
  else
    rank = max_rank;

  oyExportEnd_();
  return rank;
}

/** Function oyConfig_FindString
 *  @brief   search in data sets for a key/value
 *  @memberof oyConfig_s
 *
 *  @param[in]     config              the configuration to be checked
 *                                     wether or not the module can make
 *                                     sense of it and support the data
 *  @param[in]     key                 the key name
 *  @param[in]     value               the optional value
 *  @return                            the found value
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/08 (Oyranos: 0.1.10)
 *  @date    2009/02/08
 */
OYAPI const char * OYEXPORT
               oyConfig_FindString   ( oyConfig_s        * config,
                                       const char        * key,
                                       const char        * value )
{
  const char * text = 0;
  oyConfig_s * s = config;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )


  text = oyOptions_FindString( config->data, key, value );
  if(!text)
    text = oyOptions_FindString( config->backend_core, key, value );
  if(!text)
    text = oyOptions_FindString( config->db, key, value );

  return text;
}

/** Function oyConfig_Has
 *  @brief   search in data sets for a key
 *  @memberof oyConfig_s
 *
 *  @param[in]     config              the configuration to be checked
 *                                     wether or not the module can make
 *                                     sense of it and support the data
 *  @param[in]     key                 the key name
 *  @return                            0 - not found; 1 - key found
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/10 (Oyranos: 0.1.10)
 *  @date    2009/12/10
 */
OYAPI int  OYEXPORT
               oyConfig_Has          ( oyConfig_s        * config,
                                       const char        * key )
{
  oyOption_s * o = 0;
  int has_option = 0;
  oyConfig_s * s = config;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  o = oyOptions_Find( config->data, key );
  if(!o)
    o = oyOptions_Find( config->backend_core, key );
  if(!o)
    o = oyOptions_Find( config->db, key );

  if(o)
    has_option = 1;

  oyOption_Release( &o );

  return has_option;
}

/** Function oyConfig_Find
 *  @brief   search in data sets for a key
 *  @memberof oyConfig_s
 *
 *  @param[in]     config              the configuration to be checked
 *                                     wether or not the module can make
 *                                     sense of it and support the data
 *  @param[in]     key                 the key name
 *  @return                            the found value
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/08 (Oyranos: 0.1.10)
 *  @date    2009/02/08
 */
OYAPI oyOption_s * OYEXPORT
               oyConfig_Find         ( oyConfig_s        * config,
                                       const char        * key )
{
  oyOption_s * o = 0;
  oyConfig_s * s = config;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  o = oyOptions_Find( config->data, key );
  if(!o)
    o = oyOptions_Find( config->backend_core, key );
  if(!o)
    o = oyOptions_Find( config->db, key );

  return o;
}

/** Function oyConfig_Count
 *  @brief   number of all options
 *  @memberof oyConfig_s
 *
 *  @param[in]     config              the configuration
 *  @return                            the options count
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/08 (Oyranos: 0.1.10)
 *  @date    2009/02/08
 */
OYAPI int  OYEXPORT
               oyConfig_Count        ( oyConfig_s        * config )
{
  int error = !config;
  int n = 0;
  oyOptions_s * opts = 0;
  oyConfig_s * s = config;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  if(error <= 0)
  {
    opts = oyOptions_New( 0 );

    oyOptions_AppendOpts( opts, config->db );
    oyOptions_AppendOpts( opts, config->backend_core );
    oyOptions_AppendOpts( opts, config->data );
    n = oyOptions_Count( opts );
    oyOptions_Release( &opts );
  }

  return n;
}

/** Function oyConfig_Get
 *  @brief   get one option
 *  @memberof oyConfig_s
 *
 *  @param[in]     config              the configuration
 *  @param[in]     pos                 option position
 *  @return                            the selected option
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/08 (Oyranos: 0.1.10)
 *  @date    2009/02/08
 */
OYAPI oyOption_s * OYEXPORT
               oyConfig_Get          ( oyConfig_s        * config,
                                       int                 pos )
{
  int error = !config;
  oyOption_s * o = 0;
  oyOptions_s * opts = 0;
  oyConfig_s * s = config;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  if(error <= 0)
  {
    opts = oyOptions_New( 0 );

    oyOptions_AppendOpts( opts, config->db );
    oyOptions_AppendOpts( opts, config->backend_core );
    oyOptions_AppendOpts( opts, config->data );

    o = oyOptions_Get( opts, pos );

    oyOptions_Release( &opts );
  }

  return o;
}

/** Function oyRankMapCopy
 *  @memberof oyConfig_s
 *  @brief   copy a rank map
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/27 (Oyranos: 0.1.10)
 *  @date    2009/01/27
 */
oyRankPad *        oyRankMapCopy     ( const oyRankPad   * rank_map,
                                       oyAlloc_f           allocateFunc )
{
  oyRankPad * map = 0;
  int error = !rank_map;
  int n = 0, i;

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  if(error <= 0)
  {
    while( rank_map[n++].key ) {}

    oyAllocHelper_m_( map, oyRankPad, n + 1, allocateFunc, error = 1 );
  }

  if(error <= 0)
  {
    for(i = 0; i < n; ++i)
    {
      map[i].key = oyStringCopy_( rank_map[i].key, allocateFunc );
      map[i].match_value = rank_map[i].match_value;
      map[i].none_match_value = rank_map[i].none_match_value;
      map[i].not_found_value = rank_map[i].not_found_value;
    }
  }

  return map;
}
