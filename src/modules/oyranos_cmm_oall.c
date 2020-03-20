/** @file oyranos_cmm_stat.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2017 (C) Kai-Uwe Behrmann
 *
 *  @brief    static modules for Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2017/03/27
 */

/**
 *  The module is intented for static linking.
 */

#include <string.h>

#include "oyCMM_s.h"
#include "oyCMMapi_s_.h"
#include "oyranos_debug.h"
#include "oyranos_cache.h"
#include "oyranos_config_internal.h"
#include "oyranos_helper.h"

/* defined in sources/Struct.public_methods_definitions.c */
/** @internal
 *  @brief    get descriptions for object types
 *
 *  @version  Oyranos: 0.9.5
 *  @date     2013/0/0
 *  @since    2012/0/0 (Oyranos: 0.9.x)
 */
extern const char * (*oyStruct_GetTextFromModule_p) (
                                       oyStruct_s        * obj,
                                       oyNAME_e            name_type,
                                       uint32_t            flags );

extern oyCMM_s CUPS_cmm_module;
extern oyCMM_s elDB_cmm_module;
extern oyCMM_s lcm2_cmm_module;
extern oyCMM_s lcms_cmm_module;
extern oyCMM_s lraw_cmm_module;
extern oyCMM_s oicc_cmm_module;
extern oyCMM_s oiDB_cmm_module;
extern oyCMM_s oJPG_cmm_module;
extern oyCMM_s oPNG_cmm_module;
extern oyCMM_s oydi_cmm_module;
extern oyCMM_s oyIM_cmm_module;
extern oyCMM_s oyra_cmm_module;
extern oyCMM_s trds_cmm_module;
extern oyCMM_s oyRE_cmm_module;
extern oyCMM_s SANE_cmm_module;
extern oyCMM_s qarz_cmm_module;
extern oyCMM_s oyX1_cmm_module;

typedef struct {
  const char * name;
  oyCMM_s * cmm;
} oyStaticCMMs_s;

static oyStaticCMMs_s cmms[] = {
  { "oyIM_cmm_module-inbuild", &oyIM_cmm_module },
#ifdef HAVE_CUPS
  { "CUPS_cmm_module-inbuild", &CUPS_cmm_module },
#endif
#ifdef HAVE_ELEKTRA
  { "elDB_cmm_module-inbuild", &elDB_cmm_module },
#endif
#ifdef HAVE_LCMS2
  { "lcm2_cmm_module-inbuild", &lcm2_cmm_module },
#endif
#if defined( HAVE_LCMS ) && !defined( HAVE_LCMS2 )
  { "lcms_cmm_module-inbuild", &lcms_cmm_module },
#endif
#ifdef HAVE_LIBRAW_PLUGIN
  { "lraw_cmm_module-inbuild", &lraw_cmm_module },
#endif
  { "oicc_cmm_module-inbuild", &oicc_cmm_module },
#ifdef HAVE_OPENICC
  { "oiDB_cmm_module-inbuild", &oiDB_cmm_module },
#endif
#ifdef HAVE_JPEG
  { "oJPG_cmm_module-inbuild", &oJPG_cmm_module },
#endif
#ifdef HAVE_LIBPNG
  { "oPNG_cmm_module-inbuild", &oPNG_cmm_module },
#endif
  { "oydi_cmm_module-inbuild", &oydi_cmm_module },
  { "oyra_cmm_module-inbuild", &oyra_cmm_module },
#ifdef HAVE_LIBRAW_PLUGIN
  { "oyRE_cmm_module-inbuild", &oyRE_cmm_module },
#endif
#ifdef HAVE_X11
  { "oyX1_cmm_module-inbuild", &oyX1_cmm_module },
#endif
#ifdef APPLE
  { "qarz_cmm_module-inbuild", &qarz_cmm_module },
#endif
#ifdef HAVE_SANE
  { "SANE_cmm_module-inbuild", &SANE_cmm_module },
#endif
#ifdef HAVE_PTHREAD
  { "trds_cmm_module-inbuild", &trds_cmm_module },
#endif
  {NULL,NULL}
};

static int oy_cmms_n_ = sizeof(cmms) / sizeof(oyStaticCMMs_s) - 1;

char **      oyCMMsGetLibNamesStatic ( uint32_t          * n )
{
  char ** names = calloc(sizeof(char*), oy_cmms_n_ + 1);
  int i;
  for(i = 0; i < oy_cmms_n_; ++i)
    names[i] = strdup(cmms[i].name);

  *n = oy_cmms_n_;

  return names;
}

