#include "oyranos_module.h"

/**
 *  typedef oyCMMInit_f
 *  @brief   optional CMM init function
 *  @ingroup module_api
 *  @memberof oyCMMapi_s
 */
typedef int      (*oyCMMInit_f)      ( oyStruct_s        * filter );

/**
 *  typedef  oyCMMReset_f
 *  @brief   optional CMM reset function
 *  @ingroup module_api
 *  @see oyCMMInit_f
 *  @memberof oyCMMapi_s
 */
typedef int      (*oyCMMReset_f)     ( oyStruct_s        * filter );

/**
 *  typedef oyCMMMessageFuncSet_f
 *  @brief   optionaly sets a CMM message function
 *  @ingroup module_api
 *  @memberof oyCMMapi_s
 */
typedef int      (*oyCMMMessageFuncSet_f)( oyMessage_f     message_func );

/**
 *  typedef oyCMMCanHandle_f
 *  @brief   CMM feature declaration function
 *  @ingroup module_api
 *  @memberof oyCMMapi_s
 */
typedef int      (*oyCMMCanHandle_f) ( oyCMMQUERY_e        type,
                                       uint32_t            value );

