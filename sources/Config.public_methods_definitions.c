#include "oyranos_alpha.h"
#include "oyranos_elektra.h"
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
                                       int32_t           * rank_value )
{
  int error = !device;
  oyConfigs_s * configs = 0;
  oyConfig_s_ * s = (oyConfig_s_*)device;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  if(error <= 0)
    error = oyConfigs_FromDB( s->registration, &configs, 0 );
  if(error <= 0)
    error = oyConfig_GetFromDB( device, configs, rank_value );
  oyConfigs_Release( &configs );

  return error;
}

/** Function  oyConfig_GetFromDB
 *  @brief    search a configuration in the DB for a configuration from module
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
               oyConfig_GetFromDB    ( oyConfig_s        * device,
                                       oyConfigs_s       * configs,
                                       int32_t           * rank_value )
{
  int error = !device;
  int rank = 0, max_rank = 0, i, n;
  oyConfig_s * config = 0, * max_config = 0;
  oyConfig_s_ * s = (oyConfig_s_*)device;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  if(error <= 0)
  {
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
 *  @return                            0 - good, 1 >= error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/21 (Oyranos: 0.1.10)
 *  @date    2011/01/29
 */
OYAPI int  OYEXPORT
               oyConfig_SaveToDB     ( oyConfig_s        * config )
{
  int error = !config;
  oyOptions_s * opts = 0;
  oyConfig_s_ * s = (oyConfig_s_*)config;
  char * new_reg = 0;
  oyConfig_s_ * config_ = (oyConfig_s_*)config;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  DBG_PROG_START

  if(error <= 0)
  {
    opts = oyOptions_New( 0 );
    oyOptions_AppendOpts( opts, config_->db );
    oyOptions_AppendOpts( opts, config_->backend_core );

    error = oyOptions_SaveToDB( opts, config_->registration, &new_reg, 0 );

    /* add information about the data's origin */
    oyConfig_AddDBData( config, "key_set_name", new_reg, OY_CREATE_NEW );

    oyFree_m_( new_reg );
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

      /* A key has one slash more. Cut the last slash off.  */
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

    error = oyEraseKey_( text );

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
 *  @version Oyranos: 0.1.13
 *  @since   2009/01/26 (Oyranos: 0.1.10)
 *  @date    2010/11/21
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
  char * d_opt = 0, * d_val = 0, /* device variables */
       * p_opt = 0, * p_val = 0, /* pattern variables */
       * check_opt = 0, * check_val = 0;
  oyConfig_s_ * pattern = (oyConfig_s_*)db_pattern,
              * device  = (oyConfig_s_*)module_device;
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

#ifdef UNHIDE_CMM
      d_rank = oyConfig_DomainRank( (oyConfig_s*)device );
#else
      d_rank = 1;
#endif
      if(d_rank > 0 && d_val && d_opt)
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

#ifdef UNHIDE_CMM
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
  oyConfig_s_ * s = (oyConfig_s_*)device;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  oyExportStart_(EXPORT_CHECK_NO);

  if(error <= 0)
  {
    apis = oyCMMsGetFilterApis_( 0,0, oyConfigPriv_m(config)->registration,
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

  if(error && max_rank >= 0)
    rank = -1;
  else
    rank = max_rank;

  oyExportEnd_();
  return rank;
}
#endif

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

  o = oyOptions_Find( s->data, key );
  if(!o)
    o = oyOptions_Find( s->backend_core, key );
  if(!o)
    o = oyOptions_Find( s->db, key );

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

  o = oyOptions_Find( s->data, key );
  if(!o)
    o = oyOptions_Find( s->backend_core, key );
  if(!o)
    o = oyOptions_Find( s->db, key );

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

/** Function  oyRankMapCopy
 *  @memberof oyConfig_s
 *  @brief    Copy a rank map
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


#define OPENICC_DEVICE_JSON_HEADER_BASE \
  "{\n" \
  "  \"org\": {\n" \
  "    \"freedesktop\": {\n" \
  "      \"openicc\": {\n" \
  "        \"device\": {\n" \
  "          \"%s\":\n"
#define OPENICC_DEVICE_JSON_FOOTER_BASE \
  "        }\n" \
  "      }\n" \
  "    }\n" \
  "  }\n" \
  "}\n"


/** Function  oyDevicesFromTaxiDB
 *  @brief    search a calibration state in the taxi DB for a device
 *  @memberof oyConfig_s
 *
 *  @param[in]     device              the device
 *  @param[in]     options             not used
 *  @param[out]    devices             the obtained device calibrations
 *  @return                            0 - good, >= 1 - error + a message should
 *                                     be sent
 *
 *  @version Oyranos: 0.3.3
 *  @since   2011/09/14 (Oyranos: 0.3.2)
 *  @date    2012/01/06
 */
OYAPI int  OYEXPORT
             oyDevicesFromTaxiDB     ( oyConfig_s        * device,
                                       oyOptions_s       * options,
                                       oyConfigs_s      ** devices,
                                       oyObject_s          obj )
{
  int error = 0;
  oyConfigs_s * configs_ = 0;
  oyConfig_s_ * s = (oyConfig_s_*)device;
  char * manufacturers;
  size_t size = 0;
  const char * short_name = NULL,
             * long_name = NULL,
             * name = NULL;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 0 )

  error = oyDeviceCheckProperties ( device );


  manufacturers = oyReadUrlToMem_( "http://icc.opensuse.org/manufacturers",
                                   &size, "r", oyAllocateFunc_ );

  
  if(manufacturers)
  {
    oyjl_value_s * root = 0;
    int count = 0, i;
    char * val = NULL,
         * key = NULL;
    char * json_text = NULL;
    const char * prefix = oyConfig_FindString( device, "prefix", 0 );

    error = oyjl_tree_from_json( manufacturers, &root, NULL );
    if(prefix)
      oyStringAddPrintf_( &key, oyAllocateFunc_, oyDeAllocateFunc_,
                          "%smnft", prefix );
    else
      oyStringAddPrintf_( &key, oyAllocateFunc_, oyDeAllocateFunc_,
                          "mnft" );
    name = short_name = oyConfig_FindString( device, key, 0 );
    oyFree_m_(key);
    if(prefix)
      oyStringAddPrintf_( &key, oyAllocateFunc_, oyDeAllocateFunc_,
                          "%smanufacturer", prefix );
    else
      oyStringAddPrintf_( &key, oyAllocateFunc_, oyDeAllocateFunc_,
                          "manufacturer" );
    if(!short_name)
      name = long_name = oyConfig_FindString( device, key, 0 );
    oyFree_m_(key);

    if(oy_debug)
      WARNc1_S("manufacturers:\n%s", manufacturers);

    if(root)
    {
      int done = 0;
      oyjl_value_s * v = 0, * tv = 0;

      count = oyjl_value_count(root);
      for(i = 0; i < count; ++i)
      {
        if(short_name)
          v = oyjl_tree_get_valuef( root, 
                              "[%d]/short_name", i );
        else if(long_name)
          v = oyjl_tree_get_valuef( root, 
                              "[%d]/long_name", i );

        val = oyjl_value_text( v, oyAllocateFunc_ );
        if( val && name && strcmp( val, name) == 0 )
          done = 1;
        else
          DBG_NUM2_S("could not find device:\n%s %s",
                   oyNoEmptyString_m_(name), oyNoEmptyString_m_(val));

        if(done) break;
        if(val) oyDeAllocateFunc_(val); val = 0;
      }

      error = oyjl_tree_free( &root );

      /* get the devices */
      if(done)
      {
        char * device_db, * t = NULL;
        oyOptions_s * opts = NULL;
        oyConfig_s * dev;

        /* put a cloak around the bare meta data, so it behaves like OpenICC
         * JSON */
        oyStringAddPrintf_( &t, oyAllocateFunc_, oyDeAllocateFunc_,
                            OPENICC_DEVICE_JSON_HEADER_BASE, "dummy" );

        /* the device DB JSON contains all device meta data for one
         * mnft / manufacturer */
        device_db = oyReadUrlToMemf_( &size, "r", oyAllocateFunc_,
                            "http://icc.opensuse.org/devices/%s", val );
        STRING_ADD( t, device_db );
        oyFree_m_( device_db );

        oyStringAddPrintf_( &t, oyAllocateFunc_, oyDeAllocateFunc_,
                            "\n"OPENICC_DEVICE_JSON_FOOTER_BASE );
        device_db = t; t = NULL;

        if(oy_debug)
          oyMessageFunc_p( oyMSG_DBG,(oyStruct_s*)device,
                       OY_DBG_FORMAT_
                       "http://icc.opensuse.org/devices/%s with header:\n%s",
                       OY_DBG_ARGS_,
                       val, oyNoEmptyString_m_(device_db) );
        error = oyjl_tree_from_json( device_db, &root, NULL );

        error = oyOptions_SetFromText( &opts,
                                 "//" OY_TYPE_STD "/argv/underline_key_suffix",
                                 "TAXI", OY_CREATE_NEW );

        tv = oyjl_tree_get_valuef( root, "org/freedesktop/openicc/device/[0]" );
        count = oyjl_value_count(tv);
        for(i = 0; i < count; ++i)
        {
          error = oyOptions_SetFromInt( &opts,
                                 "//" OY_TYPE_STD "/argv/pos",
                                 i, 0, OY_CREATE_NEW );

          v = oyjl_tree_get_valuef( root, "org/freedesktop/openicc/device/[0]/[%d]/_id/$oid", i );
          val = oyjl_value_text( v, oyAllocateFunc_ );
          error = oyDeviceFromJSON( device_db, opts, &dev );

          if(dev)
          {
            int j,n;
            oyConfig_AddDBData( dev, "TAXI_id", val, OY_CREATE_NEW );
            if(val) oyDeAllocateFunc_(val); val = 0;

            v = oyjl_tree_get_valuef( root, "org/freedesktop/openicc/device/[0]/[%d]/profile_description", i );
            n = oyjl_value_count(v);
            for(j = 0; j < n; ++j)
            {
              v = oyjl_tree_get_valuef( root, "org/freedesktop/openicc/device/[0]/[%d]/profile_description/[%d]", i, j );
              val = oyjl_value_text( v, oyAllocateFunc_ );
              oyConfig_AddDBData( dev, "TAXI_profile_description", val, OY_CREATE_NEW );
              if(val) oyDeAllocateFunc_(val); val = 0;
              /* TODO store all profile descriptions */
              break;
            }

            if(!configs_)
              configs_ = oyConfigs_New(0);
            oyConfigs_MoveIn( configs_, &dev, -1 );
          }

          if(val) oyDeAllocateFunc_(val); val = 0;
          if(json_text) oyFree_m_( json_text );
        }
        oyOptions_Release( &opts );
        if(device_db) oyDeAllocateFunc_(device_db); device_db = 0;
      }
      oyjl_tree_free( &root );
    }

    oyFree_m_( manufacturers );

    if(!configs_)
    {
      oyDeviceToJSON( device, 0, &json_text, oyAllocateFunc_ );
      WARNc1_S("no profile found for\n%s", json_text);
      oyFree_m_(json_text);
    } else if(oy_debug)
    {
      count = oyConfigs_Count( configs_ );
      for( i = 0; i < count; ++i)
        WARNc1_S("%d", i);
    }

    if(configs_)
      *devices = configs_;
  }

  return error;
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
    error = oyConfig_GetFromDB( device, configs, rank_value );
  oyConfigs_Release( &configs );

  return error;
}
