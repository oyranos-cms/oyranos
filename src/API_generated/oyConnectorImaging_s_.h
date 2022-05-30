/** @file oyConnectorImaging_s_.h

   [Template file inheritance graph]
   +-> oyConnectorImaging_s_.template.h
   |
   +-> oyConnector_s_.template.h
   |
   +-- Base_s_.h

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


#ifndef OY_CONNECTOR_IMAGING_S__H
#define OY_CONNECTOR_IMAGING_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyConnectorImagingPriv_m( var ) ((oyConnectorImaging_s_*) (var))

typedef struct oyConnectorImaging_s_ oyConnectorImaging_s_;

  
#include <oyranos_object.h>


#include "oyranos_module.h"
#include "oyranos_image.h"
#include "oyCMMapi5_s_.h"
  

#include "oyConnectorImaging_s.h"

/* Include "ConnectorImaging.private.h" { */

/* } Include "ConnectorImaging.private.h" */

/** @internal
 *  @struct   oyConnectorImaging_s_
 *  @brief      Node connection descriptor
 *  @ingroup  objects_conversion
 *  @extends  oyConnector_s
 */
struct oyConnectorImaging_s_ {

/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */




/* Include "Connector.members.h" { */
  /** Support at least "name" for UIs. */
  oyCMMGetText_f       getText;
  const char        ** texts;          /**< zero terminated list for getText */

  char               * connector_type; /**< a @ref registration string */
  /** Check if two oyCMMapi7_s filter connectors of type oyConnector_s can 
   *  match each other inside a given socket and a plug. */
  oyCMMFilterSocket_MatchPlug_f  filterSocket_MatchPlug;

  /** make requests and receive data, by part of oyFilterPlug_s */
  int                  is_plug;

/* } Include "Connector.members.h" */

/* Include "ConnectorImaging.members.h" { */
  oyDATATYPE_e       * data_types;     /**< supported float's and int's */
  int                  data_types_n;   /**< # elements in data_types array */
  int                  max_color_offset;
  int                  min_channels_count;
  int                  max_channels_count;
  int                  min_color_count;
  int                  max_color_count;
  int                  can_planar;     /**< can read separated channels */
  int                  can_interwoven; /**< can read continuous channels */
  int                  can_swap;       /**< can swap color channels (BGR)*/
  int                  can_swap_bytes; /**< non host byte order */
  int                  can_revert;     /**< revert 1 -> 0 and 0 -> 1 */
  int                  can_premultiplied_alpha;
  int                  can_nonpremultiplied_alpha;
  int                  can_subpixel;   /**< understand subpixel order */
  /** describe which channel types the connector requires */
  oyCHANNELTYPE_e    * channel_types;
  int                  channel_types_n;/**< count in channel_types */
  int                  id;             /**< relative to oyFilterCore_s, e.g. 1*/
  /**< connector is mandatory or optional, important for modules */
  int                  is_mandatory;

/* } Include "ConnectorImaging.members.h" */

};


oyConnectorImaging_s_*
  oyConnectorImaging_New_( oyObject_s object );
oyConnectorImaging_s_*
  oyConnectorImaging_Copy_( oyConnectorImaging_s_ *connectorimaging, oyObject_s object);
oyConnectorImaging_s_*
  oyConnectorImaging_Copy__( oyConnectorImaging_s_ *connectorimaging, oyObject_s object);
int
  oyConnectorImaging_Release_( oyConnectorImaging_s_ **connectorimaging );



/* Include "ConnectorImaging.private_methods_declarations.h" { */

/* } Include "ConnectorImaging.private_methods_declarations.h" */



void oyConnectorImaging_Release__Members( oyConnectorImaging_s_ * connectorimaging );
int oyConnectorImaging_Init__Members( oyConnectorImaging_s_ * connectorimaging );
int oyConnectorImaging_Copy__Members( oyConnectorImaging_s_ * dst, oyConnectorImaging_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CONNECTOR_IMAGING_S__H */
