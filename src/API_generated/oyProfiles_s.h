/** @file oyProfiles_s.h

   [Template file inheritance graph]
   +-> oyProfiles_s.template.h
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



#ifndef OY_PROFILES_S_H
#define OY_PROFILES_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyProfiles_s oyProfiles_s;



#include "oyStruct_s.h"

#include "oyProfile_s.h"


/* Include "Profiles.public.h" { */
/** \addtogroup objects_profile
 *  @{ *//* objects_profile */
#define OY_ALLOW_DUPLICATES 0x100               /**< show as well duplicate profile ID's */
/** @} *//* objects_profile */

/* } Include "Profiles.public.h" */


/* Include "Profiles.dox" { */
/** @struct  oyProfiles_s
 *  @ingroup objects_profile
 *  @extends oyStruct_s
 *  @brief   Tell about the conversion profiles
 *
 *  @note This class holds a list of objects [list]
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */

/* } Include "Profiles.dox" */

struct oyProfiles_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};



OYAPI oyProfiles_s* OYEXPORT
  oyProfiles_New( oyObject_s object );
OYAPI oyProfiles_s* OYEXPORT
  oyProfiles_Copy( oyProfiles_s *profiles, oyObject_s obj );
OYAPI int OYEXPORT
  oyProfiles_Release( oyProfiles_s **profiles );

OYAPI int  OYEXPORT
           oyProfiles_MoveIn          ( oyProfiles_s       * list,
                                       oyProfile_s       ** ptr,
                                       int                 pos );
OYAPI int  OYEXPORT
           oyProfiles_ReleaseAt       ( oyProfiles_s       * list,
                                       int                 pos );
OYAPI oyProfile_s * OYEXPORT
           oyProfiles_Get             ( oyProfiles_s       * list,
                                       int                 pos );
OYAPI int  OYEXPORT
           oyProfiles_Count           ( oyProfiles_s       * list );
OYAPI int  OYEXPORT
           oyProfiles_Clear           ( oyProfiles_s       * list );
OYAPI int  OYEXPORT
           oyProfiles_Sort            ( oyProfiles_s       * list,
                                       int32_t           * rank_list );



/* Include "Profiles.public_methods_declarations.h" { */
OYAPI oyProfiles_s * OYEXPORT
                 oyProfiles_Create   ( oyProfiles_s      * patterns,
                                       uint32_t            flags,
                                       oyObject_s          object);
OYAPI oyProfiles_s * OYEXPORT
                 oyProfiles_ForStd   ( oyPROFILE_e         std_profile_class,
                                       uint32_t            flags,
                                       int               * current,
                                       oyObject_s          object);
OYAPI int  OYEXPORT
                 oyProfiles_DeviceRank(oyProfiles_s      * list,
                                       oyConfig_s        * device,
                                       int32_t           * rank_list );
OYAPI int OYEXPORT oyProfiles_Rank   ( oyProfiles_s      * list,
                                       oyConfig_s        * device,
                                       char                path_separator,
                                       char                key_separator,
                                       int                 flags,
                                       int32_t           * rank_list );

/* } Include "Profiles.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_PROFILES_S_H */
