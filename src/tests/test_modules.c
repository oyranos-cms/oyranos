#include "oyCMMapi6_s.h"
#include "oyranos_module_internal.h"
#include "oyranos_cache.h"
#include "oyranos_debug.h"
#include "oyCMMapi5_s_.h"
#include <oyjl.h>

int main( int argc OY_UNUSED, char ** argv OY_UNUSED)
{
  oyInit_();

  oyCMMapi6_s * api = oyCMMapi6_New(0);
  const char * in,*out;
  int error = 0;

  if(!api)
  {
    oyMessageFunc_p(oyMSG_WARN,0,"No oyCMMapi_s generated");
    return 1;
  }

  in = oyCMMapi6_GetDataType(api,0);
  out = oyCMMapi6_GetDataType(api,1);

  oyMessageFunc_p(oyMSG_WARN,api,"%s %s", in?in:"",out?out:"");

  oyCMMapi6_Release(&api);
  oyObjectTreePrint( 0x01 | 0x02 | 0x08, __FILE__ );

  {
    char ** files = NULL;
    uint32_t  files_n = 0;
    int i;

    files = oyCMMsGetLibNames_p(&files_n);
    oyMessageFunc_p(oyMSG_WARN,NULL,"internal API oyCMMsGetLibNames_p() = %d", files_n);
    if(files_n)
    {
      for(i = 0; (uint32_t)i < files_n; ++i)
      {
        oyCMMinfo_s * cmm_info = oyCMMinfoFromLibName_p(files[i]);

        if(cmm_info)
        {
          oyCMMapi_s * tmp;
          char ** files2 = NULL;
          uint32_t  files2_n = 0;

          oyMessageFunc_p(oyMSG_WARN,cmm_info,"oyCMMinfoFromLibName_p(file[%d]): success \"%s\"", i, files[i]);

          tmp = oyCMMinfo_GetApi( cmm_info );
          if(!tmp)
          {
            oyMessageFunc_p(oyMSG_ERROR, cmm_info, "[%d]: no meta API obtained from API info", i);
          	return 1;
          }

          while(tmp && tmp->type_ != oyOBJECT_CMM_API5_S)
            tmp = oyCMMapi_GetNext(tmp);

          if(tmp && tmp->type_ == oyOBJECT_CMM_API5_S)
          {
            oyCMMapi5_s_ * api5 = (oyCMMapi5_s_*) tmp;
            int j;
            oyOBJECT_e type = oyCMMapi_Check_( tmp );
            if(type != oyOBJECT_CMM_API5_S)
            {
              oyMessageFunc_p(oyMSG_ERROR,NULL,"[%d]: API check failed from \"%s\"", i, files[i]);
              break;
            }
            files2 = oyCMMsGetNames_(&files2_n, api5->sub_paths, api5->ext,
                                api5->data_type == 0 ? oyPATH_MODULE :
                                                       oyPATH_SCRIPT);
            for(j = 0; (uint32_t)j < files2_n; ++j)
            {
              const char * file_name = files2[j];
              oyCMMinfo_s * cmm_info2 = oyCMMinfoFromLibName_p(file_name);
              if(cmm_info2)
                oyMessageFunc_p(oyMSG_WARN,api5,"[%d][%d]: success %s %s %d \"%s\"", i,j, oyCMMinfo_GetCMM(cmm_info2), oyCMMinfo_GetVersion(cmm_info2), oyCMMinfo_GetCompatibility(cmm_info2), file_name);
              else
                oyMessageFunc_p(oyMSG_ERROR,NULL,"[%d][%d]: no API info obtained from \"%s\"", i,j, file_name);
            }
            oyjlStringListRelease( &files2, files2_n, free );

            if(!files2_n)
            {
              error = 1;
              oyMessageFunc_p(oyMSG_ERROR,api5,"[%d]: no libraries found by meta module", i);
            }

          } else
          {
            oyMessageFunc_p(oyMSG_ERROR,tmp,"[%d]: %s - not a meta module in \"%s\"", i, oyStruct_GetText((oyStruct_s*)tmp, oyNAME_NAME, 0x00), files[i]);
            error = 1;
          }
        } else
        {
          oyMessageFunc_p(oyMSG_ERROR,NULL,"[%d]: no API info obtained from \"%s\"", i, files[i]);
      	  error = 1;
        }
      }
    } else
    {
      const char * t = getenv("OY_MODULE_PATH");
      error = 1;
      //oy_debug = 4;
      //files = oyCMMsGetLibNames_p(&files_n);
      error = system("ls -R");
      oyMessageFunc_p(oyMSG_ERROR,NULL,"no  meta module (OY_MODULE_PATH=%s)", t?t:"");
    }
    oyjlStringListRelease( &files, files_n, free );
  }

  oyLibCoreRelease(0);
  oyAlphaFinish_(0);
  oyObjectTreePrint( 0x01 | 0x02 | 0x08, __FILE__ );
  oyDebugLevelCacheRelease();

  return error;
}

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

  //oyProfiles_Release( &oy_profile_list_cache_ );
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
          free(api_->id_);

        if(reset)
          error = reset( (oyStruct_s*) api );
        if(error > 0)
        {
          cmm_info = NULL;
          DBG_NUM1_S("reset failed: %s", registration);
        }

        api = oyCMMapi_GetNext(api);
      }
      /* release in reverse order and avoid any miracles with chained objects */
      for(j = api_count-1; j >= 0; --j)
      {
        oyCMMapi_s * api_ = apis[j];
        if(api_->release)
          api_->release((oyStruct_s**)&api_);
      }
      if(apis)
        free(apis);
    }
  }
  oyStructList_Release( &oy_cmm_infos_ );

  oyStructList_Release( &oy_cmm_handles_ );
  oyStructList_Release_( &oy_profile_s_file_cache_ );
  oyOptions_Release( &oy_db_cache_ );
  //oyOptions_Release( &oy_config_options_dummy_ );

  //*get_oy_db_cache_init_() = 0;
}
