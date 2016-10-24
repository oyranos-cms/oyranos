#include "oyranos_db.h"
#include "oyranos_devices.h"
#include "oyranos_devices_internal.h"

#include "oyjl/oyjl_tree.h"

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

    if(oyStrstr_( key, s->registration ) != 0)
    {
      oyFree_m_(tmp);
      STRING_ADD( tmp, key );
    }
    else if(oyStrrchr_( key, OY_SLASH_C ) != 0)
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
 *  @brief    search a configuration in the DB for a configuration from module
 *  @memberof oyConfig_s
 *
 *  @param[in]     device              the to be checked configuration from
 *                                     oyConfigs_FromPattern_f
 *  @param[in]     options             supported is "module" containing a 
 *                                     registration string to select a module,
 *                                     see as well oyConfigs_FromDB()
 *  @param[out]    rank_value          the number of matches between config and
 *                                     pattern, -1 means invalid
 *  @return                            0 - good, >= 1 - error + a message should
 *                                     be sent
 *
 *  @version Oyranos: 0.3.0
 *  @since   2009/01/26 (Oyranos: 0.1.10)
 *  @date    2012/09/15
 */
OYAPI int  OYEXPORT
               oyConfig_GetDB        ( oyConfig_s        * device,
                                       oyOptions_s       * options,
                                       int32_t           * rank_value )
{
  int error = !device;
  oyConfigs_s * configs = 0;
  oyConfig_s_ * s = (oyConfig_s_*)device;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  if(error <= 0)
    error = oyConfigs_FromDB( s->registration, options, &configs, 0 );
  if(error <= 0)
    error = oyConfig_SelectDB( device, configs, rank_value );
  oyConfigs_Release( &configs );

  return error;
}

/** Function  oyConfig_SelectDB
 *  @brief    select a db configuration
 *  @memberof oyConfig_s
 *
 *  This function is useful to fill the best matching config into the db options set.
 *
 *  @param[in,out] device              the to be checked configuration from
 *                                     oyConfigs_FromPattern_f
 *  @param[in]     configs             the possible configs to select the best db match from
 *  @param[out]    rank_value          the number of matches between config and
 *                                     pattern, -1 means invalid
 *  @return                            0 - good, >= 1 - error + a message should
 *                                     be sent
 *
 *  @version Oyranos: 0.9.6
 *  @date    2015/02/15
 *  @since   2009/01/26 (Oyranos: 0.3.2)
 */
