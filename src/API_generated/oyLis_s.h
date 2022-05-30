/** @file oyLis_s.h

   [Template file inheritance graph]
   +-> oyLis_s.template.h
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



#ifndef OY_LIS_S_H
#define OY_LIS_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyLis_s oyLis_s;


#include "oyStruct_s.h"


/* Include "Lis.public.h" { */

/* } Include "Lis.public.h" */


/* Include "Lis.dox" { */
/** @struct oyList_s
 *  @ingroup objects_generic
 *  @extends oyStruct_s
 *  @brief   a pointer list
 *  @internal
 *  
 *  @version Oyranos: x.x.x
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */

/* } Include "Lis.dox" */

struct oyLis_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};


OYAPI oyLis_s* OYEXPORT
  oyLis_New( oyObject_s object );
OYAPI oyLis_s* OYEXPORT
  oyLis_Copy( oyLis_s *lis, oyObject_s obj );
OYAPI int OYEXPORT
  oyLis_Release( oyLis_s **lis );



/* Include "Lis.public_methods_declarations.h" { */

/* } Include "Lis.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_LIS_S_H */
