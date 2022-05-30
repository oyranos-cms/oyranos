/** @file oyProfileTag_s.h

   [Template file inheritance graph]
   +-> oyProfileTag_s.template.h
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



#ifndef OY_PROFILE_TAG_S_H
#define OY_PROFILE_TAG_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyProfileTag_s oyProfileTag_s;



#include "oyStruct_s.h"

#include "icc34.h"
#include "oyStructList_s.h"


/* Include "ProfileTag.public.h" { */

/* } Include "ProfileTag.public.h" */


/* Include "ProfileTag.dox" { */
/** @struct  oyProfileTag_s
 *  @ingroup objects_profile
 *  @extends oyStruct_s
 *  @brief  A profile constituting element
 *  
 *  
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */

/* } Include "ProfileTag.dox" */

struct oyProfileTag_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};


OYAPI oyProfileTag_s* OYEXPORT
  oyProfileTag_New( oyObject_s object );
OYAPI oyProfileTag_s* OYEXPORT
  oyProfileTag_Copy( oyProfileTag_s *profiletag, oyObject_s obj );
OYAPI int OYEXPORT
  oyProfileTag_Release( oyProfileTag_s **profiletag );



/* Include "ProfileTag.public_methods_declarations.h" { */
OYAPI oyProfileTag_s * OYEXPORT
               oyProfileTag_Create   ( oyStructList_s    * list,
                                       icTagSignature      tag_use,
                                       icTagTypeSignature  tag_type,
                                       uint32_t            version,
                                       oyObject_s          object );
OYAPI oyProfileTag_s * OYEXPORT
               oyProfileTag_CreateFromText (
                                       const char        * text,
                                       icTagTypeSignature  tag_type,
                                       icTagSignature      tag_usage,
                                       oyObject_s          object );
OYAPI oyProfileTag_s * OYEXPORT
               oyProfileTag_CreateFromData ( 
                                       icTagSignature      sig,
                                       icTagTypeSignature  type,
                                       oySTATUS_e          status,
                                       size_t              tag_size,
                                       oyPointer           tag_block,
                                       oyObject_s          object );
OYAPI int  OYEXPORT
               oyProfileTag_Set      ( oyProfileTag_s    * tag,
                                       icTagSignature      sig,
                                       icTagTypeSignature  type,
                                       oySTATUS_e          status,
                                       size_t              tag_size,
                                       oyPointer         * tag_block );
OYAPI oyStructList_s  * OYEXPORT
                oyProfileTag_Get     ( oyProfileTag_s    * tag );
OYAPI char **  OYEXPORT
                oyProfileTag_GetText ( oyProfileTag_s    * tag,
                                       int32_t           * n,
                                       const char        * language,
                                       const char        * country,
                                       int32_t           * tag_size,
                                       oyAlloc_f           allocateFunc );
OYAPI int  OYEXPORT
                 oyProfileTag_GetBlock(oyProfileTag_s    * tag,
                                       oyPointer         * tag_block,
                                       size_t            * tag_size,
                                       oyAlloc_f           allocateFunc );
OYAPI size_t  OYEXPORT
               oyProfileTag_GetOffset( oyProfileTag_s    * tag );
OYAPI int  OYEXPORT
               oyProfileTag_SetOffset( oyProfileTag_s    * tag,
                                       size_t              offset );
OYAPI oySTATUS_e  OYEXPORT
               oyProfileTag_GetStatus( oyProfileTag_s    * tag );
OYAPI int  OYEXPORT
               oyProfileTag_SetStatus( oyProfileTag_s    * tag,
                                       oySTATUS_e          status );
OYAPI const char *  OYEXPORT
               oyProfileTag_GetCMM   ( oyProfileTag_s    * tag );
OYAPI int  OYEXPORT
               oyProfileTag_SetCMM   ( oyProfileTag_s    * tag,
                                       const char        * cmm );
OYAPI const char *  OYEXPORT
               oyProfileTag_GetLastCMM(oyProfileTag_s    * tag );
OYAPI int  OYEXPORT
               oyProfileTag_SetLastCMM(oyProfileTag_s    * tag,
                                       const char        * cmm );
OYAPI const char *  OYEXPORT
               oyProfileTag_GetRequiredCMM (
                                       oyProfileTag_s    * tag );
OYAPI int  OYEXPORT
               oyProfileTag_SetRequiredCMM (
                                       oyProfileTag_s    * tag,
                                       const char        * cmm );
OYAPI icTagSignature  OYEXPORT
               oyProfileTag_GetUse   ( oyProfileTag_s    * tag );
OYAPI icTagTypeSignature  OYEXPORT
               oyProfileTag_GetType  ( oyProfileTag_s    * tag );
OYAPI size_t  OYEXPORT
               oyProfileTag_GetSize  ( oyProfileTag_s    * tag );
OYAPI size_t  OYEXPORT
               oyProfileTag_GetSizeCheck (
                                       oyProfileTag_s    * tag );
OYAPI void  OYEXPORT
               oyProfileTag_SetSizeCheck (
                                       oyProfileTag_s    * tag,
                                       size_t              size_check );

/* } Include "ProfileTag.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_PROFILE_TAG_S_H */
