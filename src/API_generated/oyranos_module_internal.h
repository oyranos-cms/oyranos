/** @file oyranos_module_internal.h

   [Template file inheritance graph]
   +-- oyranos_module_internal.template.h

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


#ifndef OYRANOS_MODULE_INTERNAL_H
#define OYRANOS_MODULE_INTERNAL_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#include "oyranos.h"
#include "oyranos_module.h"

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

#include "oyHash_s.h"
#include "oyCMMapi3_s.h"
#include "oyCMMapiFilter_s.h"
#include "oyCMMapiFilters_s.h"
#include "oyCMMapis_s.h"
#include "oyConversion_s.h"
#include "oyFilterNodes_s.h"
#include "oyFilterPlug_s.h"
#include "oyFilterPlugs_s.h"

#include "oyCMMapi_s.h"
#include "oyCMMinfo_s.h"
typedef  oyOBJECT_e(*oyCMMapi_Check_f)(oyCMMinfo_s       * cmm_info,
                                       oyCMMapi_s        * api,
                                       oyPointer           data,
                                       uint32_t          * rank );

/** typedef  oyCMMFilter_ValidateOptions_f
 *  @brief    a function to check and validate options
 *  @ingroup  module_api
 *  @memberof oyCMMapi4_s
 *  @memberof oyCMMapi5_s
 *
 *  @param[in]     filter              the filter
 *  @param[in]     validate            to validate
 *  @param[in]     statical            convert to a statical version
 *  @param[out]    ret                 0 if nothing changed otherwise >=1
 *  @return                            corrected options or zero
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/01/02 (Oyranos: 0.1.8)
 *  @date    2008/11/02
 */
typedef oyOptions_s * (*oyCMMFilter_ValidateOptions_f)
                                     ( oyFilterCore_s    * filter,
                                       oyOptions_s       * validate,
                                       int                 statical,
                                       uint32_t          * result );

/** typedef oyCMMFilterScan_f
 *  @brief   load a filter from a in memory data blob
 *  @ingroup module_api
 *  @memberof oyCMMapi5_s
 *
 *  @param[in]     data                filter data blob
 *  @param[in]     size                data size
 *  @param[in]     file_name           the filter file for information or zero
 *  @param[in]     type                filter type
 *  @param[in]     num                 number of filter
 *  @param[out]    registration        filter registration string
 *  @param[out]    name                filter name
 *  @param[in]     allocateFunc        e.g. malloc
 *  @param[out]    info                oyCMMinfo_s pointer to set
 *  @param[in]     object              e.g. Oyranos object
 *  @return                            0 on success; error >= 1; -1 not found; unknown < -1
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/11/22 (Oyranos: 0.1.9)
 *  @date    2008/12/17
 */
typedef int          (*oyCMMFilterScan_f) (
                                       oyPointer           data,
                                       size_t              size,
                                       const char        * file_name,
                                       oyOBJECT_e          type,
                                       int                 num,
                                       char             ** registration,
                                       char             ** name,
                                       oyAlloc_f           allocateFunc,
                                       oyCMMinfo_s      ** info,
                                       oyObject_s          object );

/** typedef oyCMMFilterLoad_f
 *  @brief   load a filter from a in memory data blob
 *  @ingroup module_api
 *  @memberof oyCMMapi5_s
 *
 *  @param[in]     data                data blob
 *  @param[in]     size                data size
 *  @return                            filter
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/11/22 (Oyranos: 0.1.9)
 *  @date    2008/12/28
 */
typedef oyCMMapiFilter_s * (*oyCMMFilterLoad_f) (
                                       oyPointer           data,
                                       size_t              size,
                                       const char        * file_name,
                                       oyOBJECT_e          type,
                                       int                 num );


/** @typedef oyWIDGET_EVENT_e
 *  @ingroup module_api
 */
typedef enum {
  oyWIDGET_OK,
  oyWIDGET_CORRUPTED,
  oyWIDGET_REDRAW,
  oyWIDGET_HIDE,
  oyWIDGET_SHOW,
  oyWIDGET_ACTIVATE,
  oyWIDGET_DEACTIVATE,
  oyWIDGET_UNDEFINED
} oyWIDGET_EVENT_e;

/** @typedef  oyWidgetEvent_f
 *  @ingroup  module_api
 */
typedef oyWIDGET_EVENT_e   (*oyWidgetEvent_f)
                                     ( oyOptions_s       * options,
                                       oyWIDGET_EVENT_e    type,
                                       oyStruct_s        * event );

