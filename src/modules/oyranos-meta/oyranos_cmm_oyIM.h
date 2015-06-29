/** @file oyranos_cmm_oyIM.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2008-2009 (C) Kai-Uwe Behrmann
 *
 *  @brief    registration definitions for Oyranos backends
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2008/12/16
 */

#ifndef OYRANOS_CMM_OYIM_H
#define OYRANOS_CMM_OYIM_H

#include "oyranos_config_internal.h"

#include "oyCMMapi3_s_.h"
#include "oyCMMapi5_s_.h"

#include "oyranos_cmm.h"
#include "oyranos_helper.h"
#include "oyranos_icc.h"
#include "oyranos_i18n.h"
#include "oyranos_io.h"
#include "oyranos_definitions.h"
#include "oyranos_texts.h"

/* --- internal definitions --- */

#define CMM_NICK "oyIM"

extern oyMessage_f oyIM_msg;



int                oyIMCMMInit       ( oyStruct_s        * filter );
int                oyIMIconv         ( const char        * input,
                                       size_t              len,
                                       char              * output,
                                       const char        * from_codeset );
int      oyIMCMMMessageFuncSet       ( oyMessage_f         message_func );
oyStructList_s * oyIMProfileTag_GetValues(
                                       oyProfileTag_s    * tag );
int      oyIMProfileTag_Create       ( oyProfileTag_s    * tag,
                                       oyStructList_s    * list,
                                       icTagTypeSignature  tag_type,
                                       uint32_t            version );
oyWIDGET_EVENT_e   oyIMWidgetEvent   ( oyOptions_s       * options,
                                       oyWIDGET_EVENT_e    type,
                                       oyStruct_s        * event );

extern oyCMMapi3_s_  oyIM_api3;
extern oyCMMapi5_s_  oyIM_api5_meta_c;

#endif /* OYRANOS_CMM_OYIM_H */
