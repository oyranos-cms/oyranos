/** typedef  oyConfigs_FromPattern_f
 *  @brief   return available configurations
 *  @ingroup module_api
 *  @memberof oyCMMapi8_s
 *
 *  @param[in]     registration        a selection string marked with "///config"
 *  @param[in]     options             the complete options from the user
 *  @param[out]    configs             the returned configurations
 *  @return                            0 - good, 1 <= error, -1 >= unknown issue, look for messages
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/18 (Oyranos: 0.1.10)
 *  @date    2009/01/18
 */
typedef int      (*oyConfigs_FromPattern_f) (
                                       const char        * registration,
                                       oyOptions_s       * options,
                                       oyConfigs_s      ** configs );

/** typedef  oyConfigs_Modify_f
 *  @brief   enrich/manipulate existing configurations
 *  @ingroup module_api
 *  @memberof oyCMMapi8_s
 *
 *  @param[in,out] configs             the configurations
 *  @param[in]     options             the complete options from the user
 *  @return                            0 - good, 1 <= error, -1 >= unknown issue, look for messages
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/08/21 (Oyranos: 0.1.10)
 *  @date    2009/08/21
 */
typedef int      (*oyConfigs_Modify_f)( oyConfigs_s       * configs,
                                        oyOptions_s       * options );

/** typedef  oyConfig_Rank_f
 *  @brief   check for correctness
 *  @ingroup module_api
 *  @memberof oyCMMapi8_s
 *
 *  The function is called in Oyranos' core to give a module a hook to check  
 *  and accept a device. If the module does not find its previously set  
 *  handles and can neigther open the device from the device_name then
 *  chances are good that it can't handle and should return zero otherwise    
 *  one. Oyranos will then try an other module with this device.
 *
 *  @param   config                    the to be checked configuration
 *  @return                            - 0 - indifferent,
 *                                     - >= 1 - rank,
 *                                     - <= -1 error + a message should be sent
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/16 (Oyranos: 0.1.10)
 *  @date    2009/01/26
 */
typedef int  (*oyConfig_Rank_f)     ( oyConfig_s         * config );
