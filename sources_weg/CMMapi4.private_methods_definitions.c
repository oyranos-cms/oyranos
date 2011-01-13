/** @internal
 *  Function oyCMMapi4_selectFilter_
 *  @brief   filter the desired api
 *  @memberof oyCMMapi_s_
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/12/16 (Oyranos: 0.1.9)
 *  @date    2008/12/16
 */
oyOBJECT_e   oyCMMapi4_selectFilter_ ( oyCMMapi_s_       * api,
                                       oyPointer           data )
{
  oyOBJECT_e type = oyOBJECT_NONE,
             searched = oyOBJECT_CMM_API4_S;
  int error = !data || !api;
  oyRegistrationData_s * reg_filter;
  oyCMMapi4_s_ * cmm_api = (oyCMMapi4_s_ *) api;
  int found = 0;

  if(error <= 0)
    reg_filter = (oyRegistrationData_s*) data;

  if(error <= 0 &&
     api->type == searched &&
     reg_filter->type == searched)
  {
    if(reg_filter->registration)
    {
      if(oyFilterRegistrationMatch( cmm_api->registration,
                                    reg_filter->registration, api->type ))
        found = 1;
    } else
      found = 1;

    if( found )
      type = api->type;
  }

  return type;
}
