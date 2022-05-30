/** @file oyCMMapi8_s_.h

   [Template file inheritance graph]
   +-> oyCMMapi8_s_.template.h
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


#ifndef OY_CMM_API8_S__H
#define OY_CMM_API8_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyCMMapi8Priv_m( var ) ((oyCMMapi8_s_*) (var))

typedef struct oyCMMapi8_s_ oyCMMapi8_s_;

  
#include <oyranos_object.h>




#include "oyranos_module_internal.h"
#include "oyCMMinfo_s_.h"

#include "oyCMMapi5_s_.h"

#include "oyConfigs_s.h"
#include "oyCMMapi5_s.h"
#include "oyCMMui_s.h"

#include "oyCMMapi_s_.h"
  

#include "oyCMMapi8_s.h"

/* Include "CMMapi8.private.h" { */
/** typedef  oyConfigs_FromPattern_f
 *  @brief   return available configurations
 *  @ingroup module_api
 *  @memberof oyCMMapi8_s
 *
 *  @param[in]     registration        a selection string marked with "///config"
 *  @param[in]     options             the complete options from the user
 *  @param[out]    configs             the returned configurations
 *  @return                            0 - good, 1 <= error, -1 >= unknown issue, look for messages
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/18 (Oyranos: 0.1.10)
 *  @date    2009/01/18
 */
typedef int      (*oyConfigs_FromPattern_f) (
                                       const char        * registration,
                                       oyOptions_s       * options,
                                       oyConfigs_s      ** configs );

/** typedef  oyConfigs_Modify_f
 *  @brief   enrich/manipulate existing configurations
 *  @ingroup module_api
 *  @memberof oyCMMapi8_s
 *
 *  @param[in,out] configs             the configurations
 *  @param[in]     options             the complete options from the user
 *  @return                            0 - good, 1 <= error, -1 >= unknown issue, look for messages
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/08/21 (Oyranos: 0.1.10)
 *  @date    2009/08/21
 */
typedef int      (*oyConfigs_Modify_f)( oyConfigs_s       * configs,
                                        oyOptions_s       * options );

/** typedef  oyConfig_Rank_f
 *  @brief   check for correctness
 *  @ingroup module_api
 *  @memberof oyCMMapi8_s
 *
 *  The function is called in Oyranos' core to give a module a hook to check  
 *  and accept a device. If the module does not find its previously set  
 *  handles and can neigther open the device from the device_name then
 *  chances are good that it can't handle and should return zero otherwise    
 *  one. Oyranos will then try an other module with this device.
 *
 *  @param   config                    the to be checked configuration
 *  @return                            - 0 - indifferent,
 *                                     - >= 1 - rank,
 *                                     - <= -1 error + a message should be sent
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/16 (Oyranos: 0.1.10)
 *  @date    2009/01/26
 */
typedef int  (*oyConfig_Rank_f)     ( oyConfig_s         * config );

/* } Include "CMMapi8.private.h" */

/** @internal
 *  @struct   oyCMMapi8_s_
 *  @brief      Configuration handler
 *  @ingroup  module_api
 *  @extends  oyCMMapiFilter_s
 */
struct oyCMMapi8_s_ {

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

/* Include "CMMapi8.members.h" { */
  /** obtain configs matching to a set of options */
  oyConfigs_FromPattern_f oyConfigs_FromPattern;
  /** manipulate given configs */
  oyConfigs_Modify_f oyConfigs_Modify;
  oyConfig_Rank_f  oyConfig_Rank;      /**< test config */

  oyCMMui_s      * ui;                 /**< a UI description */
  oyIcon_s       * icon;               /**< module associated icon */

  /** zero terminated list of rank attributes;
   *  The data is just informational. In case all properties to rank a given 
   *  device pair are well known, this rank_map can be copied into each 
   *  oyConfig_s device created by oyConfigs_FromPattern():
   *  @code
      oyConfig_s * device = oyConfig_FromRegistration( CMM_BASE_REG, 0);
      device->rank_map = oyRankMapCopy( dDev_api8->rank_map,
                                        device->oy_->allocateFunc_);
      @endcode
   *  For additional dynamic rank pads the allocation can be done individually.
   */
  oyRankMap      * rank_map;

/* } Include "CMMapi8.members.h" */

};


oyCMMapi8_s_*
  oyCMMapi8_New_( oyObject_s object );
oyCMMapi8_s_*
  oyCMMapi8_Copy_( oyCMMapi8_s_ *cmmapi8, oyObject_s object);
oyCMMapi8_s_*
  oyCMMapi8_Copy__( oyCMMapi8_s_ *cmmapi8, oyObject_s object);
int
  oyCMMapi8_Release_( oyCMMapi8_s_ **cmmapi8 );



/* Include "CMMapi8.private_methods_declarations.h" { */

/* } Include "CMMapi8.private_methods_declarations.h" */



void oyCMMapi8_Release__Members( oyCMMapi8_s_ * cmmapi8 );
int oyCMMapi8_Init__Members( oyCMMapi8_s_ * cmmapi8 );
int oyCMMapi8_Copy__Members( oyCMMapi8_s_ * dst, oyCMMapi8_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CMM_API8_S__H */
