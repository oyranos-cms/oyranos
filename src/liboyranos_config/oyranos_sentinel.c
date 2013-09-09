/** @file oyranos_sentinel.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2006-2012 (C) Kai-Uwe Behrmann
 *
 *  @brief    library sentinels
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2006/09/14
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "oyranos_config_internal.h"
#include "oyranos.h"
#include "oyranos_alpha_internal.h"
#include "oyranos_debug.h"
#include "oyranos_elektra.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_internal.h"
#include "oyranos_sentinel.h"


static int export_setting = 1;
/* static int export_path = 1; */
static int export_monitor = 1;
static int export_cmm = 1;
extern int oy_sentinel_initialised_;

int oyExportStart_(int export_check)
{
  /*static int export_cmms = 1;*/
  int start = 0;

# define EXPORT_( flag, var, func ) \
  if(export_check & flag && var) \
  { \
    var = 0; \
    func; \
    start = 1; \
  }

  EXPORT_( EXPORT_SETTING, export_setting, oyOpen_() )
  /*EXPORT_( EXPORT_PATH, export_path, oyPathAdd_ (OY_PROFILE_PATH_USER_DEFAULT) )*/
  /* Currently the monitor API is a link time module and outside the basic API.
     So we cant rely on it on runtime here. 
     This will change when Monitor support will be a runtime link in module.
   */
  EXPORT_( EXPORT_MONITOR, export_monitor, start = 1 )

  oyInit_();
  return start;
}

int oyExportReset_(int export_check)
{
  int action = 0;
  if(export_check & EXPORT_SETTING)
  {
    if(!export_setting) action = 1;
    export_setting = 1;
    oyCloseReal__();
  }
  
  if(export_check & EXPORT_MONITOR)
  {
    if(!export_monitor) action = 1;
    export_monitor = 1;
  }
  
  if(export_check & EXPORT_CMMS)
  {
    if(!export_cmm) action = 1;
    export_cmm = 1;
  }

  oy_sentinel_initialised_ = 0;

  if(export_check & EXPORT_I18N)
    oyI18Nreset_();

  return action;
}

void     oyFinish_                   ( int                 unused )
{
  oyI18Nreset_();
  oyAlphaFinish_( unused );
}

#include "oyProfiles_s.h"
#include "oyStructList_s_.h"

extern oyStructList_s_ * oy_profile_s_file_cache_;
extern oyStructList_s * oy_cmm_cache_;
extern oyStructList_s * oy_meta_module_cache_;
extern oyProfiles_s * oy_profile_list_cache_;

/** @internal
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/04/16 (Oyranos: 0.1.10)
 *  @date    2009/04/16
 */
void     oyAlphaFinish_              ( int                 unused )
{
  oyProfiles_Release( &oy_profile_list_cache_ );
  oyStructList_Release( &oy_meta_module_cache_ );
  oyStructList_Release( &oy_cmm_cache_ );
  oyStructList_Release_( &oy_profile_s_file_cache_ );
}
