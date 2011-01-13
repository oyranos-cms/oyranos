/** typedef oyCMMFilterScan_f
 *  @brief   load a filter from a in memory data blob
 *  @ingroup module_api
 *  @memberof oyCMMapi5_s
 *
 *  @param[in]     data                filter data blob
 *  @param[in]     size                data size
 *  @param[in]     file_name           the filter file for information or zero
 *  @param[in]     type                filter type
 *  @param[in]     num                 number of filter
 *  @param[out]    registration        filter registration string
 *  @param[out]    name                filter name
 *  @param[in]     allocateFunc        e.g. malloc
 *  @param[out]    info                oyCMMInfo_s pointer to set
 *  @param[in]     object              e.g. Oyranos object
 *  @return                            0 on success; error >= 1; -1 not found; unknown < -1
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/22 (Oyranos: 0.1.9)
 *  @date    2008/12/17
 */
typedef int          (*oyCMMFilterScan_f) (
                                       oyPointer           data,
                                       size_t              size,
                                       const char        * file_name,
                                       oyOBJECT_e          type,
                                       int                 num,
                                       char             ** registration,
                                       char             ** name,
                                       oyAlloc_f           allocateFunc,
                                       oyCMMInfo_s      ** info,
                                       oyObject_s          object );

/** typedef oyCMMFilterLoad_f
 *  @brief   load a filter from a in memory data blob
 *  @ingroup module_api
 *  @memberof oyCMMapi5_s
 *
 *  @param[in]     data                data blob
 *  @param[in]     size                data size
 *  @return                            filter
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/11/22 (Oyranos: 0.1.9)
 *  @date    2008/12/28
 */
typedef oyCMMapiFilter_s * (*oyCMMFilterLoad_f) (
                                       oyPointer           data,
                                       size_t              size,
                                       const char        * file_name,
                                       oyOBJECT_e          type,
                                       int                 num );

/** typedef  oyCMMFilterSocket_MatchPlugIn_f
 *  @brief   verify connectors matching each other
 *  @ingroup module_api
 *  @memberof oyCMMapi5_s
 *
 *  A implementation for images is included in the core function
 *  oyFilterSocket_MatchImagePlug().
 *
 *  @param         socket              a filter socket
 *  @param         plug                a filter plug
 *  @return                            1 on success, otherwise 0
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/04/20 (Oyranos: 0.1.10)
 *  @date    2009/04/20
 */
typedef int          (*oyCMMFilterSocket_MatchPlug_f) (
                                       oyFilterSocket_s  * socket,
                                       oyFilterPlug_s    * plug );

