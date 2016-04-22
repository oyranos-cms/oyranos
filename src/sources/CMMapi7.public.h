/** typedef oyCMMFilterPlug_Run_f
 *  @brief   get a pixel or channel from the previous filter
 *  @ingroup module_api
 *  @memberof oyCMMapi7_s
 *
 *  You have to call oyCMMFilter_CreateContext_t or oyCMMFilter_ContextFromMem_t first.
 *  The API provides flexible pixel access and cache configuration by the
 *  passed oyPixelAccess_s object. The filters internal precalculated data
 *  are passed by the filter object.
 *
 *  @code
    while (
    error = oyCMMFilterSocket_GetNext( filter_plug, pixel_access ) == 0
    ) {}; @endcode
 *
 *  @param[in]     plug                including the CMM's private data, connector is the requesting plug to obtain a handle for calling back
 *  @param[in]     pixel_access        processing order instructions
 *  @return                            -1 end; 0 on success; error > 1
 *
 *  A requested context will be stored in oyFilterNode_s::backend_data.
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/03 (Oyranos: 0.1.8)
 *  @date    2008/07/28
 */
typedef int (*oyCMMFilterPlug_Run_f) ( oyFilterPlug_s    * plug,
                                       oyPixelAccess_s   * pixel_access );

