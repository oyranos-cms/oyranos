/** @file oyCMMapi10_s_.h

   [Template file inheritance graph]
   +-> oyCMMapi10_s_.template.h
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


#ifndef OY_CMM_API10_S__H
#define OY_CMM_API10_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyCMMapi10Priv_m( var ) ((oyCMMapi10_s_*) (var))

typedef struct oyCMMapi10_s_ oyCMMapi10_s_;

  
#include <oyranos_object.h>



#include "oyranos_module_internal.h"
#include "oyCMMinfo_s_.h"

#include "oyCMMapi5_s_.h"
  

#include "oyCMMapi10_s.h"

/* Include "CMMapi10.private.h" { */

/* } Include "CMMapi10.private.h" */

/** @internal
 *  @struct   oyCMMapi10_s_
 *  @brief      Generic data exchange
 *  @ingroup  module_api
 *  @extends  oyCMMapiFilter_s
 */
struct oyCMMapi10_s_ {

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

/* Include "CMMapi10.members.h" { */
  /** translated, e.g. "help": "createICC" "create a ICC profile" "With ..."
   */
  oyCMMGetText_f   getText;
  /** zero terminated list for ::getText()
   *  include "help", "can_handle" and the supported commands
   */
  char          ** texts;              /**< zero terminated list for getText */

  oyMOptions_Handle_f oyMOptions_Handle; /**< @memberof oyCMMapi10_s */

/* } Include "CMMapi10.members.h" */

};


oyCMMapi10_s_*
  oyCMMapi10_New_( oyObject_s object );
oyCMMapi10_s_*
  oyCMMapi10_Copy_( oyCMMapi10_s_ *cmmapi10, oyObject_s object);
oyCMMapi10_s_*
  oyCMMapi10_Copy__( oyCMMapi10_s_ *cmmapi10, oyObject_s object);
int
  oyCMMapi10_Release_( oyCMMapi10_s_ **cmmapi10 );



/* Include "CMMapi10.private_methods_declarations.h" { */

/* } Include "CMMapi10.private_methods_declarations.h" */



void oyCMMapi10_Release__Members( oyCMMapi10_s_ * cmmapi10 );
int oyCMMapi10_Init__Members( oyCMMapi10_s_ * cmmapi10 );
int oyCMMapi10_Copy__Members( oyCMMapi10_s_ * dst, oyCMMapi10_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CMM_API10_S__H */
