/** @file oyranos_monitor_effect.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2018 (C) Kai-Uwe Behrmann
 *
 *  @internal
 *  @brief    monitor effect APIs
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2018/02/19
 */

#ifndef OYRANOS_MONITOR_EFFECT_H
#define OYRANOS_MONITOR_EFFECT_H

#include "oyProfiles_s.h"

int      oyProfile_CreateEffectVCGT  ( oyProfile_s       * prof );
int      oyGetLinearEffectProfile    ( oyProfiles_s      * effects );
int      oyProfileAddWhitePointEffect( oyProfile_s       * monitor_profile,
                                       oyOptions_s      ** module_options );
uint16_t*oyProfileGetWhitePointRamp  ( int                 width,
                                       oyProfile_s       * p,
                                       oyOptions_s       * options );
uint16_t*oyProfile_GetVCGT           ( oyProfile_s       * profile,
                                       int               * width );
int      oyProfile_SetVCGT           ( oyProfile_s       * profile,
                                       uint16_t          * vcgt,
                                       int                 width );

#endif /* OYRANOS_MONITOR_EFFECT_H */

