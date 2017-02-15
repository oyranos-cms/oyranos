#include "oyCMMapi6_s.h"
#include "oyranos_module_internal.h"
#include "oyCMMapi5_s_.h"

int main( int argc, char ** argv)
{
  oyCMMapi6_s * api = oyCMMapi6_New(0);
  const char * in,*out;
  int error = 0;

  if(!api)
    oyMessageFunc_p(oyMSG_WARN,0,"No oyCMMapi_s generated");

  in = oyCMMapi6_GetDataType(api,0);
  out = oyCMMapi6_GetDataType(api,1);

  oyMessageFunc_p(oyMSG_WARN,api,"%s %s", in?in:"",out?out:"");

  oyCMMapi6_Release(&api);

  {
    char ** files = NULL;
    uint32_t  files_n = 0;
    int i;

    files = oyCMMsGetLibNames_(&files_n);
    oyMessageFunc_p(oyMSG_WARN,NULL,"internal API oyCMMsGetLibNames_() = %d", files_n);
    if(files_n)
    {
      for(i = 0; i < files_n; ++i)
      {
        oyCMMinfo_s * cmm_info = oyCMMinfoFromLibName_(files[i]);

        if(cmm_info)
        {
          oyCMMapi_s * tmp;
          char ** files2 = NULL;
          uint32_t  files2_n = 0;

          oyMessageFunc_p(oyMSG_WARN,cmm_info,"oyCMMinfoFromLibName_(file[%d]): success \"%s\"", i, files[i]);

          tmp = oyCMMinfo_GetApi( cmm_info );
	  if(!tmp)
	  {
            oyMessageFunc_p(oyMSG_ERROR, cmm_info, "[%d]: no meta API obtained from API info", i);
	    return 1;
	  }

	  while(tmp && tmp->type_ != oyOBJECT_CMM_API5_S)
            tmp = oyCMMapi_GetNext(tmp);

	  if(tmp->type_ == oyOBJECT_CMM_API5_S)
	  {
            oyCMMapi5_s_ * api5 = (oyCMMapi5_s_*) tmp;
            int j;
            files2 = oyCMMsGetNames_(&files2_n, api5->sub_paths, api5->ext,
                                api5->data_type == 0 ? oyPATH_MODULE :
                                                       oyPATH_SCRIPT);
            for(j = 0; j < files2_n; ++j)
            {
              oyCMMinfo_s * cmm_info2 = oyCMMinfoFromLibName_(files2[j]);
	      if(cmm_info2)
                oyMessageFunc_p(oyMSG_WARN,api5,"[%d][%d]: success %s %s %d \"%s\"", i,j, oyCMMinfo_GetCMM(cmm_info2), oyCMMinfo_GetVersion(cmm_info2), oyCMMinfo_GetCompatibility(cmm_info2), files2[j]);
	      else
                oyMessageFunc_p(oyMSG_ERROR,NULL,"[%d][%d]: no API info obtained from \"%s\"", i,j, files2[j]);
	    }

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
      oy_debug = 4;
      files = oyCMMsGetLibNames_(&files_n);
      oyMessageFunc_p(oyMSG_ERROR,NULL,"no  meta module (OY_MODULE_PATH=%s)", t?t:"");
    }
  }

  return error;
}

