/** @internal
 *  Function oyOptions_ForFilter_
 *  @memberof oyOptions_s
 *  @brief   provide Oyranos behaviour settings
 *
 *  The returned options are read in from the Elektra settings and if thats not
 *  available from the inbuild defaults. The later can explicitely selected with
 *  oyOPTIONSOURCE_FILTER passed as flags argument.
 *  The key names map to the registration and XML syntax.
 *
 *  To obtain all front end options from a meta module use: @verbatim
    flags = oyOPTIONATTRIBUTE_ADVANCED |
            oyOPTIONATTRIBUTE_FRONT |
            OY_SELECT_COMMON @endverbatim
 *
 *  @param[in]     filter              the filter
 *  @param[in]     flags               for inbuild defaults |
 *                                     oyOPTIONSOURCE_FILTER;
 *                                     for options marked as advanced |
 *                                     oyOPTIONATTRIBUTE_ADVANCED |
 *                                     OY_SELECT_FILTER |
 *                                     OY_SELECT_COMMON
 *  @param         object              the optional object
 *  @return                            the options
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/08 (Oyranos: 0.1.9)
 *  @date    2009/07/27
 */
oyOptions_s *  oyOptions_ForFilter_  ( oyFilterCore_s    * filter,
                                       uint32_t            flags,
                                       oyObject_s          object )
{
  oyOptions_s * s = 0,
              * opts_tmp = 0,
              * opts_tmp2 = 0;
  oyOption_s * o = 0;
  int error = !filter || !filter->api4_;
  char * type_txt = oyFilterRegistrationToText( filter->registration_,
                                                oyFILTER_REG_TYPE, 0 );
  oyCMMapi5_s * api5 = 0;
  int i,n;

  /* by default we parse both sources */
  if(!(flags & OY_SELECT_FILTER) && !(flags & OY_SELECT_COMMON))
    flags |= OY_SELECT_FILTER | OY_SELECT_COMMON;

  if(!error)
  {
    /*
        Programm:
        1. get filter and its type
        2. get implementation for filter type
        3. parse static common options from meta module
        4. parse static options from filter
        5. merge both
        6. get stored values from disk
     */

    /*  1. get filter */

    /*  2. get implementation for filter type */
    api5 = filter->api4_->api5_;

    /*  3. parse static common options from a policy module */
    if(api5 && flags & OY_SELECT_COMMON)
    {
      oyCMMapiFilters_s * apis;
      int apis_n = 0;
      uint32_t         * rank_list = 0;
      oyCMMapi9_s * cmm_api9 = 0;
      char * klass, * api_reg;

      klass = oyFilterRegistrationToText( filter->registration_,
                                          oyFILTER_REG_TYPE, 0 );
      api_reg = oyStringCopy_("//", oyAllocateFunc_ );
      STRING_ADD( api_reg, klass );
      oyFree_m_( klass );

      s = oyOptions_New( 0 );

      apis = oyCMMsGetFilterApis_( 0, api_reg,
                                   oyOBJECT_CMM_API9_S,
                                   &rank_list, 0);
      apis_n = oyCMMapiFilters_Count( apis );
      for(i = 0; i < apis_n; ++i)
      {
        cmm_api9 = (oyCMMapi9_s*) oyCMMapiFilters_Get( apis, i );
        if(oyFilterRegistrationMatch( filter->registration_, cmm_api9->pattern,
                                      oyOBJECT_NONE ))
        {
          opts_tmp = oyOptions_FromText( cmm_api9->options, 0, object );
          oyOptions_AppendOpts( s, opts_tmp );
          oyOptions_Release( &opts_tmp );
        }
        if(cmm_api9->release)
          cmm_api9->release( (oyStruct_s**)&cmm_api9 );
      }
      oyCMMapiFilters_Release( &apis );
      oyFree_m_( api_reg );
      opts_tmp = s; s = 0;
    }
    /* requires step 2 */

    /*  4. parse static options from filter */
    if(flags & OY_SELECT_FILTER)
      opts_tmp2 = oyOptions_FromText( filter->api4_->ui->options, 0, object );

    /*  5. merge */
    s = oyOptions_FromBoolean( opts_tmp, opts_tmp2, oyBOOLEAN_UNION, object );

    oyOptions_Release( &opts_tmp );
    oyOptions_Release( &opts_tmp2 );

    /*  6. get stored values */
    n = oyOptions_Count( s );
    for(i = 0; i < n && error <= 0; ++i)
    {
      o = oyOptions_Get( s, i );
      oyOption_SetSource( o, oyOPTIONSOURCE_FILTER );
      /* ask Elektra */
      if(!(flags & oyOPTIONSOURCE_FILTER))
        error = oyOption_SetValueFromDB( o );
      oyOption_Release( &o );
    }
#if 0
    error = oyOptions_DoFilter ( s, flags, type_txt );
#endif
  }

  if(type_txt)
    oyDeAllocateFunc_( type_txt );

  return s;
}

/** Function  oyFilterCore_SetCMMapi4_
 *  @memberof oyFilterCore_s
 *  @brief    Lookup and initialise a new filter object
 *  @internal
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/11/27 (Oyranos: 0.1.9)
 *  @date    2009/02/28
 */
int          oyFilterCore_SetCMMapi4_( oyFilterCore_s_   * s,
                                       oyCMMapi4_s_      * cmm_api4 )
{
  int error = !s;
  oyAlloc_f allocateFunc_ = 0;
  static const char * lang = 0;
  int update = 1;

  if(error <= 0)
    allocateFunc_ = s->oy_->allocateFunc_;

  error = !(cmm_api4 && cmm_api4->type == oyOBJECT_CMM_API4_S);

  if(error <= 0)
  {
    s->registration_ = oyStringCopy_( cmm_api4->registration,
                                      allocateFunc_);

    s->category_ = oyStringCopy_( cmm_api4->ui->category, allocateFunc_ );

    /* we lock here as cmm_api4->oyCMMuiGet might not be thread save */
    {
      if(!lang)
        lang = oyLanguage();

      oyObject_Lock( s->oy_, __FILE__, __LINE__ );
      if(lang &&
         oyStrcmp_( oyNoEmptyName_m_(oyLanguage()), lang ) == 0)
        update = 0;

      oyObject_UnLock( s->oy_, __FILE__, __LINE__ );
    }

    s->api4_ = (oyCMMapi4_s*)cmm_api4;
  }

  if(error && s)
    oyFilterCore_Release_( &s );

  return error;
}
