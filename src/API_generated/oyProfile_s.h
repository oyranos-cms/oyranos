/** @file oyProfile_s.h

   [Template file inheritance graph]
   +-> oyProfile_s.template.h
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



#ifndef OY_PROFILE_S_H
#define OY_PROFILE_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */




#include <icc34.h>
  
#include <oyranos_object.h>

typedef struct oyProfile_s oyProfile_s;



#include "oyStruct_s.h"

#include "oyranos.h"
#include "oyranos_profile.h"
#include "oyProfileTag_s.h"
#include "oyConfig_s.h"


/* Include "Profile.public.h" { */
/** \addtogroup objects_profile ICC Profile APIs
 *
 *  @{ *//* objects_profile */
#define OY_NO_CACHE_READ            0x01        /**< read not from cache */
#define OY_NO_CACHE_WRITE           0x02        /**< write not from cache */
/* decode */
#define oyToNoCacheRead_m(r)        ((r)&1)
#define oyToNoCacheWrite_m(w)       (((w) >> 1)&1)

#define OY_FROM_PROFILE 0x04                    /**< use from profile */
#define OY_COMPUTE      0x08                    /**< compute newly */
#define OY_ICC_VERSION_2 0x10                   /**< filter for version 2 profiles */
#define OY_ICC_VERSION_4 0x20                   /**< filter for version 4 profiles */
#define OY_SKIP_NON_DEFAULT_PATH 0x40           /**< ignore profiles outside of default paths */
#define OY_NO_REPAIR     0x80                   /**< do not try to repair or fix profiles */
#define OY_NO_LOAD       0x100                  /**< do not load profile, create fragment */
#define OY_SKIP_MTIME_CHECK 0x200               /**< ignore checking time stamp; old bahaviour for faster caching */
/** @} *//* objects_profile */

/* } Include "Profile.public.h" */


/* Include "Profile.dox" { */
/** @struct  oyProfile_s
 *  @ingroup objects_profile
 *  @extends oyStruct_s
 *  @brief   A profile and its attributes
 *  
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */

/* } Include "Profile.dox" */

struct oyProfile_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};


OYAPI oyProfile_s* OYEXPORT
  oyProfile_New( oyObject_s object );
OYAPI oyProfile_s* OYEXPORT
  oyProfile_Copy( oyProfile_s *profile, oyObject_s obj );
OYAPI int OYEXPORT
  oyProfile_Release( oyProfile_s **profile );



/* Include "Profile.public_methods_declarations.h" { */
OYAPI oyProfile_s * OYEXPORT
                   oyProfile_FromStd ( oyPROFILE_e         type,
                                       uint32_t            flags,
                                       oyObject_s          object);
OYAPI oyProfile_s * OYEXPORT
                   oyProfile_FromName( const char*         name,
                                       uint32_t            flags,
                                       oyObject_s          object);
OYAPI oyProfile_s * OYEXPORT
                   oyProfile_FromFile( const char*         file_name,
                                       uint32_t            flags,
                                       oyObject_s          object);
OYAPI oyProfile_s * OYEXPORT
                   oyProfile_FromMatrix (
                                       double              pandg[9],
                                       int                 icc_profile_flags,
                                       const char        * name  );
OYAPI oyProfile_s * OYEXPORT
                   oyProfile_FromMem ( size_t              buf_size,
                                       const oyPointer     buf,
                                       uint32_t            flags,
                                       oyObject_s          object);
OYAPI oyProfile_s * OYEXPORT
                   oyProfile_FromSignature(
                                       icSignature         sig,
                                       oySIGNATURE_TYPE_e  type,
                                       oyObject_s          object );
OYAPI oyProfile_s * OYEXPORT
                   oyProfile_FromMD5(  uint32_t          * md5,
                                       uint32_t            flags,
                                       oyObject_s          object );
OYAPI oyProfile_s * OYEXPORT
                   oyProfile_FromTaxiDB (
                                       oyOptions_s       * options,
                                       oyObject_s          object );
OYAPI int OYEXPORT oyProfile_Install ( oyProfile_s       * profile,
                                       oySCOPE_e           scope,
                                       oyOptions_s       * options );
OYAPI int OYEXPORT
         oyProfile_GetChannelsCount ( oyProfile_s * color );
OYAPI icSignature OYEXPORT
             oyProfile_GetSignature (  oyProfile_s       * profile,
                                       oySIGNATURE_TYPE_e  type );
OYAPI int OYEXPORT
             oyProfile_SetSignature (  oyProfile_s       * profile,
                                       icSignature         sig,
                                       oySIGNATURE_TYPE_e  type );
OYAPI void OYEXPORT
            oyProfile_SetChannelNames( oyProfile_s * color,
                                       oyObject_s        * names_chan );
OYAPI const oyObject_s * OYEXPORT
                   oyProfile_GetChannelNames( oyProfile_s * color);
OYAPI const char   * OYEXPORT
                   oyProfile_GetChannelName ( oyProfile_s * profile,
                                         int               channel_pos,
                                         oyNAME_e          type );
OYAPI const char  * OYEXPORT
                   oyProfile_GetID   ( oyProfile_s       * profile );
OYAPI int OYEXPORT
                   oyProfile_Equal   ( oyProfile_s       * profileA,
                                       oyProfile_s       * profileB );
OYAPI const char  * OYEXPORT
                   oyProfile_GetText ( oyProfile_s       * profile,
                                       oyNAME_e            type );
OYAPI oyPointer OYEXPORT
                   oyProfile_GetMem  ( oyProfile_s       * profile,
                                       size_t            * size,
                                       uint32_t            flag,
                                       oyAlloc_f           allocateFunc );
OYAPI size_t OYEXPORT
                   oyProfile_GetSize ( oyProfile_s       * profile,
                                       uint32_t            flag );
OYAPI oyProfileTag_s * OYEXPORT
               oyProfile_GetTagByPos ( oyProfile_s   * profile,
                                       int                 pos );
OYAPI oyProfileTag_s * OYEXPORT
                oyProfile_GetTagById ( oyProfile_s    * profile,
                                       icTagSignature      id );
OYAPI int OYEXPORT
                   oyProfile_GetTagCount( oyProfile_s    * profile );
OYAPI int OYEXPORT
                   oyProfile_TagMoveIn(oyProfile_s       * profile,
                                       oyProfileTag_s   ** tag,
                                       int                 pos );
OYAPI int OYEXPORT
                   oyProfile_TagReleaseAt ( oyProfile_s  * profile,
                                       int                 pos );
OYAPI int OYEXPORT
                   oyProfile_AddTagText ( oyProfile_s       * profile,
                                          icSignature         signature,
                                          const char        * text );
OYAPI const char * OYEXPORT
                   oyProfile_GetFileName ( oyProfile_s   * profile,
                                           int             dl_pos );
OYAPI int OYEXPORT
                   oyProfile_GetDevice ( oyProfile_s     * profile,
                                         oyConfig_s      * device );
OYAPI int  OYEXPORT
                 oyProfile_GetMD5    ( oyProfile_s       * profile,
                                       int                 flags,
                                       uint32_t          * md5 );
OYAPI int  OYEXPORT oyProfile_GetWhitePoint (
                                       oyProfile_s       * profile,
                                       double            * CIE_XYZ );
const char * oyProfile_FindMeta      ( oyProfile_s       * profile,
                                       const char        * key,
                                       const char        * value );
OYAPI int  OYEXPORT
                 oyProfile_AddDevice(oyProfile_s       * profile,
                                       oyConfig_s        * device,
                                       oyOptions_s       * options );

/* } Include "Profile.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_PROFILE_S_H */
