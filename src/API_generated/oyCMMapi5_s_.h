/** @file oyCMMapi5_s_.h

   [Template file inheritance graph]
   +-> oyCMMapi5_s_.template.h
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


#ifndef OY_CMM_API5_S__H
#define OY_CMM_API5_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyCMMapi5Priv_m( var ) ((oyCMMapi5_s_*) (var))

typedef struct oyCMMapi5_s_ oyCMMapi5_s_;

  
#include <oyranos_object.h>



#include "oyranos_module_internal.h"
#include "oyCMMinfo_s_.h"

#include "oyCMMapiFilter_s.h"
#include "oyCMMapi_s_.h"
#include "oyFilterSocket_s.h"
  

#include "oyCMMapi5_s.h"

/* Include "CMMapi5.private.h" { */

/* } Include "CMMapi5.private.h" */

/** @internal
 *  @struct   oyCMMapi5_s_
 *  @brief      Module or script loader
 *  @ingroup  module_api
 *  @extends  oyCMMapi_s
 */
struct oyCMMapi5_s_ {

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

/* Include "CMMapi5.members.h" { */
  /** a colon separated list of sub paths to expect the scripts in,
      e.g. "color/shiva:color/octl" */
  const char     * sub_paths;
  /** optional filename extensions, e.g. "shi:ctl" */
  const char     * ext;
  /** 0: libs - libraries, Oyranos searches in the XDG_LIBRARY_PATH and sub_paths, The library will be provided as file_name\n  1: scripts - platform independent filters, Oyranos will search in the XDG_DATA_* paths, Script are provided as i memory blobs */
  int32_t          data_type;          /**< */

  oyCMMFilterLoad_f                oyCMMFilterLoad; /**< */
  oyCMMFilterScan_f                oyCMMFilterScan; /**< */

/* } Include "CMMapi5.members.h" */

};


oyCMMapi5_s_*
  oyCMMapi5_New_( oyObject_s object );
oyCMMapi5_s_*
  oyCMMapi5_Copy_( oyCMMapi5_s_ *cmmapi5, oyObject_s object);
oyCMMapi5_s_*
  oyCMMapi5_Copy__( oyCMMapi5_s_ *cmmapi5, oyObject_s object);
int
  oyCMMapi5_Release_( oyCMMapi5_s_ **cmmapi5 );



/* Include "CMMapi5.private_methods_declarations.h" { */

/* } Include "CMMapi5.private_methods_declarations.h" */



void oyCMMapi5_Release__Members( oyCMMapi5_s_ * cmmapi5 );
int oyCMMapi5_Init__Members( oyCMMapi5_s_ * cmmapi5 );
int oyCMMapi5_Copy__Members( oyCMMapi5_s_ * dst, oyCMMapi5_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CMM_API5_S__H */
