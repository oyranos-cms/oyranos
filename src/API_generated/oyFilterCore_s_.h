/** @file oyFilterCore_s_.h

   [Template file inheritance graph]
   +-> oyFilterCore_s_.template.h
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


#ifndef OY_FILTER_CORE_S__H
#define OY_FILTER_CORE_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyFilterCorePriv_m( var ) ((oyFilterCore_s_*) (var))

typedef struct oyFilterCore_s_ oyFilterCore_s_;

  
#include <oyranos_object.h>


#include "oyCMMapi4_s.h"
#include "oyCMMapi4_s_.h"
  

#include "oyFilterCore_s.h"

/* Include "FilterCore.private.h" { */


/* } Include "FilterCore.private.h" */

/** @internal
 *  @struct   oyFilterCore_s_
 *  @brief      A basic filter to manipulate data
 *  @ingroup  objects_conversion
 *  @extends  oyStruct_s
 */
struct oyFilterCore_s_ {

/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */



/* Include "FilterCore.members.h" { */
  char               * registration_;  /**< @private a registration name, e.g. "org/oyranos/imaging/scale", see as well @ref registration */

  char               * category_;      /**< @private the ui menue category for this filter, to be specified */

  oyOptions_s        * options_;       /**< @private local options */

  oyCMMapi4_s_       * api4_;          /**< @private oyranos library interfaces */

/* } Include "FilterCore.members.h" */

};


oyFilterCore_s_*
  oyFilterCore_New_( oyObject_s object );
oyFilterCore_s_*
  oyFilterCore_Copy_( oyFilterCore_s_ *filtercore, oyObject_s object);
oyFilterCore_s_*
  oyFilterCore_Copy__( oyFilterCore_s_ *filtercore, oyObject_s object);
int
  oyFilterCore_Release_( oyFilterCore_s_ **filtercore );



/* Include "FilterCore.private_methods_declarations.h" { */
int          oyFilterCore_SetCMMapi4_( oyFilterCore_s_   * s,
                                       oyCMMapi4_s_      * cmm_api4 );

/* } Include "FilterCore.private_methods_declarations.h" */



void oyFilterCore_Release__Members( oyFilterCore_s_ * filtercore );
int oyFilterCore_Init__Members( oyFilterCore_s_ * filtercore );
int oyFilterCore_Copy__Members( oyFilterCore_s_ * dst, oyFilterCore_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_FILTER_CORE_S__H */
