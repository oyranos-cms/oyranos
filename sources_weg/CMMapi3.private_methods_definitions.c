/** Function    oyCMMapi3_Query_
 *  @memberof   oyCMMapi3_s
 *  @brief      Implements oyCMMapi_Check_f
 *  @internal
 *
 *  The data argument is expected to be oyCMMapiQueries_s.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/09/02 (Oyranos: 0.1.10)
 *  @date    2009/09/02
 */
oyOBJECT_e   oyCMMapi3_Query_        ( oyCMMInfo_s_      * cmm_info,
                                       oyCMMapi_s_       * api,
                                       oyPointer           data,
                                       uint32_t          * rank )
{
  oyCMMapiQueries_s * queries = data;
  uint32_t rank_ = 0;
  int prefered = 1;
  oyCMMapi3_s_ * api3 = 0;

  if(api->type == oyOBJECT_CMM_API3_S)
    api3 = (oyCMMapi3_s_*) api;

  if(memcmp( queries->prefered_cmm, cmm_info->cmm, 4 ) == 0)
    prefered = 10;

  rank_ = oyCMMCanHandle_( api3, queries );

  if(rank)
    *rank = rank_ * prefered;

  if(rank_)
    return api->type;
  else
    return oyOBJECT_NONE;
}
