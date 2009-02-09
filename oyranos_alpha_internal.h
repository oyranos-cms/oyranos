/** @file oyranos_alpha_internal.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2004-2008  Kai-Uwe Behrmann
 *
 */

/**
 *  @internal
 *  @brief    misc alpha internal APIs
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *  new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2004/11/25
 */

#ifndef OYRANOS_ALPHA_INTERNAL_H
#define OYRANOS_ALPHA_INTERNAL_H

#include "oyranos_alpha.h"
#include "oyranos_cmm.h"
#include "oyranos_cmms.h"

/** \addtogroup alpha Alpha API's

 *  @{
 */

/** @internal
 *  @brief a CMM handle to collect resources
 *
 *  @since Oyranos: version 0.1.8
 *  @date  5 december 2007 (API 0.1.8)
 */
typedef struct {
  oyOBJECT_e           type_;          /**< internal struct type oyOBJECT_CMM_HANDLE_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */
  char               * lib_name;       /**< the CMM */
  oyCMMInfo_s        * info;           /**< the modules info struct */
  oyPointer            dso_handle;     /**< the ldopen library handle */
} oyCMMhandle_s;

oyCMMhandle_s *  oyCMMhandle_New_    ( oyObject_s          object );
oyCMMhandle_s *  oyCMMhandle_Copy_   ( oyCMMhandle_s     * handle,
                                       oyObject_s          object );
int              oyCMMhandle_Release_( oyCMMhandle_s    ** handle );

int              oyCMMhandle_Set_    ( oyCMMhandle_s     * handle,
                                       oyCMMInfo_s       * info,
                                       oyPointer           dso_handle,
                                       const char        * lib_name );

oyCMMptr_s *       oyCMMptr_New_     ( oyAlloc_f           allocateFunc );
oyCMMptr_s *       oyCMMptr_Copy_    ( oyCMMptr_s        * cmm_ptr,
                                       oyAlloc_f           allocateFunc );
int                oyCMMptr_Release_ ( oyCMMptr_s       ** cmm_ptr );

int                oyCMMptr_Set_     ( oyCMMptr_s        * cmm_ptr,
                                       const char        * lib_name,
                                       const char        * resource,
                                       oyPointer           ptr,
                                       const char        * func_name,
                                       oyPointer_release_f ptrRelease );
int          oyCMMptr_ConvertData    ( oyCMMptr_s        * cmm_ptr,
                                       oyCMMptr_s        * cmm_ptr_out,
                                       oyFilterNode_s    * node );
oyCMMptr_s * oyStruct_GetCMMPtr_     ( oyStruct_s        * data,
                                       const char        * cmm );
oyCMMptr_s** oyStructList_GetCMMptrs_( oyStructList_s    * list,
                                       const char        * lib_name );


void         oyOption_UpdateFlags_   ( oyOption_s        * s );
int    oyOptions_SetInstrumentTextKey_(oyOptions_s       * options,
                                       const char        * instrument_type,
                                       const char        * instrument_class,
                                       const char        * key,
                                       const char        * value );
char * oyInstrumentRegistrationCreate_(const char        * instrument_type,
                                       const char        * instrument_class,
                                       const char        * key,
                                       char              * old_text );
int    oyOptions_SetRegistrationTextKey_(
                                       oyOptions_s       * options,
                                       const char        * registration,
                                       const char        * key,
                                       const char        * value );

oyProfile_s* oyProfile_FromMemMove_  ( size_t              size,
                                       oyPointer         * block,
                                       int                 flags,
                                       oyObject_s          object);
oyProfile_s *  oyProfile_FromFile_   ( const char        * name,
                                       uint32_t            flags,
                                       oyObject_s          object );
oyPointer    oyProfile_TagsToMem_    ( oyProfile_s       * profile,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc );
int          oyProfile_ToFile_       ( oyProfile_s       * profile,
                                       const char        * file_name );
int32_t      oyProfile_Match_        ( oyProfile_s       * pattern,
                                       oyProfile_s       * profile );
int32_t      oyProfile_Hashed_       ( oyProfile_s       * s );
char *       oyProfile_GetFileName_r ( oyProfile_s       * profile,
                                       oyAlloc_f           allocateFunc );
oyProfileTag_s * oyProfile_GetTagByPos_( oyProfile_s     * profile,
                                       int                 pos );
int          oyProfile_TagMoveIn_    ( oyProfile_s       * profile,
                                       oyProfileTag_s   ** obj,
                                       int                 pos );

oyFilter_s * oyFilter_New_           ( oyObject_s          object );
int          oyFilter_SetCMMapi4_    ( oyFilter_s        * s,
                                       oyCMMapi4_s       * cmm_api4 );

oyStructList_s * oyFilterNode_DataGet_(oyFilterNode_s    * node,
                                       int                 get_plug );

oyColourConversion_s* oyColourConversion_Create_ (
                                       oyOptions_s       * opts,
                                       oyImage_s         * in,
                                       oyImage_s         * out,
                                       oyObject_s          object);
oyPointer    oyColourConversion_ToMem_(
                                       oyColourConversion_s * oy,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc );
const char *   oyColourContextGetID_ ( oyStruct_s      * s,
                                       oyOptions_s     * opts,
                                       oyImage_s       * in,
                                       oyImage_s       * out);
