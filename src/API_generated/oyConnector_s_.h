/** @file oyConnector_s_.h

   [Template file inheritance graph]
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


#ifndef OY_CONNECTOR_S__H
#define OY_CONNECTOR_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyConnectorPriv_m( var ) ((oyConnector_s_*) (var))

typedef struct oyConnector_s_ oyConnector_s_;

  
#include <oyranos_object.h>
  

#include "oyConnector_s.h"

/* Include "Connector.private.h" { */

/* } Include "Connector.private.h" */

/** @internal
 *  @struct   oyConnector_s_
 *  @brief      A filter connection description structure
 *  @ingroup  objects_conversion
 *  @extends  oyStruct_s
 */
struct oyConnector_s_ {

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

};


oyConnector_s_*
  oyConnector_New_( oyObject_s object );
oyConnector_s_*
  oyConnector_Copy_( oyConnector_s_ *connector, oyObject_s object);
oyConnector_s_*
  oyConnector_Copy__( oyConnector_s_ *connector, oyObject_s object);
int
  oyConnector_Release_( oyConnector_s_ **connector );



/* Include "Connector.private_methods_declarations.h" { */

/* } Include "Connector.private_methods_declarations.h" */



void oyConnector_Release__Members( oyConnector_s_ * connector );
int oyConnector_Init__Members( oyConnector_s_ * connector );
int oyConnector_Copy__Members( oyConnector_s_ * dst, oyConnector_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CONNECTOR_S__H */
