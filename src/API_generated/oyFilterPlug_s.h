/** @file oyFilterPlug_s.h

   [Template file inheritance graph]
   +-> oyFilterPlug_s.template.h
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



#ifndef OY_FILTER_PLUG_S_H
#define OY_FILTER_PLUG_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyFilterPlug_s oyFilterPlug_s;



#include "oyStruct_s.h"



/* Include "FilterPlug.public.h" { */

/* } Include "FilterPlug.public.h" */


/* Include "FilterPlug.dox" { */
/** @struct  oyFilterPlug_s
 *  @ingroup objects_conversion
 *  @extends oyStruct_s
 *  @brief   A filter connection structure
 *
 * The active input version of a oyConnector_s.
 *  Each plug can connect to exact one socket.
 \dot
digraph G {
  bgcolor="transparent";
  node[ shape=plaintext, fontname=Helvetica, fontsize=10 ];
  edge[ fontname=Helvetica, fontsize=10 ];
  rankdir=LR
  a [URL="structoyFilterSocket__s.html", label=<
<table border="0" cellborder="1" cellspacing="4">
  <tr> <td>Filter A</td>
      <td bgcolor="red" width="10" port="s"> socket </td>
  </tr>
</table>>
  ]
  b [label=< 
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

/* } Include "FilterPlug.dox" */

struct oyFilterPlug_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};


OYAPI oyFilterPlug_s* OYEXPORT
  oyFilterPlug_New( oyObject_s object );
OYAPI oyFilterPlug_s* OYEXPORT
  oyFilterPlug_Copy( oyFilterPlug_s *filterplug, oyObject_s obj );
OYAPI int OYEXPORT
  oyFilterPlug_Release( oyFilterPlug_s **filterplug );



/* Include "FilterPlug.public_methods_declarations.h" { */
#include "oyConnector_s.h"
#include "oyFilterNode_s.h"
#include "oyFilterSocket_s.h"
#include "oyPixelAccess_s.h"
#include "oyImage_s.h"
OYAPI int  OYEXPORT
                 oyFilterPlug_Callback(
                                       oyFilterPlug_s    * c,
                                       oyCONNECTOR_EVENT_e e );
OYAPI int  OYEXPORT
                 oyFilterPlug_ConnectIntoSocket (
                                       oyFilterPlug_s    * p,
                                       oyFilterSocket_s  * s );
OYAPI oyImage_s * OYEXPORT
             oyFilterPlug_ResolveImage(oyFilterPlug_s    * plug,
                                       oyFilterSocket_s  * socket,
                                       oyPixelAccess_s   * ticket );
OYAPI oyFilterNode_s * OYEXPORT
             oyFilterPlug_GetNode    ( oyFilterPlug_s    * p );
OYAPI oyFilterNode_s * OYEXPORT
             oyFilterPlug_GetRemoteNode
                                     ( oyFilterPlug_s    * p );
OYAPI oyFilterSocket_s * OYEXPORT
             oyFilterPlug_GetSocket  ( oyFilterPlug_s    * p );
OYAPI oyConnector_s * OYEXPORT
             oyFilterPlug_GetPattern ( oyFilterPlug_s    * p );
OYAPI const char * OYEXPORT
             oyFilterPlug_GetRelatives(oyFilterPlug_s    * p );

/* } Include "FilterPlug.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_FILTER_PLUG_S_H */