const char *   oyContextCollectData_ ( oyStruct_s        * s,
                                       oyOptions_s       * opts,
                                       oyStructList_s    * ins,
                                       oyStructList_s    * outs );

int          oyCMMdsoRelease_        ( const char        * lib_name );
int          oyCMMdsoSearch_         ( const char        * lib_name );
oyPointer    oyCMMdsoGet_            ( const char        * cmm,
                                       const char        * lib_name );

/**
 *  @internal
 *  @enum    oyREQUEST_e
 *  @brief   grade of request complying
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/00/00 (Oyranos: 0.1.8)
 *  @date    2008/00/00
 */
typedef enum {
  oyREQUEST_NONE,                      /**< deactives the request */
  oyREQUEST_SLIGTH,
  oyREQUEST_MID,
  oyREQUEST_MUCH,
  oyREQUEST_HARD                       /**< requirement, fail if not present */
} oyREQUEST_e;

/**
 *  @internal
 *  @struct oyCMMapiQuery_s
 *  @brief  check/prepare a module
 *
 *  @since  Oyranos: version 0.1.8
 *  @date   2008/01/03 (API 0.1.8)
 */
typedef struct {
  oyCMMQUERY_e         query;
  uint32_t             value;
  oyREQUEST_e          request;
} oyCMMapiQuery_s;

/**
 *  @internal
 *  @struct oyCMMapiQuerie_s
 *  @brief  check/prepare a module
 *
 *  @since  Oyranos: version 0.1.8
 *  @date   2008/01/03 (API 0.1.8)
 */
typedef struct {
  int                  n;
  oyCMMapiQuery_s   ** queries;
  char                 prefered_cmm[5];
} oyCMMapiQueries_s;


typedef  oyOBJECT_e(*oyCMMapi_Check_f)(oyCMMapi_s        * api,
                                       oyPointer           data );
char *           oyCMMnameFromLibName_(const char        * lib_name);
oyCMMInfo_s *    oyCMMInfoFromLibName_(const char        * lib_name );
char *           oyCMMInfoPrint_     ( oyCMMInfo_s       * cmm_info );
oyCMMInfo_s *    oyCMMOpen_          ( const char        * lib_name );
oyCMMapi_s *     oyCMMsGetApi__      ( oyOBJECT_e          type,
                                       const char        * lib_name,
                                       oyCMMapi_Check_f    apiCheck,
                                       oyPointer           check_pointer,
                                       int                 num );
oyCMMapi_s *     oyCMMsGetApi_       ( oyOBJECT_e          type,
                                       const char        * cmm_required,
                                       oyCMMapiQueries_s * capabilities,
                                       char             ** lib_used,
                                       oyCMMapi_Check_f    apiCheck,
                                       oyPointer           check_pointer );
oyCMMapi5_s *oyCMMGetMetaApi_        ( const char        * cmm_required,
                                       oyCMMapiQueries_s * queries,
                                       const char        * registration );
oyCMMapiFilters_s*oyCMMsGetFilterApis_(const char        * cmm_required,
                                       oyCMMapiQueries_s * queries,
                                       const char        * registration,
                                       oyOBJECT_e          type,
                                       uint32_t         ** rank_list,
                                       uint32_t          * count );
oyCMMapiFilter_s *oyCMMsGetFilterApi_( const char        * cmm_required,
                                       oyCMMapiQueries_s * queries,
                                       const char        * registration,
                                       oyOBJECT_e          type );
int    oyIsOfTypeCMMapiFilter        ( oyOBJECT_e          type );
oyCMMapiBase_s *oyCMMsGetApiFromRegistration_(
                                       oyOBJECT_e          type,
                                       const char        * cmm_required,
                                       oyCMMapiQueries_s * queries,
                                       const char        * registration );

oyOBJECT_e       oyCMMapi_Check_     ( oyCMMapi_s        * api );
oyCMMhandle_s *  oyCMMFromCache_     ( const char        * lib_name );
oyCMMInfo_s *    oyCMMGet_           ( const char        * cmm );
int              oyCMMRelease_       ( const char        * cmm );
unsigned int     oyCMMapiIsReady_    ( oyOBJECT_e          type );


int          oyPointerReleaseFunc_   ( oyPointer         * ptr );

int          oySizeofDatatype        ( oyDATATYPE_e        t );

uint32_t     oyCMMtoId               ( const char        * cmm );
int          oyIdToCMM               ( uint32_t            cmmId,
                                       char              * cmm );



#define hashTextAdd_m( text_ ) \
  oyStringAdd_( &hash_text, text_, s->oy_->allocateFunc_, \
                            s->oy_->deallocateFunc_ );

/** \addtogroup misc Miscellaneous

 *  @{
 */

/** @internal
 *  pixel mask description 
 */
enum {
  oyLAYOUT = 0, /* remembering the layout */
  oyPOFF_X,     /* pixel count x offset */
  oyPOFF_Y,     /* pixel count y offset */
  oyCOFF,       /* channel offset */
  oyDATA_SIZE,  /* sample size in byte */
  oyCHANS,      /* number of channels */
  oyCHAN0       /* first colour channel */
};



/** @} *//*alpha
 */

#endif /* OYRANOS_ALPHA_INTERNAL_H */
