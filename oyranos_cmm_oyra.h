/** @file oyranos_cmm_oyra.h
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  Copyright (C) 2008  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    registration definitions for Oyranos backends
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:\n new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2008/10/07
 */

#ifndef OYRANOS_CMM_OYRA_H
#define OYRANOS_CMM_OYRA_H

#include "config.h"
#include "oyranos_alpha.h"
#include "oyranos_cmm.h"
#include "oyranos_helper.h"
#include "oyranos_icc.h"
#include "oyranos_i18n.h"
#include "oyranos_io.h"
#include "oyranos_definitions.h"
#include "oyranos_texts.h"

/* --- internal definitions --- */

#define CMM_NICK "oyra"

extern oyMessage_f message;



int                oyraCMMInit       ( );
int                oyraIconv         ( const char        * input,
                                       size_t              len,
                                       char              * output,
                                       const char        * from_codeset );
int      oyraCMMMessageFuncSet       ( oyMessage_f         message_func );
int      oyStructList_MoveInName     ( oyStructList_s    * texts,
                                       char             ** text,
                                       int                 pos );
int      oyStructList_AddName        ( oyStructList_s    * texts,
                                       const char        * text,
                                       int pos );
oyStructList_s * oyraProfileTag_GetValues(
                                       oyProfileTag_s    * tag );
int      oyraProfileTag_Create       ( oyProfileTag_s    * tag,
                                       oyStructList_s    * list,
                                       icTagTypeSignature  tag_type,
                                       uint32_t            version );
int      oyraFilter_ImageRootCanHandle(oyCMMQUERY_e      type,
                                       uint32_t          value );
oyOptions_s* oyraFilter_ImageRootValidateOptions (
                                       oyFilter_s        * filter,
                                       oyOptions_s       * validate,
                                       int                 statical,
                                       uint32_t          * result );
oyWIDGET_EVENT_e   oyraWidgetEvent   ( oyOptions_s       * options,
                                       oyWIDGET_EVENT_e    type,
                                       oyStruct_s        * event );
oyPointer  oyraFilter_ImageRootContextToMem (
                                       oyFilter_s        * filter,
                                       size_t            * size,
                                       oyCMMptr_s        * oy,
                                       oyAlloc_f           allocateFunc );

extern oyDATATYPE_e  oyra_image_data_types[7];

extern oyCMMapi4_s   oyra_api4_image_output_ppm;
extern oyCMMapi4_s   oyra_api4_image_output;
extern oyCMMapi4_s   oyra_api4_image_root;
extern oyCMMapi3_s   oyra_api3;

#endif /* OYRANOS_CMM_OYRA_H */
