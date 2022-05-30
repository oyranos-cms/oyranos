/** @file oyProfile_s_.h

   [Template file inheritance graph]
   +-> oyProfile_s_.template.h
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


#ifndef OY_PROFILE_S__H
#define OY_PROFILE_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyProfilePriv_m( var ) ((oyProfile_s_*) (var))

typedef struct oyProfile_s_ oyProfile_s_;



#include <icc34.h>
  
#include <oyranos_object.h>


#include "oyStructList_s.h"
#include "oyProfileTag_s.h"
#include "oyConfig_s.h"
  

#include "oyProfile_s.h"

/* Include "Profile.private.h" { */

/* } Include "Profile.private.h" */

/** @internal
 *  @struct   oyProfile_s_
 *  @brief      A profile and its attributes
 *  @ingroup  objects_profile
 *  @extends  oyStruct_s
 */
struct oyProfile_s_ {

/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */



/* Include "Profile.members.h" { */
  char               * file_name_;     /*!< @private file name for loading on request */
  size_t               size_;          /*!< @private ICC profile size */
  void               * block_;         /*!< @private ICC profile data */
  icColorSpaceSignature sig_;          /*!< @private ICC profile signature */
  oyPROFILE_e          use_default_;   /*!< @private if > 0 : take from settings */
  oyObject_s         * names_chan_;    /*!< @private user visible channel description */
  int                  channels_n_;    /*!< @private number of channels */
  oyStructList_s     * tags_;          /**< @private list of header + tags */
  int                  tags_modified_; /**< @private note manipulation */
  oyConfig_s         * meta_;          /**< @private parsed meta tag */

/* } Include "Profile.members.h" */

};


oyProfile_s_*
  oyProfile_New_( oyObject_s object );
oyProfile_s_*
  oyProfile_Copy_( oyProfile_s_ *profile, oyObject_s object);
oyProfile_s_*
  oyProfile_Copy__( oyProfile_s_ *profile, oyObject_s object);
int
  oyProfile_Release_( oyProfile_s_ **profile );



/* Include "Profile.private_methods_declarations.h" { */
oyProfile_s_* oyProfile_FromMemMove_  ( size_t              size,
                                       oyPointer         * block,
                                       int                 flags,
                                       int               * error_return,
                                       oyObject_s          object);
oyProfile_s_ *  oyProfile_FromFile_  ( const char        * name,
                                       uint32_t            flags,
                                       oyObject_s          object );
oyPointer    oyProfile_TagsToMem_    ( oyProfile_s_      * profile,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc );
int          oyProfile_ToFile_       ( oyProfile_s_      * profile,
                                       const char        * file_name );
int32_t      oyProfile_Match_        ( oyProfile_s_      * pattern,
                                       oyProfile_s_      * profile );
int32_t      oyProfile_Hashed_       ( oyProfile_s_      * s );
char *       oyProfile_GetFileName_r ( oyProfile_s_      * profile,
                                       uint32_t            flags,
                                       oyAlloc_f           allocateFunc );
oyProfileTag_s * oyProfile_GetTagByPos_( oyProfile_s_    * profile,
                                       int                 pos );
int             oyProfile_GetTagCount_ ( oyProfile_s_      * profile );
int          oyProfile_TagMoveIn_    ( oyProfile_s_      * profile,
                                       oyProfileTag_s   ** obj,
                                       int                 pos );
int          oyProfile_TagReleaseAt_ ( oyProfile_s_      * profile,
                                       int                 pos );
oyPointer    oyProfile_WriteHeader_  ( oyProfile_s_      * profile,
                                       size_t            * size );
oyPointer    oyProfile_WriteTags_    ( oyProfile_s_      * profile,
                                       size_t            * size,
                                       oyPointer           icc_header,
                                       oyPointer           icc_list,
                                       oyAlloc_f           allocateFunc );
oyPointer    oyProfile_WriteTagTable_( oyProfile_s_      * profile,
                                       size_t            * size );

int oyProfile_HasID_          ( oyProfile_s_      * s );
int oyProfile_GetHash_        ( oyProfile_s_      * s,
                                       int                 flags );
#if 0
oyChar *       oyProfile_GetCMMText_ ( oyProfile_s       * profile,
                                       oyNAME_e            type,
                                       const char        * language,
                                       const char        * country );
#endif

/* } Include "Profile.private_methods_declarations.h" */



void oyProfile_Release__Members( oyProfile_s_ * profile );
int oyProfile_Init__Members( oyProfile_s_ * profile );
int oyProfile_Copy__Members( oyProfile_s_ * dst, oyProfile_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_PROFILE_S__H */
