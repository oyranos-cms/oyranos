{% load gsoc_extensions %}
{% include "source_file_header.txt" %}

#ifndef {{ file_name|underscores|upper|tr:". _" }}
#define {{ file_name|underscores|upper|tr:". _" }}

{% include "cpp_begin.h" %}

#include "oyOptions_s.h"
#include "oyFilterCore_s.h"
#include "oyCMMInfo_s.h"
#include "oyCMMapi_s.h"

//#include "oyStruct_s.h"
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

/**
 *  type:
 *  - oyNAME_NICK: compact, e.g. "GPU"
 *  - oyNAME_NAME: a short explanation, e.g. "Rendering with GPU"
 *  - oyNAME_DESCRIPTION: a long explanation, e.g. "Accelerated calculations on a GPU"
 *
 *  @param         select              one from "name", "manufacturer" or "copyright"
 *  @param         type                select flavour
 *  @param         object              the object to ask for a optional context
 *                                     This will typical be the object to which
 *                                     this function belongs, but can be stated
 *                                     otherwise.
 *  @return                            text string or zero
 *
 *  @version Oyranos: 0.3.0
 *  @since   2008/12/23 (Oyranos: 0.1.10)
 *  @date    2011/02/01
 */
typedef
const char *    (* oyCMMGetText_f)   ( const char        * select,
                                       oyNAME_e            type,
                                       oyStruct_s        * context );

typedef  oyOBJECT_e(*oyCMMapi_Check_f)(oyCMMInfo_s       * cmm_info,
                                       oyCMMapi_s        * api,
                                       oyPointer           data,
                                       uint32_t          * rank );

/** @brief   icon data
 *  @ingroup cmm_handling
 *  @extends oyStruct_s
 *
 *  Since: 0.1.8
 */
typedef struct {
  oyOBJECT_e       type;               /*!< struct type oyOBJECT_ICON_S */
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyPointer        dummy;              /**< keep to zero */
  int              width;              /**< */
  int              height;             /**< */
  float          * data;               /*!< should be sRGB matched */
  char           * file_list;          /*!< colon ':' delimited list of icon file names, SVG, PNG */
} oyIcon_s;


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

{% include "cpp_end.h" %}

#endif /* {{ file_name|underscores|upper|tr:". _" }} */
