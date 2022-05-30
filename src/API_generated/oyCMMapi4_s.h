/** @file oyCMMapi4_s.h

   [Template file inheritance graph]
   +-> oyCMMapi4_s.template.h
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



#ifndef OY_CMM_API4_S_H
#define OY_CMM_API4_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyCMMapi4_s oyCMMapi4_s;



#include "oyStruct_s.h"

#include "oyPointer_s.h"


/* Include "CMMapi4.public.h" { */
#include "oyCMMui_s.h"
#include "oyFilterNode_s.h"

/* } Include "CMMapi4.public.h" */


/* Include "CMMapi4.dox" { */
/** @struct  oyCMMapi4_s
 *  @ingroup module_api
 *  @extends oyCMMapiFilter_s
 *  @brief   Context creator
 *  
 *  The structure contains functions to specify and prepare parameters or
 *  a context dedicated for a filter node class. UI elements can be included for
 *  manual settings. As per definition the result of this filter is serialised,
 *  and can therefore be cached by Oyranos' core.
 *
 *  Different filters have to provide this struct each one per filter.
 *
 *  The oyCMMFilterNode_ContextToMem @see oyCMMFilterNode_ContextToMem_f
 *  should be implemented in case the context_type is set to a
 *  context data type.
 *
 *  @todo in commit 5a2b7396 the connection descriptors where moved to 
 *  oyCMMapi7_s. For a context, which must be created from the processing data
 *  this is not relyable, as the interpolator can not be shure that the context
 *  plug-in supports the same or larger set of connectors.
 *
 *  @version Oyranos: 0.9.5
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 *  @date    2013/06/09
 */

/* } Include "CMMapi4.dox" */

struct oyCMMapi4_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};



OYAPI oyCMMapi4_s* OYEXPORT
  oyCMMapi4_New( oyObject_s object );
OYAPI oyCMMapi4_s* OYEXPORT
  oyCMMapi4_Copy( oyCMMapi4_s *cmmapi4, oyObject_s obj );
OYAPI int OYEXPORT
  oyCMMapi4_Release( oyCMMapi4_s **cmmapi4 );

OYAPI int  OYEXPORT
           oyCMMapi4_SetBackendContext          ( oyCMMapi4_s       * api,
                                       oyPointer_s       * ptr );
OYAPI oyPointer_s * OYEXPORT
           oyCMMapi4_GetBackendContext          ( oyCMMapi4_s       * api );



/* Include "CMMapi4.public_methods_declarations.h" { */
/** typedef oyCMMFilterNode_ContextToMem_f
 *  @brief   store a CMM filter context into a memory blob
 *  @ingroup module_api
 *  @memberof oyCMMapi4_s
 *
 *  The goal is to have a data blob for later reusing. It is as well used for
 *  exchange and analysis. A oyCMMapi4_s filter with context_type member set to
 *  something should implement this function and fill the data
 *  blob with the according context data for easy forwarding and
 *  on disk caching.
 *
 *  @param[in,out] node                access to the complete filter struct,
 *                                     most important to handle is the options
 *                                     and image members
 *  @param[out]    size                size in return 
 *  @param         allocateFunc        memory allocator for the returned data
 *  @return                            the CMM memory blob, preferedly ICC
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/02 (Oyranos: 0.1.8)
 *  @date    2008/07/02
 */
typedef oyPointer(*oyCMMFilterNode_ContextToMem_f) (
                                       oyFilterNode_s    * node,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc );

/** typedef oyCMMFilterNode_GetText_f
 *  @brief   describe a CMM filter context
 *  @ingroup module_api
 *  @memberof oyCMMapi4_s
 *
 *  For a oyNAME_NICK and oyNAME_NAME type argument, the function shall
 *  describe only those elements, which are relevant to the result of the
 *  context creation. The resulting string is CMM specific by intention.
 *
 *  Serialise into:
 *  - oyNAME_NICK: XML ID
 *  - oyNAME_NAME: XML
 *  - oyNAME_DESCRIPTION: ??
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/27 (Oyranos: 0.1.10)
 *  @date    2008/12/27
 */
typedef char *(*oyCMMFilterNode_GetText_f) (
                                       oyFilterNode_s    * node,
                                       oyNAME_e            type,
                                       oyAlloc_f           allocateFunc );

#include "oyCMMapi_s.h"
OYAPI oyCMMapi4_s*  OYEXPORT
                   oyCMMapi4_Create  ( oyCMMInit_f         init,
                                       oyCMMReset_f        reset,
                                       oyCMMMessageFuncSet_f msg_set,
                                       const char        * registration,
                                       int32_t             version[3],
                                       int32_t             module_api[3],
                                       const char        * context_type,
                                       oyCMMFilterNode_ContextToMem_f oyCMMFilterNode_ContextToMem,
                                       oyCMMFilterNode_GetText_f oyCMMFilterNode_GetText,
                                       oyCMMui_s         * ui,
                                       oyObject_s          object );
OYAPI oyCMMui_s*  OYEXPORT
                   oyCMMapi4_GetUi   ( oyCMMapi4_s       * object );

/* } Include "CMMapi4.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CMM_API4_S_H */
