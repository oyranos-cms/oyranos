/** @internal
 *  @file oyranos_cmm.h
 *
 *  Oyranos is an open source Colour Management System 
 * 
 *  @par Copyright:
 *            2007-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief external CMM module API
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2007/11/12
 */


#ifndef OYRANOS_CMM_H
#define OYRANOS_CMM_H

#include "oyranos.h"
#include "oyranos_alpha.h"
#include "oyPointer_s.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @brief   CMM capabilities query enum
 *  @ingroup module_api
 *
 *  @since: 0.1.8
 */
typedef enum {
  oyQUERY_OYRANOS_COMPATIBILITY,       /*!< provides the Oyranos version and expects the CMM compiled or compatibility Oyranos version back */
  oyQUERY_PROFILE_FORMAT = 20,         /*!< value 1 == ICC */
  oyQUERY_PROFILE_TAG_TYPE_READ,       /**< value a icTagTypeSignature (ICC) */
  oyQUERY_PROFILE_TAG_TYPE_WRITE,      /**< value a icTagTypeSignature (ICC) */
  oyQUERY_MAX
} oyCMMQUERY_e;

#define oyCMM_PROFILE "oyPR"
#define oyCMM_COLOUR_CONVERSION "oyCC"
#define oyCOLOUR_ICC_DEVICE_LINK "oyDL"


/** @brief   CMM data to Oyranos cache
 *
 *  @param[in]     data                the data struct know to the module
 *  @param[in,out] oy                  the Oyranos cache struct to fill by the module
 *  @return                            error
 *
 *  @version Oyranos: 0.1.9
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 *  @date    2008/11/06
 */
typedef int      (*oyCMMobjectOpen_f)( oyStruct_s        * data,
                                       oyPointer_s       * oy );

typedef void     (*oyCMMProgress_f)  ( int                 ID,
                                       double              progress );


typedef icSignature (*oyCMMProfile_GetSignature_f) (
                                       oyPointer_s       * cmm_ptr,
                                       int                 pcs);

int              oyCMMlibMatchesCMM  ( const char        * lib_name,
                                       const char        * cmm );

/*oyPointer          oyCMMallocateFunc ( size_t              size );
void               oyCMMdeallocateFunc(oyPointer           mem );*/


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
typedef const char* (*oyWidgetGet_f) ( uint32_t          * result );

/* -------------------------------------------------------------------------*/
typedef struct oyCMMapi5_s oyCMMapi5_s;

/* implemented filter functions */
int      oyFilterPlug_ImageRootRun   ( oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket );

const char * oyCMMgetText            ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context );
extern const char * oyCMM_texts[4];
const char * oyCMMgetImageConnectorPlugText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context );
const char * oyCMMgetImageConnectorSocketText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context );
extern const char * oy_image_connector_texts[2];

/* miscellaneous helpers */
int oyCMMWarnFunc( int code, const oyStruct_s * context, const char * format, ... );


/** @struct  oyObjectInfo_s
 *  @brief   a static object
 *  @extends oyStruct_s
 *
 *  The data field is to be placed alternatively to a oyObject_s at
 *  oyStruct_s::oy_ for a static object. It provides basic object data.
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/02 (Oyranos: 0.3.0)
 *  @date    2011/02/02
 */
typedef struct oyObjectInfoStatic_s {
  oyOBJECT_e       type;               /**< oyOBJECT_INFO_STATIC_S */
  oyPointer        dummy1;             /**< keep zero */
  oyPointer        dummy2;             /**< keep zero */
  oyPointer        dummy3;             /**< keep zero */
  /** inherited structures
   *  start with the position of the last member, followed by the first
   *  object type until the last one. Seven levels of inheritance are possible
   *  at maximum.
   */
  oyOBJECT_e       inheritance[8];
} oyObjectInfoStatic_s;

extern oyObjectInfoStatic_s oy_connector_imaging_static_object;

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_CMM_H */
