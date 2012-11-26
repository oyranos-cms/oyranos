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
