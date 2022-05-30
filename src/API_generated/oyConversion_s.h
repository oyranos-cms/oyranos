/** @file oyConversion_s.h

   [Template file inheritance graph]
   +-> oyConversion_s.template.h
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



#ifndef OY_CONVERSION_S_H
#define OY_CONVERSION_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyConversion_s oyConversion_s;



#include "oyStruct_s.h"

#include "oyranos_image.h"
#include "oyOptions_s.h"
#include "oyImage_s.h"
#include "oyProfile_s.h"
#include "oyFilterNode_s.h"
#include "oyPixelAccess_s.h"


/* Include "Conversion.public.h" { */

/* } Include "Conversion.public.h" */


/* Include "Conversion.dox" { */
/** @struct  oyConversion_s
 *  @ingroup objects_conversion
 *  @extends oyStruct_s
 *  @brief   Image Manipulation by a Graph (DAG)
 *
 *  The main and most simple entry points are oyConversion_CreateBasicPixels(),
 *  oyConversion_Correct() and oyConversion_RunPixels(). More details can be
 *  read in @ref graph_usage.
 *
 *  Order of filters matters.
 *  The processing direction is a bit like raytracing as nodes request their
 *  parent.
 *
 *  The graph is allowed to be a directed graph without cycles.
 \dot
digraph G {
  bgcolor="transparent";
  rankdir=LR
  graph [fontname=Helvetica, fontsize=12];
  node [shape=record, fontname=Helvetica, fontsize=10, style="filled,rounded"];
  edge [fontname=Helvetica, fontsize=10];

  a [ label="{<plug> 0| Filter Node 1 == Input |<socket>}"];
  b [ label="{<plug> 1| Filter Node 2 |<socket>}"];
  c [ label="{<plug> 1| Filter Node 3 |<socket>}"];
  d [ label="{<plug> 2| Filter Node 4 == Output |<socket>}"];

  subgraph cluster_0 {
    label="Oyranos Filter Graph";
    color=gray;

    a:socket -> b:plug [arrowtail=normal, arrowhead=none, label=request];
    b:socket -> d:plug [arrowtail=normal, arrowhead=none, label=request];
    a:socket -> c:plug [arrowtail=normal, arrowhead=none, label=request];
    c:socket -> d:plug [arrowtail=normal, arrowhead=none, label=request];
  }
}
 \enddot
 *  oyConversion_s shall provide access to the graph and help in processing
 *  and managing nodes.\n
 \dot
digraph G {
  bgcolor="transparent";
  rankdir=LR
  graph [fontname=Helvetica, fontsize=12];
  node [shape=record, fontname=Helvetica, fontsize=10, style="rounded"];
  edge [fontname=Helvetica, fontsize=10];

  conversion [shape=plaintext, label=<
<table border="0" cellborder="1" cellspacing="0" bgcolor="lightgray">
  <tr><td>oyConversion_s</td></tr>
  <tr><td>
     <table border="0" cellborder="0" align="left">
       <tr><td align="left">...</td></tr>
       <tr><td align="left" port="in">+input</td></tr>
       <tr><td align="left" port="out">+out_</td></tr>
       <tr><td align="left">...</td></tr>
     </table>
     </td></tr>
  <tr><td> </td></tr>
</table>>,
                    style=""];

  a [ label="{<plug> 0| Filter Node 1 == Input |<socket>}"];
  b [ label="{<plug> 1| Filter Node 2 |<socket>}"];
  c [ label="{<plug> 1| Filter Node 3 |<socket>}"];
  d [ label="{<plug> 2| Filter Node 4 == Output |<socket>}"];

  subgraph cluster_0 {
    label="oyConversion_s with attached Filter Graph";
    color=gray;

    a:socket -> b:plug [arrowtail=normal];
    b:socket -> d:plug [arrowtail=normal];
    a:socket -> c:plug [arrowtail=normal];
    c:socket -> d:plug [arrowtail=normal];

    conversion:in -> a;
    conversion:out -> d;
  }

  conversion
}
 \enddot
 * @section graph_usage Graph Live Cycle
    \b Creating \b Graphs: \n
 *  Most simple is to use the oyConversion_CreateBasicPixels() function to 
 *  create a profile to profile and possible image buffer to image buffer linear
 *  graph.\n
 *  The other possibility is to create a non linear graph. The input member can
 *  be accessed for this directly.
 *
 *  While it would be possible to have several open ends in a graph, there
 *  are two endpoints considered as special. The input member prepresents the
 *  top most required node to be provided in a oyConversion_s graph. The
 *  input node is accessible for user manipulation. The other one is the out_
 *  member. It is the closing node in the graph. It will be set by Oyranos
 *  during closing the graph.
 *
 *  \b Configuring \b Graphs: \n
 *  Image graphs for displayable result should in most cases be synced with
 *  user and system settings. Image graphs are typically used in three categories:
 *  - desktop widgets, web browsers, file managers, text editors: oyConversion_Correct( flags=0 )
 *  - editing applications, image viewers: oyConversion_Correct( flags=@ref oyOPTIONATTRIBUTE_ADVANCED )
 *  - command line tools without displaying: just as is, most likely skip oyConversion_Correct()
 *
 *  \b Using \b Graphs: \n
 *  To obtain the data the oyConversion_RunPixels() and
 *  oyConversion_GetOnePixel() functions are available.
 \dot
digraph G {
  bgcolor="transparent";
  rankdir=LR
  graph [fontname=Helvetica, fontsize=12];
  node [shape=record, fontname=Helvetica, fontsize=10, style="rounded"];
  edge [fontname=Helvetica, fontsize=12];

  a [ label="{<plug> 0| Filter Node 1 == Input |<socket>}"];
  b [ label="{<plug> 1| Filter Node 2 |<socket>}"];
  c [ label="{<plug> 1| Filter Node 3 |<socket>}"];
  d [ label="{<plug> 2| Filter Node 4 == Output |<socket>}"];
  app [ label="application", style=filled ]

  subgraph cluster_0 {
    label="Data Flow";
    color=gray;
    a:socket -> b:plug [label=data];
    b:socket -> d:plug [label=data];
    a:socket -> c:plug [label=data];
    c:socket -> d:plug [label=data];
    d:socket -> app [label=<<table  border="0" cellborder="0"><tr><td>return of<br/>oyConversion_RunPixels()</td></tr></table>>];
  }
}
 \enddot
 *
 *  \b Releasing \b Graphs: \n
 *  Giving memory back is simple as oyConversion_Release()
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/08 (Oyranos: 0.1.8)
 *  @date    2008/06/08
 */

