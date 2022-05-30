
/** @file oyranos_module.h

   [Template file inheritance graph]
   +-- oyranos_module.template.h

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


#ifndef OYRANOS_MODULE_H
#define OYRANOS_MODULE_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#include "oyFilterCore_s.h"
#include "oyOptions_s.h"

typedef struct oyIcon_s oyIcon_s;

const char * oyCMMgetImageConnectorPlugText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context );
const char * oyCMMgetImageConnectorSocketText (
                                       const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context );
extern const char * oy_image_connector_texts[2];

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

/**
 *  typedef oyCMMGetText_f
 *  @brief   get strings from a CMM
 *  @ingroup module_api
 *  @memberof oyCMMapi_s
 *
 *  type:
 *  - oyNAME_NICK: compact, e.g. "GPU"
 *  - oyNAME_NAME: a short explanation, e.g. "Rendering with GPU"
 *  - oyNAME_DESCRIPTION: a long explanation, e.g. "Accelerated calculations on a GPU"
 *
 *  @param         select              one from "name", "manufacturer" or "copyright"
 *  @param         type                select flavour
 *  @param         context             the object to ask for a optional context
 *                                     This will typical be the object to which
 *                                     this function belongs, but can be stated
 *                                     otherwise.
 *  @return                            text string or zero
 *
 *  @version Oyranos: 0.3.0
 *  @since   2008/12/23 (Oyranos: 0.1.10)
 *  @date    2011/02/01
 */
typedef const char*(* oyCMMGetText_f)( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context );


#define oyCMM_PROFILE "oyPR"
#define oyCMM_COLOR_CONVERSION "oyCC"
#define oyCOLOR_ICC_DEVICE_LINK "oyDL"             /**< A color transformation in the format of a ICC device link profile. */


#define OY_FILTEREDGE_FREE             0x01        /**< list free edges */
#define OY_FILTEREDGE_CONNECTED        0x02        /**< list connected edges */
#define OY_FILTEREDGE_LASTTYPE         0x04        /**< list last type edges */
/* decode */
#define oyToFilterEdge_Free_m(r)       ((r)&1)
#define oyToFilterEdge_Connected_m(r)  (((r) >> 1)&1)
#define oyToFilterEdge_LastType_m(r)   (((r) >> 2)&1)

/** @enum    oyCONNECTOR_EVENT_e
 *  @ingroup objects_conversion
 *  @brief   Connector events types
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/00/00 (Oyranos: 0.1.8)
 *  @date    2008/00/00
 */
typedef enum {
  oyCONNECTOR_EVENT_OK,                /**< kind of ping */
  oyCONNECTOR_EVENT_CONNECTED,         /**< connection established */
  oyCONNECTOR_EVENT_RELEASED,          /**< released the connection */
  oyCONNECTOR_EVENT_DATA_CHANGED,      /**< call to update image views */
  oyCONNECTOR_EVENT_STORAGE_CHANGED,   /**< new data accessors */
  oyCONNECTOR_EVENT_INCOMPATIBLE_DATA, /**< can not process image */
  oyCONNECTOR_EVENT_INCOMPATIBLE_OPTION,/**< can not handle option */
  oyCONNECTOR_EVENT_INCOMPATIBLE_CONTEXT,/**< can not handle profile */
  oyCONNECTOR_EVENT_INCOMPLETE_GRAPH   /**< can not completely process */
} oyCONNECTOR_EVENT_e;

const char *       oyConnectorEventToText (
                                       oyCONNECTOR_EVENT_e e );

#include "oyFilterPlug_s.h"
#include "oyPixelAccess_s.h"
int      oyFilterPlug_ImageRootRun   ( oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket );

/** @enum    oyFILTER_REG_MODE_e
 *  @ingroup objects_conversion
 *
 *  @version Oyranos: 0.1.11
 *  @since   2010/08/12 (Oyranos: 0.1.11)
 *  @date    2010/08/12
 */
typedef enum {
  oyFILTER_REG_MODE_NONE = 0,
  /** segments starting with underbar are implementation attributes */
  oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR = 0x0100
} oyFILTER_REG_MODE_e;
char   oyFilterRegistrationModify    ( const char        * registration,
                                       oyFILTER_REG_MODE_e mode,
                                       char             ** result,
                                       oyAlloc_f           allocateFunc );
#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */


#endif /* OYRANOS_MODULE_H */
