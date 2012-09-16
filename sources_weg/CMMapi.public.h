/**
 *  typedef oyCMMInit_f
 *  @brief   optional CMM init function
 *  @ingroup module_api
 *  @memberof oyCMMapi_s
 */
typedef int      (*oyCMMInit_f)      ( oyStruct_s        * filter );

/**
 *  typedef oyCMMMessageFuncSet_f
 *  @brief   optionaly sets a CMM message function
 *  @ingroup module_api
 *  @memberof oyCMMapi_s
 */
typedef int      (*oyCMMMessageFuncSet_f)( oyMessage_f     message_func );

