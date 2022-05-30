/** @file oyConfigs_s_.h

   [Template file inheritance graph]
   +-> oyConfigs_s_.template.h
   |
   +-> BaseList_s_.h
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


#ifndef OY_CONFIGS_S__H
#define OY_CONFIGS_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyConfigsPriv_m( var ) ((oyConfigs_s_*) (var))

typedef struct oyConfigs_s_ oyConfigs_s_;

  
#include <oyranos_object.h>

#include "oyStructList_s.h"
  

#include "oyConfigs_s.h"

/* Include "Configs.private.h" { */

/* } Include "Configs.private.h" */

/** @internal
 *  @struct   oyConfigs_s_
 *  @brief      A Configs list
 *  @ingroup  objects_value
 *  @extends  oyStruct_s
 */
struct oyConfigs_s_ {

/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */


/* Include "BaseList_s_.h" { */
  oyStructList_s * list_;              /**< the list data */
/* } Include "BaseList_s_.h" */

};



oyConfigs_s_*
  oyConfigs_New_( oyObject_s object );
oyConfigs_s_*
  oyConfigs_Copy_( oyConfigs_s_ *configs, oyObject_s object);
oyConfigs_s_*
  oyConfigs_Copy__( oyConfigs_s_ *configs, oyObject_s object);
int
  oyConfigs_Release_( oyConfigs_s_ **configs );

#if 0
/* FIXME Are these needed at all?? */
int
           oyConfigs_MoveIn_         ( oyConfigs_s_      * list,
                                       oyConfig_s       ** ptr,
                                       int                 pos );
int
           oyConfigs_ReleaseAt_      ( oyConfigs_s_      * list,
                                       int                 pos );
oyConfig_s *
           oyConfigs_Get_            ( oyConfigs_s_      * list,
                                       int                 pos );
int
           oyConfigs_Count_          ( oyConfigs_s_      * list );
#endif



/* Include "Configs.private_methods_declarations.h" { */

/* } Include "Configs.private_methods_declarations.h" */




#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CONFIGS_S__H */
