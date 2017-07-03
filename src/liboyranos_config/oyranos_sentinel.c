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
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2006/09/14
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "oyranos_config_internal.h"
#include "oyranos.h"
#include "oyranos_alpha_internal.h"
#include "oyranos_debug.h"
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

  EXPORT_( EXPORT_SETTING, export_setting, export_setting = 0 )
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

/* @param          flags               - 0x01 keep caches
 *                                     - 0x02 keep i18n
 */
void     oyFinish_                   ( int                 flags )
{
  if(!(flags & 0x01))
    oyI18Nreset_();
  if(!(flags & 0x02))
    oyAlphaFinish_( 0 );
  if(oy_debug_objects >= 0)
    oyObjectTreePrint( 0x01 | 0x02 );
}

#include "oyProfile_s_.h"
#include "oyProfiles_s.h"
#include "oyStructList_s_.h"
#include "oyranos_cache.h"

/** @internal
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/04/16 (Oyranos: 0.1.10)
 *  @date    2009/04/16
 */
void     oyAlphaFinish_              ( int                 unused )
{
  oyProfiles_Release( &oy_profile_list_cache_ );
  oyStructList_Release( &oy_cmm_cache_ );
  oyStructList_Release( &oy_cmm_infos_ );
  oyStructList_Release( &oy_cmm_handles_ );
  oyStructList_Release_( &oy_profile_s_file_cache_ );
  if(oy_profile_s_std_cache_)
  {
     int i, count = oyDEFAULT_PROFILE_END - oyDEFAULT_PROFILE_START;
     for(i = 0; i < count; ++i)
       if(oy_profile_s_std_cache_[i])
         oy_profile_s_std_cache_[i]->release( (oyStruct_s**) &oy_profile_s_std_cache_[i] );
  }
  oyOptions_Release( &oy_db_cache_ );
  *get_oy_db_cache_init_() = 0;
}

#include "oyranos_alpha.h"
#include "oyranos_string.h"
#include "oyranos_module_internal.h"
#include "oyCMMinfo_s.h"
/** @internal
 *
 *  @version Oyranos: 0.9.6
 *  @date    2015/01/28
 *  @since   2015/01/28 (Oyranos: 0.9.6)
 */
char *     oyAlphaPrint_             ( int                 verbose )
{
  char * text = NULL;
  const char * t;
  oyStringAddPrintf_( &text, 0,0,
                      "oy_profile_list_cache_: %d\noy_cmm_cache_: %d\noy_cmm_infos_: %d\noy_cmm_handles_: %d\noy_profile_s_file_cache_: %d\noy_db_cache_: %d\n",
  oyProfiles_Count( oy_profile_list_cache_ ),
  oyStructList_Count( oy_cmm_cache_ ), /* oyHash_s */
  oyStructList_Count( oy_cmm_infos_ ), /* oyCMMhandle_s */
  oyStructList_Count( oy_cmm_handles_ ), /* oyPointer_s */
  oyStructList_Count( (oyStructList_s*) oy_profile_s_file_cache_ ),
  oyOptions_Count( oy_db_cache_ ) );
  if(verbose)
  {
    int n,i;
    t = oyOptions_GetText( oy_db_cache_, oyNAME_NICK );
    oyStringAddPrintf_( &text, 0,0, "%s\n", t );
    oyStringAddPrintf_( &text, 0,0, "oy_cmm_cache_ (oyHash_s): %s\n", oyCMMCacheListPrint_() );
    n = oyStructList_Count(oy_cmm_infos_);
    oyStringAddPrintf_( &text, 0,0, "oy_cmm_infos_ (oyCMMhandle_s): %d\n",  oyStructList_Count( oy_cmm_infos_ ) );
    for(i = 0; i < n; ++i)
    {
      oyCMMinfo_s * s = 0;
      oyCMMhandle_s * cmmh = (oyCMMhandle_s *) oyStructList_GetType_(
                                               (oyStructList_s_*)oy_cmm_infos_,
                                               i,
                                               oyOBJECT_CMM_HANDLE_S );

      if(cmmh)
        s = (oyCMMinfo_s*) cmmh->info;

      if( s && s->type_ == oyOBJECT_CMM_INFO_S &&
          *((uint32_t*)oyCMMinfo_GetCMM(s)) )
      {
        oyStringAddPrintf_( &text, 0,0,
        "[%d] CMM_INFO: %s\n", i, cmmh->lib_name );
      } else
        oyStringAddPrintf_( &text, 0,0,
        "[%d]           %s\n", i, cmmh->lib_name);
    }
  }
  return text;
}
