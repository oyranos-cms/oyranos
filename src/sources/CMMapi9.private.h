/** typedef  oyConversion_Correct_f
 *  @brief   Check for correctly adhering to policies
 *  @ingroup module_api
 *  @memberof oyCMMapi9_s
 *
 *  Without any options the module shall
 *  perform graph analysis and correct the graph.
 *
 *  @see oyConversion_Correct()
 *
 *  @par Typical Options:
 *  - "command"-"help" - a string option issuing a help text as message
 *  - "verbose" - reporting changes as message
 *
 *  @param   conversion                the to be checked configuration
 *  @param[in]     flags               for inbuild defaults |
 *                                     oyOPTIONSOURCE_FILTER;
 *                                     for options marked as advanced |
 *                                     oyOPTIONATTRIBUTE_ADVANCED |
 *                                     OY_SELECT_FILTER |
 *                                     OY_SELECT_COMMON
 *  @param   options                   options to the policy module
 *  @return                            0 - indifferent, >= 1 - error
 *                                     + a message should be sent
 *
 *  @version Oyranos: 0.1.13
 *  @since   2009/07/23 (Oyranos: 0.1.10)
 *  @date    2010/11/27
 */
typedef int  (*oyConversion_Correct_f) (
                                       oyConversion_s    * conversion,
                                       uint32_t            flags,
                                       oyOptions_s       * options );


/** typedef  oyCMMGetFallback_f
 *  @brief   get pattern specific module fallback
 *  @ingroup module_api
 *  @memberof oyCMMapi9_s
 *
 *  @param[in]     node                the node context
 *  @param[in]     flags               unused
 *  @param[in]     select_core         apply to api4 core else to api7
 *  @param[in]     allocate_func       user supplied memory allocator
 *  @return                            the fallback string
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/06/25
 *  @since   2014/06/25 (Oyranos: 0.9.6)
 */
typedef char * (*oyCMMGetFallback_f) ( oyFilterNode_s    * node,
                                       uint32_t            flags,
                                       int                 select_core,
                                       oyAlloc_f           allocate_func );

/** typedef  oyCMMRegistrationToName_f
 *  @brief   get names from a module registration
 *  @ingroup module_api
 *  @memberof oyCMMapi9_s
 *
 *  @param[in]     node                the node context
 *  @param[in]     name_type           oyNAME_e, oyNAME_PATTERN
 *  @param[in]     flags               unused
 *  @param[in]     select_core         apply to api4 core else to api7
 *  @param[in]     allocate_func       user supplied memory allocator
 *  @return                            the fallback string
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/06/29
 *  @since   2014/06/29 (Oyranos: 0.9.6)
 */
typedef char * (*oyCMMRegistrationToName_f) (
                                       const char        * registration,
                                       int                 name_type,
                                       uint32_t            flags,
                                       int                 select_core,
                                       oyAlloc_f           allocate_func );

/** typedef  oyCMMGetDefaultPattern_f
 *  @brief   get module default
 *  @ingroup module_api
 *  @memberof oyCMMapi9_s
 *
 *  @param[in]     pattern             the basic pattern
 *  @param[in]     flags               unused
 *  @param[in]     select_core         apply to api4 core else to api7
 *  @param[in]     allocate_func       user supplied memory allocator
 *  @return                            the default pattern string
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/07/21
 *  @since   2014/07/21 (Oyranos: 0.9.6)
 */
typedef char * (*oyCMMGetDefaultPattern_f) (
                                       const char        * base_pattern,
                                       uint32_t            flags,
                                       int                 select_core,
                                       oyAlloc_f           allocate_func );
