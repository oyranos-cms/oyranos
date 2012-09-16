/** @internal
 *  @file oyranos_alpha_internal.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2004-2008  Kai-Uwe Behrmann
 *
 */

/**
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
#include "oyranos_module.h"
#include "oyObject_s_.h"
#include "oyCMMapi4_s.h"
#include "oyCMMapis_s.h"
#include "oyCMMapiFilters_s.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

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

int          oyPointer_ConvertData   ( oyPointer_s       * cmm_ptr,
                                       oyPointer_s       * cmm_ptr_out,
                                       oyFilterNode_s    * node );
oyPointer_s * oyStruct_GetModulePtr_ ( oyStruct_s        * data,
                                       const char        * cmm );
oyPointer_s** oyStructList_GetModulePtrs_ (
                                       oyStructList_s    * list,
                                       const char        * lib_name );


int          oyObject_UnRef          ( oyObject_s          obj );
int    oyOptions_SetDeviceTextKey_   ( oyOptions_s       * options,
                                       const char        * device_type,
                                       const char        * device_class,
                                       const char        * key,
                                       const char        * value );
char * oyDeviceRegistrationCreate_   ( const char        * device_type,
                                       const char        * device_class,
                                       const char        * key,
                                       char              * old_text );
int    oyOptions_SetRegistrationTextKey_(
                                       oyOptions_s       * options,
                                       const char        * registration,
                                       const char        * key,
                                       const char        * value );

int    oyTextIccDictMatch            ( const char        * text,
                                       const char        * pattern,
                                       double              delta );


oyFilterCore_s * oyFilterCore_New_   ( oyObject_s          object );
int          oyFilterCore_SetCMMapi4_( oyFilterCore_s    * s,
                                       oyCMMapi4_s       * cmm_api4 );

oyStructList_s * oyFilterNode_GetData_(oyFilterNode_s    * node,
                                       int                 get_plug );
int  oyColourConvert_ ( oyProfile_s       * p_in,
                        oyProfile_s       * p_out,
                        oyPointer           buf_in,
                        oyPointer           buf_out,
                        oyDATATYPE_e        buf_type_in,
                        oyDATATYPE_e        buf_type_out,
                        oyOptions_s       * options,
                        int                 count );

const char *   oyContextCollectData_ ( oyStruct_s        * s,
                                       oyOptions_s       * opts,
                                       oyStructList_s    * ins,
                                       oyStructList_s    * outs );

int          oyCMMdsoRelease_        ( const char        * lib_name );
int          oyCMMdsoSearch_         ( const char        * lib_name );
oyPointer    oyCMMdsoGet_            ( const char        * cmm,
                                       const char        * lib_name );


char *           oyCMMnameFromLibName_(const char        * lib_name);
char *           oyCMMInfoPrint_     ( oyCMMInfo_s       * cmm_info );
oyCMMInfo_s *    oyCMMOpen_          ( const char        * lib_name );
oyCMMapi_s *     oyCMMsGetApi__      ( oyOBJECT_e          type,
                                       const char        * lib_name,
                                       oyCMMapi_Check_f    apiCheck,
                                       oyPointer           check_pointer,
                                       int                 num );
oyCMMapi_s *     oyCMMsGetApi_       ( oyOBJECT_e          type,
                                       const char        * cmm_required,
                                       char             ** lib_used,
                                       oyCMMapi_Check_f    apiCheck,
                                       oyPointer           check_pointer );
oyCMMapis_s *    oyCMMsGetMetaApis_  ( const char        * cmm_required );
oyCMMapiFilters_s*oyCMMsGetFilterApis_(const char        * cmm_meta,
                                       const char        * cmm_required,
                                       const char        * registration,
                                       oyOBJECT_e          type,
                                       uint32_t            flags,
                                       uint32_t         ** rank_list,
                                       uint32_t          * count );
oyCMMapiFilter_s *oyCMMsGetFilterApi_( const char        * cmm_required,
                                       const char        * registration,
                                       oyOBJECT_e          type );
int    oyIsOfTypeCMMapiFilter        ( oyOBJECT_e          type );
oyCMMapi_s * oyCMMsGetApiFromRegistration_(
                                       oyOBJECT_e          type,
                                       const char        * cmm_required,
                                       const char        * registration );

oyCMMhandle_s *  oyCMMFromCache_     ( const char        * lib_name );
oyCMMInfo_s *    oyCMMGet_           ( const char        * cmm );
int              oyCMMRelease_       ( const char        * cmm );
unsigned int     oyCMMapiIsReady_    ( oyOBJECT_e          type );


uint32_t     oyCMMtoId               ( const char        * cmm );
int          oyIdToCMM               ( uint32_t            cmmId,
                                       char              * cmm );



#define hashTextAdd_m( text_ ) \
  oyStringAdd_( &hash_text, text_, s->oy_->allocateFunc_, \
                            s->oy_->deallocateFunc_ );

#define oyCheckType_m( typ, action ) \
  if( !s || s->type != typ) \
  { \
    WARNc3_S( "%s %s(%s)", _("Unexpected object type:"), \
              oyStructTypeToText( s ? s->type : oyOBJECT_NONE ), \
              oyStructTypeToText( typ )) \
    action; \
  }

void     oyAlphaFinish_              ( int                 unused );


/** \addtogroup misc Miscellaneous

 *  @{
 */


int      oySizeofDatatype            ( oyDATATYPE_e        t );
const char *   oyDatatypeToText      ( oyDATATYPE_e        t);
const char *   oyValueTypeText       ( oyVALUETYPE_e       type );


/** @} *//*alpha
 */

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_ALPHA_INTERNAL_H */