OYAPI int  OYEXPORT
               oyConfig_SelectDB     ( oyConfig_s        * device,
                                       oyConfigs_s       * configs,
                                       int32_t           * rank_value )
{
  int error = !device;
  int32_t rank = 0, max_rank = 0;
  int i, n;
  oyConfig_s * config = 0, * max_config = 0;
  oyConfig_s_ * s = (oyConfig_s_*)device;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  if(error <= 0)
  {
    n = oyConfigs_Count( configs );

    /** 1. iterate over all configs */
    for( i = 0; i < n; ++i )
    {
      config = oyConfigs_Get( configs, i );

      /** 2. oyConfig_Compare() with the provided device */
      error = oyConfig_Compare( device, config, &rank );
      DBG_PROG1_S("rank: %d\n", rank);
      /** 3. store the biggest rank_value */
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

  /** 4. copy the config::db from configs with the biggest rank_value into
   *     the device::db
   */
  if(error <= 0 && max_config)
  {
    oyOptions_Release( &s->db );
    s->db = oyOptions_Copy( ((oyConfig_s_*)max_config)->db, 0 );
    oyConfig_Release( &max_config );
  }

  return error;
}


/** Function  oyConfig_SaveToDB
 *  @memberof oyConfig_s
 *  @brief    Store a oyConfig_s in DB
 *
 *  The new key set name is stored inside the key "key_set_name".
 *
 *  @param[in]     config              the configuration
 *  @param         scope               oySCOPE_USER and oySCOPE_SYS are possible
 *  @return                            0 - good, 1 >= error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/21 (Oyranos: 0.1.10)
 *  @date    2011/01/29
 */
OYAPI int  OYEXPORT
               oyConfig_SaveToDB     ( oyConfig_s        * config,
                                       oySCOPE_e           scope )
{
  int error = !config;
  oyOptions_s * opts = 0;
  oyConfig_s_ * s = (oyConfig_s_*)config;
  char * new_reg = NULL,
       * key = NULL;
  oyConfig_s_ * config_ = (oyConfig_s_*)config;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  DBG_PROG_START

  if(error <= 0)
  {
    opts = oyOptions_New( 0 );
    oyOptions_AppendOpts( opts, config_->db );
    oyOptions_AppendOpts( opts, config_->backend_core );

    error = oyOptions_SaveToDB( opts, scope, config_->registration, &new_reg,0);

    /* add information about the data's origin */
    oyStringAddPrintf( &key, oyAllocateFunc_, oyDeAllocateFunc_, "%s/key_set_name",
                       oyConfig_GetRegistration( config ) );
    error = oyOptions_SetFromText( &config_->data, key, new_reg, OY_CREATE_NEW );

    oyFree_m_( new_reg );
    oyFree_m_( key );
    oyOptions_Release( &opts );
  }

  DBG_PROG_ENDE
  return error;
}

/** Function  oyConfig_EraseFromDB
 *  @memberof oyConfig_s
 *  @brief    Remove a oyConfig_s from DB
 *
 *  @param[in]     config              the configuration
 *  @param         scope               oySCOPE_USER and oySCOPE_SYS are possible
 *  @return                            0 - good, 1 >= error
 *
 *  @version Oyranos: 0.9.6
 *  @date    2015/03/07
 *  @since   2009/01/27 (Oyranos: 0.1.10)
 */
OYAPI int  OYEXPORT oyConfig_EraseFromDB (
                                       oyConfig_s        * config,
                                       oySCOPE_e           scope )
{
  int error = !config;
  oyOption_s * o = 0;
  int i;
  const char * text = 0;
  char * t = 0,
       * tmp = 0;
  oyConfig_s_ * s = (oyConfig_s_*)config;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  if(error <= 0)
  {
    i = 0;
    text = s->registration;
    if(text)
      while( (text = oyStrchr_(++text, OY_SLASH_C)) != 0)
        ++i;

    if(i != 4)
    {
      o = oyOptions_Get( s->db, 0 );
      i = 0;
      text = 0;
      if(o)
        text = oyOption_GetRegistration( o );
      if(text)
        while( (text = oyStrchr_(++text, OY_SLASH_C)) != 0)
          ++i;

      /** If a key has one slash more than 4. Cut the last slash off.  */
      if(i == 5)
      {
        tmp = oyStringCopy_( oyOption_GetRegistration( o ), oyAllocateFunc_ );
        t = oyStrrchr_(tmp, OY_SLASH_C);
        t[0] = 0;
        text = tmp;
      }

    }
    else
      text = s->registration;

    /** Cut off any attributes to the config::registration. */
    if(!tmp)
      text = tmp = oyStringCopy( text, oyAllocateFunc_ );
    t = oyStrrchr_(tmp, '.');
    if(t)
      t[0] = 0;

    error = oyDBEraseKey( text, scope );

    if(tmp)
      oyFree_m_( tmp );
    oyOption_Release( &o );
  }

  oyExportEnd_();
  DBG_PROG_ENDE
  return error;
}

/** Function  oyConfig_Compare
 *  @memberof oyConfig_s
 *  @brief    Check for matching to a given pattern
 *
 *  @param[in]     module_device       the to be checked configuration from
 *                                     oyConfigs_FromPattern_f;
 *                                     Additional allowed are DB configs.
 *  @param[in]     db_pattern          the to be compared configuration from
 *                                     elsewhere, e.g. ICC dict tag
 *  @param[out]    rank_value          the number of matches between config and
 *                                     pattern, -1 means invalid
 *  @return                            0 - good, >= 1 - error + a message should
 *                                     be sent
 *
 *  @version Oyranos: 0.9.6
 *  @date    2015/08/03
 *  @since   2009/01/26 (Oyranos: 0.1.10)
 */
int            oyConfig_Compare      ( oyConfig_s        * module_device,
                                       oyConfig_s        * db_pattern,
                                       int32_t           * rank_value )
{
  int error = !module_device || !db_pattern;
  int domain_n, pattern_n, i, j, k, l,
      rank = 0,
      has_opt;
  oyOption_s * d = 0,
             * p = 0,
             * check = 0;
  oyOptions_s * dopts = 0; /* device options */
  char * d_opt = 0, * d_val = 0, /* device variables */
       * p_opt = 0, * p_val = 0, /* pattern variables */
       * check_opt = 0, * check_val = 0;
  oyConfig_s_ * pattern = (oyConfig_s_*)db_pattern,
              * device  = (oyConfig_s_*)module_device;
  oyRankMap  * rank_map = 0;
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
#ifdef DEBUG
      else if(oy_debug > 2)
        WARNc1_S("No key/values pairs found in %s", device->registration);
#endif
    }

    pattern_n = oyOptions_Count( pattern->db );
    for(i = 0; i < domain_n; ++i)
    {
      d = oyOptions_Get( dopts, i );
      d_opt = oyFilterRegistrationToText( oyOption_GetRegistration(d),
                                          oyFILTER_REG_MAX, 0);
      d_val = oyOption_GetValueText( d, oyAllocateFunc_ );
      has_opt = 0;

      /* check for double occurences */
      for(l = 0; l < i; ++l)
      {
        check = oyOptions_Get( dopts, l );
        check_opt = oyFilterRegistrationToText( oyOption_GetRegistration(check),
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

      if(d_val && d_opt)
      for( j = 0; j < pattern_n; ++j )
      {
        p = oyOptions_Get( pattern->db, j );

        p_opt = oyFilterRegistrationToText( oyOption_GetRegistration(p),
                                            oyFILTER_REG_MAX,
                                            0 );

        if(p_opt && oyStrstr_(p_opt, d_opt))
        {
          p_val = oyOption_GetValueText( p, oyAllocateFunc_ );
          has_opt = 1;

          /** Option name is equal and and value matches : increase rank value
           *  
           *        we need a comparision mechanism here. The pattern value
           *        should be expandable to several values.
           *        Do we need more than the ICC dict style syntax here?
           */
          if(p_val && oyTextIccDictMatch( d_val, p_val, 0.0005 ))
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
          break;
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
              DBG_NUM4_S( "%s %s/%s -> %d",
                          oyNoEmptyString_m_(d_opt),
                          oyNoEmptyString_m_(d_val),
                          oyNoEmptyString_m_(p_val), rank); 
              break;
            }
            ++k;
          }
      }

      oyOption_Release( &d );
      if(d_opt)
        oyFree_m_( d_opt );
      if(d_val)
        oyFree_m_( d_val );
    }
  }

  if(rank_value)
    *rank_value = rank;

  return error;
}

/** Function  oyConfig_DomainRank
 *  @memberof oyConfig_s
 *  @brief    Check for being recognised by a given module
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
  oyCMMapi8_s_ * cmm_api8 = 0;
  oyConfig_s_ * s = (oyConfig_s_*)config;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  oyExportStart_(EXPORT_CHECK_NO);

  if(error <= 0)
  {
    apis = oyCMMsGetFilterApis_( oyConfigPriv_m(config)->registration,
                                 oyOBJECT_CMM_API8_S,
                                 oyFILTER_REG_MODE_NONE,
                                 &rank_list, &apis_n);
    error = !apis;
  }

  if(error <= 0)
  {
    for(i = 0; i < apis_n; ++i)
    {
      cmm_api8 = (oyCMMapi8_s_*) oyCMMapiFilters_Get( apis, i );

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

  if(error && max_rank > 0)
    rank = -1;
  else
    rank = max_rank;

  oyExportEnd_();
  return rank;
}

/** Function  oyConfig_FindString
 *  @memberof oyConfig_s
 *  @brief    Search in data sets for a key/value
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
  oyConfig_s_ * s = (oyConfig_s_*)config;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )


  text = oyOptions_FindString( s->data, key, value );
  if(!text)
    text = oyOptions_FindString( s->backend_core, key, value );
  if(!text)
    text = oyOptions_FindString( s->db, key, value );

  return text;
}

/** Function  oyConfig_Has
 *  @memberof oyConfig_s
 *  @brief    Search in data sets for a key
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
  oyConfig_s_ * s = (oyConfig_s_*)config;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  o = oyOptions_Find( s->data, key, oyNAME_PATTERN );
  if(!o)
    o = oyOptions_Find( s->backend_core, key, oyNAME_PATTERN );
  if(!o)
    o = oyOptions_Find( s->db, key, oyNAME_PATTERN );

  if(o)
    has_option = 1;

  oyOption_Release( &o );

  return has_option;
}

/** Function  oyConfig_Find
 *  @memberof oyConfig_s
 *  @brief    Search in data sets for a key
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
  oyConfig_s_ * s = (oyConfig_s_*)config;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  o = oyOptions_Find( s->data, key, oyNAME_PATTERN );
  if(!o)
    o = oyOptions_Find( s->backend_core, key, oyNAME_PATTERN );
  if(!o)
    o = oyOptions_Find( s->db, key, oyNAME_PATTERN );

  return o;
}

/** Function  oyConfig_Count
 *  @memberof oyConfig_s
 *  @brief    Number of all options
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
  oyConfig_s_ * s = (oyConfig_s_*)config;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  if(error <= 0)
  {
    opts = oyOptions_New( 0 );

    oyOptions_AppendOpts( opts, s->db );
    oyOptions_AppendOpts( opts, s->backend_core );
    oyOptions_AppendOpts( opts, s->data );
    n = oyOptions_Count( opts );
    oyOptions_Release( &opts );
  }

  return n;
}

/** Function  oyConfig_Get
 *  @memberof oyConfig_s
 *  @brief    Get one option
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
  oyConfig_s_ * s = (oyConfig_s_*)config;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  if(error <= 0)
  {
    opts = oyOptions_New( 0 );

    oyOptions_AppendOpts( opts, s->db );
    oyOptions_AppendOpts( opts, s->backend_core );
    oyOptions_AppendOpts( opts, s->data );

    o = oyOptions_Get( opts, pos );

    oyOptions_Release( &opts );
  }

  return o;
}
/** Function  oyConfig_GetOptions
 *  @memberof oyConfig_s
 *  @brief    Get options from a source
 *
 *  This API allows low level operations on a Config object.
 *  Do not release the returned Options without replacement inside the
 *  Config object.
 *
 *  A Config object has three kind of options.
 *
 *  The data base "db" properties are for persistent storage in a DB,
 *  e.g. "org/freedesktop/imaging/config.monitor.xorg/1/manufacturer=EIZO"
 *
 *  The "backend_core" options are the module core properties, the ones to
 *  identify the device and store in DB. They must be filled by the module.
 *  e.g. "org/freedesktop/imaging/config.monitor.xorg/manufacturer=EIZO"
 *
 *  The "data" properties are additional options without identification
 *  purpose and not intended for DB storage,
 *  e.g. "org/freedesktop/imaging/config.monitor.xorg/edid=oyBlob_s*"
 *
 *  @param[in]     config              the configuration
 *  @param[in]     source              use of the options,
 *                                     - "db" for persistent options
 *                                     - "backend_core" for module options
 *                                     - "data" for module options without 
 *                                       identification purpose
 *  @return                            the selected options, in the domain of
 *                                     the config
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/10/03
 *  @since    2012/10/03 (Oyranos: 0.5.0)
 */
OYAPI oyOptions_s ** OYEXPORT
               oyConfig_GetOptions   ( oyConfig_s        * config,
                                       const char        * source )
{
  oyConfig_s_ * s = (oyConfig_s_*)config;
  static oyOptions_s * dummy = 0;

  if(!dummy)
    dummy = oyOptions_New( 0 );

  if(!s)
  {
    oyOptions_Clear( dummy );
    return &dummy;
  }

  oyCheckType__m( oyOBJECT_CONFIG_S, oyOptions_Clear( dummy ); return &dummy )

  if(source && strcmp(source,"db") == 0)
  {
    return &s->db;
  } else
  if(source && strcmp(source,"backend_core") == 0)
  {
    return &s->backend_core;
  } else
  if(source && strcmp(source,"data") == 0)
  {
    return &s->data;
  }

  oyOptions_Clear( dummy );
  return &dummy;
}

/** Function  oyConfig_SetRankMap
 *  @memberof oyConfig_s
 *  @brief    Set the ranking table
 *
 *  @param[in,out] config              the configuration
 *  @param[in]     rank_map            the new rank map
 *  @return                            error
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/10/03
 *  @since    2012/10/03 (Oyranos: 0.5.0)
 */
OYAPI int  OYEXPORT
               oyConfig_SetRankMap   ( oyConfig_s        * config,
                                       const oyRankMap   * rank_map )
{
  oyConfig_s_ * s = (oyConfig_s_*)config;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 1 )

  if(s->rank_map)
  {
    oyRankMapRelease( &s->rank_map, s->oy_->deallocateFunc_ );
  }
  s->rank_map = oyRankMapCopy( rank_map, s->oy_->allocateFunc_ );

  return 0;
}
/** Function  oyConfig_GetRankMap
 *  @memberof oyConfig_s
 *  @brief    Get the ranking table
 *
 *  @param[in]     config              the configuration
 *  @return        rank_map            the rank map
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/10/03
 *  @since    2012/10/03 (Oyranos: 0.5.0)
 */
