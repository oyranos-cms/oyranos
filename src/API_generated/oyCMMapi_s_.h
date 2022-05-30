/** @file oyCMMapi_s_.h

   [Template file inheritance graph]
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


#ifndef OY_CMM_API_S__H
#define OY_CMM_API_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyCMMapiPriv_m( var ) ((oyCMMapi_s_*) (var))

typedef struct oyCMMapi_s_ oyCMMapi_s_;

  
#include <oyranos_object.h>


#include "oyranos_module_internal.h"
#include "oyCMMinfo_s_.h"
  

#include "oyCMMapi_s.h"

/* Include "CMMapi.private.h" { */
/** typedef   oyCMMOptions_Check_f
 *  @brief    a function to check options
 *  @ingroup  module_api
 *  @memberof oyCMMapi_s
 *
 *  @param[in]     options             the options
 *  @return                            0 - good, 1 - bad
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/07/15 (Oyranos: 0.1.10)
 *  @date    2009/07/15
 */
typedef int  (*oyCMMOptions_Check_f) ( oyOptions_s       * validate );


/* } Include "CMMapi.private.h" */

/** @internal
 *  @struct   oyCMMapi_s_
 *  @brief      the basic API interface
 *  @ingroup  module_api
 *  @extends  oyStruct_s
 */
struct oyCMMapi_s_ {

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

};


oyCMMapi_s_*
  oyCMMapi_New_( oyObject_s object );
oyCMMapi_s_*
  oyCMMapi_Copy_( oyCMMapi_s_ *cmmapi, oyObject_s object);
oyCMMapi_s_*
  oyCMMapi_Copy__( oyCMMapi_s_ *cmmapi, oyObject_s object);
int
  oyCMMapi_Release_( oyCMMapi_s_ **cmmapi );



/* Include "CMMapi.private_methods_declarations.h" { */
oyOBJECT_e       oyCMMapi_Check_     ( oyCMMapi_s        * api );
oyOBJECT_e   oyCMMapi_CheckWrap_     ( oyCMMinfo_s       * cmm_info,
                                       oyCMMapi_s        * api,
                                       oyPointer           data,
                                       uint32_t          * rank );
oyOBJECT_e   oyCMMapi_selectFilter_  ( oyCMMinfo_s_      * info,
                                       oyCMMapi_s_       * api,
                                       oyPointer           data,
                                       uint32_t          * rank );

/* } Include "CMMapi.private_methods_declarations.h" */



void oyCMMapi_Release__Members( oyCMMapi_s_ * cmmapi );
int oyCMMapi_Init__Members( oyCMMapi_s_ * cmmapi );
int oyCMMapi_Copy__Members( oyCMMapi_s_ * dst, oyCMMapi_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CMM_API_S__H */
