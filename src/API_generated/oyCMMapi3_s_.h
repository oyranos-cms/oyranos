/** @file oyCMMapi3_s_.h

   [Template file inheritance graph]
   +-> oyCMMapi3_s_.template.h
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


#ifndef OY_CMM_API3_S__H
#define OY_CMM_API3_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyCMMapi3Priv_m( var ) ((oyCMMapi3_s_*) (var))

typedef struct oyCMMapi3_s_ oyCMMapi3_s_;



#include <oyranos_icc.h>
  
#include <oyranos_object.h>



#include "oyranos_module_internal.h"
#include "oyCMMinfo_s_.h"

#include "oyStructList_s.h"
#include "oyProfileTag_s.h"
#include "oyCMMapi_s_.h"
  

#include "oyCMMapi3_s.h"

/* Include "CMMapi3.private.h" { */
#include "oyranos_module_internal.h"

/**
 *  typedef oyCMMProfileTag_GetValues_f
 *  @memberof oyCMMapi3_s
 */
typedef oyStructList_s *    (*oyCMMProfileTag_GetValues_f) (
                                       oyProfileTag_s    * tag );
/**
 *  typedef oyCMMProfileTag_Create_f
 *  @memberof oyCMMapi3_s
 */
typedef int                 (*oyCMMProfileTag_Create_f) (
                                       oyProfileTag_s    * tag,
                                       oyStructList_s    * list,
                                       icTagTypeSignature  tag_type,
                                       uint32_t            version );

/* } Include "CMMapi3.private.h" */

/** @internal
 *  @struct   oyCMMapi3_s_
 *  @brief      The API 3 to implement and set to provide low level ICC profile support
 *  @ingroup  module_api
 *  @extends  oyCMMapi_s
 */
struct oyCMMapi3_s_ {

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

/* Include "CMMapi3.members.h" { */
  oyCMMCanHandle_f oyCMMCanHandle;     /**< */

  oyCMMProfileTag_GetValues_f oyCMMProfileTag_GetValues; /**< @memberof oyCMMapi3_s */
  oyCMMProfileTag_Create_f oyCMMProfileTag_Create; /**< @memberof oyCMMapi3_s */

/* } Include "CMMapi3.members.h" */

};


oyCMMapi3_s_*
  oyCMMapi3_New_( oyObject_s object );
oyCMMapi3_s_*
  oyCMMapi3_Copy_( oyCMMapi3_s_ *cmmapi3, oyObject_s object);
oyCMMapi3_s_*
  oyCMMapi3_Copy__( oyCMMapi3_s_ *cmmapi3, oyObject_s object);
int
  oyCMMapi3_Release_( oyCMMapi3_s_ **cmmapi3 );



/* Include "CMMapi3.private_methods_declarations.h" { */
oyOBJECT_e   oyCMMapi3_Query_        ( oyCMMinfo_s_      * cmm_info,
                                       oyCMMapi_s_       * api,
                                       oyPointer           data,
                                       uint32_t          * rank );

/* } Include "CMMapi3.private_methods_declarations.h" */



void oyCMMapi3_Release__Members( oyCMMapi3_s_ * cmmapi3 );
int oyCMMapi3_Init__Members( oyCMMapi3_s_ * cmmapi3 );
int oyCMMapi3_Copy__Members( oyCMMapi3_s_ * dst, oyCMMapi3_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CMM_API3_S__H */
