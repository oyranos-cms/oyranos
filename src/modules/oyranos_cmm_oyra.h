/** @file oyranos_cmm_oyra.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2008-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    registration definitions for Oyranos backends
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2008/10/07
 */

#ifndef OYRANOS_CMM_OYRA_H
#define OYRANOS_CMM_OYRA_H

#include "oyCMMapi4_s_.h"
#include "oyCMMapi7_s_.h"

#include "oyranos_config_internal.h"
#include "oyranos_cmm.h"
#include "oyranos_helper.h"
#include "oyranos_icc.h"
#include "oyranos_i18n.h"
#include "oyranos_io.h"
#include "oyranos_definitions.h"
#include "oyranos_texts.h"

/* --- internal definitions --- */

#define CMM_NICK "oyra"
#define CMM_VERSION {OYRANOS_VERSION_A,OYRANOS_VERSION_B,OYRANOS_VERSION_C}

extern oyMessage_f oyra_msg;



int                oyraCMMInit       ( oyStruct_s        * filter );
int                oyraIconv         ( const char        * input,
                                       size_t              len,
                                       char              * output,
                                       const char        * from_codeset );
int      oyraCMMMessageFuncSet       ( oyMessage_f         message_func );
oyStructList_s * oyraProfileTag_GetValues(
                                       oyProfileTag_s    * tag );
int      oyraProfileTag_Create       ( oyProfileTag_s    * tag,
                                       oyStructList_s    * list,
                                       icTagTypeSignature  tag_type,
                                       uint32_t            version );
int      oyraFilter_ImageRootCanHandle(oyCMMQUERY_e      type,
                                       uint32_t          value );
oyOptions_s* oyraFilter_ImageRootValidateOptions (
                                       oyFilterCore_s    * filter,
                                       oyOptions_s       * validate,
                                       int                 statical,
                                       uint32_t          * result );
oyWIDGET_EVENT_e   oyraWidgetEvent   ( oyOptions_s       * options,
                                       oyWIDGET_EVENT_e    type,
                                       oyStruct_s        * event );
int      oyraFilterPlug_ImageRootRun ( oyFilterPlug_s    * requestor_plug,
                                       oyPixelAccess_s   * ticket );

extern oyDATATYPE_e  oyra_image_data_types[7];

extern oyCMMapi4_s_   oyra_api4_image_load;
extern oyCMMapi7_s_   oyra_api7_image_load;
extern oyCMMapi4_s_   oyra_api4_image_input_ppm;
extern oyCMMapi7_s_   oyra_api7_image_input_ppm;
extern oyCMMapi4_s_   oyra_api4_image_write_ppm;
extern oyCMMapi7_s_   oyra_api7_image_write_ppm;
extern oyCMMapi4_s_   oyra_api4_image_rectangles;
extern oyCMMapi7_s_   oyra_api7_image_rectangles;
extern oyCMMapi4_s_   oyra_api4_image_output;
extern oyCMMapi7_s_   oyra_api7_image_output;
extern oyCMMapi4_s_   oyra_api4_image_root;
extern oyCMMapi7_s_   oyra_api7_image_root;
oyCMMapi_s * oyraApi4ImageScaleCreate(void);
oyCMMapi_s * oyraApi7ImageScaleCreate(void);

#endif /* OYRANOS_CMM_OYRA_H */
