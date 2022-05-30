/** @file oyCMMapi7_s_.h

   [Template file inheritance graph]
   +-> oyCMMapi7_s_.template.h
   |
   +-> oyCMMapiFilter_s_.template.h
   |
   +-> oyCMMapi_s_.template.h
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


#ifndef OY_CMM_API7_S__H
#define OY_CMM_API7_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyCMMapi7Priv_m( var ) ((oyCMMapi7_s_*) (var))

typedef struct oyCMMapi7_s_ oyCMMapi7_s_;

  
#include <oyranos_object.h>




#include "oyranos_module_internal.h"
#include "oyCMMinfo_s_.h"

#include "oyCMMapi5_s_.h"

#include "oyConnector_s.h"
  

#include "oyCMMapi7_s.h"

/* Include "CMMapi7.private.h" { */

/* } Include "CMMapi7.private.h" */

/** @internal
 *  @struct   oyCMMapi7_s_
 *  @brief      Data processing node
 *  @ingroup  module_api
 *  @extends  oyCMMapiFilter_s
 */
struct oyCMMapi7_s_ {

/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */





/* Include "CMMapi.members.h" { */
  oyCMMapi_s     * next;               /**< the next CMM api */

  oyCMMInit_f      oyCMMInit;          /**< */
  oyCMMReset_f     oyCMMReset;         /**< */
  oyCMMMessageFuncSet_f oyCMMMessageFuncSet; /**< */

  /** e.g. "sw/oyranos.org/color.tonemap.imaging/hydra.shiva.CPU.GPU" or "sw/oyranos.org/color/icc.lcms.CPU",
      see as well @ref registration */
  char           * registration;

  /** 0: major - should be stable for the live time of a filter, \n
      1: minor - mark new features, \n
      2: patch version - correct errors */
  int32_t          version[3];

  /** 0: last major Oyranos version during development time, e.g. 0
   *  1: last minor Oyranos version during development time, e.g. 0
   *  2: last Oyranos patch version during development time, e.g. 10
   */
  int32_t          module_api[3];

  char           * id_;                /**< @private Oyranos id; keep to zero */

/* } Include "CMMapi.members.h" */

/* Include "CMMapiFilter.members.h" { */
  oyCMMapi5_s_   * api5_;            /**< @private meta module; keep to zero */
  oyPointer_s    * runtime_context;  /**< data needed to run the filter */

/* } Include "CMMapiFilter.members.h" */

/* Include "CMMapi7.members.h" { */
  /** mandatory for all filters; Special care has to taken for the
      oyPixelAccess_s argument to this function. */
  oyCMMFilterPlug_Run_f oyCMMFilterPlug_Run;
  /** e.g. oyCMMFilterPlug_Run specific context data "lcCC" */
  char             context_type[8];

  /** We have to tell about valid input and output connectors, by 
      passively providing enough informations. */
  oyConnector_s ** plugs;
  uint32_t         plugs_n;            /**< number of different plugs */ 
  /** additional allowed number for last input connector, e.g. typical 0 */
  uint32_t         plugs_last_add;
  oyConnector_s ** sockets;            /**< the socket description */
  uint32_t         sockets_n;          /**< number of sockets */
  /** additional allowed number for last output connector, e.g. typical 0 */
  uint32_t         sockets_last_add;
  /** A zero terminated list of strings. The semantics are plugin defined.
   *  e.g.: {"key1=value1","key2=value2",0} */
  char **          properties;

/* } Include "CMMapi7.members.h" */

};


oyCMMapi7_s_*
  oyCMMapi7_New_( oyObject_s object );
oyCMMapi7_s_*
  oyCMMapi7_Copy_( oyCMMapi7_s_ *cmmapi7, oyObject_s object);
oyCMMapi7_s_*
  oyCMMapi7_Copy__( oyCMMapi7_s_ *cmmapi7, oyObject_s object);
int
  oyCMMapi7_Release_( oyCMMapi7_s_ **cmmapi7 );



/* Include "CMMapi7.private_methods_declarations.h" { */

/* } Include "CMMapi7.private_methods_declarations.h" */



void oyCMMapi7_Release__Members( oyCMMapi7_s_ * cmmapi7 );
int oyCMMapi7_Init__Members( oyCMMapi7_s_ * cmmapi7 );
int oyCMMapi7_Copy__Members( oyCMMapi7_s_ * dst, oyCMMapi7_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CMM_API7_S__H */
