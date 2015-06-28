/** @file oyranos_cmm_oyIM_meta_c.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2008-2014 (C) Kai-Uwe Behrmann
 *
 *  @brief    meta module for C language module support in Oyranos
 *  @internal
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2008/12/16
 */

#include "oyranos_config_internal.h"

#include "oyCMM_s.h"
#include "oyCMMapi4_s_.h"

#include "oyranos_cmm.h"
#include "oyranos_cmm_oyIM.h"
#include "oyranos_helper.h"
#include "oyranos_icc.h"
#include "oyranos_i18n.h"
#include "oyranos_io.h"
#include "oyranos_definitions.h"
#include "oyranos_string.h"
#include "oyranos_texts.h"

#include <iconv.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef HAVE_POSIX
#include <dlfcn.h>
#endif



char * oyIMstructGetText             ( oyStruct_s        * item,
                                       oyNAME_e            type,
                                       int                 flags,
                                       oyAlloc_f           allocateFunc )
{
  char * text = 0;
  oyProfile_s * prof = 0;
  oyImage_s * image = 0;

  if(item->type_ == oyOBJECT_PROFILE_S)
  {
    text = oyStringCopy_( oyProfile_GetText( prof, oyNAME_DESCRIPTION ),
                          allocateFunc );
  } else if(item->type_ == oyOBJECT_IMAGE_S)
  {
    image = (oyImage_s*) item;

    if(flags == oyOBJECT_PROFILE_S)
    {
      oyProfile_s * p = oyImage_GetProfile( image );
      text = oyStringCopy_( oyProfile_GetText( p, type ),
                            allocateFunc );
      oyProfile_Release( &p );
    }
    else
      text = oyStringCopy_( oyObject_GetName( image->oy_, type ),
                            allocateFunc );
  }

  return text;
}



/**
 *  @brief   oyIM oyCMMapi5_s implementation
 *
 *  A filter interpreter loading. This function implements
 *  oyCMMFilterLoad_f for oyCMMapi5_s::oyCMMFilterLoad().
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/17 (Oyranos: 0.1.10)
 *  @date    2008/12/17
 */
oyCMMapiFilter_s * oyIMFilterLoad    ( oyPointer           data,
                                       size_t              size,
                                       const char        * file_name,
                                       oyOBJECT_e          type,
                                       int                 num )
{
  oyCMMapiFilter_s * api = 0;
  api = (oyCMMapiFilter_s*) oyCMMsGetApi__( type, file_name, 0,0, num );
  return api;
}

#ifdef NO_OPT
# ifdef HAVE_POSIX
# define DLOPEN 1
# endif
#endif

/**
 *  @brief   oyIM oyCMMapi5_s implementation
 *
 *  A interpreter preview for filters. This function implements
 *  oyCMMFilterScan_f for oyCMMapi5_s::oyCMMFilterScan().
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/13 (Oyranos: 0.1.10)
 *  @date    2008/12/13
 */
int          oyIMFilterScan          ( oyPointer           data,
                                       size_t              size,
                                       const char        * lib_name,
                                       oyOBJECT_e          type,
                                       int                 num,
                                       char             ** registration,
                                       char             ** name,
                                       oyAlloc_f           allocateFunc,
                                       oyCMMinfo_s      ** info,
                                       oyObject_s          object )
{
  oyCMM_s * cmm_info = 0;
  oyCMMapi_s_ * api = 0;
  oyCMMapi4_s_ * api4 = 0;
  int error = !lib_name;
  int ret = -2;
  char * cmm = oyCMMnameFromLibName_(lib_name);

  if(!error)
  {
#if DLOPEN
    oyPointer dso_handle = 0;

    if(!error)
    {
      if(lib_name)
        dso_handle = dlopen( lib_name, RTLD_LAZY );

      error = !dso_handle;

      if(error)
      {
        char * errstr = dlerror();
        WARNc2_S( "\n  dlopen( %s, RTLD_LAZY):\n  \"%s\"", lib_name,
                  oyNoEmptyString_m_( errstr ) );
        system("  echo $LD_LIBRARY_PATH");
      }
    }
#endif

    /* open the module */
    if(!error)
    {
#if DLOPEN
      char * info_sym = oyAllocateFunc_(24);

      oySprintf_( info_sym, "%s%s", cmm, OY_MODULE_NAME );
#endif

#if DLOPEN
      cmm_info = (oyCMM_s*) dlsym (dso_handle, info_sym);

      if(info_sym)
        oyFree_m_(info_sym);
#else
      cmm_info = (oyCMM_s*)oyCMMinfoFromLibName_( lib_name );
#endif

      error = !cmm_info;

#if DLOPEN
      if(error)
      {
        char * errstr = dlerror();
        WARNc2_S("\n  %s:\n  \"%s\"", lib_name, oyNoEmptyString_m_( errstr ) );
      }
#endif

      if(!error)
        if(oyCMMapi_Check_( cmm_info->api ))
          api = (oyCMMapi_s_*)cmm_info->api;

      if(!error && api)
      {
        int x = 0;
        int found = 0;
        while(!found)
        {
          if(api && api->type_ == type)
          {
            if(x == num)
              found = 1;
            else
              ++x;
          }
          if(!api)
            found = 1;
          if(!found)
            api = (oyCMMapi_s_*)api->next;
        }

        if(api && found)
        {
          if(api->type_ == type)
            api4 = (oyCMMapi4_s_ *) api;
          if(registration)
            *registration = oyStringCopy_( api4->registration, allocateFunc );
          if(name)
            *name = oyStringCopy_( api4->ui->getText("name",oyNAME_NAME,
                                                     (oyStruct_s*)api4->ui),
                                   allocateFunc );
          if(info)
            *info = oyCMMinfo_Copy( (oyCMMinfo_s*)cmm_info, object );
          ret = 0;
        } else
          ret = -1;
      }
    }

#if DLOPEN
    if(dso_handle)
      dlclose( dso_handle );
    dso_handle = 0;
#endif
  }

  if(error)
    ret = error;

  if(cmm)
    oyDeAllocateFunc_(cmm);
  cmm = 0;

  return ret;
}

/** @instance oyIM_api5
 *  @brief    oyIM oyCMMapi5_s implementation
 *
 *  a interpreter for imaging modules
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/11/13 (Oyranos: 0.1.9)
 *  @date    2010/06/25
 */
oyCMMapi5_s_ oyIM_api5_meta_c = {

  oyOBJECT_CMM_API5_S, /* oyStruct_s::type */
  0,0,0, /* unused oyStruct_s fileds; keep to zero */
  0, /* oyCMMapi_s * next */
  
  oyIMCMMInit, /* oyCMMInit_f */
  oyIMCMMMessageFuncSet, /* oyCMMMessageFuncSet_f */

  /* @todo registration: for what? */
  OY_TOP_SHARED OY_SLASH OY_DOMAIN_INTERNAL OY_SLASH OY_TYPE_STD OY_SLASH CMM_NICK,

  {0,0,2}, /* int32_t version[3] */
  CMM_API_VERSION, /**< int32_t module_api[3] */
  0,   /* id_; keep empty */

  OY_CMMSUBPATH, /* sub_paths */
  0, /* ext */
  0, /* data_type - 0: libs - libraries,\n  1: scripts - platform independent filters */

  oyIMFilterLoad, /* oyCMMFilterLoad_f oyCMMFilterLoad */
  oyIMFilterScan, /* oyCMMFilterScan_f */
};


