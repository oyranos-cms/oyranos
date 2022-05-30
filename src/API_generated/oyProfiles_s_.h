/** @file oyProfiles_s_.h

   [Template file inheritance graph]
   +-> oyProfiles_s_.template.h
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


#ifndef OY_PROFILES_S__H
#define OY_PROFILES_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyProfilesPriv_m( var ) ((oyProfiles_s_*) (var))

typedef struct oyProfiles_s_ oyProfiles_s_;

  
#include <oyranos_object.h>

#include "oyStructList_s.h"
  

#include "oyProfiles_s.h"

/* Include "Profiles.private.h" { */

/* } Include "Profiles.private.h" */

/** @internal
 *  @struct   oyProfiles_s_
 *  @brief      Tell about the conversion profiles
 *  @ingroup  objects_profile
 *  @extends  oyStruct_s
 */
struct oyProfiles_s_ {

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



oyProfiles_s_*
  oyProfiles_New_( oyObject_s object );
oyProfiles_s_*
  oyProfiles_Copy_( oyProfiles_s_ *profiles, oyObject_s object);
oyProfiles_s_*
  oyProfiles_Copy__( oyProfiles_s_ *profiles, oyObject_s object);
int
  oyProfiles_Release_( oyProfiles_s_ **profiles );

#if 0
/* FIXME Are these needed at all?? */
int
           oyProfiles_MoveIn_         ( oyProfiles_s_      * list,
                                       oyProfile_s       ** ptr,
                                       int                 pos );
int
           oyProfiles_ReleaseAt_      ( oyProfiles_s_      * list,
                                       int                 pos );
oyProfile_s *
           oyProfiles_Get_            ( oyProfiles_s_      * list,
                                       int                 pos );
int
           oyProfiles_Count_          ( oyProfiles_s_      * list );
#endif



/* Include "Profiles.private_methods_declarations.h" { */

/* } Include "Profiles.private_methods_declarations.h" */




#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_PROFILES_S__H */
