/** @file oyranos_sentinel.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2006-2022 (C) Kai-Uwe Behrmann
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
#include "oyranos_module_internal.h"
#include "oyranos_sentinel.h"
#include "oyCMMapi_s_.h"

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

#include "oyranos_cache.h"
#include "oyranos_db.h"
#ifdef HAVE_LIBXML2
#include <libxml/parser.h>
#endif
/* @param          flags               - 0x01 keep i18n
 *                                     - 0x02 keep caches
 *                                     - 0x04 keep object list
 *                                     - 0x08 keep core with object registration and i18n
 */
void     oyFinish_                   ( int                 flags )
{
  oyMessageFunc_p(oyMSG_DBG, 0, OY_DBG_FORMAT_ "------------------------- flags: %d", OY_DBG_ARGS_, flags);
  if(!(flags & FINISH_IGNORE_I18N))
  {
    oyMessageFunc_p(oyMSG_DBG, 0, OY_DBG_FORMAT_ "oyI18Nreset_", OY_DBG_ARGS_);
    oyI18Nreset_();
  }
  if(!(flags & FINISH_IGNORE_CORE) || !(flags & FINISH_IGNORE_CACHES))
  {
    oyMessageFunc_p(oyMSG_DBG, 0, OY_DBG_FORMAT_ "oyLibCoreRelease", OY_DBG_ARGS_);
    oyLibCoreRelease();
  }
  if(!(flags & FINISH_IGNORE_CACHES))
  {
    oyMessageFunc_p(oyMSG_DBG, 0, OY_DBG_FORMAT_ "oyAlphaFinish_", OY_DBG_ARGS_);
    oyAlphaFinish_( 0 );
  }
  if(oy_debug_objects >= 0)
    oyObjectTreePrint( 0x01 | 0x02, NULL );
  /* clean object tracks after print */
  if(!(flags & FINISH_IGNORE_OBJECT_LIST))
    oyObjectGetList( 0 );
  oyMessageFunc_p(oyMSG_DBG, 0, OY_DBG_FORMAT_ "oyDebugLevelCacheRelease", OY_DBG_ARGS_);
  oyDebugLevelCacheRelease();

  oyMessageFunc_p(oyMSG_DBG, 0, OY_DBG_FORMAT_ "oyDbHandlingReset", OY_DBG_ARGS_);
  oyDbHandlingReset();

#ifdef HAVE_LIBXML2
  oyMessageFunc_p(oyMSG_DBG, 0, OY_DBG_FORMAT_ "xmlCleanupParser", OY_DBG_ARGS_);
  xmlCleanupParser();
#endif
}

#include "oyProfile_s_.h"
#include "oyProfiles_s.h"
#include "oyStructList_s_.h"

/** @internal
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/04/16 (Oyranos: 0.1.10)
 *  @date    2009/04/16
 */
void     oyAlphaFinish_              ( int                 unused OY_UNUSED )
{
  if(get_oy_msg_func_n_())
  {
    fprintf(stderr, "FATAL: releasing modules before cleaning static structs\nUse oyLibCoreRelease() before oyAlphaFinish_()\n");
    if(!getenv("OYJL_NO_EXIT")) exit(1);
  }
  /* before releasing all modules, the threads module should be used to close the mutex */
  oyObjectIdRelease();

  oyProfiles_Release( &oy_profile_list_cache_ );
  oyStructList_Release( &oy_cmm_cache_ );
  {
    int n = oyStructList_Count( oy_cmm_infos_ ), i;
    for(i = 0; i < n; ++i)
    {
      int error = 0, api_count = 0, j = 0;
      oyCMMinfo_s * cmm_info = 0;
      oyCMMapi_s ** apis;
      oyCMMhandle_s * cmmh = (oyCMMhandle_s *) oyStructList_GetType_(
                                               (oyStructList_s_*)oy_cmm_infos_,
                                               i,
                                               oyOBJECT_CMM_HANDLE_S );

      if(cmmh)
        cmm_info = (oyCMMinfo_s*) cmmh->info;
      else
        continue;

      oyCMMapi_s * api = oyCMMinfo_GetApi( (oyCMMinfo_s*) cmm_info );
      while(api)
      {
        ++api_count;
        api = oyCMMapi_GetNext(api);
      }

      apis = calloc( sizeof(oyCMMapi_s*), api_count );

      api = oyCMMinfo_GetApi( (oyCMMinfo_s*) cmm_info );
      while(api)
      {
        const char * registration = oyCMMapi_GetRegistration(api);
        /* init */
        oyCMMReset_f reset = oyCMMapi_GetResetF(api);
        oyCMMapi_s_ * api_ = (oyCMMapi_s_*) api;

        apis[j] = api; ++j;

        if(api_->id_)
          oyFree_m_(api_->id_);

        if(reset)
          error = reset( (oyStruct_s*) api );
        if(error > 0)
        {
          cmm_info = NULL;
          DBG_NUM1_S("reset failed: %s", registration);
        }

        api = oyCMMapi_GetNext(api);
      }

      if(cmm_info)
      /* release in reverse order and avoid any miracles with chained objects */
      for(j = api_count-1; j >= 0; --j)
      {
        oyCMMapi_s * api_ = apis[j];
        if(api_->release)
        {
          oyCMMapi_s * api_ptr = api_,
                     * cmm_info_api = oyCMMinfo_GetApi(cmm_info);
          api_->release((oyStruct_s**)&api_);
          if(api_ptr == cmm_info_api)
            oyCMMinfo_SetApi(cmm_info, NULL);
          if(j > 0)
          {
            api_ = apis[j-1];
            oyCMMapi_SetNext(api_, NULL);
          }
        }
      }
      if(apis)
        free(apis);
    }
  }
  oyStructList_Release( &oy_cmm_infos_ );

  oyStructList_Release( &oy_cmm_handles_ );
  oyStructList_Release_( &oy_profile_s_file_cache_ );
  oyOptions_Release( &oy_db_cache_ );
  oyOptions_Release( &oy_config_options_dummy_ );

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
      else
        continue;

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
