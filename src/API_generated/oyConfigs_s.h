/** @file oyConfigs_s.h

   [Template file inheritance graph]
   +-> oyConfigs_s.template.h
   |
   +-> BaseList_s.h
   |
   +-> Base_s.h
   |
   +-- oyStruct_s.template.h

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */



#ifndef OY_CONFIGS_S_H
#define OY_CONFIGS_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyConfigs_s oyConfigs_s;




#include "oyStruct_s.h"

#include "oyConfig_s.h"

#include "oyOptions_s.h"


/* Include "Configs.public.h" { */

/* } Include "Configs.public.h" */


/* Include "Configs.dox" { */
/** @struct  oyConfigs_s
 *  @ingroup objects_value
 *  @extends oyStruct_s
 *  @brief   A Configs list
 *  
 *  Managing the plural of oyConfig_s for the sake of typesafety.
 *  @note This class holds a list of objects [list]
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/19 (Oyranos: 0.1.10)
 *  @date    2009/01/19
 */

/* } Include "Configs.dox" */

struct oyConfigs_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};



OYAPI oyConfigs_s* OYEXPORT
  oyConfigs_New( oyObject_s object );
OYAPI oyConfigs_s* OYEXPORT
  oyConfigs_Copy( oyConfigs_s *configs, oyObject_s obj );
OYAPI int OYEXPORT
  oyConfigs_Release( oyConfigs_s **configs );

OYAPI int  OYEXPORT
           oyConfigs_MoveIn          ( oyConfigs_s       * list,
                                       oyConfig_s       ** ptr,
                                       int                 pos );
OYAPI int  OYEXPORT
           oyConfigs_ReleaseAt       ( oyConfigs_s       * list,
                                       int                 pos );
OYAPI oyConfig_s * OYEXPORT
           oyConfigs_Get             ( oyConfigs_s       * list,
                                       int                 pos );
OYAPI int  OYEXPORT
           oyConfigs_Count           ( oyConfigs_s       * list );
OYAPI int  OYEXPORT
           oyConfigs_Clear           ( oyConfigs_s       * list );
OYAPI int  OYEXPORT
           oyConfigs_Sort            ( oyConfigs_s       * list,
                                       int32_t           * rank_list );



/* Include "Configs.public_methods_declarations.h" { */
OYAPI int  OYEXPORT
               oyConfigs_FromDomain  ( const char        * registration_domain,
                                       oyOptions_s       * options,
                                       oyConfigs_s      ** configs,
                                       oyObject_s          object );
OYAPI int  OYEXPORT
               oyConfigs_FromDeviceClass (
                                       const char        * device_type,
                                       const char        * device_class,
                                       oyOptions_s       * options,
                                       oyConfigs_s      ** configs,
                                       oyObject_s          object );
OYAPI int OYEXPORT oyConfigs_FromDB  ( const char        * registration,
                                       oyOptions_s       * options,
                                       oyConfigs_s      ** configs,
                                       oyObject_s          object );
OYAPI int  OYEXPORT
                 oyConfigs_Modify    ( oyConfigs_s       * configs,
                                       oyOptions_s       * options );
OYAPI int  OYEXPORT
                 oyConfigs_SelectSimilars (
                                       oyConfigs_s       * list,
                                       const char        * pattern[][2],
                                       oyConfigs_s      ** filtered );
OYAPI int  OYEXPORT
                 oyConfigDomainList  ( const char        * registration_pattern,
                                       char            *** list,
                                       uint32_t          * count,
                                       uint32_t         ** rank_list,
                                       oyAlloc_f           allocateFunc );
OYAPI int  OYEXPORT
               oyConfig_GetBestMatchFromTaxiDB(
                                       oyConfig_s        * device,
                                       int32_t           * rank_value );

/* } Include "Configs.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CONFIGS_S_H */
