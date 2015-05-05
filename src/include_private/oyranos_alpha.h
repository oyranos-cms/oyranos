/** @file oyranos_alpha.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2004-2012 (C) Kai-Uwe Behrmann
 *
 *  @brief    object APIs
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @since    2004/11/25
 */

/** @date      25. 11. 2004 */


#ifndef OYRANOS_ALPHA_H
#define OYRANOS_ALPHA_H

#include "oyranos.h"
#include "oyranos_conversion.h"
#include "oyranos_icc.h"
#include "oyranos_image.h"
#include "oyranos_object.h"
#include "oyBlob_s.h"
#include "oyFilterCore_s.h"
#include "oyHash_s.h"
#include "oyName_s.h"
#include "oyObject_s.h"
#include "oyOptions_s.h"
#include "oyProfile_s.h"
#include "oyStruct_s.h"
#include "oyStructList_s.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define OY_IN_PLANING
#ifdef OY_IN_PLANING



/** @struct  oyCallback_s
 *  @brief   a Callback object
 *  @extends oyStruct_s
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/18 (Oyranos: 0.1.10)
 *  @date    2009/11/18
 */
typedef struct oyCallback_s {
  oyOBJECT_e           type_;          /**< struct type oyOBJECT_CALLBACK_S */ 
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  void               (*callback)();    /**< a generic callback function */
  oyPointer            data;           /**< generic data to pass to the above
                                            callback */
} oyCallback_s;

OYAPI oyCallback_s * OYEXPORT
           oyCallback_New            ( oyObject_s          object );
OYAPI oyCallback_s * OYEXPORT
           oyCallback_Copy           ( oyCallback_s      * obj,
                                       oyObject_s          object);
OYAPI int  OYEXPORT
           oyCallback_Release        ( oyCallback_s     ** obj );




/* --- color conversion --- */



/** @enum    oyDATALAYOUT_e
 *  @brief   ideas for basic data arrangements
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/00/00 (Oyranos: 0.1.8)
 *  @date    2008/00/00
 */
typedef enum oyDATALAYOUT_e {
  oyDATALAYOUT_NONE,
  oyDATALAYOUT_CURVE,                 /**< equally spaced curve, oyDATALAYOUT_e[0], size[1], min[2], max[3], elements[4]... */
  oyDATALAYOUT_MATRIX,                /**< 3x3 matrix, oyDATALAYOUT_e[0], a1[1],a2[2],a3,b1,b2,b3,c1,c2,c3 */
  oyDATALAYOUT_TABLE,                 /**< CLUT, oyDATALAYOUT_e[0], table dimensions in[1], array out[2], size of first dimension[3], size of second [4], ... size of last[n], elements[n+1]... */
  oyDATALAYOUT_ARRAY,                 /**< value array, oyDATALAYOUT_e[0], size[1], elements[2]... */
  /*oyDATALAYOUT_PICEWISE_CURVE,*/       /**< paired curve, layout as in oyDATA_LAYOUT_CURVE but with elements grouped to two */
  /*oyDATALAYOUT_HULL,*/              /**< oyDATALAYOUT_e[0], triangle count[1], indixes[2], XYZ triples[3..5]..., followed by interwoven index + normale + midpoint triples[n]... */
  /*oyDATALAYOUT_,*/                /**< */
  oyDATALAYOUT_MAX                    /**< */
} oyDATALAYOUT_e;




typedef struct oyUiHandler_s oyUiHandler_s;





/* --- CMM API --- */


char   *       oyDumpColorToCGATS   ( const double      * channels,
                                       size_t              n,
                                       oyProfile_s       * prof,
                                       oyAlloc_f           allocateFunc,
                                       const char        * DESCRIPTOR );


/**
 *  @brief   handle parser output and build the UI
 *
 *  @param[in]     cur                 libxml2 node
 *  @param[in]     collected_elements  from libxml2
 *  @param[in]     handler_context     the toolkit context
 *  @return                            ns + ':' + name
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/08/30 (Oyranos: 0.1.10)
 *  @date    2009/08/30
 */
typedef int  (*oyUiHandler_f)        ( oyPointer           cur,
                                       oyOptions_s       * collected_elements,
                                       oyPointer           handler_context );

/** @struct  oyUiHandler_s
 *  @brief   provide a list of handlers to build the UI
 *
 *  A parser will read out the XFORMS elements and collect those a UI handler
 *  claims interesst in. The handler is then called to process the collected 
 *  elements and to build the UI.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/08/30 (Oyranos: 0.1.10)
 *  @date    2009/11/10
 */
struct oyUiHandler_s {
  oyOBJECT_e           type;           /**< oyOBJECT_UI_HANDLER_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< @private features name and hash */

  char               * dialect;        /**< currently only "oyFORMS",
                                            a subset of W3C XFORMS */
  char               * parser_type;    /**< currently only "libxml2" */
  oyUiHandler_f        handler;        /**< The handler which obtains the parsed
                                            results and a context to construct
                                            the UI. */
  char               * handler_type;   /**< informational handler context type*/
  /** The elements to collect by the parser. Levels are separated by slash '/'.
   *  Alternatives are separated by a point '.' . The list is zero terminated.
   *  e.g. "xf:select1/xf:choices/xf:item/xf:label.xf:value" means, get all 
   *  "xf:select1/xf:choices/xf:item/" paths and
   *  pick there the xf:label and xf:value elements.
   */
  char              ** element_searches;
};

char *       oyXFORMsFromModelAndUi  ( const char        * data,
                                       const char        * ui_text,
                                       const char       ** namespaces,
                                       const char        * head_injection,
                                       oyAlloc_f           allocate_func );
int          oyXFORMsRenderUi        ( const char        * xforms,
                                       oyUiHandler_s    ** ui_handlers,
                                       oyPointer           user_data );



#endif /* OY_IN_PLANING */


/* --- deprecated definitions --- */

/** @deprecated */
int      oyGetMonitorInfo            ( const char        * display,
                                       char             ** manufacturer,
                                       char             ** model,
                                       char             ** serial,
                                       char             ** display_geometry,
                                       char             ** system_port,
                                       oyBlob_s         ** edit,
                                       oyAlloc_f           allocate_func );

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_ALPHA_H */
