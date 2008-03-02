/**
 * Oyranos is an open source Colour Management System 
 * 
 * Copyright (C) 2004-2006  Kai-Uwe Behrmann
 *
 * @autor: Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 * -----------------------------------------------------------------------------
 */

/** @file @internal
 *  @brief external CMM API
 */

/** @date      12. 11. 2007 */


#ifndef OYRANOS_CMM_H
#define OYRANOS_CMM_H

#include "oyranos.h"
#include "oyranos_alpha.h"

#ifdef __cplusplus
extern "C" {
namespace oyranos
{
#endif /* __cplusplus */

typedef enum {
  oyQUERY_OYRANOS_COMPATIBILITY,       /*!< provides the Oyranos version and expects the CMM compiled or compatibility Oyranos version back */
  oyQUERY_PIXELLAYOUT_DATATYPE,        /*!< takes a oyDATATYPE_e arg as value */
  oyQUERY_PIXELLAYOUT_CHANNELCOUNT,
  oyQUERY_PIXELLAYOUT_SWAP_COLOURCHANNELS,
  oyQUERY_PIXELLAYOUT_COLOURCHANNEL_OFFSET,
  oyQUERY_PIXELLAYOUT_PLANAR,
  oyQUERY_PIXELLAYOUT_FLAVOUR,
  oyQUERY_HDR,                         /*!< value a oyDATATYPE_e (oyHALF...) */
  oyQUERY_PROFILE_FORMAT = 20          /*!< value 1 == ICC */
} oyCMMQUERY_e;

int                oyCMMCanHandle      ( oyCMMQUERY_e      type,
                                         int               value );

oyCMMptr_s         oyCMMProfileOpen    ( oyPointer         block,
                                         size_t            size );

oyCMMptr_s         oyCMMTransformCreate( oyPointer       * cmm_profile_array,
                                         int               profiles_n,
                                         int               pixel_layout_in,
                                         int               pixel_layout_out,
                                         int               intent,
                                         int               proofing_intent,
                                         int               flags );
                                         
typedef oyChar*  (*oyCMMProgress_t)    ( int               ID,
                                         double            progress );
oyPointer          oyCMMTransformRun   ( int               ID,
                                         oyCMMptr_s      * cmm_transform,
                                         oyCMMProgress_t   progress );



oyPointer          oyCMMallocateFunc   ( size_t            size );
void               oyCMMdeallocateFunc ( oyPointer         mem );

/** @brief icon data
 *
 *  since: (Oyranos: version 0.1.8)
 */
typedef struct {
  oyOBJECT_TYPE_e  type;               /*!< struct type oyOBJECT_TYPE_ICON_S */
  int              width;
  int              height;
  float          * data;               /*!< should be sRGB matched */
  oyChar         * file_list;          /*!< colon ':' delimited list of absolute files names, probably SVG, PNG, XPM, BMP */
} oyIcon_s;

/** @brief CMM infos
 *
 *  since: (Oyranos: version 0.1.8)
 */
typedef struct {
  oyOBJECT_TYPE_e  type;               /*!< struct type oyOBJECT_TYPE_CMM_INFO_S */
  icSignature      cmmId;              /*!< ICC signature 'lcms' */
  oyChar         * version;            /*!< non localisable 'v1.17' */
  oyName_s       * manufacturer;       /*!< localisable 'Marti' 'Marti Maria' 'support email: @; internet: www.littlecms.com; sources: ...' */
  oyName_s       * copyright;          /*!< localisable 'MIT','MIT License'.. */
  int              oy_compatibility;   /*!< last supported Oyranos CMM API : OYRANOS_VERSION */
  oyIcon_s      ** icon;               /*!< zero terminated list of a icon pyramid */
} oyCMMInfo_s;

oyCMMInfo_s *      oyCMMInfoGet        ( );

#ifdef __cplusplus
} /* extern "C" */
} /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OYRANOS_CMM_H */
