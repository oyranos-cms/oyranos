#include "oyranos_module.h"

/**
 *  typedef oyCMMinfoInit_f
 *  @brief   optional module init function
 *  @ingroup module_api
 *  @memberof oyCMMinfo_s
 */
typedef int      (*oyCMMinfoInit_f)  ( oyStruct_s        * library );

/**
 *  typedef oyCMMinfoReset_f
 *  @brief   optional module reset function
 *  @ingroup module_api
 *  @memberof oyCMMinfo_s
 *  @see oyCMMinfoInit_f
 */
typedef int      (*oyCMMinfoReset_f) ( oyStruct_s        * library );

