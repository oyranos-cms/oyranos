/** @file oyCMMapi6_s_.h

   [Template file inheritance graph]
   +-> oyCMMapi6_s_.template.h
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


#ifndef OY_CMM_API6_S__H
#define OY_CMM_API6_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyCMMapi6Priv_m( var ) ((oyCMMapi6_s_*) (var))

typedef struct oyCMMapi6_s_ oyCMMapi6_s_;

  
#include <oyranos_object.h>




#include "oyranos_module_internal.h"
#include "oyCMMinfo_s_.h"

#include "oyCMMapi5_s_.h"

#include "oyPointer_s.h"
#include "oyFilterNode_s.h"
#include "oyCMMapi6_s_.h"
  

#include "oyCMMapi6_s.h"

/* Include "CMMapi6.private.h" { */

/* } Include "CMMapi6.private.h" */

/** @internal
 *  @struct   oyCMMapi6_s_
 *  @brief      Context convertor
 *  @ingroup  module_api
 *  @extends  oyCMMapiFilter_s
 */
struct oyCMMapi6_s_ {

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

/* Include "CMMapi6.members.h" { */
  /** oyCMMapi4_s::context_type typic data; e.g. "oyDL" */
  char           * data_type_in;
  /** oyCMMapi7_s::context_type specific data; e.g. "lcCC" */
  char           * data_type_out;
  oyModuleData_Convert_f oyModuleData_Convert;

/* } Include "CMMapi6.members.h" */

};


oyCMMapi6_s_*
  oyCMMapi6_New_( oyObject_s object );
oyCMMapi6_s_*
  oyCMMapi6_Copy_( oyCMMapi6_s_ *cmmapi6, oyObject_s object);
oyCMMapi6_s_*
  oyCMMapi6_Copy__( oyCMMapi6_s_ *cmmapi6, oyObject_s object);
int
  oyCMMapi6_Release_( oyCMMapi6_s_ **cmmapi6 );



/* Include "CMMapi6.private_methods_declarations.h" { */

/* } Include "CMMapi6.private_methods_declarations.h" */



void oyCMMapi6_Release__Members( oyCMMapi6_s_ * cmmapi6 );
int oyCMMapi6_Init__Members( oyCMMapi6_s_ * cmmapi6 );
int oyCMMapi6_Copy__Members( oyCMMapi6_s_ * dst, oyCMMapi6_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CMM_API6_S__H */
