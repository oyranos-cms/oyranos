/** @file oyranos_monitor_internal_x11.h
 *
 *  Oyranos is an open source Color Management System 
 *
 *  Copyright (C) 2005-2010  Kai-Uwe Behrmann
 *
 */

/**
 *  @brief    monitor device detection
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @license: new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2005/02/01
 */

#ifndef OYRANOS_MONITOR_INTERNAL_X11_H
#define OYRANOS_MONITOR_INTERNAL_X11_H

#include "oyranos_monitor_hooks.h"
#include "oyranos_monitor_internal.h"
#include "oyCMMapi10_s_.h"


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define CMM_NICK "oyX1"

extern oyMonitorHooks_s * oyX1MonitorHooks;
extern oyCMMapi10_s_      oyX1_api10_set_xcm_region_handler;
#define next_api (oyCMMapi_s*) & oyX1_api10_set_xcm_region_handler

#define CMMInit                 catCMMfunc( oyX1, CMMInit )
#define _initialised            catCMMfunc( oyX1, _initialised )
#define CMMMessageFuncSet       catCMMfunc( oyX1, CMMMessageFuncSet )
#define _msg                    catCMMfunc( oyX1, _msg )
#define DeviceFromName_         catCMMfunc( oyX1, DeviceFromName_ )
#define Configs_Modify          catCMMfunc( oyX1, Configs_Modify )
#define GetText                 catCMMfunc( oyX1, GetText )
#define Api8UiGetText           catCMMfunc( oyX1, Api8UiGetText )
#define hooks                   catCMMfunc( oyX1, MonitorHooks )
#define _help_list              catCMMfunc( oyX1, _help_list )
#define _help_properties        catCMMfunc( oyX1, _help_properties )
#define _help_system_specific   catCMMstruct( oyX1, help_system_specific )
#define _help_setup             catCMMfunc( oyX1, _help_setup )
#define _help_unset             catCMMfunc( oyX1, _help_unset )
#define _help_add_edid_to_icc   catCMMfunc( oyX1, _help_add_edid_to_icc )
#define _help                   catCMMfunc( oyX1, _help )
#define ConfigsUsage            catCMMfunc( oyX1, ConfigsUsage )
#define Configs_FromPattern     catCMMfunc( oyX1, Configs_FromPattern )
#define Config_Rank             catCMMfunc( oyX1, Config_Rank )
#define _api8_ui_texts          catCMMfunc( oyX1, _api8_ui_texts )
#define _api8_ui                catCMMfunc( oyX1, _api8_ui )
#define _api8_icon              catCMMfunc( oyX1, _api8_icon )
#define _api8                   catCMMfunc( oyX1, _api8 )
#define _cmm_module             catCMMfunc( oyX1, _cmm_module )
#define _texts                  catCMMfunc( oyX1, _texts )
#define GetMonitorInfo_lib      catCMMstruct( oyX1, getInfo )
#define GetAllScreenNames       catCMMstruct( oyX1, getAllMonitorNames )
#define MonitorProfileSetup     catCMMstruct( oyX1, setupProfile )
#define MonitorProfileUnset     catCMMstruct( oyX1, unsetProfile )
#define GetRectangleFromDevice  catCMMstruct( oyX1, getRectangle )
#define GetMonitorProfile       catCMMstruct( oyX1, getProfile )
#define MoveColorServerProfiles catCMMstruct( oyX1, moveColorServerProfiles )

int                CMMInit           ( );
int                CMMMessageFuncSet ( oyMessage_f         message_func );



#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* OYRANOS_MONITOR_INTERNAL_X11_H */
