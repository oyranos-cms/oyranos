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
#include "oyranos_config_internal.h"

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

#include "oyCMMinfo_s.h"
oyCMMinfo_s* oyCMMinfoFromLibNameStatic (
                                       const char        * lib_name )
{
  oyCMM_s * cmm_info = NULL;
  oyCMMapi_s * api = 0;
  int i;
  for(i = 0; i < oy_cmms_n_; ++i)
    if(strcmp(cmms[i].name,lib_name) == 0)
    {
      cmm_info = cmms[i].cmm;
      break;
    }

  if(cmm_info)
  {
    oyCMMinfoInit_f init = oyCMMinfo_GetInitF((oyCMMinfo_s*) cmm_info);
    {
      int error = 0;

      if(init)
        error = init( (oyStruct_s*)cmm_info );
      if(error > 0)
      {
        oyMessageFunc_p( oyMSG_ERROR, (oyStruct_s*) cmm_info, "init returned with %d %s" ,
		         error, lib_name );
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
          DBG_NUM1_S("api check failed: %s", lib_name);
        }
      }

      if(error <= 0 && api)
      {
        error = oyCMMapi_GetMessageFuncSetF(api)( oyMessageFunc_p );

        /* init */
        if(error <= 0)
          error = oyCMMapi_GetInitF(api)( (oyStruct_s*) api );
        if(error > 0)
        {
          cmm_info = NULL;
          DBG_NUM1_S("init failed: %s", lib_name);
        }
      }
    }
  }

  return (oyCMMinfo_s*) cmm_info;
}

#if defined(COMPILE_STATIC)
char **  (*oyCMMsGetLibNames_p) ( uint32_t* ) = &oyCMMsGetLibNamesStatic;
oyCMMinfo_s* (*oyCMMinfoFromLibName_p)(const char*) = &oyCMMinfoFromLibNameStatic;
#endif
