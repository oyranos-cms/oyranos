/** @file oyProfileTag_s_.h

   [Template file inheritance graph]
   +-> oyProfileTag_s_.template.h
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


#ifndef OY_PROFILE_TAG_S__H
#define OY_PROFILE_TAG_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyProfileTagPriv_m( var ) ((oyProfileTag_s_*) (var))

typedef struct oyProfileTag_s_ oyProfileTag_s_;

  
#include <oyranos_object.h>


#include <icc34.h>
#include <oyranos_icc.h>
  

#include "oyProfileTag_s.h"

/* Include "ProfileTag.private.h" { */

/* } Include "ProfileTag.private.h" */

/** @internal
 *  @struct   oyProfileTag_s_
 *  @brief     A profile constituting element
 *  @ingroup  objects_profile
 *  @extends  oyStruct_s
 */
struct oyProfileTag_s_ {

/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */



/* Include "ProfileTag.members.h" { */
  icTagSignature       use;            /**< tag functionality inside profile */
  icTagTypeSignature   tag_type_;      /**< @private tag type to decode the data block */

  oySTATUS_e           status_;        /**< @private status at load time */

  size_t               offset_orig;    /**< information from profile read */
  size_t               size_;          /**< @private data block size */
  size_t               size_check_;    /**< @private calculated data block size */
  oyPointer            block_;         /**< @private the data to interprete */

  char                 profile_cmm_[5];/**< @private the profile prefered CMM */
  char                 required_cmm[5];/**< selected a certain CMM */
  char                 last_cmm_[5];   /**< info: last processing CMM */

/* } Include "ProfileTag.members.h" */

};


oyProfileTag_s_*
  oyProfileTag_New_( oyObject_s object );
oyProfileTag_s_*
  oyProfileTag_Copy_( oyProfileTag_s_ *profiletag, oyObject_s object);
oyProfileTag_s_*
  oyProfileTag_Copy__( oyProfileTag_s_ *profiletag, oyObject_s object);
int
  oyProfileTag_Release_( oyProfileTag_s_ **profiletag );



/* Include "ProfileTag.private_methods_declarations.h" { */

/* } Include "ProfileTag.private_methods_declarations.h" */



void oyProfileTag_Release__Members( oyProfileTag_s_ * profiletag );
int oyProfileTag_Init__Members( oyProfileTag_s_ * profiletag );
int oyProfileTag_Copy__Members( oyProfileTag_s_ * dst, oyProfileTag_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_PROFILE_TAG_S__H */
