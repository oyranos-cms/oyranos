/** @file oyranos_cmm.h
 *
 *  Oyranos is an open source Colour Management System 
 * 
 *  Copyright (C) 2004-2008  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief external CMM API
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @license: new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2007/11/12
 */


#ifndef OYRANOS_CMM_H
#define OYRANOS_CMM_H

#include "oyranos.h"
#include "oyranos_alpha.h"

#ifdef __cplusplus
extern "C" {
namespace oyranos
{
#endif /* __cplusplus */

/** @brief CMM capabilities query enum
 *
 *  @since: 0.1.8
 */
typedef enum {
  oyQUERY_OYRANOS_COMPATIBILITY,       /*!< provides the Oyranos version and expects the CMM compiled or compatibility Oyranos version back */
  oyQUERY_PIXELLAYOUT_DATATYPE,        /*!< takes a oyDATATYPE_e arg as value */
  oyQUERY_PIXELLAYOUT_CHANNELCOUNT,
  oyQUERY_PIXELLAYOUT_SWAP_COLOURCHANNELS,
  oyQUERY_PIXELLAYOUT_COLOURCHANNEL_OFFSET,
  oyQUERY_PIXELLAYOUT_PLANAR,
  oyQUERY_PIXELLAYOUT_FLAVOUR,
  oyQUERY_HDR,                         /*!< value a oyDATATYPE_e (oyHALF...) */
  oyQUERY_PROFILE_FORMAT = 20,         /*!< value 1 == ICC */
  oyQUERY_PROFILE_TAG_TYPE_READ,       /**< value a icTagTypeSignature (ICC) */
  oyQUERY_PROFILE_TAG_TYPE_WRITE,      /**< value a icTagTypeSignature (ICC) */
  oyQUERY_MAX
} oyCMMQUERY_e;

typedef int      (*oyCMMCanHandle_t) ( oyCMMQUERY_e        type,
                                       uint32_t            value );

typedef int      (*oyCMMInit_t)      ( void );

/** @brief CMM pointer
 *
 *  The oyCMMptr_s is used internally and for CMM's.
 *  Memory management is done by Oyranos' oyAllocateFunc_ and oyDeallocateFunc_.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
typedef struct {
  oyOBJECT_TYPE_e      type;           /*!< internal struct type oyOBJECT_TYPE_CMM_POINTER_S */
  oyStruct_CopyF_t     copy;           /**< copy function */
  oyStruct_ReleaseF_t  release;        /**< release function */
  oyPointer        dummy;              /**< keep to zero */
  char                 cmm[5];         /*!< the CMM */
  char                 func_name[32];  /*!< optional the CMM's function name */
  oyPointer            ptr;            /*!< a CMM's data pointer */
  char                 resource[5];    /**< the resource type */
  oyStruct_releaseF_t  ptrRelease;     /*!< CMM's deallocation function */
  int                  ref;            /**< Oyranos reference counter */
} oyCMMptr_s;

/*typedef oyChar * (*oyCMMProfile_GetText_t)( oyCMMptr_s   * cmm_ptr,
                                       oyNAME_e            type,
                                       const char          language[4],
                                       const char          country[4],
                                       oyAllocFunc_t       allocateFunc );
*/
typedef int      (*oyCMMProfile_Open_t)( oyPointer         block,
                                       size_t              size,
                                       oyCMMptr_s        * oy );
#define oyCMM_PROFILE "oyPR"

typedef int      (*oyCMMColourConversion_Create_t) (
                                       oyCMMptr_s       ** cmm_profile_array,
                                       int                 profiles_n,
                                       uint32_t            pixel_layout_in,
                                       uint32_t            pixel_layout_out,
                                       int                 intent,
                                       int                 proofing_intent,
                                       uint32_t            flags,
                                       oyCMMptr_s        * oy );
typedef int      (*oyCMMColourConversion_FromMem_t) (
                                       oyPointer           mem,
                                       size_t              size,
                                       oyPixel_t           oy_pixel_layout_in,
                                       oyPixel_t           oy_pixel_layout_out,
                                       icColorSpaceSignature colour_space_in,
                                       icColorSpaceSignature colour_space_out,
                                       int                 intent,
                                       oyCMMptr_s        * oy );
typedef oyPointer(*oyCMMColourConversion_ToMem_t) (
                                       oyCMMptr_s        * oy,
                                       size_t            * size,
                                       oyAllocFunc_t       allocateFunc );
#define oyCMM_COLOUR_CONVERSION "oyCC"

typedef void     (*oyCMMProgress_t)  ( int                 ID,
                                       double              progress );

typedef int      (*oyCMMColourConversion_Run_t)(
                                       oyCMMptr_s        * cmm_transform,
                                       oyPointer           in_data,
                                       oyPointer           out_data,
                                       size_t              count,
                                       oyCMMProgress_t     progress );


typedef icSignature (*oyCMMProfile_GetSignature_t) (
                                       oyCMMptr_s        * cmm_ptr,
                                       int                 pcs);


/*oyPointer          oyCMMallocateFunc ( size_t              size );
void               oyCMMdeallocateFunc(oyPointer           mem );*/

typedef int      (*oyCMMMessageFuncSet_t)( oyMessageFunc_t message_func );

typedef oyCMMInfo_s* (*oyCMMInfo_Get_t) (void);

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

typedef oyOptions_s*(*oyOptions_Get_t)(oyOptions_s       * validate,
                                       uint32_t          * result );
typedef const char* (*oyWidgetGet_t) ( uint32_t          * result );
typedef oyWIDGET_EVENT_e   (*oyWidgetEvent_t)
                                     ( oyOptions_s       * options,
                                       oyWIDGET_EVENT_e    type,
                                       oyStruct_s        * event );



/** @brief the generic part if a API to implement and set by a CMM
 *
 *  @since Oyranos: version 0.1.8 2007/12/12
 *  @date  12 december 2007 (API 0.1.8)
 */
struct oyCMMapi_s {
  oyOBJECT_TYPE_e  type;               /**< struct type oyOBJECT_TYPE_CMM_API_S */
  oyPointer        dummya;             /**< keep to zero */
  oyPointer        dummyb;             /**< keep to zero */
  oyPointer        dummyc;             /**< keep to zero */
  oyCMMapi_s     * next;

  oyCMMInit_t      oyCMMInit;
  oyCMMMessageFuncSet_t oyCMMMessageFuncSet;
  oyCMMCanHandle_t oyCMMCanHandle;
};


/** @brief the API 1 to implement and set by a CMM
 *
 *  @since Oyranos: version 0.1.8 2007/12/05
 *  @date  21 december 2007 (API 0.1.8)
 */
typedef struct {
  oyOBJECT_TYPE_e  type;               /**< struct type oyOBJECT_TYPE_CMM_API1_S */
  oyPointer        dummya;             /**< keep to zero */
  oyPointer        dummyb;             /**< keep to zero */
  oyPointer        dummyc;             /**< keep to zero */
  oyCMMapi_s     * next;

  oyCMMInit_t      oyCMMInit;
  oyCMMMessageFuncSet_t oyCMMMessageFuncSet;
  oyCMMCanHandle_t oyCMMCanHandle;

  oyCMMProfile_Open_t oyCMMProfile_Open;
  /*oyCMMProfile_GetText_t oyCMMProfile_GetText;*/
  oyCMMProfile_GetSignature_t oyCMMProfile_GetSignature;
  oyCMMColourConversion_Create_t oyCMMColourConversion_Create;
  oyCMMColourConversion_FromMem_t oyCMMColourConversion_FromMem;
  oyCMMColourConversion_ToMem_t oyCMMColourConversion_ToMem;
  oyCMMColourConversion_Run_t oyCMMColourConversion_Run;
} oyCMMapi1_s;


typedef int   (*oyGetMonitorInfo_t)  ( const char        * display,
                                       char             ** manufacturer,
                                       char             ** model,
                                       char             ** serial,
                                       oyAllocFunc_t       allocate_func);
typedef int   (*oyGetScreenFromPosition_t) (
                                       const char        * display_name,
                                       int                 x,
                                       int                 y );
typedef char* (*oyGetDisplayNameFromPosition_t) (
                                       const char        * display_name,
                                       int                 x,
                                       int                 y,
                                       oyAllocFunc_t       allocate_func);
typedef char* (*oyGetMonitorProfile_t)(const char        * display,
                                       size_t            * size,
                                       oyAllocFunc_t       allocate_func);
typedef char* (*oyGetMonitorProfileName_t) (
                                       const char        * display,
                                       oyAllocFunc_t       allocate_func);
typedef int   (*oySetMonitorProfile_t)(const char        * display_name,
                                       const char        * profil_name );
typedef int   (*oyActivateMonitorProfiles_t) (
                                       const char        * display_name);


/** @struct oyCMMapi2_s
 *  @brief the API 2 to implement and set to provide windowing support
 *
 *  @since Oyranos: version 0.1.8
 *  @date  10 december 2007 (API 0.1.8)
 */
typedef struct {
  oyOBJECT_TYPE_e  type;               /**< struct type oyOBJECT_TYPE_CMM_API2_S */
  oyPointer        dummya;             /**< keep to zero */
  oyPointer        dummyb;             /**< keep to zero */
  oyPointer        dummyc;             /**< keep to zero */
  oyCMMapi_s     * next;

  oyCMMInit_t      oyCMMInit;
  oyCMMMessageFuncSet_t oyCMMMessageFuncSet;
  oyCMMCanHandle_t oyCMMCanHandle;

  oyGetMonitorInfo_t oyGetMonitorInfo;
  oyGetScreenFromPosition_t oyGetScreenFromPosition;

  oyGetDisplayNameFromPosition_t oyGetDisplayNameFromPosition;
  oyGetMonitorProfile_t oyGetMonitorProfile;
  oyGetMonitorProfileName_t oyGetMonitorProfileName;

  oySetMonitorProfile_t oySetMonitorProfile;
  oyActivateMonitorProfiles_t oyActivateMonitorProfiles;

} oyCMMapi2_s;


typedef oyStructList_s *    (*oyCMMProfileTag_GetValues_t) (
                                       oyProfileTag_s    * tag );
typedef int                 (*oyCMMProfileTag_Create_t) ( 
                                       oyProfileTag_s    * tag,
                                       oyStructList_s    * list,
                                       icTagTypeSignature  tag_type,
                                       uint32_t            version );

/** @struct oyCMMapi3_s
 *  @brief the API 3 to implement and set to provide low level ICC profile
 *         support
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/01/02 (Oyranos: 0.1.8)
 *  @date    2008/01/02
 */
typedef struct {
  oyOBJECT_TYPE_e  type;               /**< struct type oyOBJECT_TYPE_CMM_API3_S */
  oyPointer        dummya;             /**< keep to zero */
  oyPointer        dummyb;             /**< keep to zero */
  oyPointer        dummyc;             /**< keep to zero */
  oyCMMapi_s     * next;

  oyCMMInit_t      oyCMMInit;
  oyCMMMessageFuncSet_t oyCMMMessageFuncSet;
  oyCMMCanHandle_t oyCMMCanHandle;

  oyCMMProfileTag_GetValues_t oyCMMProfileTag_GetValues;
  oyCMMProfileTag_Create_t oyCMMProfileTag_Create;
} oyCMMapi3_s;


/** @type    oyCMMConversion_CreateBasic_t
 *  @brief   create a basic filter context from two images
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/06/24
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 */
typedef int      (*oyCMMConversion_Create_t) (
                                       oyFilter_s        * filter,
                                       oyCMMptr_s        * oy );


/** @struct oyCMMapi4_s
 *  @brief the API 4 to implement and set to provide Filter support
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 *  @date    2008/06/24
 */
typedef struct {
  oyOBJECT_TYPE_e  type;               /**< struct type oyOBJECT_TYPE_CMM_API3_S */
  oyPointer        dummya;             /**< keep to zero */
  oyPointer        dummyb;             /**< keep to zero */
  oyPointer        dummyc;             /**< keep to zero */
  oyCMMapi_s     * next;

  oyCMMInit_t      oyCMMInit;
  oyCMMMessageFuncSet_t oyCMMMessageFuncSet;
  oyCMMCanHandle_t oyCMMCanHandle;


  oyFILTER_TYPE_e  filter_type;        /**< filter type */
  const char     * registration;       /**< a registration name, e.g. "org.oyranos.lcms" */

  const char     * widgets;            /**< Oyranos XForms subset widgets */
  oyOptions_Get_t  oyOptions_Get;      /**< provide options */
  oyWidgetEvent_t  oyWidget_Event;     /**< handle widget events */

  oyName_s         name;               /**< translatable, eg "lcms" "little cms" "..." */
  const char       category[256];      /**< menu structure */
  oyOptions_s    * options;            /**< options */
  char *         * opts_ui_;           /**< xml ui elements for filter options*/


} oyCMMapi4_s;


#ifdef __cplusplus
} /* extern "C" */
} /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OYRANOS_CMM_H */
