/** @file oyCMMapi7_s.h

   [Template file inheritance graph]
   +-> oyCMMapi7_s.template.h
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



#ifndef OY_CMM_API7_S_H
#define OY_CMM_API7_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyCMMapi7_s oyCMMapi7_s;

#include "oyFilterPlug_s.h"
#include "oyPixelAccess_s.h"



#include "oyStruct_s.h"

#include "oyPointer_s.h"



/* Include "CMMapi7.public.h" { */
/** typedef oyCMMFilterPlug_Run_f
 *  @brief   get a pixel or channel from the previous filter
 *  @ingroup module_api
 *  @memberof oyCMMapi7_s
 *
 *  You have to call oyCMMFilter_CreateContext_t or oyCMMFilter_ContextFromMem_t first.
 *  The API provides flexible pixel access and cache configuration by the
 *  passed oyPixelAccess_s object. The filters internal precalculated data
 *  are passed by the filter object.
 *
 *  @code
    while (
    error = oyCMMFilterSocket_GetNext( filter_plug, pixel_access ) == 0
    ) {}; @endcode
 *
 *  @param[in]     plug                including the CMM's private data, connector is the requesting plug to obtain a handle for calling back
 *  @param[in]     pixel_access        processing order instructions
 *  @return                            -1 end; 0 on success; error > 1
 *
 *  A requested context will be stored in oyFilterNode_s::backend_data.
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/03 (Oyranos: 0.1.8)
 *  @date    2008/07/28
 */
typedef int (*oyCMMFilterPlug_Run_f) ( oyFilterPlug_s    * plug,
                                       oyPixelAccess_s   * pixel_access );


/* } Include "CMMapi7.public.h" */


/* Include "CMMapi7.dox" { */
/** @struct  oyCMMapi7_s
 *  @ingroup module_api
 *  @extends oyCMMapiFilter_s
 *  @brief   Data processing node
 *  
 *  The structure forms a node element in a directed acyclic graph. It's 
 *  connectors allow to communicate cababilities and semantics. This node type
 *  acts mostly on data processing.
 *
 *  The node can be inserted inside oyConversion_CreateBasicPixelsFromBuffers()
 *  or used directly with oyFilterNode_NewWith() and oyFilterNode_Connect().
 *
 *  It is possible to delegate parameter or context creation to other
 *  specialised filter types.
 *  The filter context can be stored in oyFilterNode_s::backend_data if the
 *  oyCMMapi7_s::context_type is filled with a understood format hint.
 *  The registration should provide keywords to select the processing function.
 *
 *  (in CM jargon - interpolator)
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/15 (Oyranos: 0.1.9)
 *  @date    2008/12/27
 */

/* } Include "CMMapi7.dox" */

struct oyCMMapi7_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};



OYAPI oyCMMapi7_s* OYEXPORT
  oyCMMapi7_New( oyObject_s object );
OYAPI oyCMMapi7_s* OYEXPORT
  oyCMMapi7_Copy( oyCMMapi7_s *cmmapi7, oyObject_s obj );
OYAPI int OYEXPORT
  oyCMMapi7_Release( oyCMMapi7_s **cmmapi7 );

OYAPI int  OYEXPORT
           oyCMMapi7_SetBackendContext          ( oyCMMapi7_s       * api,
                                       oyPointer_s       * ptr );
OYAPI oyPointer_s * OYEXPORT
           oyCMMapi7_GetBackendContext          ( oyCMMapi7_s       * api );



/* Include "CMMapi7.public_methods_declarations.h" { */
OYAPI int OYEXPORT
             oyCMMapi7_Run           ( oyCMMapi7_s       * api7,
                                       oyFilterPlug_s *    plug,
                                       oyPixelAccess_s *   ticket );
#include "oyCMMapi_s.h"
OYAPI oyCMMapi7_s *  OYEXPORT
             oyCMMapi7_Create        ( oyCMMInit_f         init,
                                       oyCMMReset_f        reset,
                                       oyCMMMessageFuncSet_f msg_set,
                                       const char        * registration,
                                       int32_t             version[3],
                                       int32_t             module_api[3],
                                       const char        * context_type,
                                       oyCMMFilterPlug_Run_f run,
                                       oyConnector_s    ** plugs,
                                       uint32_t            plugs_n,
                                       uint32_t            plugs_last_add,
                                       oyConnector_s    ** sockets,
                                       uint32_t            sockets_n,
                                       uint32_t            sockets_last_add,
                                       const char       ** properties,
                                       oyObject_s          object );

/* } Include "CMMapi7.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CMM_API7_S_H */
