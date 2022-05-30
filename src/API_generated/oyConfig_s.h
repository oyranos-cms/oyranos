/** @file oyConfig_s.h

   [Template file inheritance graph]
   +-> oyConfig_s.template.h
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



#ifndef OY_CONFIG_S_H
#define OY_CONFIG_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyConfig_s oyConfig_s;



#include "oyStruct_s.h"

#include "oyranos.h"
#include "oyBlob_s.h"
#include "oyConfigs_s.h"
#include "oyOption_s.h"
#include "oyOptions_s.h"


/* Include "Config.public.h" { */

/* } Include "Config.public.h" */


/* Include "Config.dox" { */
/** @struct  oyConfig_s
 *  @ingroup objects_value
 *  @extends oyStruct_s
 *  @brief   A group of options for a device
 *  
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/15 (Oyranos: 0.1.10)
 *  @date    2009/01/15
 */

/* } Include "Config.dox" */

struct oyConfig_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};


OYAPI oyConfig_s* OYEXPORT
  oyConfig_New( oyObject_s object );
OYAPI oyConfig_s* OYEXPORT
  oyConfig_Copy( oyConfig_s *config, oyObject_s obj );
OYAPI int OYEXPORT
  oyConfig_Release( oyConfig_s **config );



/* Include "Config.public_methods_declarations.h" { */
/** Function  oyConfig_Set
 *  @memberof oyConfig_s
 *  @brief    Unimplemented!
 */
OYAPI int  OYEXPORT
               oyConfig_Set          ( oyConfig_s        * config,
                                       const char        * registration_domain,
                                       oyOptions_s       * options,
                                       oyBlob_s          * data );
OYAPI int  OYEXPORT
               oyConfig_AddDBData    ( oyConfig_s        * config,
                                       const char        * key,
                                       const char        * value,
                                       uint32_t            flags );
OYAPI int  OYEXPORT
               oyConfig_ClearDBData  ( oyConfig_s        * config );
OYAPI int  OYEXPORT
               oyConfig_GetDB        ( oyConfig_s        * device,
                                       oyOptions_s       * options,
                                       int32_t           * rank_value );
OYAPI int  OYEXPORT
               oyConfig_SaveToDB     ( oyConfig_s        * config,
                                       oySCOPE_e           scope );
OYAPI int  OYEXPORT
               oyConfig_SelectDB     ( oyConfig_s        * device,
                                       oyConfigs_s       * configs,
                                       int32_t           * rank_value );
OYAPI int  OYEXPORT
               oyConfig_EraseFromDB  ( oyConfig_s        * config,
                                       oySCOPE_e           scope );
OYAPI int OYEXPORT
               oyConfig_Match        ( oyConfig_s        * module_device,
                                       oyConfig_s        * db_pattern,
                                       char                path_separator,
                                       char                key_separator,
                                       int                 flags,
                                       int32_t           * rank_value );
OYAPI int  OYEXPORT
               oyConfig_DomainRank   ( oyConfig_s        * config );
OYAPI const char * OYEXPORT
               oyConfig_FindString   ( oyConfig_s        * config,
                                       const char        * key,
                                       const char        * value );
OYAPI int  OYEXPORT
               oyConfig_Has          ( oyConfig_s        * config,
                                       const char        * key );
OYAPI oyOption_s * OYEXPORT
               oyConfig_Find         ( oyConfig_s        * config,
                                       const char        * key );
OYAPI int  OYEXPORT
               oyConfig_Count        ( oyConfig_s        * config );
OYAPI oyOption_s * OYEXPORT
               oyConfig_Get          ( oyConfig_s        * config,
                                       int                 pos );
OYAPI oyOptions_s ** OYEXPORT
               oyConfig_GetOptions   ( oyConfig_s        * config,
                                       const char        * source );
OYAPI int OYEXPORT oyConfig_FromJSON ( const char        * registration,
                                       const char        * json_text,
                                       oyOptions_s       * options,
                                       oyObject_s          object,
                                       oyConfig_s       ** device );
OYAPI oyConfig_s * OYEXPORT
               oyConfig_FromRegistration
                                     ( const char        * registration,
                                       oyObject_s          object );
OYAPI const char *  OYEXPORT
               oyConfig_GetRegistration
                                     ( oyConfig_s        * config );
OYAPI int  OYEXPORT
               oyConfig_SetRankMap   ( oyConfig_s        * config,
                                       const oyRankMap   * rank_map );
OYAPI const oyRankMap *  OYEXPORT
               oyConfig_GetRankMap   ( oyConfig_s        * config );
OYAPI oyRankMap * OYEXPORT
                 oyRankMapCopy       ( const oyRankMap   * rank_map,
                                       oyAlloc_f           allocateFunc );
OYAPI void OYEXPORT
                 oyRankMapRelease    ( oyRankMap        ** rank_map,
                                       oyDeAlloc_f         deAllocateFunc );
OYAPI int  OYEXPORT oyRankMapAppend  ( oyRankMap        ** rank_map,
                                       const char        * key,
                                       int                 match_value,
                                       int                 none_match_value,
                                       int                 not_found_value,
                                       oyAlloc_f           allocateFunc,
                                       oyDeAlloc_f         deAllocateFunc );
OYAPI int  OYEXPORT oyRankMapFromJSON( const char        * json_text,
                                       oyOptions_s       * options,
                                       oyRankMap        ** rank_map,
                                       oyAlloc_f           allocateFunc );
OYAPI int OYEXPORT oyRankMapToJSON   ( const oyRankMap   * rank_map,
                                       oyOptions_s       * options,
                                       char             ** json_text,
                                       oyAlloc_f           allocateFunc );
OYAPI int OYEXPORT oyRankMapList     ( const char        * filter,
                                       oyOptions_s       * options,
                                       char            *** rank_map_file_names,
                                       oyAlloc_f           allocateFunc );

/* } Include "Config.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CONFIG_S_H */
