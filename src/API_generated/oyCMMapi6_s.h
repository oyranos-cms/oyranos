/** @file oyCMMapi6_s.h

   [Template file inheritance graph]
   +-> oyCMMapi6_s.template.h
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



#ifndef OY_CMM_API6_S_H
#define OY_CMM_API6_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyCMMapi6_s oyCMMapi6_s;


#include "oyStruct_s.h"


/* Include "CMMapi6.public.h" { */
#include "oyFilterNode_s.h"

/**
 *  typedef oyModuleData_Convert_f
 *  @brief   convert between data formats
 *  @ingroup module_api
 *  @memberof oyCMMapi6_s
 *
 *  The function might be used to provide a module specific context.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/27 (Oyranos: 0.1.10)
 *  @date    2008/12/27
 */
typedef int(*oyModuleData_Convert_f) ( oyPointer_s       * data_in,
                                       oyPointer_s       * data_out,
                                       oyFilterNode_s    * node );

/* } Include "CMMapi6.public.h" */


/* Include "CMMapi6.dox" { */
/** @struct  oyCMMapi6_s
 *  @ingroup module_api
 *  @extends oyCMMapiFilter_s
 *  @brief   Context convertor
 *  @internal
 *  
 *  The context provided by a filter can be exotic. The API provides the means
 *  to get him into a known format. With this format connector it is possible to
 *  interface otherwise divergine formats.
 *
 \dot
digraph G {
  rankdir=LR
  node [shape=record, fontname=Helvetica, fontsize=10];
  edge [fontname=Helvetica, fontsize=10];

  a [ label="ICC device link - oyDL" ];
  b [ label="littleCMS specific device link - lcCC" ];

  a -> b [arrowtail=none, arrowhead=normal];
}
 \enddot
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/15 (Oyranos: 0.1.9)
 *  @date    2008/12/27
 */

/* } Include "CMMapi6.dox" */

struct oyCMMapi6_s {
/* Include "Struct.members.h" { */
oyOBJECT_e           type_;          /**< @private struct type */
oyStruct_Copy_f      copy;           /**< copy function */
oyStruct_Release_f   release;        /**< release function */
oyObject_s           oy_;            /**< @private features name and hash */

/* } Include "Struct.members.h" */
};


OYAPI oyCMMapi6_s* OYEXPORT
  oyCMMapi6_New( oyObject_s object );
OYAPI oyCMMapi6_s* OYEXPORT
  oyCMMapi6_Copy( oyCMMapi6_s *cmmapi6, oyObject_s obj );
OYAPI int OYEXPORT
  oyCMMapi6_Release( oyCMMapi6_s **cmmapi6 );



/* Include "CMMapi6.public_methods_declarations.h" { */
const char *       oyCMMapi6_GetDataType
                                     ( oyCMMapi6_s       * api,
                                       int                 out_direction );
oyModuleData_Convert_f
                   oyCMMapi6_GetConvertF
                                     ( oyCMMapi6_s       * api );

/* } Include "CMMapi6.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CMM_API6_S_H */