OYAPI const oyRankMap *  OYEXPORT
               oyConfig_GetRankMap   ( oyConfig_s        * config )
{
  oyConfig_s_ * s = (oyConfig_s_*)config;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  return s->rank_map;
}

/** Function  oyRankMapCopy
 *  @memberof oyConfig_s
 *  @brief    Copy a rank map
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/27 (Oyranos: 0.1.10)
 *  @date    2009/01/27
 */
OYAPI oyRankMap * OYEXPORT oyRankMapCopy
                                     ( const oyRankMap   * rank_map,
                                       oyAlloc_f           allocateFunc )
{
  oyRankMap * map = 0;
  int error = !rank_map;
  int n = 0, i = 0;

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  if(error <= 0)
  {
    while( rank_map[i++].key ) ++n;

    oyAllocHelper_m_( map, oyRankMap, n + 1, allocateFunc, error = 1 );
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
/** Function  oyRankMapRelease
 *  @memberof oyConfig_s
 *  @brief    Release a Rank Map
 *
 *  @param[in,out] rank_map            the rank map
 *  @param[in]     deAllocateFunc      the memory release function
 *  @return                            error
 *
 *  @version  Oyranos: 0.5.0
 *  @date     2012/10/03
 *  @since    2012/10/03 (Oyranos: 0.5.0)
 */
OYAPI void  OYEXPORT oyRankMapRelease( oyRankMap        ** rank_map,
                                       oyDeAlloc_f         deAllocateFunc )
{
  int error = !rank_map || !*rank_map;
  int n = 0, i = 0;

  if(!deAllocateFunc)
    deAllocateFunc = oyDeAllocateFunc_;

  if(error <= 0)
  {
    oyRankMap * map = *rank_map;
    while( (*rank_map)[i++].key ) ++n;
    for(i = 0; i < n; ++i)
    {
      deAllocateFunc( map[i].key ); map[i].key = 0;
      map[i].match_value = 0;
      map[i].none_match_value = 0;
      map[i].not_found_value = 0;
    }
    deAllocateFunc( map );
    *rank_map = 0;
  }

}

/** Function  oyRankMapAppend
 *  @memberof oyConfig_s
 *  @brief    Append a rank map entry
 *
 *  A helper function to continually construct rank maps.
 *
 *  @param[in,out] rank_map            a pointer to a data structure,
 *                                     The pointed to structure can be NULL.
 *  @param[in]     key                 the key to apply ranking to
 *  @param[in]     match_value         a found and matched value
 *  @param[in]     none_match_value    a found and not matched value
 *  @param[in]     not_found_value     a not found value
 *  @param[in]     allocateFunc        the memory allocation function, optional
 *  @param[in]     deAllocateFunc      the memory release function, optional
 *
 *  @version Oyranos: 0.9.5
 *  @date    2013/02/22
 *  @since   2013/02/22 (Oyranos: 0.9.5)
 */
OYAPI int  OYEXPORT oyRankMapAppend  ( oyRankMap        ** rank_map,
                                       const char        * key,
                                       int                 match_value,
                                       int                 none_match_value,
                                       int                 not_found_value,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deAllocateFunc )
{
  int n = 0, i = 0,
      error = 0;
  oyRankMap * rm;

  if(*rank_map)
    while((*rank_map)[i++].key) n++;

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;
  if(!deAllocateFunc)
    deAllocateFunc = oyDeAllocateFunc_;

  rm = allocateFunc( sizeof(oyRankMap) * (n+2) );
  if(!rm)
  {
      oyMessageFunc_p( oyMSG_ERROR, (oyStruct_s*)0, OY_DBG_FORMAT_ "\n"
                       "Could not allocate enough memory", OY_DBG_ARGS_ );
      error = -1;
      return error;
  }
  if(n)
  {
    memcpy(rm, *rank_map, n*sizeof(oyRankMap));
    for(i = 0; i < n; ++i)
      rm[i].key = oyStringCopy_((*rank_map)[i].key, allocateFunc);
    oyRankMapRelease( rank_map, deAllocateFunc );
  }

  rm[n].key = oyStringCopy_(key, allocateFunc);
  rm[n].match_value = match_value;
  rm[n].none_match_value = none_match_value;
  rm[n].not_found_value = not_found_value;

  rm[n+1].key = NULL;
  rm[n+1].match_value = 0;
  rm[n+1].none_match_value = 0;
  rm[n+1].not_found_value = 0;

  *rank_map = rm;

  return error;
}


/** Function  oyRankMapFromJSON
 *  @memberof oyConfig_s
 *  @brief    Create a Rank Map
 *
 *  @param[in]     json_text           the rank map definition
 *  @param[in]     options             optional
 *                                     - "pos" integer selects position in array
 *  @param[out]    rank_map            the result
 *  @param[in]     allocateFunc        the memory allocate function
 *  @return                            0 - good, >= 1 - error + a message should
 *                                     be sent, < 0 - an issue
 *
 *  @version  Oyranos: 0.9.5
 *  @date     2013/02/13
 *  @since    2013/02/13 (Oyranos: 0.9.5)
 */
OYAPI int  OYEXPORT oyRankMapFromJSON( const char        * json_text,
                                       oyOptions_s       * options,
                                       oyRankMap        ** rank_map,
                                       oyAlloc_f           allocateFunc )
{
  oyRankMap * map = 0;
  int error = !json_text;

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  if(error <= 0)
  {
    oyjl_val json = 0,
             json_rankm = 0;
    char * key, * t;
    const char * xpath = "org/freedesktop/openicc/rank_map/[0]/[%d]";
    int count, i;
    int32_t pos = 0;
    oyjl_val v;

    t = oyAllocateFunc_(256);
    json = oyjl_tree_parse( json_text, t, 256 );
    if(t[0])
    {
      WARNc2_S( "%s: %s\n", _("found issues parsing JSON"), t );
      error = 1;
    }
    oyFree_m_(t);

    oyOptions_FindInt( options, "pos", 0, &pos );
    json_rankm = oyjl_tree_get_valuef( json, xpath, pos );

    count = oyjl_value_count( json_rankm );
    oyAllocHelper_m_( map, oyRankMap, count + 1, allocateFunc, error = 1 );

    for(i = 0; i < count; ++i)
    {
      v = oyjl_value_pos_get( json_rankm, i );
      if(json_rankm->type == oyjl_t_object)
        key = oyStringCopy_( json_rankm->u.object.keys[i], allocateFunc );
      else
        key = 0;

      if(key && oyjl_value_count( v ))
      {
        map[i].key = key; key = 0;
        if(v->type == oyjl_t_array)
        {
#define ma( target, pos ) \
          if( v->u.array.len >= pos+1) \
          { \
            if( v->u.array.values[pos]->type == oyjl_t_number && \
                v->u.array.values[pos]->u.number.flags & OYJL_NUMBER_INT_VALID ) \
              target = v->u.array.values[pos]->u.number.i; \
            else if( v->u.array.values[pos]->type == oyjl_t_string ) \
              target = atoi(v->u.array.values[pos]->u.string); \
          }
#if 0
          if( v->u.array.len >= 1)
          {
            if( OYJL_IS_NUMBER(v->u.array.values[0]) &&
                OYJL_IS_DOUBLE(v->u.array.values[0]) )
              map[i].match_value = v->u.array.values[0]->u.number.i;
            else if( OYJL_IS_STRING(v->u.array.values[0]) )
              map[i].match_value = atoi(OYJL_GET_STRING(v->u.array.values[0]));
          }
#else
        ma( map[i].match_value, 0 )
#endif
        ma( map[i].none_match_value, 1)
        ma( map[i].not_found_value, 2)
#undef  ma
        }
      }
    }
  }

  if(error <= 0 && rank_map)
  {
    *rank_map = map;
  }

  return error;
}

#define OPENICC_DEVICE_JSON_RANK_MAP_HEADER \
  "{\n" \
  "  \"org\": {\n" \
  "    \"freedesktop\": {\n" \
  "      \"openicc\": {\n" \
  "        \"rank_map\": {\n" \
  "          \"%s\": [{\n"
#define OPENICC_DEVICE_JSON_RANK_MAP_FOOTER \
  "            }\n" \
  "          ]\n" \
  "        }\n" \
  "      }\n" \
  "    }\n" \
  "  }\n" \
  "}\n"

/** Function  oyRankMapToJSON
 *  @memberof oyConfig_s
 *  @brief    Create JSON from a Rank Map
 *
 *  @param[in]     rank_map            the rank map
 *  @param[in]     options             optional
 *                                     - "pos" integer selects position in array
 *                                     - "device_class" will be placed into the JSON
 *  @param[out]    json_text           the result
 *  @param[in]     allocateFunc        the memory allocate function
 *  @return                            0 - good, >= 1 - error + a message should
 *                                     be sent, < 0 - an issue
 *
 *  @version  Oyranos: 0.9.5
 *  @date     2013/02/13
 *  @since    2013/02/13 (Oyranos: 0.9.5)
 */
OYAPI int OYEXPORT oyRankMapToJSON   ( const oyRankMap   * rank_map,
                                       oyOptions_s       * options,
                                       char             ** json_text,
                                       oyAlloc_f           allocateFunc )
{
  int error = !rank_map;
  int n = 0, i = 0;
  char * t = 0;
  const char * device_class = oyOptions_FindString( options, "device_class", 0 );

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  if(error <= 0)
  {
    const oyRankMap * map = rank_map;
    while( (rank_map)[i++].key ) ++n;

    if(n)
      oyStringAddPrintf_( &t, oyAllocateFunc_, oyDeAllocateFunc_,
                          OPENICC_DEVICE_JSON_RANK_MAP_HEADER,
                          oyNoEmptyString_m_( device_class ));

    for(i = 0; i < n; ++i)
    {
      if(i > 0 && map[i].key)
        STRING_ADD( t, ",\n" );
      if(map[i].key)
        oyStringAddPrintf_( &t, oyAllocateFunc_, oyDeAllocateFunc_,
  "              \"%s\": [%d, %d, %d]",
                            map[i].key,
                            map[i].match_value,
                            map[i].none_match_value,
                            map[i].not_found_value );
      else
        STRING_ADD( t, "\n" );
    }

    if(n)
    {
      STRING_ADD( t, "\n" );
      STRING_ADD( t, OPENICC_DEVICE_JSON_RANK_MAP_FOOTER );
    }

    if(t && json_text)
    {
      if(allocateFunc != oyAllocateFunc_)
      {
        *json_text = oyStringCopy_( t, allocateFunc );
        oyFree_m_( t );
      } else
        *json_text = t;
    }
  }

  return error;
}

/** Function  oyRankMapList
 *  @memberof oyConfig_s
 *  @brief    List installed Rank Map Files
 *
 *  Load the file content with oyRankMapFromJSON().
 *
 *  @param[in]     filter              a string to prefilter file names; optional
 *  @param[in]     options             - "path" - show paths instead of files
 *  @param[out]    rank_map_file_names zero terminated list of rank map JSON
 *                                     files
 *  @param[in]     allocateFunc        the memory allocate function
 *  @return                            0 - good, >= 1 - error + a message should
 *                                     be sent, < 0 - an issue
 *
 *  @version  Oyranos: 0.9.6
 *  @date     2015/08/04
 *  @since    2015/08/04 (Oyranos: 0.9.6)
 */
OYAPI int OYEXPORT oyRankMapList     ( const char        * filter,
                                       oyOptions_s       * options,
                                       char            *** rank_map_file_names,
                                       oyAlloc_f           allocateFunc )
{
  int error = !rank_map_file_names;
  int n = 0, i = 0;
  int list_rank_paths = oyOptions_FindString( options, "path", 0 ) ? 1 : 0;

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  if(error <= 0)
  {
    const char * subdir = "color/rank-map";
    int data = oyYES,
        owner = oySCOPE_USER_SYS;
    const char * dir_string = NULL,
               * suffix = "json";

    if(error <= 0 && list_rank_paths)
    {
      int path_names_n = 0;

      *rank_map_file_names = oyDataPathsGet_( &n, subdir, oyALL,
                                              owner, allocateFunc );
      if(oy_debug)
        for( i = 0; i < path_names_n; ++i )
          DBG_NUM1_S( "%s\n", rank_map_file_names[i] );

      return error;
    }

    if(error <= 0)
      *rank_map_file_names = oyDataFilesGet_( &n, subdir, data, owner,
                                      dir_string, filter, suffix,
                                      allocateFunc );
    if(oy_debug)
      DBG_NUM1_S( "found rank maps: %d", n );
    for( i = 0; i < n; ++i )
    {
      const char * file = (*rank_map_file_names)[i];

      if(oy_debug)
        DBG_NUM2_S( "%d: %s\n", i, file );
    }
  }

  return error;
}


/** Function  oyConfig_FromRegistration
 *  @memberof oyConfig_s
 *  @brief    New config with registration
 *
 *  @param         registration        no or full qualified registration
 *  @param         object              the optional object
 *  @return                            the config
 *
 *  @version Oyranos: 0.1.11
 *  @since   2010/08/09 (Oyranos: 0.1.11)
 *  @date    2010/08/09
 */
OYAPI oyConfig_s * OYEXPORT
               oyConfig_FromRegistration( const char        * registration,
                                          oyObject_s          object )
{
  oyConfig_s_ *s = (oyConfig_s_*)oyConfig_New( object );

  if(registration)
    s->registration = oyStringCopy_( registration, s->oy_->allocateFunc_ );

  return (oyConfig_s*)s;
}



/** Function oyConfig_GetBestMatchFromTaxiDB
 *  @brief   search a profile ID in the Taxi DB for a configuration
 *  @memberof oyConfig_s
 *
 *  @param[in]     device              the to be checked configuration from
 *                                     oyConfigs_FromPattern_f
 *  @param[out]    rank_value          the number of matches between config and
 *                                     pattern, -1 means invalid
 *  @return                            0 - good, >= 1 - error + a message should
 *                                     be sent
 *
 *  @version Oyranos: 0.3.2
 *  @since   2009/01/26 (Oyranos: 0.3.2)
 *  @date    2011/09/14
 */
OYAPI int  OYEXPORT
               oyConfig_GetBestMatchFromTaxiDB(
                                       oyConfig_s        * device,
                                       int32_t           * rank_value )
{
  int error = !device;
  oyConfigs_s * configs = 0;
  oyConfig_s_ * s = (oyConfig_s_*)device;
  oyOptions_s * options = 0;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  if(error <= 0)
  {
    error = oyDevicesFromTaxiDB( device, options, &configs, 0 );
    oyOptions_Release( &options );
  }
  if(error <= 0)
    error = oyConfig_SelectDB( device, configs, rank_value );
  oyConfigs_Release( &configs );

  return error;
}

/** Function  oyConfig_GetRegistration
 *  @memberof oyConfig_s
 *  @brief    Get the registration string @ref registration
 *
 *  @param[in]     config              the config
 *  @return                            the registration string
 *
 *  @version Oyranos: 0.5.0
 *  @date    2012/09/19
 *  @since   2012/09/19 (Oyranos: 0.5.0)
 */
OYAPI const char *  OYEXPORT
               oyConfig_GetRegistration
                                     ( oyConfig_s        * config )
{
  oyConfig_s_ * s = (oyConfig_s_*)config;

  if(!config)
    return NULL;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  return s->registration;
}

