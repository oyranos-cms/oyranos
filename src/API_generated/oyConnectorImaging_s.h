/** @file oyConnectorImaging_s.h

   [Template file inheritance graph]
   +-> oyConnectorImaging_s.template.h
   |
   +-> oyConnector_s.template.h
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



#ifndef OY_CONNECTOR_IMAGING_S_H
#define OY_CONNECTOR_IMAGING_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyConnectorImaging_s oyConnectorImaging_s;




#include "oyStruct_s.h"

#include "oyConnector_s.h"

#include "oyranos_image.h"


/* Include "ConnectorImaging.public.h" { */

/* } Include "ConnectorImaging.public.h" */


/* Include "ConnectorImaging.dox" { */
/** @struct  oyConnectorImaging_s
 *  @ingroup objects_conversion
 *  @extends oyConnector_s
 *  @brief   Node connection descriptor
 *
 *  This structure holds informations about the connection capabilities.
 *  It holds common structure members of oyFilterPlug_s and oyFilterSocket_s.
 *
 *  To signal a value is not initialised or does not apply, set the according
 *  integer value to -1.
 *
 *  @todo generalise the connector properties
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/07/26 (Oyranos: 0.1.8)
 *  @date    2010/06/25
 */

/* } Include "ConnectorImaging.dox" */

struct oyConnectorImaging_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};


OYAPI oyConnectorImaging_s* OYEXPORT
  oyConnectorImaging_New( oyObject_s object );
OYAPI oyConnectorImaging_s* OYEXPORT
  oyConnectorImaging_Copy( oyConnectorImaging_s *connectorimaging, oyObject_s obj );
OYAPI int OYEXPORT
  oyConnectorImaging_Release( oyConnectorImaging_s **connectorimaging );




/* Include "ConnectorImaging.public_methods_declarations.h" { */
OYAPI int  OYEXPORT
                 oyConnectorImaging_SetDataTypes (
                                       oyConnectorImaging_s  * c,
                                       oyDATATYPE_e          * data_types,
                                       int                     data_types_n );
OYAPI int  OYEXPORT
                 oyConnectorImaging_GetDataTypes (
                                       oyConnectorImaging_s  * c,
                                       const oyDATATYPE_e   ** data_types,
                                       int                   * data_types_n );
OYAPI int  OYEXPORT
                 oyConnectorImaging_GetChannelTypes (
                                       oyConnectorImaging_s  * c,
                                       const oyCHANNELTYPE_e** channel_types,
                                       int                   * channel_types_n );

/** @enum    oyCONNECTOR_IMAGING_CAP_e
 *  @brief   Capabilities of oyConnectorImaging_s
 *
 *  @version Oyranos: 0.5.0
 *  @since   2012/09/06 (Oyranos: 0.5.0)
 *  @date    2012/09/06
 */
typedef enum {
  oyCONNECTOR_IMAGING_CAP_IS_PLUG,           /**< is not a socket */
  oyCONNECTOR_IMAGING_CAP_MAX_COLOR_OFFSET,  /**< offset to first color sample */
  oyCONNECTOR_IMAGING_CAP_MIN_CHANNELS_COUNT,/**< minimal possible channels */
  oyCONNECTOR_IMAGING_CAP_MAX_CHANNELS_COUNT,/**< maximal count of possible channels */
  oyCONNECTOR_IMAGING_CAP_MIN_COLOR_COUNT,   /**< minimal count of possible color channels */
  oyCONNECTOR_IMAGING_CAP_MAX_COLOR_COUNT,   /**< maximal count of possible color channels */
  oyCONNECTOR_IMAGING_CAP_CAN_PLANAR,        /**< can read separated channels */
  oyCONNECTOR_IMAGING_CAP_CAN_INTERWOVEN,    /**< can read continuous channels */
  oyCONNECTOR_IMAGING_CAP_CAN_SWAP,          /**< can swap color channels (BGR)*/
  oyCONNECTOR_IMAGING_CAP_CAN_SWAP_BYTES,    /**< non host byte order */
  oyCONNECTOR_IMAGING_CAP_CAN_REVERT,        /**< revert 1 -> 0 and 0 -> 1 */
  oyCONNECTOR_IMAGING_CAP_CAN_PREMULTIPLIED_ALPHA, /**< can handle premultiplied alpha channels */
  oyCONNECTOR_IMAGING_CAP_CAN_NONPREMULTIPLIED_ALPHA, /**< can handle non premultiplied alpha channels */
  oyCONNECTOR_IMAGING_CAP_CAN_SUBPIXEL,      /**< understand subpixel order */
  oyCONNECTOR_IMAGING_CAP_ID,                /**< relative to oyFilterCore_s, e.g. 1*/
  /**< connector is mandatory or optional, important for modules */
  oyCONNECTOR_IMAGING_CAP_IS_MANDATORY
} oyCONNECTOR_IMAGING_CAP_e;
OYAPI int  OYEXPORT
                 oyConnectorImaging_GetCapability (
                                       oyConnectorImaging_s    * c,
                                       oyCONNECTOR_IMAGING_CAP_e type );
OYAPI int  OYEXPORT
                 oyConnectorImaging_SetCapability (
                                       oyConnectorImaging_s    * c,
                                       oyCONNECTOR_IMAGING_CAP_e type,
                                       int                       value );


/* } Include "ConnectorImaging.public_methods_declarations.h" */

OYAPI int  OYEXPORT
                 oyConnectorImaging_SetTexts( oyConnectorImaging_s     * obj,
                                       oyCMMGetText_f      getText,
                                       const char       ** text_classes );
OYAPI const char **  OYEXPORT
                 oyConnectorImaging_GetTexts( oyConnectorImaging_s     * obj );
OYAPI const char *  OYEXPORT
                 oyConnectorImaging_GetText ( oyConnectorImaging_s     * obj,
                                       const char        * name_class,
                                       oyNAME_e            type );
OYAPI int  OYEXPORT
                 oyConnectorImaging_IsPlug  ( oyConnectorImaging_s     * obj );
OYAPI int  OYEXPORT
                 oyConnectorImaging_SetIsPlug(oyConnectorImaging_s     * obj,
                                       int                 is_plug );
OYAPI const char *  OYEXPORT
                oyConnectorImaging_GetReg  ( oyConnectorImaging_s     * obj );
OYAPI int  OYEXPORT
                 oyConnectorImaging_SetReg  ( oyConnectorImaging_s     * obj,
                                       const char        * type_registration );

OYAPI int  OYEXPORT
                 oyConnectorImaging_SetMatch( oyConnectorImaging_s     * obj,
                                       oyCMMFilterSocket_MatchPlug_f func );
OYAPI oyCMMFilterSocket_MatchPlug_f  OYEXPORT
                 oyConnectorImaging_GetMatch (
                                       oyConnectorImaging_s     * obj );


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CONNECTOR_IMAGING_S_H */
