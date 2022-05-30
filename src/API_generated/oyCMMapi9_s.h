/** @file oyCMMapi9_s.h

   [Template file inheritance graph]
   +-> oyCMMapi9_s.template.h
   |
   +-> oyCMMapiFilter_s.template.h
   |
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



#ifndef OY_CMM_API9_S_H
#define OY_CMM_API9_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyCMMapi9_s oyCMMapi9_s;



#include "oyStruct_s.h"

#include "oyPointer_s.h"


/* Include "CMMapi9.public.h" { */
#include "oyCMMobjectType_s.h"


/* } Include "CMMapi9.public.h" */


/* Include "CMMapi9.dox" { */
/** @struct  oyCMMapi9_s
 *  @ingroup module_api
 *  @extends oyCMMapiFilter_s
 *  @brief   Graph policies and various pattern handlers
 *  @internal
 *
 *  The oyCMMapi9_s module is responsible to handle all requests for a
 *  certain filter pattern. Be that policy checking, object type registration
 *  or module specific tasks like registration name conversion or fallback
 *  resolving.
 *
 *  The user visible Oyranos settings are subject to be applied to graphs.
 *  This module type provides an interface to check and verified a graph
 *  according to arbitrary policies in a automated fashion. 
 *  The usage of graph policy modules is optional and
 *  can be controled by the front end function oyConversion_Correct().
 *
 *  The policy check can cover options and data checks. Graph analysis and 
 *  correction is possible as well.
 *
 *  Differences to native node modules are that, a policy module runs before
 *  the graph starts data processing, it can provide options for a group of 
 *  node modules including a UI and provide additional object types.
 *
 *  @version Oyranos: 0.9.6
 *  @date    2014/06/29
 *  @since   2009/07/23 (Oyranos: 0.1.10)
 */

/* } Include "CMMapi9.dox" */

struct oyCMMapi9_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};



OYAPI oyCMMapi9_s* OYEXPORT
  oyCMMapi9_New( oyObject_s object );
OYAPI oyCMMapi9_s* OYEXPORT
  oyCMMapi9_Copy( oyCMMapi9_s *cmmapi9, oyObject_s obj );
OYAPI int OYEXPORT
  oyCMMapi9_Release( oyCMMapi9_s **cmmapi9 );

OYAPI int  OYEXPORT
           oyCMMapi9_SetBackendContext          ( oyCMMapi9_s       * api,
                                       oyPointer_s       * ptr );
OYAPI oyPointer_s * OYEXPORT
           oyCMMapi9_GetBackendContext          ( oyCMMapi9_s       * api );



/* Include "CMMapi9.public_methods_declarations.h" { */
oyCMMobjectType_s **
                   oyCMMapi9_GetObjectTypes
                                     ( oyCMMapi9_s       * api );

/* } Include "CMMapi9.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CMM_API9_S_H */
