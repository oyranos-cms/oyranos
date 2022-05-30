/** @file oyCMMapi_s.h

   [Template file inheritance graph]
   +-> oyCMMapi_s.template.h
   |
   +-> Base_s.h
   |
   +-- oyStruct_s.template.h

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */



#ifndef OY_CMM_API_S_H
#define OY_CMM_API_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyCMMapi_s oyCMMapi_s;


#include "oyStruct_s.h"


/* Include "CMMapi.public.h" { */
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


/* } Include "CMMapi.public.h" */


/* Include "CMMapi.dox" { */
/** @struct  oyCMMapi_s
 *  @ingroup module_api
 *  @extends oyStruct_s
 *  @brief   the basic API interface
 *  
 *  The registration should provide keywords for selection.
 *  The api5_ member is missed for oyCMMapi5_s.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/16 (Oyranos: 0.1.10)
 *  @date    2009/01/16
 */

/* } Include "CMMapi.dox" */

struct oyCMMapi_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};


OYAPI oyCMMapi_s* OYEXPORT
  oyCMMapi_New( oyObject_s object );
OYAPI oyCMMapi_s* OYEXPORT
  oyCMMapi_Copy( oyCMMapi_s *cmmapi, oyObject_s obj );
OYAPI int OYEXPORT
  oyCMMapi_Release( oyCMMapi_s **cmmapi );



/* Include "CMMapi.public_methods_declarations.h" { */
void               oyCMMapi_Set      ( oyCMMapi_s        * api,
                                       oyCMMInit_f         init,
                                       oyCMMReset_f        reset,
                                       oyCMMMessageFuncSet_f msg_set,
                                       const char        * registration,
                                       int32_t             version[3],
                                       int32_t             module_api[3] );
void               oyCMMapi_SetNext  ( oyCMMapi_s        * api,
                                       oyCMMapi_s        * next );
oyCMMapi_s *       oyCMMapi_GetNext  ( oyCMMapi_s        * api );
oyCMMInit_f        oyCMMapi_GetInitF ( oyCMMapi_s        * api );
oyCMMReset_f       oyCMMapi_GetResetF( oyCMMapi_s        * api );
oyCMMMessageFuncSet_f
                   oyCMMapi_GetMessageFuncSetF
                                     ( oyCMMapi_s        * api );
const char *       oyCMMapi_GetRegistration
                                     ( oyCMMapi_s        * api );

/* } Include "CMMapi.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CMM_API_S_H */
