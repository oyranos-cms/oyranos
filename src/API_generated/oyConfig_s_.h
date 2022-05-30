/** @file oyConfig_s_.h

   [Template file inheritance graph]
   +-> oyConfig_s_.template.h
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


#ifndef OY_CONFIG_S__H
#define OY_CONFIG_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyConfigPriv_m( var ) ((oyConfig_s_*) (var))

typedef struct oyConfig_s_ oyConfig_s_;

  
#include <oyranos_object.h>


#include "oyCMMapi8_s_.h"
#include "oyCMMapiFilter_s_.h"
#include "oyCMMapiFilters_s.h"
#include "oyOptions_s_.h"

#include "oyranos_module_internal.h"
  

#include "oyConfig_s.h"

/* Include "Config.private.h" { */

/* } Include "Config.private.h" */

/** @internal
 *  @struct   oyConfig_s_
 *  @brief      A group of options for a device
 *  @ingroup  objects_value
 *  @extends  oyStruct_s
 */
struct oyConfig_s_ {

/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */



/* Include "Config.members.h" { */
  uint32_t             id;             /**< id to map to events and widgets */
  /** This property contains the identifier for communication with a Oyranos
   *  or a module through Oyranos. It defines the basic key path name to store
   *  configuration.\n
   *  e.g. "org/freedesktop/imaging/config.monitor.xorg" \n
   *  see as well @ref registration */
  char               * registration;
  int                  version[3];     /**< as for oyCMMapi4_s::version */

  /** data base (Elektra) properties,
  e.g. "org/freedesktop/imaging/config.monitor.xorg/1/manufacturer=EIZO"*/
  oyOptions_s        * db;
  /** These are the module core properties, the ones to identify the 
   *  device and store in DB. They must be filled by the module.
  e.g. "org/freedesktop/imaging/config.monitor.xorg/manufacturer=EIZO" */
  oyOptions_s        * backend_core;
  /** Additional informations from modules, with non identification purpose,
   *  can be stored herein,
  e.g. "org/freedesktop/imaging/config.monitor.xorg/edid=oyBlob_s*" */
  oyOptions_s        * data;

  oyRankMap          * rank_map;       /**< zero terminated list; key compare */

/* } Include "Config.members.h" */

};


oyConfig_s_*
  oyConfig_New_( oyObject_s object );
oyConfig_s_*
  oyConfig_Copy_( oyConfig_s_ *config, oyObject_s object);
oyConfig_s_*
  oyConfig_Copy__( oyConfig_s_ *config, oyObject_s object);
int
  oyConfig_Release_( oyConfig_s_ **config );



/* Include "Config.private_methods_declarations.h" { */

/* } Include "Config.private_methods_declarations.h" */



void oyConfig_Release__Members( oyConfig_s_ * config );
int oyConfig_Init__Members( oyConfig_s_ * config );
int oyConfig_Copy__Members( oyConfig_s_ * dst, oyConfig_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CONFIG_S__H */