/* } Include "Conversion.dox" */

struct oyConversion_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};


OYAPI oyConversion_s* OYEXPORT
  oyConversion_New( oyObject_s object );
OYAPI oyConversion_s* OYEXPORT
  oyConversion_Copy( oyConversion_s *conversion, oyObject_s obj );
OYAPI int OYEXPORT
  oyConversion_Release( oyConversion_s **conversion );



/* Include "Conversion.public_methods_declarations.h" { */
OYAPI int  OYEXPORT
                 oyConversion_Correct (
                                       oyConversion_s    * conversion,
                                       const char        * registration,
                                       uint32_t            flags,
                                       oyOptions_s       * options );
OYAPI oyConversion_s *  OYEXPORT
                oyConversion_CreateBasicPixels (
                                       oyImage_s         * input,
                                       oyImage_s         * output,
                                       oyOptions_s       * options,
                                       oyObject_s          object );
OYAPI oyConversion_s *  OYEXPORT
                oyConversion_CreateBasicPixelsFromBuffers (
                                       oyProfile_s       * p_in,
                                       oyPointer           buf_in,
                                       oyPixel_t           buf_type_in,
                                       oyProfile_s       * p_out,
                                       oyPointer           buf_out,
                                       oyPixel_t           buf_type_out,
                                       oyOptions_s       * options,
                                       int                 count );
OYAPI oyConversion_s *  OYEXPORT
                oyConversion_CreateFromImage (
                                       oyImage_s         * image_in,
                                       oyOptions_s       * module_options,
                                       oyProfile_s       * output_profile,
                                       oyDATATYPE_e        buf_type_out,
                                       uint32_t            flags,
                                       oyObject_s          obj );
OYAPI oyFilterGraph_s *  OYEXPORT
                oyConversion_GetGraph (
                                       oyConversion_s    * conversion );
OYAPI oyImage_s *  OYEXPORT
                oyConversion_GetImage (
                                       oyConversion_s    * conversion,
                                       uint32_t            flags );
OYAPI oyFilterNode_s *  OYEXPORT
                oyConversion_GetNode (
                                       oyConversion_s    * conversion,
                                       uint32_t            flags );
OYAPI int  OYEXPORT
                 oyConversion_GetOnePixel (
                                       oyConversion_s    * conversion,
                                       double              x,
                                       double              y,
                                       oyPixelAccess_s   * pixel_access );
OYAPI int  OYEXPORT
                 oyConversion_RunPixels (
                                       oyConversion_s    * conversion,
                                       oyPixelAccess_s   * pixel_access );
OYAPI int  OYEXPORT
                 oyConversion_Set  ( oyConversion_s    * conversion,
                                       oyFilterNode_s    * input,
                                       oyFilterNode_s    * output );
OYAPI char *  OYEXPORT
                oyConversion_ToText (
                                       oyConversion_s    * conversion,
                                       const char        * head_line,
                                       int                 reserved,
                                       oyAlloc_f           allocateFunc );

/* } Include "Conversion.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CONVERSION_S_H */
