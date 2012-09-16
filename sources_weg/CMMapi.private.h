/** typedef   oyCMMOptions_Check_f
 *  @brief    a function to check options
 *  @ingroup  module_api
 *  @memberof oyCMMapi_s
 *
 *  @param[in]     options             the options
 *  @return                            0 - good, 1 - bad
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/07/15 (Oyranos: 0.1.10)
 *  @date    2009/07/15
 */
typedef int  (*oyCMMOptions_Check_f) ( oyOptions_s       * validate );

/**
 *  typedef oyCMMCanHandle_f
 *  @brief   CMM feature declaration function
 *  @ingroup module_api
 *  @memberof oyCMMapi_s
 */
typedef int      (*oyCMMCanHandle_f) ( oyCMMQUERY_e        type,
                                       uint32_t            value );

