/** @file oyFilterSocket_s.h

   [Template file inheritance graph]
   +-> oyFilterSocket_s.template.h
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



#ifndef OY_FILTER_SOCKET_S_H
#define OY_FILTER_SOCKET_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyFilterSocket_s oyFilterSocket_s;



#include "oyStruct_s.h"

#include "oyranos_conversion.h"
#include "oyranos_module.h"
#include "oyConnector_s.h"
#include "oyFilterNode_s.h"
#include "oyFilterPlug_s.h"
#include "oyFilterPlugs_s.h"


/* Include "FilterSocket.public.h" { */

/* } Include "FilterSocket.public.h" */


/* Include "FilterSocket.dox" { */
/** @struct  oyFilterSocket_s
 *  @ingroup objects_conversion
 *  @extends oyStruct_s
 *  @brief   A filter connection structure
 *  
 * The passive output version of a oyConnector_s.
 \dot
digraph G {
  bgcolor="transparent";
  node[ shape=plaintext, fontname=Helvetica, fontsize=10 ];
  edge[ fontname=Helvetica, fontsize=10 ];
  rankdir=LR
  a [label=<
<table border="0" cellborder="1" cellspacing="4">
  <tr> <td>Filter A</td>
      <td bgcolor="red" width="10" port="s"> socket </td>
  </tr>
</table>>
  ]
  b [URL="structoyFilterPlug__s.html", label=< 
<table border="0" cellborder="1" cellspacing="4">
  <tr><td bgcolor="lightblue" width="10" port="p"> plug </td>
      <td>Filter B</td>
  </tr>
</table>>
  ]
  subgraph { rank=min a }

  b:p->a:s [arrowtail=crow, arrowhead=box];
} 
 \enddot
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/29 (Oyranos: 0.1.8)
 *  @date    2008/07/29
 */

/* } Include "FilterSocket.dox" */

struct oyFilterSocket_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};


OYAPI oyFilterSocket_s* OYEXPORT
  oyFilterSocket_New( oyObject_s object );
OYAPI oyFilterSocket_s* OYEXPORT
  oyFilterSocket_Copy( oyFilterSocket_s *filtersocket, oyObject_s obj );
OYAPI int OYEXPORT
  oyFilterSocket_Release( oyFilterSocket_s **filtersocket );



/* Include "FilterSocket.public_methods_declarations.h" { */
OYAPI int  OYEXPORT
                 oyFilterSocket_Callback (
                                       oyFilterPlug_s    * c,
                                       oyCONNECTOR_EVENT_e e );
OYAPI int  OYEXPORT
                 oyFilterSocket_SignalToGraph (
                                       oyFilterSocket_s  * c,
                                       oyCONNECTOR_EVENT_e e );
OYAPI oyFilterNode_s * OYEXPORT
             oyFilterSocket_GetNode  ( oyFilterSocket_s  * c );
OYAPI oyStruct_s * OYEXPORT
             oyFilterSocket_GetData  ( oyFilterSocket_s  * c );
OYAPI int OYEXPORT
             oyFilterSocket_SetData  ( oyFilterSocket_s  * socket,
                                       oyStruct_s        * data );
OYAPI oyFilterPlugs_s * OYEXPORT
             oyFilterSocket_GetPlugs ( oyFilterSocket_s  * c );
OYAPI oyConnector_s * OYEXPORT
             oyFilterSocket_GetPattern(oyFilterSocket_s  * c );
OYAPI const char * OYEXPORT
             oyFilterSocket_GetRelatives (
                                       oyFilterSocket_s  * c );
OYAPI int  OYEXPORT
             oyFilterSocket_MatchImagingPlug (
                                       oyFilterSocket_s  * socket,
                                       oyFilterPlug_s    * plug );

/* } Include "FilterSocket.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_FILTER_SOCKET_S_H */