#define OY_FILTEREDGE_FREE             0x01        /**< list free edges */
#define OY_FILTEREDGE_CONNECTED        0x02        /**< list connected edges */
#define OY_FILTEREDGE_LASTTYPE         0x04        /**< list last type edges */
/* decode */
#define oyToFilterEdge_Free_m(r)       ((r)&1)
#define oyToFilterEdge_Connected_m(r)  (((r) >> 1)&1)
#define oyToFilterEdge_LastType_m(r)   (((r) >> 2)&1)

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
  oyCMMinfo_s        * info;           /**< the modules info struct */
  oyPointer            dso_handle;     /**< the ldopen library handle */
} oyCMMhandle_s;

oyCMMhandle_s *  oyCMMhandle_New_    ( oyObject_s          object );
oyCMMhandle_s *  oyCMMhandle_Copy_   ( oyCMMhandle_s     * handle,
                                       oyObject_s          object );
int              oyCMMhandle_Release_( oyCMMhandle_s    ** handle );

int              oyCMMhandle_Set_    ( oyCMMhandle_s     * handle,
                                       oyCMMinfo_s       * info,
                                       oyPointer           dso_handle,
                                       const char        * lib_name );

oyCMMapiFilters_s * oyCMMsGetFilterApis_(const char        * registration,
                                         oyOBJECT_e          type,
                                         uint32_t            flags,
                                         uint32_t         ** rank_list,
                                         uint32_t          * count );
char *           oyCMMnameFromLibName_(const char        * lib_name);
char *           oyCMMinfoPrint_     ( oyCMMinfo_s       * cmm_info,
                                       int                 simple );
oyCMMinfo_s *    oyCMMOpen_          ( const char        * lib_name );
oyCMMhandle_s *  oyCMMFromCache_     ( const char        * lib_name );
int              oyCMMRelease_       ( const char        * cmm );

uint32_t     oyCMMtoId               ( const char        * cmm );
int          oyIdToCMM               ( uint32_t            cmmId,
                                       char              * cmm );

oyCMMapi_s *     oyCMMsGetApi__      ( oyOBJECT_e          type,
                                       const char        * lib_name,
                                       oyCMMapi_Check_f    apiCheck,
                                       oyPointer           check_pointer,
                                       int                 num );
oyCMMapi_s *     oyCMMsGetApi_       ( oyOBJECT_e          type,
                                       char             ** lib_used,
                                       oyCMMapi_Check_f    apiCheck,
                                       oyPointer           check_pointer );
oyHash_s *   oyCMMCacheListGetEntry_ ( const char        * hash_text );
oyStructList_s** oyCMMCacheList_     ( void );
char   *     oyCMMCacheListPrint_    ( void );
oyCMMapis_s *  oyCMMGetMetaApis_     ( );
int              oyCMMCanHandle_    ( oyCMMapi3_s        * api,
                                      oyCMMapiQueries_s  * queries );
char **          oyCMMsGetNames_     ( uint32_t          * n,
                                       const char        * sub_path,
                                       const char        * ext,
                                       oyPATH_TYPE_e       path_type );
int          oyCMMdsoReference_      ( const char        * lib_name,
                                       oyPointer           ptr );
int          oyCMMdsoRelease_        ( const char        * lib_name );
int          oyCMMdsoSearch_         ( const char        * lib_name );
oyPointer    oyCMMdsoGet_            ( const char        * lib_name );

char **     oyDataFilesGet_          ( int             * count,
                                       const char      * subdir,
                                       int               data,
                                       int               owner,
                                       const char      * dir_string,
                                       const char      * string,
                                       const char      * suffix,
                                       oyAlloc_f         allocateFunc );
char **     oyLibFilesGet_           ( int             * count,
                                       const char      * subdir,
                                       int               owner,
                                       const char      * dir_string,
                                       const char      * string,
                                       const char      * suffix,
                                       oyAlloc_f         allocateFunc );

const char * oyStruct_GetTextFromModule (
                                       oyStruct_s        * obj,
                                       oyNAME_e            name_type,
                                       uint32_t            flags );
int    oyIsOfTypeCMMapiFilter        ( oyOBJECT_e          type );

int    oyAdjacencyListAdd_           ( oyFilterPlug_s    * plug,
                                       oyFilterNodes_s   * nodes,
                                       oyFilterPlugs_s   * edges,
                                       const char        * selector,
                                       int                 flags );


char   oyCMMapiNumberToChar          ( oyOBJECT_e          api_number );
extern oyCMMinfo_s* (*oyCMMinfoFromLibName_p) (
                                       const char        * lib_name );
extern char**   (*oyCMMsGetLibNames_p)(uint32_t          * n );
oyCMMapiFilter_s* oyCMMsGetFilterApi_( const char        * registration,
                                       oyOBJECT_e          type );


void oyShowGraph_( oyFilterNode_s * c, const char * selector );




void               oyShowConversion_ ( oyConversion_s    * conversion,
                                       uint32_t            flags );


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OYRANOS_MODULE_INTERNAL_H */
