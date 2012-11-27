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

 *  Oyranos is an open source Colour Management System
 *
 *  @par Copyright:
 *            2004-2012 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @date     2012/10/08
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


/* Include "CMMapi4.public.h" { */

/* } Include "CMMapi4.public.h" */


/* Include "CMMapi4.dox" { */
/** @struct  oyCMMapi4_s
 *  @ingroup module_api
 *  @extends oyCMMapiFilter_s
 *  @brief   Context creator
 *  @internal
 *  
 *  The structure contains functions to specify and prepare parameters or
 *  a context dedicated for a filter node class. UI elements can be included for
 *  manual settings. As per definition the result of this filter is serialised,
 *  and can therefore be cached by Oyranos' core.
 *
 *  Different filters have to provide this struct each one per filter.
 *
 *  The ::oyCMMFilterNode_ContextToMem @see oyCMMFilterNode_ContextToMem_f
 *  should be implemented in case the context_type is set to a
 *  context data type.
 *
 *  @todo in commit 5a2b7396 the connection descriptors where moved to 
 *  oyCMMapi7_s. For a context, which must be created from the processing data
 *  this is not relyable, as the interpolator can not be shure that the context
 *  plug-in supports the same or larger set of connectors.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 *  @date    2009/09/06
 */

/* } Include "CMMapi4.dox" */

struct oyCMMapi4_s {
/* Include "Struct.members.h" { */
oyOBJECT_e           type_;          /**< @private struct type */
oyStruct_Copy_f      copy;           /**< copy function */
oyStruct_Release_f   release;        /**< release function */
oyObject_s           oy_;            /**< @private features name and hash */

/* } Include "Struct.members.h" */
};


OYAPI oyCMMapi4_s* OYEXPORT
  oyCMMapi4_New( oyObject_s object );
OYAPI oyCMMapi4_s* OYEXPORT
  oyCMMapi4_Copy( oyCMMapi4_s *cmmapi4, oyObject_s obj );
OYAPI int OYEXPORT
  oyCMMapi4_Release( oyCMMapi4_s **cmmapi4 );



/* Include "CMMapi4.public_methods_declarations.h" { */

/* } Include "CMMapi4.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CMM_API4_S_H */