static int oy_cmm_infos_initialised_ = 0;
void oyCMMinfoRegister()
{
  oyCMM_s * cmm_info = NULL;
  oyCMMapi_s * api = 0;
  int i, error = 0,
      init = 1; /* 0 - reset */

  if(oy_cmm_infos_initialised_)
  {
    if(!oy_cmm_infos_)
    {
      init = 0; /* 0 - reset */
      oy_cmm_infos_initialised_ = 0;
    }
    else
      return;
  } else
  {
    if(!oy_cmm_infos_)
      oy_cmm_infos_ = oyStructList_Create( 0, "oy_cmm_infos_", 0 );

    /* initialise module type lookup */
    if(!oyStruct_GetTextFromModule_p)
      oyStruct_GetTextFromModule_p = oyStruct_GetTextFromModule;

    ++oy_cmm_infos_initialised_;
  }


  for(i = 0; i < oy_cmms_n_; ++i)
  {
    oyCMMhandle_s * cmm_handle = 0;

    cmm_info = cmms[i].cmm;

    if(cmm_info)
    {
      oyCMMinfoInit_f initF = oyCMMinfo_GetInitF((oyCMMinfo_s*) cmm_info);
      oyCMMinfoReset_f resetF = oyCMMinfo_GetResetF((oyCMMinfo_s*) cmm_info);
      {
        int error = 0;

        if(init && initF)
          error = initF( (oyStruct_s*)cmm_info );
        else if(!init && resetF)
          error = resetF( (oyStruct_s*)cmm_info );

        if(error > 0)
        {
          oyMessageFunc_p( oyMSG_ERROR, (oyStruct_s*) cmm_info, "%s returned with %d %s" ,
		                       init?"init":"reset", error, cmms[i].name );
          cmm_info = NULL;
        }

        if(error <= 0)
        {
          oyOBJECT_e type = oyCMMapi_Check_( oyCMMinfo_GetApi( (oyCMMinfo_s*) cmm_info ) );
          if(type != oyOBJECT_NONE)
          {
            api = oyCMMinfo_GetApi( (oyCMMinfo_s*) cmm_info );
          } else
          {
            cmm_info = NULL;
            DBG_NUM1_S("api check failed: %s", cmms[i].name);
          }
        }

        if(error <= 0 && api)
        {
          error = oyCMMapi_GetMessageFuncSetF(api)( oyMessageFunc_p );

          while(api)
          {
            const char * registration = oyCMMapi_GetRegistration(api);
            /* init */
            if(error <= 0)
            {
              if(init)
                error = oyCMMapi_GetInitF(api)( (oyStruct_s*) api );
              else
                error = oyCMMapi_GetResetF(api)( (oyStruct_s*) api );
            }
            if(error > 0)
            {
              cmm_info = NULL;
              DBG_NUM3_S("%s failed: %s %s", init?"init":"reset", cmms[i].name, registration);
            }
            api = oyCMMapi_GetNext(api);
          }
        }
      }
    }

    if(!init)
      continue;

    cmm_handle = oyCMMhandle_New_(0);

    /* store always */
    error = oyCMMhandle_Set_( cmm_handle, (oyCMMinfo_s*) cmm_info, NULL, cmms[i].name );
    if(error <= 0)
      oyStructList_MoveIn(oy_cmm_infos_, (oyStruct_s**)&cmm_handle, -1, 0);
  }

  if(!init)
    oyCMMinfoRegister();
}

#include "oyCMMinfo_s.h"
oyCMMinfo_s* oyCMMinfoFromLibNameStatic (
                                       const char        * lib_name )
{
  oyCMM_s * cmm_info = NULL;
  int i;

  oyCMMinfoRegister();

  for(i = 0; i < oy_cmms_n_; ++i)
    if(strcmp(cmms[i].name,lib_name) == 0)
    {
      cmm_info = cmms[i].cmm;
      break;
    }

  return (oyCMMinfo_s*) cmm_info;
}

#if defined(COMPILE_STATIC)
char **  (*oyCMMsGetLibNames_p) ( uint32_t* ) = &oyCMMsGetLibNamesStatic;
oyCMMinfo_s* (*oyCMMinfoFromLibName_p)(const char*) = &oyCMMinfoFromLibNameStatic;
#endif
