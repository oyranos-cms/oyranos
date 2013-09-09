/** @file oyCMMapi10_s.h

   [Template file inheritance graph]
   +-> oyCMMapi10_s.template.h
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
 *            2004-2012 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @date     2012/10/08
 */



#ifndef OY_CMM_API10_S_H
#define OY_CMM_API10_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyCMMapi10_s oyCMMapi10_s;


#include "oyStruct_s.h"


/* Include "CMMapi10.public.h" { */

/* } Include "CMMapi10.public.h" */


/* Include "CMMapi10.dox" { */
/** @struct  oyCMMapi10_s
 *  @ingroup module_api
 *  @extends oyCMMapiFilter_s
 *  @brief   Generic data exchange
 *  @internal
 *  
 *  This API provides a very generic interface to exchange data between Oyranos
 *  core and a module.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/11 (Oyranos: 0.1.10)
 *  @date    2009/12/11
 */

/* } Include "CMMapi10.dox" */

struct oyCMMapi10_s {
/* Include "Struct.members.h" { */
oyOBJECT_e           type_;          /**< @private struct type */
oyStruct_Copy_f      copy;           /**< copy function */
oyStruct_Release_f   release;        /**< release function */
oyObject_s           oy_;            /**< @private features name and hash */

/* } Include "Struct.members.h" */
};


OYAPI oyCMMapi10_s* OYEXPORT
  oyCMMapi10_New( oyObject_s object );
OYAPI oyCMMapi10_s* OYEXPORT
  oyCMMapi10_Copy( oyCMMapi10_s *cmmapi10, oyObject_s obj );
OYAPI int OYEXPORT
  oyCMMapi10_Release( oyCMMapi10_s **cmmapi10 );



/* Include "CMMapi10.public_methods_declarations.h" { */

/* } Include "CMMapi10.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CMM_API10_S_H */
