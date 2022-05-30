/** @file oyCMMapi4_s_.h

   [Template file inheritance graph]
   +-> oyCMMapi4_s_.template.h
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


#ifndef OY_CMM_API4_S__H
#define OY_CMM_API4_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyCMMapi4Priv_m( var ) ((oyCMMapi4_s_*) (var))

typedef struct oyCMMapi4_s_ oyCMMapi4_s_;

  
#include <oyranos_object.h>




#include "oyranos_module_internal.h"
#include "oyCMMinfo_s_.h"

#include "oyCMMapi5_s_.h"

#include "oyCMMapi5_s.h"
#include "oyFilterNode_s.h"
#include "oyCMMui_s_.h"
  

#include "oyCMMapi4_s.h"

/* Include "CMMapi4.private.h" { */
/** @internal
 *  @memberof oyCMMapi4_s
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/12/16 (Oyranos: 0.1.9)
 *  @date    2008/12/16
 */
typedef struct {
  oyOBJECT_e type;
  const char        * registration;
} oyRegistrationData_s;


/* } Include "CMMapi4.private.h" */

/** @internal
 *  @struct   oyCMMapi4_s_
 *  @brief      Context creator
 *  @ingroup  module_api
 *  @extends  oyCMMapiFilter_s
 */
struct oyCMMapi4_s_ {

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

/* Include "CMMapi4.members.h" { */
  /** mandatory for "//color/icc" context producing filters */
  oyCMMFilterNode_ContextToMem_f   oyCMMFilterNode_ContextToMem;
  /** optional for a set oyCMMFilterNode_ContextToMem,
   *  used to override a Oyranos side hash creation */
  oyCMMFilterNode_GetText_f        oyCMMFilterNode_GetText;
  /** the data type of the context returned by oyCMMapi4_s::oyCMMFilterNode_ContextToMem_f, mandatory for a set oyCMMFilterNode_ContextToMem
   *  e.g. oyCOLOR_ICC_DEVICE_LINK / "oyDL" */
  char             context_type[8];

  /** a UI description
   *  Obligatory is a implemented oyCMMapi4_s::ui->getText( x, y ) call. The x
   *  argument shall cover "name" and "help" */
  oyCMMui_s_     * ui;                 /**< a UI description */

/* } Include "CMMapi4.members.h" */

};


oyCMMapi4_s_*
  oyCMMapi4_New_( oyObject_s object );
oyCMMapi4_s_*
  oyCMMapi4_Copy_( oyCMMapi4_s_ *cmmapi4, oyObject_s object);
oyCMMapi4_s_*
  oyCMMapi4_Copy__( oyCMMapi4_s_ *cmmapi4, oyObject_s object);
int
  oyCMMapi4_Release_( oyCMMapi4_s_ **cmmapi4 );



/* Include "CMMapi4.private_methods_declarations.h" { */
oyOBJECT_e   oyCMMapi4_SelectFilter_ ( oyCMMapi_s_       * api,
                                       oyPointer           data );

/* } Include "CMMapi4.private_methods_declarations.h" */



void oyCMMapi4_Release__Members( oyCMMapi4_s_ * cmmapi4 );
int oyCMMapi4_Init__Members( oyCMMapi4_s_ * cmmapi4 );
int oyCMMapi4_Copy__Members( oyCMMapi4_s_ * dst, oyCMMapi4_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CMM_API4_S__H */
