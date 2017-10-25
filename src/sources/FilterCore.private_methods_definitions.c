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
#if 0
  static const char * lang = 0;
  int update = 1;
#endif

  if(error <= 0)
    allocateFunc_ = s->oy_->allocateFunc_;

  if(error <= 0)
    error = !(cmm_api4 && cmm_api4->type_ == oyOBJECT_CMM_API4_S);

  if(error <= 0)
  {
    s->registration_ = oyStringCopy_( cmm_api4->registration,
                                      allocateFunc_);

    s->category_ = oyStringCopy_( cmm_api4->ui->category, allocateFunc_ );

    /* we lock here as cmm_api4->oyCMMuiGet might not be thread save */
    {
#if 0
      if(!lang)
        lang = oyLanguage();

      oyObject_Lock( s->oy_, __FILE__, __LINE__ );
      if(lang &&
         oyStrcmp_( oyNoEmptyName_m_(oyLanguage()), lang ) == 0)
        update = 0;

      oyObject_UnLock( s->oy_, __FILE__, __LINE__ );
#endif
    }

    s->api4_ = cmm_api4;
  }

  if(error && s)
    oyFilterCore_Release_( &s );

  return error;
}
