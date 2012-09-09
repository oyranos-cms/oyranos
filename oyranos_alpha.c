/** @file oyranos_alpha.c
 *
 *  Oyranos is an open source Colour Management System 
 *
 *  @par Copyright:
 *            2004-2012 (C) Kai-Uwe Behrmann
 *
 *  @brief    object APIs
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/bsd-license.php>
 *  @since    2004/11/25
 */

#include "oyCMMapis_s.h"
#include "oyCMMapi6_s.h"
#include "oyCMMapi8_s.h"
#include "oyCMMapi9_s.h"
#include "oyCMMInfo_s.h"
#include "oyConfig_s.h"
#include "oyConfigs_s.h"
#include "oyConfig_s_.h"
#include "oyObserver_s.h"
#include "oyOption_s.h"
#include "oyOptions_s.h"
#include "oyPointer_s.h"
#include "oyProfile_s_.h"
#include "oyStructList_s_.h"
#include "oyranos_devices.h"

#include "oyranos_types.h"
#include "oyranos_alpha.h"
#include "oyranos_alpha_internal.h"
#include "oyranos_object_internal.h"
#include "oyranos_generic_internal.h"
#include "oyranos_cmm.h"
#include "oyranos_elektra.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_io.h"
#include "oyranos_sentinel.h"
#include "oyranos_string.h"
#include "oyranos_texts.h"

#include "oyjl/oyjl_tree.h"

#ifdef HAVE_POSIX
#include <dlfcn.h>
#else
#include <ltdl.h>
#define dlopen  lt_dlopen
#define dlsym   lt_dlsym
#define dlerror lt_dlerror
#define dlclose lt_dlclose
#endif
#include <math.h>
#include <locale.h>   /* LC_NUMERIC */
#include <limits.h>

#define OY_ERR if(l_error != 0) error = l_error;

#ifdef DEBUG_
#define DEBUG_OBJECT 1
#endif

#ifdef NO_OPT
/* speed comparision test */
#define USE_OLD_STRING_API 1
#else
#define OY_USE_OBJECT_POOL_ 0
#endif



/* internal declarations */

char **          oyCMMsGetNames_     ( uint32_t          * n,
                                       const char        * sub_path,
                                       const char        * ext,
                                       const char        * required_cmm,
                                       oyPATH_TYPE_e       path_type );
char **          oyCMMsGetLibNames_  ( uint32_t          * n,
                                       const char        * required_cmm );

/** \addtogroup alpha Alpha API's

 *  @{
 */

/** \addtogroup misc Miscellaneous

 *  @{
 */


#define PT_ANY       0    /* Don't check colorspace */
                          /* 1 & 2 are reserved */
#define PT_GRAY      3
#define PT_RGB       4
#define PT_CMY       5
#define PT_CMYK      6
#define PT_YCbCr     7
#define PT_YUV       8      /* Lu'v' */
#define PT_XYZ       9
#define PT_Lab       10
#define PT_YUVK      11     /* Lu'v'K */
#define PT_HSV       12
#define PT_HLS       13
#define PT_Yxy       14
#define PT_HiFi      15

/**
 *  @internal
 *  Function lcmsColorSpace
 *  @brief get lcms notation from icColorSpaceSignature
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/21 (API 0.1.8)
 */
int lcmsColorSpace(icColorSpaceSignature ProfileSpace)
{    
       switch ((unsigned int)ProfileSpace) {

       case icSigGrayData: return  PT_GRAY;
       case icSigRgbData:  return  PT_RGB;
       case icSigCmyData:  return  PT_CMY;
       case icSigCmykData: return  PT_CMYK;
       case icSigYCbCrData:return  PT_YCbCr;
       case icSigLuvData:  return  PT_YUV;
       case icSigXYZData:  return  PT_XYZ;
       case icSigLabData:  return  PT_Lab;
       case icSigLuvKData: return  PT_YUVK;
       case icSigHsvData:  return  PT_HSV;
       case icSigHlsData:  return  PT_HLS;
       case icSigYxyData:  return  PT_Yxy;

       case icSig6colorData:
       case icSigMCH6Data: return PT_HiFi;

       default:  return 0;
       }
}

/** Function oyDumpColourToCGATS
 *  @brief create CGATS text from double value array and a profile
 *
 *  The funtion operates on raw colour values. Alpha is not supported.
 *  The intention is to have a small debugging only tool. The values are
 *  required to come prescaled (IE RGB 0...255, XYZ 0...1, CMYK 0...100 ...)
 *
 *  @todo should be merged to some structure like oyImage_s.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/21 (API 0.1.8)
 */
oyChar *     oyDumpColourToCGATS     ( const double      * channels,
                                       size_t              n,
                                       oyProfile_s       * prof,
                                       oyAlloc_f           allocateFunc,
                                       const oyChar      * DESCRIPTOR )
{
  int channels_n = oyProfile_GetChannelsCount( prof );
  int i;
  size_t len = n * channels_n * 16 + 2024;
  oyChar * text = allocateFunc( len );
  const oyChar * prof_name = prof ? ((oyProfile_s_*)prof)->file_name_ : "";
  const char * format = "";
  char * daten = text;
  /*double value = 0;*/

  const char *fomate[17] = {
    "SAMPLE_ID UNKNOWN 0", /* PT_ANY */
    "SAMPLE_ID UNKNOWN 1", /* 1 */
    "SAMPLE_ID UNKNOWN 2", /* 2 */
    "SAMPLE_ID        GRAY", /* 3 PT_GRAY */
    "SAMPLE_ID        RGB_R RGB_G RGB_B", /* 4 PT_RGB */
    "SAMPLE_ID        CMY_C  CMY_M  CMY_Y", /* 5 PT_CMY */
    "SAMPLE_ID        CMYK_C  CMYK_M  CMYK_Y  CMYK_K", /* 6 PT_CMYK */
    "SAMPLE_ID        ", /* 7 PT_YCbCr */
    "SAMPLE_ID        ", /* 8 PT_YUV */
    "SAMPLE_ID        XYZ_X XYZ_Y XYZ_Z", /* 9 PT_XYZ */
    "SAMPLE_ID        LAB_L LAB_A LAB_B", /* 10 PT_Lab */
    "SAMPLE_ID        ", /* 11 PT_YUVK */
    "SAMPLE_ID        ", /* 12 PT_HSV */
    "SAMPLE_ID        ", /* 13 PT_HLS */
    "SAMPLE_ID        ", /* 14 PT_Yxy */
    "SAMPLE_ID        ", /* 15 PT_HiFi */
    0
  };

  icColorSpaceSignature cspace = oyProfile_GetSignature( prof, oySIGNATURE_COLOUR_SPACE );

  int lcms_space = lcmsColorSpace( cspace );
  char * save_locale = oyStringCopy_( setlocale(LC_NUMERIC, 0 ), oyAllocateFunc_);
  format = fomate[lcms_space];

  n *= channels_n;

  oySprintf_( daten ,
              "OYCGATS\n"
              "\n"
              "ORIGINATOR        \"%s\"\n"
              "DESCRIPTOR        \"%s\"\n"
              "CREATED           \"%s\"\n"
              "\n"
              "NUMBER_OF_FIELDS  %d\n"
              "BEGIN_DATA_FORMAT\n"
              "%s\n"
              "END_DATA_FORMAT\n"
              "\n"
              "NUMBER_OF_SETS    %d\n"
              "BEGIN_DATA\n",
              prof_name?prof_name:"--",
              DESCRIPTOR?DESCRIPTOR:"--",
              "--",
              channels_n + 1,
              format,
              (int)n/channels_n
            );

  setlocale(LC_NUMERIC, "C");
    for(i = 0; i < (int)n; ++i)
    {
      int modulo_k = i%(channels_n);
      if(modulo_k)
        oySprintf_( &daten[oyStrlen_(daten)], "       ");
      else
      {
        if(i)
          oySprintf_( &daten[oyStrlen_(daten)], "\n");
        oySprintf_( &daten[strlen(daten)], "%d     ", (i/channels_n) + 1);
      }
      oySprintf_( &daten[oyStrlen_(daten)], "%.03f", channels[i] );
    }
    oySprintf_( &daten[oyStrlen_(daten)], "\nEND_DATA\n");

  setlocale(LC_NUMERIC, save_locale);
  oyFree_m_( save_locale );
    
  return text;
}


/** @brief copy pure colours
 *
 *  Handle colour only.
 *  With a empty \b from variable set -1 as default in \b to.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  september 2007 (API 0.1.8)
 */
void
oyCopyColour ( const double * from,
               double       * to,
               int            n,
               oyProfile_s  * ref,
               int            channels_n )
{
  int i, j;
  icColorSpaceSignature sig = 0;
  int c = 0;
  int error = 0;

  if(!n || !to)
    return;

  if(ref)
    sig = oyProfile_GetSignature ( ref, oySIGNATURE_COLOUR_SPACE );
  else
    sig = icSigXYZData;

  c = oyICCColourSpaceGetChannelCount( sig );

  if(from)
  {
    error = !memcpy( to, from, sizeof(double) * (n * c) );
    if(error)
      WARNc_S("Problem with memcpy.");

  } 
  else
  {
    if(!channels_n && c)
      channels_n = c;
    else if(channels_n && !c)
      c = channels_n;

    if(channels_n)
    switch(sig)
    {
      case icSigLabData:
      case icSigLuvData:
      case icSigHsvData:
      case icSigHlsData:
      case icSigYCbCrData:
           for( i = 0; i < n; ++i )
           {
             to[i*channels_n+0] = -1;
             to[i*channels_n+1] = 0;
             to[i*channels_n+2] = 0;
             for( j = c; j < channels_n; ++j )
               if(j==c)
                 to[i*channels_n+j] = 1; /* set alpha */
               else
                 to[i*channels_n+j] = 0;
           }
           break;
      case icSigXYZData:
      case icSigRgbData:
      default:
           for( i = 0; i < n; ++i )
           {
             for( j = 0; j < channels_n; ++j )
               if(j < c)
                 to[i*channels_n+j] = -1;
               else if(j==c)
                 to[i*channels_n+j] = 1; /* set alpha */
               else
                 to[i*channels_n+j] = 0;
           }
           break;
    }
  }
}


/** @} *//* misc */





/* --- Object handling API's start ------------------------------------ */


/** \addtogroup misc Miscellaneous

 *  @{
 */


/** \addtogroup objects_generic Generic Objects

 *  @{
 */



/** Function oyCallback_New
 *  @memberof oyCallback_s
 *  @brief   allocate a new Callback object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/18 (Oyranos: 0.1.10)
 *  @date    2009/11/18
 */
OYAPI oyCallback_s * OYEXPORT
           oyCallback_New            ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CALLBACK_S;
# define STRUCT_TYPE oyCallback_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = 0;

  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );
  if(error)
    WARNc_S( "memset failed" );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyCallback_Copy;
  s->release = (oyStruct_Release_f) oyCallback_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
  if(error)
    WARNc_S( "oyObject_SetParent failed" );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */


  return s;
}

/** @internal
 *  Function oyCallback_Copy_
 *  @memberof oyCallback_s
 *  @brief   real copy a Callback object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/18 (Oyranos: 0.1.10)
 *  @date    2009/11/18
 */
oyCallback_s * oyCallback_Copy_
                                     ( oyCallback_s      * obj,
                                       oyObject_s          object )
{
  oyCallback_s * s = 0;
  int error = 0;

  if(!obj || !object)
    return s;

  s = oyCallback_New( object );
  error = !s;

  if(!error)
  {
    s->callback = obj->callback;
    s->data = obj->data;
  }

  if(error)
    oyCallback_Release( &s );

  return s;
}

/** Function oyCallback_Copy
 *  @memberof oyCallback_s
 *  @brief   copy or reference a Callback object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/18 (Oyranos: 0.1.10)
 *  @date    2009/11/18
 */
OYAPI oyCallback_s * OYEXPORT
           oyCallback_Copy           ( oyCallback_s      * obj,
                                       oyObject_s          object )
{
  oyCallback_s * s = obj;

  if(!obj)
    return 0;

  oyCheckType__m( oyOBJECT_CALLBACK_S, return 0 )

  if(obj && !object)
  {
    s = obj;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyCallback_Copy_( obj, object );

  return s;
}
 
/** Function oyCallback_Release
 *  @memberof oyCallback_s
 *  @brief   release and possibly deallocate a Callback object
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/11/18 (Oyranos: 0.1.10)
 *  @date    2009/11/18
 */
OYAPI int  OYEXPORT
           oyCallback_Release        ( oyCallback_s     ** obj )
{
  /* ---- start of common object destructor ----- */
  oyCallback_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  oyCheckType__m( oyOBJECT_CALLBACK_S, return 1 )

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */


  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}




/**
 *  @} *//* objects_generic
 */

/** @} *//* misc */



/** \addtogroup module_api

 *  @{
 */



/** @internal
 *  @brief   convert between oyPointer_s data
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2008/12/28
 */
int          oyPointer_ConvertData   ( oyPointer_s       * cmm_ptr,
                                       oyPointer_s       * cmm_ptr_out,
                                       oyFilterNode_s    * node )
{
  int error = !cmm_ptr || !cmm_ptr_out;
  oyCMMapi6_s * api6 = 0;
  char * reg = 0, * tmp = 0;

  if(error <= 0)
  {
    reg = oyStringCopy_( "//", oyAllocateFunc_ );
    tmp = oyFilterRegistrationToText( node->core->registration_,
                                      oyFILTER_REG_TYPE,0);
    STRING_ADD( reg, tmp );
    oyFree_m_( tmp );
    STRING_ADD( reg, "/" );
    STRING_ADD( reg, oyPointer_GetResourceName( cmm_ptr ) );
    STRING_ADD( reg, "_" );
    STRING_ADD( reg, oyPointer_GetResourceName( cmm_ptr_out ) );

    api6 = (oyCMMapi6_s*) oyCMMsGetFilterApi_( 0, reg, oyOBJECT_CMM_API6_S );

    error = !api6;
  }

  if(error <= 0 && api6->oyModuleData_Convert)
    error = api6->oyModuleData_Convert( cmm_ptr, cmm_ptr_out, node );
  else
    error = 1;

  if(error)
    WARNc_S("Could not convert context");

  return error;
}


oyStructList_s * oy_cmm_handles_ = 0;
/** @internal
 *  @brief Oyranos wrapper for dlopen
 *
 *  @since Oyranos: version 0.1.8
 *  @date  28 november 2007 (API 0.1.8)
 */
int oyDlclose(oyPointer* handle)
{
  if(handle && *handle)
  {
    dlclose(*handle);
    *handle = 0;
    return 0;
  }
  return 1;
}

/** @internal
 *  @brief register Oyranos CMM dlopen handle
 *
 *  use in pair with oyCMMdsoRelease_
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
int          oyCMMdsoReference_    ( const char        * lib_name,
                                     oyPointer           ptr )
{
  int i, n;
  int found = 0;
  int error = 0;

  if(!oy_cmm_handles_)
  {
    oy_cmm_handles_ = oyStructList_New( 0 );
    error = !oy_cmm_handles_;
  }

  if(error <= 0 && oy_cmm_handles_->type_ != oyOBJECT_STRUCT_LIST_S)
    error = 1;

  n = oyStructList_Count(oy_cmm_handles_);
  if(error <= 0)
  for(i = 0; i < n; ++i)
  {
    oyStruct_s * obj = oyStructList_Get_((oyStructList_s_*)oy_cmm_handles_, i);
    oyPointer_s * s;

    if(obj && obj->type_ == oyOBJECT_POINTER_S)
      s = (oyPointer_s*) obj;
    else
      s = 0;

    if( s && oyPointer_GetLibName( s ) && lib_name &&
        !oyStrcmp_( oyPointer_GetLibName( s ), lib_name ) )
    {
      found = 1;
      oyStructList_ReferenceAt_((oyStructList_s_*)oy_cmm_handles_, i);
      if(ptr)
      {
        if(!oyPointer_GetPointer( s ))
          oyPointer_Set( s, 0, 0, ptr, 0, 0 );
        /*else
          WARNc_S(("Attempt to register dso handle multiple times."));*/
      }
    }
  }

  if(!found)
  {
    oyPointer_s * s = oyPointer_New(0);
    oyStruct_s * oy_cmm_struct = 0;

    error = !s;

    if(error <= 0)
      error = oyPointer_Set( s, lib_name, 0, ptr, "oyDlclose", oyDlclose );

    if(error <= 0)
      oy_cmm_struct = (oyStruct_s*) s;

    if(error <= 0)
      oyStructList_MoveIn(oy_cmm_handles_, &oy_cmm_struct, -1, 0);
  }

  return error;
}

/** @internal
 *  @brief search a dlopen handle for a Oyranos CMM
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 november 2007 (API 0.1.8)
 */
int          oyCMMdsoSearch_         ( const char        * lib_name )
{
  int i, n;
  int pos = -1;
  int error = 0;

  if(!oy_cmm_handles_)
    return 1;

  if(oy_cmm_handles_->type_ != oyOBJECT_STRUCT_LIST_S)
    error = 1;

  n = oyStructList_Count(oy_cmm_handles_);
  if(error <= 0)
  for(i = 0; i < n; ++i)
  {
    oyStruct_s * obj = oyStructList_Get_((oyStructList_s_*)oy_cmm_handles_, i);
    oyPointer_s * s = 0;

    if(obj && obj->type_ == oyOBJECT_POINTER_S)
      s = (oyPointer_s*) obj;

    error = !s;

    if(error <= 0)
    if( oyPointer_GetLibName(s) && lib_name &&
        !oyStrcmp_( oyPointer_GetLibName(s), lib_name ) )
      pos = i;
  }

  return pos;
}

/* defined in sources/Struct.public_methods_definitions.c */
extern const char * (*oyStruct_GetTextFromModule_p) (
                                       oyStruct_s        * obj,
                                       oyNAME_e            name_type,
                                       uint32_t            flags );

/** Function  oyStruct_GetTextFromModule
 *  @memberof oyStruct_s
 *  @brief    get object infos from a module
 *
 *  @param[in,out] obj                 the objects structure
 *  @param[in]     name_type           the type
 *  @param[in]     flags               @see oyStruct_GetText
 *  @return                            the text
 *
 *  @version  Oyranos: 0.3.3
 *  @since    2009/09/15 (Oyranos: 0.3.3)
 *  @date     2011/10/31
 */
const char * oyStruct_GetTextFromModule (
                                       oyStruct_s        * obj,
                                       oyNAME_e            name_type,
                                       uint32_t            flags )
{
  int error = !obj;
  const char * text = 0;

  if(!error)
    text = oyObject_GetName( obj->oy_, oyNAME_NICK );

  if(!error && !text)
  {
    if(obj->type_)
    {
      oyCMMapiFilters_s * apis;
      int apis_n = 0, i,j,n;
      oyCMMapi9_s * cmm_api9 = 0;
      char * api_reg = 0;

      apis = oyCMMsGetFilterApis_( 0,0, api_reg, oyOBJECT_CMM_API9_S,
                                   oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR,
                                   0, 0);
      apis_n = oyCMMapiFilters_Count( apis );
      for(i = 0; i < apis_n; ++i)
      {
        cmm_api9 = (oyCMMapi9_s*) oyCMMapiFilters_Get( apis, i );

        n = 0;
        while( cmm_api9->object_types && cmm_api9->object_types[n])
          ++n;
        for(j = 0; j < n; ++j)
          if( cmm_api9->object_types[j]->oyCMMobjectGetText &&
              cmm_api9->object_types[j]->id == obj->type_ )
          {
            text = cmm_api9->object_types[j]->oyCMMobjectGetText( flags?0:obj,
                                                   name_type, 0 );
            if(text)
              break;
          }

        if(cmm_api9->release)
          cmm_api9->release( (oyStruct_s**)&cmm_api9 );

        if(text)
          break;
      }
      oyCMMapiFilters_Release( &apis );
    }
  }

  if(!error && !text)
    text = oyStructTypeToText( obj->type_ );

  return text;
}

/** @internal
 *  @brief release Oyranos CMM dlopen handle
 *
 *  use in pair in oyCMMdsoReference_
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 *  @date    2008/12/08
 */
int          oyCMMdsoRelease_      ( const char        * lib_name )
{
  int error = 0;

  /* inactive */
#if 0
  int found = -1;

  if(!oy_cmm_handles_)
    return 1;

  if(oy_cmm_handles_->type_ != oyOBJECT_STRUCT_LIST_S)
    error = 1;

  if(error <= 0)
    found = oyCMMdsoSearch_(cmm);

  if(found >= 0)
    oyStructList_ReleaseAt(oy_cmm_handles_, found);
#endif
  return error;
}

/** @internal
 *  @brief get Oyranos CMM dlopen handle
 *
 *  Search the cache for a handle and return it. Or dlopen the library and 
 *  store and reference the handle in the cache.
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 november 2007 (API 0.1.8)
 */
oyPointer    oyCMMdsoGet_            ( const char        * cmm,
                                       const char        * lib_name )
{
  int found = -1;
  oyPointer dso_handle = 0;

if(!lib_name)
    return 0;

#ifdef HAVE_POSIX
  found = oyCMMdsoSearch_(lib_name);

  if(found >= 0)
  {
    oyPointer_s * s = (oyPointer_s*)oyStructList_GetType_( (oyStructList_s_*)oy_cmm_handles_, found,
                                                  oyOBJECT_POINTER_S );

    if(s)
      dso_handle = oyPointer_GetPointer(s);
  }

  if(!dso_handle)
  {
    dso_handle = dlopen( lib_name, RTLD_LAZY );

    if(!dso_handle)
    {
      WARNc2_S( "\n  dlopen( %s, RTLD_LAZY):\n  \"%s\"", lib_name, dlerror() );
      system("  echo $LD_LIBRARY_PATH");
    }

    /* initialise module type lookup */
    if(!oyStruct_GetTextFromModule_p)
      oyStruct_GetTextFromModule_p = oyStruct_GetTextFromModule;
  }

  if(dso_handle)
    oyCMMdsoReference_( lib_name, dso_handle );

  return dso_handle;
#endif
}



/** @internal
 *  @brief new module handle
 *
 *  @since Oyranos: version 0.1.8
 *  @date  6 december 2007 (API 0.1.8)
 */
oyCMMhandle_s *    oyCMMhandle_New_    ( oyObject_s        object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CMM_HANDLE_S;
# define STRUCT_TYPE oyCMMhandle_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = 0;
  
  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );
  if(error)
    WARNc_S("memset failed");

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyCMMhandle_Copy_;
  s->release = (oyStruct_Release_f) oyCMMhandle_Release_;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  return s;
}

/** @internal
 *  @brief copy module handle 
 *
 *  @since Oyranos: version 0.1.8
 *  @date  6 december 2007 (API 0.1.8)
 */
oyCMMhandle_s *  oyCMMhandle_Copy_     ( oyCMMhandle_s   * handle,
                                         oyObject_s        object )
{
  int error = !handle;
  oyCMMhandle_s * s = 0;

  if(error <= 0 && object)
  {
    s = oyCMMhandle_New_(object);
    error = !s;
    if(error <= 0)
    {
      if(handle)
        WARNc1_S("Dont know how to copy CMM[%s] handle.", handle->lib_name);

      error = oyCMMhandle_Set_( s, 0, 0, 0 );
      if(!error)
        oyCMMhandle_Release_( &s );

    } else
      WARNc_S("Could not create a new object.");

  } else if(handle)
  {
    oyObject_Copy( handle->oy_ );
    s = handle;
  }

  return s;
}

/** @internal
 *  @brief release module handle 
 *
 *  @since Oyranos: version 0.1.8
 *  @date  6 december 2007 (API 0.1.8)
 */
int              oyCMMhandle_Release_( oyCMMhandle_s    ** obj )
{
  int error = 0;
  /* ---- start of common object destructor ----- */
  oyCMMhandle_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  oyCheckType__m( oyOBJECT_CMM_HANDLE_S, return 1 )

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  error = oyCMMdsoRelease_( s->lib_name );

  s->dso_handle = 0;
  s->info = 0;
  oyFree_m_( s->lib_name );


  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return error;
}

/** @internal
 */
int              oyCMMhandle_Set_    ( oyCMMhandle_s     * handle,
                                       oyCMMInfo_s       * info,
                                       oyPointer           dso_handle,
                                       const char        * lib_name )
{
  int error = 0;

  if(!handle || !info || !dso_handle)
    error = 1;

  if(error <= 0)
  {
    handle->info = info;
    handle->dso_handle = dso_handle;
    handle->lib_name = oyStringCopy_( lib_name, handle->oy_->allocateFunc_ );
    oyCMMdsoReference_( handle->lib_name, dso_handle );
  }

  return error;
}



/** @internal
 *  @brief internal Oyranos module handle list
 *
 *  @since Oyranos: version 0.1.8
 *  @date  6 december 2007 (API 0.1.8)
 */
oyStructList_s * oy_cmm_infos_ = 0;

/** @internal
 *  @brief search a Oyranos module handle in a internal list
 *
 *  a intermediate step
 *
 *  @since Oyranos: version 0.1.8
 *  @date  6 december 2007 (API 0.1.8)
 */
oyCMMhandle_s *  oyCMMFromCache_     ( const char        * lib_name )
{
  int error = !lib_name;
  int n, i;
  oyCMMhandle_s * cmm_handle = 0;

  if(error <= 0 && !oy_cmm_infos_)
  {
    oy_cmm_infos_ = oyStructList_New( 0 );
    error = !oy_cmm_infos_;
  }

  if(error <= 0 && oy_cmm_infos_->type_ != oyOBJECT_STRUCT_LIST_S)
    error = 1;

  n = oyStructList_Count(oy_cmm_infos_);
  if(error <= 0)
  for(i = 0; i < n; ++i)
  {
    oyCMMhandle_s * cmmh = (oyCMMhandle_s*) oyStructList_GetType_((oyStructList_s_*)oy_cmm_infos_,
                                                i, oyOBJECT_CMM_HANDLE_S );
    oyCMMInfo_s * s = 0;

    if(cmmh)
      s = (oyCMMInfo_s*) cmmh->info;

    if( s && s->type == oyOBJECT_CMM_INFO_S &&
        (uint32_t*)&s->cmm &&
        !oyStrcmp_( cmmh->lib_name, lib_name ) )
    {
      cmm_handle = oyCMMhandle_Copy_( cmmh, 0 );
      error = oyStructList_ReferenceAt_( (oyStructList_s_*)oy_cmm_infos_, i );
      if(!error)
        break;
    }
  }

  return cmm_handle;
}

/** @internal
 *
 *  @since Oyranos: version 0.1.8
 *  @date  11 december 2007 (API 0.1.8)
 */
char *           oyCMMnameFromLibName_(const char        * lib_name)
{
  char * cmm = 0;

  if(lib_name && lib_name[0])
  {
    const char * tmp = oyStrstr_( lib_name, OY_MODULE_NAME );

    if(!tmp && oyStrlen_( lib_name ) == 4)
      return oyStringCopy_( lib_name, oyAllocateFunc_ );

    if(tmp)
    {
      cmm = oyAllocateFunc_(5);
      memcpy(cmm, tmp-4, 4);
      cmm[4] = 0;
    }
  }

  return cmm;
}

/** @internal
 *  @brief compare a library name with a Oyranos CMM
 *
 *  @version Oyranos: 0.1.9
 *  @since   2007/12/08 (Oyranos: 0.1.9)
 *  @date    2008/12/08
 */
int              oyCMMlibMatchesCMM  ( const char        * lib_name,
                                       const char        * cmm )
{
  int matches = 0;
  char * tmp = oyCMMnameFromLibName_( lib_name );

  if(oyStrcmp_(tmp, cmm) == 0)
    matches = 1;

  oyFree_m_( tmp );

  return matches;
}

/** @internal
 *  @brief get all CMM/module/script names
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/16 (Oyranos: 0.1.9)
 *  @date    2010/06/28
 */
char **          oyCMMsGetNames_     ( uint32_t          * n,
                                       const char        * sub_path,
                                       const char        * ext,
                                       const char        * required_cmm,
                                       oyPATH_TYPE_e       path_type )
{
  int error = !n;
  char ** files = 0,
       ** sub_paths = 0;
  int sub_paths_n = 0;

  if(error <= 0)
  {
    int  files_n = 0, i;
    char * lib_string = oyAllocateFunc_(24);
    const char * cmm = 0;

    lib_string[0] = 0;
    if(required_cmm)
    {
      /* assuming a library file can not be smaller than the signature + 
         OY_MODULE_NAME + file extension */
      if(oyStrlen_(required_cmm) < 5)
        cmm = required_cmm;

      if(cmm)
        oySprintf_( lib_string, "%s%s", cmm, OY_MODULE_NAME );
      else
        oyStringListAddStaticString_( &files, &files_n, required_cmm,
                                       oyAllocateFunc_, oyDeAllocateFunc_ );

    } else
      oySprintf_( lib_string, "%s", OY_MODULE_NAME );

    sub_paths = oyStringSplit_( sub_path, ':', &sub_paths_n, 0 );

    /* search for a matching module file */
    for(i = 0; i < sub_paths_n; ++i)
    {
      if(!files)
      {
        if(path_type == oyPATH_MODULE)
          files = oyLibFilesGet_( &files_n, sub_paths[i], oyUSER_SYS,
                                  0, lib_string, 0, oyAllocateFunc_ );
        else if(path_type == oyPATH_SCRIPT)
          files = oyDataFilesGet_( &files_n, sub_paths[i], oyYES, oyUSER_SYS,
                                   0, lib_string, ext, oyAllocateFunc_ );
      }
    }
    error = !files;

    if(!error)
      *n = files_n;

    if(sub_paths_n && sub_paths)
      oyStringListRelease_( &sub_paths, sub_paths_n, oyDeAllocateFunc_ );

    if( lib_string )
      oyFree_m_(lib_string);
  }

  return files;
}

/** @internal
 *  @brief get all module names
 *
 *  @version Oyranos: 0.1.9
 *  @since   2007/12/12 (Oyranos: 0.1.8)
 *  @date    2008/12/16
 */
char **          oyCMMsGetLibNames_  ( uint32_t          * n,
                                       const char        * required_cmm )
{
#ifdef HAVE_POSIX
  return oyCMMsGetNames_(n, OY_METASUBPATH, 0, required_cmm, oyPATH_MODULE);
#else
  *n = 0;
  return 0;
#endif
}

/** @internal
 *  @brief pick from cache or ldopen a CMM and get the Oyranos module infos
 *
 *  @since Oyranos: version 0.1.8
 *  @date  5 december 2007 (API 0.1.8)
 */
oyCMMInfo_s *    oyCMMOpen_          ( const char        * lib_name )
{
  oyCMMInfo_s * cmm_info = 0;
  oyCMMapi_s * api = 0;
  oyCMMhandle_s * cmm_handle = 0;
  int error = !lib_name;
  char * cmm = oyCMMnameFromLibName_(lib_name);
  const char * error_text = 0;

  if(error <= 0)
  {
    oyPointer dso_handle = 0;

    if(error <= 0)
    {
      if(lib_name)
        dso_handle = oyCMMdsoGet_(cmm, lib_name);

      error = !dso_handle;

      if(error)
      {
        error_text = dlerror();
        WARNc2_S( "\n  error while dlopen'ing lib:\n    %s\n  dlerror(): %s",
                  lib_name, error_text ? error_text : "no text" );
      }
    }

    /* open the module */
    if(error <= 0)
    {
      char * info_sym = oyAllocateFunc_(24);
      int api_found = 0;

      oySprintf_( info_sym, "%s%s", cmm, OY_MODULE_NAME );

      cmm_info = (oyCMMInfo_s*) dlsym (dso_handle, info_sym);

      if(info_sym)
        oyFree_m_(info_sym);

      error = !cmm_info;

      if(error)
        WARNc_S(dlerror());

      if(error <= 0)
        if(oyCMMapi_Check_( cmm_info->api ))
          api = cmm_info->api;

      if(error <= 0 && api)
      {
        error = api->oyCMMMessageFuncSet( oyMessageFunc_p );

        cmm_handle = oyCMMhandle_New_(0);

        /* init */
        if(error <= 0)
        error = api->oyCMMInit( (oyStruct_s*) api );
        if(error <= 0)
          error = oyCMMhandle_Set_( cmm_handle, cmm_info, dso_handle, lib_name);

        api_found = 1;
      }

      /* store */
      if(error <= 0 && api_found)
        oyStructList_MoveIn(oy_cmm_infos_, (oyStruct_s**)&cmm_handle, -1, 0);
    }

    oyCMMdsoRelease_( lib_name );
  }

  if(cmm)
    oyDeAllocateFunc_(cmm);
  cmm = 0;

  return cmm_info;
}

/** @internal
 *  @brief pick from cache or ldopen a CMM and get the Oyranos module infos
 *
 *  @since Oyranos: version 0.1.8
 *  @date  5 december 2007 (API 0.1.8)
 */
oyCMMInfo_s *    oyCMMInfoFromLibName_(const char        * lib_name )
{
  oyCMMInfo_s * cmm_info = 0;
  oyCMMhandle_s * cmm_handle = 0;
  int error = !lib_name;
  int found = 0;

  if(error <= 0)
  {
    cmm_handle = oyCMMFromCache_( lib_name );
    if(cmm_handle && cmm_handle->info->api)
    {
      cmm_info = cmm_handle->info;
      found = 1;
    }
  }

  if(error <= 0 && !found)
  {
    cmm_info = oyCMMOpen_(lib_name);
  }

  return cmm_info;
}

/** @internal
 */
char *           oyCMMInfoPrint_     ( oyCMMInfo_s       * cmm_info )
{
  char * text = 0, num[48];
  oyCMMapi_s * tmp = 0;
  oyCMMapi4_s * cmm_api4 = 0;
  oyOBJECT_e type = 0;

  if(!cmm_info || cmm_info->type != oyOBJECT_CMM_INFO_S)
    return oyStringCopy_("---", oyAllocateFunc_);

  oySprintf_(num,"%d", cmm_info->oy_compatibility );

  STRING_ADD( text, cmm_info->cmm );
  STRING_ADD( text, " " );
  STRING_ADD( text, cmm_info->backend_version );
  STRING_ADD( text, "/" );
  STRING_ADD( text, num );
  STRING_ADD( text, ":" );

#define CMMINFO_ADD_NAME_TO_TEXT( name_, select ) \
  STRING_ADD( text, "\n  " ); \
  STRING_ADD( text, name_ ); \
  STRING_ADD( text, ":\n    " ); \
  STRING_ADD( text, cmm_info->getText( select, oyNAME_NICK, (oyStruct_s*) cmm_info ) ); \
  STRING_ADD( text, "\n    " ); \
  STRING_ADD( text, cmm_info->getText( select, oyNAME_NAME, (oyStruct_s*) cmm_info ) ); \
  STRING_ADD( text, "\n    " ); \
  STRING_ADD( text, cmm_info->getText( select, oyNAME_DESCRIPTION, (oyStruct_s*) cmm_info)); \
  STRING_ADD( text, "\n" );

  CMMINFO_ADD_NAME_TO_TEXT( _("Name"), "name" )
  CMMINFO_ADD_NAME_TO_TEXT( _("Manufacturer"), "manufacturer" )
  CMMINFO_ADD_NAME_TO_TEXT( _("Copyright"), "copyright" )

      if(cmm_info)
      {
        tmp = cmm_info->api;
        STRING_ADD( text, "\n  API(s):\n" );

        while(tmp)
        {
          type = oyCMMapi_Check_(tmp);

          oySprintf_(num,"    %d:", type );
          STRING_ADD( text, num );
          STRING_ADD( text, oyStructTypeToText( tmp->type ) );

          if(type == oyOBJECT_CMM_API4_S)
          {
            cmm_api4 = (oyCMMapi4_s*) tmp;
            STRING_ADD( text, "\n    Registration: " );
            STRING_ADD( text, cmm_api4->registration );
            CMMINFO_ADD_NAME_TO_TEXT( "Name", "name" )
          }
          STRING_ADD( text, "\n" );

          tmp = tmp->next;
        }
      }
  STRING_ADD( text, "\n" );

  return text;
}

/** @internal
 *  Function oyCMMCanHandle_
 *  @brief query a module for certain capabilities
 *
 *  @return                           sum of asked capabilities
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/01/03 (Oyranos: 0.1.8)
 *  @date    2009/09/02
 */
int              oyCMMCanHandle_    ( oyCMMapi3_s        * api,
                                      oyCMMapiQueries_s  * queries )
{
  int capable = 0;
  int error = !api;
  int i, ret = 0;

  if(error <= 0)
  {
    if(queries)
    {
      for(i = 0; i < queries->n; ++i)
      {
        oyCMMapiQuery_s * query = queries->queries[i];
        ret = api->oyCMMCanHandle( query->query, query->value );

        if(ret)
          capable += query->request;
        else if(query->request == oyREQUEST_HARD)
          return 0;
      }

    } else
      capable = 1;
  }

  return capable;
}

static oyStructList_s * oy_meta_module_cache_ = 0;

/** @internal
 *  Function oyCMMGetMetaApis_
 *  @brief   get module loaders
 *
 *  This function searches for a meta modul API.
 *
 *  @param[in]   cmm                   the four byte module ID, optional
 *
 *  @version Oyranos: 0.1.11
 *  @since   2008/12/28 (Oyranos: 0.1.10)
 *  @date    2010/09/14
 */
oyCMMapis_s* oyCMMGetMetaApis_       ( const char        * cmm )
{
  oyCMMapis_s * meta_apis = 0;
  meta_apis = oyCMMsGetMetaApis_( cmm );
  return meta_apis;
}

/** @internal
 *  Function oyCMMsGetApi__
 *  @brief get a specified module
 *
 *  The oyCMMapiLoadxxx_ function family loads a API from a external module.\n
 *
 *  This function allowes to obtain a desired API from a certain library.
 *
 *  @param[in]   type                  the API to return
 *  @param[in]   lib_name              if present take this or fail, the arg
 *                                     simplifies and speeds up the search
 *  @param[in]   apiCheck              custom api selector
 *  @param[in]   check_pointer         data to pass to apiCheck
 *  @param[in]   num                   position in api chain matching to type
 *                                     and apiCheck/check_pointer starting from
 *                                     zero,
 *                                     -1 means: pick the first match, useful 
 *                                     in case the API position is known or to
 *                                     iterate through all matching API's
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/08 (Oyranos: 0.1.9)
 *  @date    2008/12/23
 */
oyCMMapi_s *     oyCMMsGetApi__      ( oyOBJECT_e          type,
                                       const char        * lib_name,
                                       oyCMMapi_Check_f    apiCheck,
                                       oyPointer           check_pointer,
                                       int                 num )
{
  int error = !type,
      i = 0;
  oyCMMapi_s * api = 0;
  uint32_t rank = 0;

  if(error <= 0 &&
     !(oyOBJECT_CMM_API1_S <= type && type < oyOBJECT_CMM_API_MAX))
    error = 1;

  if(error <= 0)
  {
    /* open the module */
    oyCMMInfo_s * cmm_info = oyCMMInfoFromLibName_( lib_name );

    if(cmm_info)
    {
      oyCMMapi_s * tmp = cmm_info->api;

      if(!apiCheck)
        apiCheck = oyCMMapi_CheckWrap_;

      while(tmp)
      {
        if(apiCheck(cmm_info, tmp, check_pointer, &rank) == type)
        {
          if((num >= 0 && num == i) ||
             num < 0 )
            api = tmp;

          ++i;
        }
        tmp = tmp->next;
      }
    }

    oyCMMInfo_Release( &cmm_info );
  }

  return api;
}

/** @internal
 *  Function oyCMMsGetFilterApis_
 *  @brief let a oyCMMapi5_s meta module open a set of modules
 *
 *  The oyCMMapiLoadxxx_ function family loads a API from a external module.\n
 *  The module system shall support:
 *    - use of the desired CMM for the task at hand
 *    - provide fallbacks for incapabilities
 *    - process in different ways and by different modules through the same API
 *    - select complementing API's
 *
 *  We have modules with well defined capabilities and some with fuzzy ones.\n
 *  For instance the X11 API's is well defined and we can use it, once it is
 *  loaded.\n
 *  A CMM for colour conversion has often limitations or certain features,
 *  which makes the search for alternatives desireable. So we search
 *  for match to our automatic criteria.\n
 *
 *
 *  This function allowes to obtain API's for matching modules/CMM's.
 *  The modules are filtered for double occurencies. The highest rank or
 *  version is selected.
 *
 *  @param[in]   cmm_meta              the selected meta API will be used,
 *                                     can be Zero or at least 4 bytes long,
 *                                     e.g. "oyIM" for the library module loader
 *  @param[in]   cmm_required          if present take this or fail, the arg
 *                                     simplifies and speeds up the search
 *  @param[in]   registration          point'.' separated list of identifiers
 *  @param[in]   type                  CMM API
 *  @param[in]   flags                 supported is 
 *                                     - oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR 
 *                                       for omitting implementation attributes
 *  @param[out]  rank_list             the ranks matching the returned list;
 *                                     without that only the most matching API  
 *                                     is returned at position 0
 *  @param[out]  count                 count of returned modules
 *  @return                            a zero terminated list of modules
 *
 *  @version Oyranos: 0.1.11
 *  @since   2008/12/19 (Oyranos: 0.1.10)
 *  @date    2010/08/12
 */
oyCMMapiFilters_s*oyCMMsGetFilterApis_(const char        * cmm_meta,
                                       const char        * cmm_required,
                                       const char        * registration,
                                       oyOBJECT_e          type,
                                       uint32_t            flags,
                                       uint32_t         ** rank_list,
                                       uint32_t          * count )
{
  int error = !oyIsOfTypeCMMapiFilter( type );
  oyCMMapiFilters_s * apis = 0, * apis2 = 0;
  oyCMMapiFilter_s * api = 0,
                   * api2 = 0;
  uint32_t * rank_list_ = 0, * rank_list2_ = 0;
  int rank_list_n = 5, count_ = 0;
  oyObject_s object = oyObject_New();

  /*{
    if(!oy_module_cache_)
      oy_module_cache_ = oyStructList_New( 0 );

    entry = oyCacheListGetEntry_ ( oy_module_cache_, registration );

    api = (oyCMMapiFilter_s*) oyHash_GetPointer_( entry, type );

    if(api)
    {
      oyHash_Release_( &entry );
      return api;
    }
  }*/

  if(error <= 0)
  {
    oyCMMapi5_s * api5 = 0;
    oyCMMapis_s * meta_apis = oyCMMGetMetaApis_( cmm_meta );
    int meta_apis_n = 0;
    char ** files = 0;
    uint32_t  files_n = 0;
    int i, j, k = 0, match_j = -1, ret, match_i = -1, rank = 0, old_rank = 0,
        n, accept;
    char * match = 0, * reg = 0;
    oyCMMInfo_s * info = 0;

    meta_apis_n = oyCMMapis_Count( meta_apis );
    for(k = 0; k < meta_apis_n; ++k)
    {
      api5 = (oyCMMapi5_s*) oyCMMapis_Get( meta_apis, k );

      files_n = 0;

      if(error <= 0)
      files = oyCMMsGetNames_(&files_n, api5->sub_paths, api5->ext,
                              cmm_required, 
                              api5->data_type == 0 ? oyPATH_MODULE :
                                                     oyPATH_SCRIPT);
      else
        WARNc2_S("%s: %s", _("Could not open meta module API"),
                 oyNoEmptyString_m_( registration ));

      if(files)
      for( i = 0; i < files_n; ++i)
      {
        ret = 0; j = 0;
        ret = api5->oyCMMFilterScan( 0,0, files[i], type, j,
                                     0, 0, oyAllocateFunc_, &info, object );
        while(!ret)
        {
          ret = api5->oyCMMFilterScan( 0,0, files[i], type, j,
                                       &reg, 0, oyAllocateFunc_, 0, 0 );
          if(!ret && reg)
          {
            rank = oyFilterRegistrationMatch( reg, registration, type );
            if((rank && OYRANOS_VERSION == info->oy_compatibility) ||
               !registration)
              ++rank;

            if(rank && rank_list)
            {

              if(!rank_list_ && !apis)
              {
                *rank_list = 0;
                oyAllocHelper_m_( *rank_list, uint32_t, rank_list_n+1, 0,
                                  goto clean );
                rank_list_ = *rank_list;
                apis = oyCMMapiFilters_New(0);
              } else
              if(count_ >= rank_list_n)
              {
                rank_list_n *= 2;
                rank_list_ = 0;
                oyAllocHelper_m_( rank_list_, uint32_t, rank_list_n+1, 0,
                                  goto clean );
                error = !memcpy( rank_list_, *rank_list,
                                 sizeof(uint32_t) * rank_list_n/2 );
                oyFree_m_(*rank_list);
                *rank_list = rank_list_;
              }

              rank_list_[k++] = rank;
              api = api5->oyCMMFilterLoad( 0,0, files[i], type, j);
              if(!api)
                continue;

              if(!api->id_)
                api->id_ = oyStringCopy_( files[i], oyAllocateFunc_ );
              api->api5_ = api5;
              if(!apis)
                apis = oyCMMapiFilters_New( 0 );
              oyCMMapiFilters_MoveIn( apis, &api, -1 );
              ++ count_;
              if(count)
                *count = count_;

            } else
            if(rank > old_rank)
            {
              match = reg;
              match_j = j;
              match_i = i;
              old_rank = rank;
            }
          }
          ++j;
          if(reg) oyFree_m_( reg );
        }
      }

      oyCMMInfo_Release( &info );
#if 0
      if(api5->release)
        api5->release( (oyStruct_s**)&api5 );
#endif
    }

    oyCMMapis_Release( &meta_apis );

    if(match && !rank_list)
    {
      apis2 = oyCMMapiFilters_New( 0 );
      api = api5->oyCMMFilterLoad( 0,0, files[match_i], type, match_j );
      if(!api->id_)
        api->id_ = oyStringCopy_( files[match_i], oyAllocateFunc_ );
      api->api5_ = api5;
      oyCMMapiFilters_MoveIn( apis2, &api, -1 );
      if(count)
        *count = 1;
    }

    if(rank_list_)
    {
      /* filter doubled entries */
      n = oyCMMapiFilters_Count( apis );
      oyAllocHelper_m_( rank_list2_, uint32_t, rank_list_n+1, 0, goto clean );
      k = 0;
      for(i = 0 ; i < n; ++i)
      {
        char * apir = 0;

        api = oyCMMapiFilters_Get( apis, i );
        if(flags | oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR)
          oyFilterRegistrationModify( api->registration,
                                    oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR,
                                      &apir, 0 );
        accept = 1;

        for(j = i+1; j < n; ++j)
        {
          char * api2r = 0;

          api2 = oyCMMapiFilters_Get( apis, j );
          if(flags | oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR)
            oyFilterRegistrationModify( api2->registration,
                                    oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR,
                                        &api2r, 0 );

          /* for equal registration compare rank and version */
          if(oyStrcmp_( apir,  api2r ) == 0 &&
             rank_list_[i] <= rank_list_[j])
            accept = 0;

          oyFree_m_(api2r);

          if(api2->release)
            api2->release( (oyStruct_s**)&api2 );
        }

        if(!apis2)
          apis2 = oyCMMapiFilters_New( 0 );

        if(accept)
        {
          oyCMMapiFilters_MoveIn( apis2, &api, -1 );
          rank_list2_[k++] = rank_list_[i];
        }

        oyFree_m_(apir);

        if(api && api->release)
          api->release( (oyStruct_s**)&api );
      }

      if(rank_list)
        *rank_list = rank_list2_;
      if(rank_list_)
        oyFree_m_(rank_list_);
      if(count)
        *count = k;
    }
      
    oyStringListRelease_( &files, files_n, oyDeAllocateFunc_ );
  }

  /*if(api)
  {
    api->id_ = lib_name;
    error = oyHash_SetPointer_( entry, (oyStruct_s*) s );
  }*/

  clean:
    oyObject_Release( &object );

  return apis2;
}

oyStructList_s * oy_cmm_filter_cache_ = 0;

/** @internal
 *  Function oyCMMsGetFilterApi_
 *  @brief let a oyCMMapi5_s meta module open a module
 *
 *  The oyCMMapiLoadxxx_ function family loads a API from a external module.\n
 *  The module system shall support:
 *    - use of the desired CMM for the task at hand
 *    - provide fallbacks for incapabilities
 *    - process in different ways and by different modules through the same API
 *
 *  We have modules with well defined capabilities and some with fuzzy ones.\n
 *  For instance the X11 API's is well defined and we can use it, once it is
 *  loaded.\n
 *  A CMM for colour conversion has often limitations or certain features,
 *  which makes the search for alternatives desireable. So we search
 *  for match to our automatic criteria.\n
 *
 *
 *  This function allowes to obtain a API for a matching modul/CMM.
 *
 *  @param[in]   cmm_required          if present take this or fail, the arg
 *                                     simplifies and speeds up the search
 *  @param[in]   registration          point'.' separated list of identifiers
 *  @param[in]   type                  CMM API
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/15 (Oyranos: 0.1.9)
 *  @date    2008/06/02
 */
oyCMMapiFilter_s *oyCMMsGetFilterApi_( const char        * cmm_required,
                                       const char        * registration,
                                       oyOBJECT_e          type )
{
  oyCMMapiFilter_s * api = 0;
  oyCMMapiFilters_s * apis = 0;
  oyHash_s * entry = 0;
#ifndef NO_OPT
  int len = oyStrlen_(registration);
  char * hash_text = oyAllocateFunc_( len + 10 );
  char api_char = oyCMMapiNumberToChar( type );

  oySprintf_( hash_text, "%s.%c_", registration, api_char ? api_char:' ');
  entry = oyCMMCacheListGetEntry_( hash_text );
  oyDeAllocateFunc_( hash_text ); hash_text = 0;
  api = (oyCMMapiFilter_s*) oyHash_GetPointer( entry, type );
  if(api)
    return api;
#endif

  apis = oyCMMsGetFilterApis_( 0,cmm_required, registration, type, 
                               oyFILTER_REG_MODE_NONE, 0,0 );

  if(apis)
  {
    api = oyCMMapiFilters_Get( apis, 0 );
    oyCMMapiFilters_Release( &apis );
#ifndef NO_OPT
#ifdef NOT_YET
    if(api->copy)
      api = api->copy
#endif
    oyHash_SetPointer( entry, (oyStruct_s*) api );
#endif
  }

  return api;
}


/** @internal
 *  Function oyCMMsGetMetaApis_
 *  @brief get oyranos modules
 *
 *  @param[in]   cmm                  the selected module, optional
 *
 *  @version Oyranos: 0.1.11
 *  @since   2010/06/25 (Oyranos: 0.1.10)
 *  @date    2010/09/14
 */
oyCMMapis_s *    oyCMMsGetMetaApis_  ( const char        * cmm )
{
  int error = 0;
  oyCMMapis_s * apis = 0;
  oyCMMapi_Check_f apiCheck = oyCMMapi_CheckWrap_;

  if(error <= 0)
  {
    char ** files = 0;
    uint32_t  files_n = 0;
    int i = 0;

    files = oyCMMsGetLibNames_(&files_n, cmm);

    if(!files_n)
      WARNc_S(_("Could not find any meta module. "
                "Did you set the OY_MODULE_PATHS variable,"
                " to point to a Oyranos module loader library?"));

    /* open the modules */
    for( i = 0; i < files_n; ++i)
    {
      oyCMMInfo_s * cmm_info = oyCMMInfoFromLibName_(files[i]);

      if(cmm_info)
      {
        oyCMMapi_s * tmp = cmm_info->api,
                   * copy = 0;

        while(tmp)
        {

          if(apiCheck(cmm_info, tmp, 0, 0) == oyOBJECT_CMM_API5_S)
          {
            if(!apis)
              apis = oyCMMapis_New(0);

            copy = tmp;
              
            oyCMMapis_MoveIn( apis, &copy, -1 );
          }

          tmp = tmp->next;
        }
      }

      oyCMMInfo_Release( &cmm_info );
    }

    oyStringListRelease_( &files, files_n, oyDeAllocateFunc_ );
  }

  return apis;
}

/** @internal
 *  Function oyCMMsGetApi_
 *  @brief get a oyranos module
 *
 *  The oyCMMapiLoadxxx_ function family loads a API from a external module.\n
 *  The module system shall support:
 *    - use of the desired CMM for the task at hand
 *    - provide fallbacks for incapabilities
 *    - process in different ways and by different modules through the same API
 *
 *  We have modules with well defined capabilities and some with fuzzy ones.\n
 *  For instance the X11 API's is well defined and we can use it, once it is
 *  loaded.\n
 *  A CMM for colour conversion has often limitations or certain features,
 *  which make it desireable. So we have to search for match to our automatic
 *  criteria.\n
 *
 *
 *  This function allowes to obtain a API for a certain modul/CMM.
 *  oyCMMapi4_s is excluded.
 *
 *  @param[in]   type                  the API type to return
 *  @param[in]   cmm_required          if present take this or fail, the arg
 *                                     simplifies and speeds up the search
 *  @param[out]  lib_used              inform about the selected CMM
 *  @param[in]   apiCheck              custom API selector
 *  @param[in]   check_pointer         data to pass to apiCheck
 *
 *  @version Oyranos: 0.1.9
 *  @since   2007/12/12 (Oyranos: 0.1.9)
 *  @date    2008/12/16
 */
oyCMMapi_s *     oyCMMsGetApi_       ( oyOBJECT_e          type,
                                       const char        * cmm_required,
                                       char             ** lib_used,
                                       oyCMMapi_Check_f    apiCheck,
                                       oyPointer           check_pointer )
{
  int error = !type;
  oyCMMapi_s * api = 0;

  if(!apiCheck)
    apiCheck = oyCMMapi_CheckWrap_;

  if(error <= 0 &&
     !(oyOBJECT_CMM_API1_S <= type && type < oyOBJECT_CMM_API_MAX))
    error = 1;

  if(error <= 0)
  {
    char ** files = 0;
    uint32_t  files_n = 0;
    int i, oy_compatibility = 0;
    uint32_t rank = 0,
             max_rank = 0;
    int max_pos = -1;

    files = oyCMMsGetLibNames_(&files_n, cmm_required);

    /* open the modules */
    for( i = 0; i < files_n; ++i)
    {
      oyCMMInfo_s * cmm_info = oyCMMInfoFromLibName_(files[i]);

      if(cmm_info)
      {
        oyCMMapi_s * tmp = cmm_info->api;

        while(tmp)
        {

          if(apiCheck(cmm_info, tmp, check_pointer, &rank) == type)
          {

              if( /* if we found already a matching version, do not exchange*/
                  oy_compatibility != OYRANOS_VERSION &&
                    /* possibly newly found */
                  ( oy_compatibility = 0 ||
                    /* or a bigger version but not greater than current oy_ */
                    ( cmm_info->oy_compatibility <= OYRANOS_VERSION &&
                      oy_compatibility < cmm_info->oy_compatibility ) ||
                    /* or we select a less greater in case we are above oy_ */
                    ( cmm_info->oy_compatibility > OYRANOS_VERSION &&
                      oy_compatibility > cmm_info->oy_compatibility )
                  )
                )
              {
                if(cmm_info->oy_compatibility == OYRANOS_VERSION)
                  ++rank;
                if(rank > max_rank)
                {
                  api = tmp;
                  max_rank = rank;
                  max_pos = i;

                }
                oy_compatibility = cmm_info->oy_compatibility;
              }
          }
          tmp = tmp->next;
        }
      }

      oyCMMInfo_Release( &cmm_info );
    }

    if(max_rank >= 0 && lib_used)
    {
      if(*lib_used)
        oyFree_m_(*lib_used);
      if(files && files_n)
        *lib_used = oyStringCopy_( files[max_pos], oyAllocateFunc_ );
    }

    oyStringListRelease_( &files, files_n, oyDeAllocateFunc_ );
  }

  return api;
}


/** @internal
 *  @brief release Oyranos module infos
 *
 *  @since Oyranos: version 0.1.8
 *  @date  6 december 2007 (API 0.1.8)
 */
int              oyCMMRelease_       ( const char        * cmm )
{
  int error = !cmm;
  int n = oyStructList_Count( oy_cmm_infos_ );
  int i;

  n = oyStructList_Count(oy_cmm_infos_);
  if(error <= 0)
  for(i = 0; i < n; ++i)
  {
    oyCMMInfo_s * s = 0;
    oyCMMhandle_s * cmmh = (oyCMMhandle_s *) oyStructList_GetType_(
                                               (oyStructList_s_*)oy_cmm_infos_,
                                               i,
                                               oyOBJECT_CMM_HANDLE_S );

    if(cmmh)
      s = (oyCMMInfo_s*) cmmh->info;

    if( s && s->type == oyOBJECT_CMM_INFO_S &&
        *(uint32_t*)&s->cmm && cmm &&
        !memcmp( s->cmm, cmm, 4 ) )
    {
      oyCMMhandle_Release_( &cmmh );
      oyStructList_ReleaseAt( oy_cmm_infos_, 0 );
    }
  }

  return error;
}
/** @} *//* module_api */



/** \addtogroup module_api Module APIs
 *
 *  Oyranos C modules provide support for data formats, data processing and
 *  process control, as well as configuration.
 *  The module architecture covers three basic layers. There are the Module 
 *  APIs for the real stuff, the Meta Module APIs for language support and the
 *  final Oyranos objects with their User APIs.\n
 *  @dot
digraph Modules {
  bgcolor="transparent";
  nodesep=.1;
  ranksep=1.;
  //rankdir=LR;
  graph [fontname=Helvetica, fontsize=14];
  node [shape=record,fontname=Helvetica, fontsize=11, width=.1];

  subgraph cluster_0 {
    label="Oyranos Module Framework";
    color=white;
    clusterrank=global;

      node [width = 1.5, style=filled];
      i [URL="structoyCMMapi5__s.html#_details", label="Module Importer\n oyCMMapi5_s"];

      p [ label="Filter nodes / Graph handling APIs\n oyFilterNode_s"];


      api4_A [URL="structoyCMMapi4__s.html#_details",label="Context Module + UI\n oyCMMapi4_s"];
      api6_A [URL="structoyCMMapi6__s.html#_details",label="Context Converter\n oyCMMapi6_s"];
      api7_A [URL="structoyCMMapi7__s.html#_details",label="Node Module\n oyCMMapi7_s"];

      m [URL="structoyCMMapi8__s.html#_details", label="Config (Device) Functions\n oyCMMapi8_s"];
      policy [URL="structoyCMMapi9__s.html#_details", label="Policy Functions\n oyCMMapi9_s"];


      subgraph cluster_1 {
        color=gray;
        label="Module APIs"

        subgraph cluster_2 {
          rank=max;
          color=red;
         style=dashed;
          node [style="filled"];
          api4_A; api6_A; api7_A;
          label="Graph Modules and Helpers";
        }

        subgraph cluster_6 {
          color=green;
          style=dashed;
          m;
          label="Device Modules";
        }

        subgraph cluster_9 {
          color=blue;
          style=dashed;
          policy;
          label="Policy Modules";
        }

      }

      subgraph cluster_10 {
        color=gray;
        node [style="filled"];
        i;
        label="Meta Module API";
      }

      subgraph cluster_11 {
        color=gray;
        node [style="filled"];
        p;
        label="User APIs";
      }

      p -> i [arrowhead="open", color=gray];
      i -> api4_A [arrowhead="open", color=red];
      i -> api6_A [arrowhead="open", color=red];
      i -> api7_A [arrowhead="open", color=red];
      i -> m [arrowhead="open", color=green];
      i -> policy [arrowhead="open", color=blue];
  }
} @enddot
 *
 *  \b User \b API:
 *  The user API allowes to contruct a filter or configuration
 *  object. Filters can be chained to directed acyclic graphs (DAGs) and data
 *  can be processed through graphs or they are deployed by higher level APIs,
 *  like named colour API.
 *
 *  \b Meta \b Modules:
 *  The modules are loaded into Oyranos by meta modules. A Meta module can
 *  support different formats of modules. The basic format are the native C
 *  structures and function declarations, which allow very detailed access and 
 *  control of Oyranos objects. These C data structures for building a module
 *  are complex and need some expertise to handle. However the meta module 
 *  interface allows to write support for modules which might be written even
 *  in scripting languages. Such filters should then be loadable as normal
 *  modules and are, depending on the choosen language, very easy to understand
 *  and to write.
 *  Beside reduced access to Oyranos native C data types, script filters might
 *  become highly interchangeable outside of Oyranos.
 *
 *  \b Module \b APIs:
 *  Several interfaces allow to write different module types. They can have
 *  access to Oyranos' configuration system, build data dependent contexts,
 *  provide access to user defined data types, create custom UIs via XFORMS and
 *  possibly SVG, simply process data and connect as node into a Oyranos DAG or
 *  expose to users as policy tool for a DAG. Different module APIs expose as
 *  different user APIs.
 *  Most module authors will want to write for one of these interfaces.
 *
 *  \b High \b Abstraction: Most module interfaces have no idea themselve about
 *  what kind of data they handle. They follow very generic and abstract ideas
 *  and rules on how to do data processing in a directed acyclic graph. 
 *  The overal idea of Oyranos' graphs can be read in the 
 *  @ref objects_conversion. 
 *  The module interfaces can implement different processing stages and tell 
 *  how to combine them in a graph by Oyranos. E.g. it is possible for on module
 *  to build a cacheable context, which can be used by different modules to
 *  process data.
 *  Most of the processing logic is inside Oyranos's core. But for efficiency
 *  and flexibility modules have access to their connected neighbour plug-ins.
 *  For instance they have to call their forerunner to request for data.
 *
 *  \b Examples: For learning how modules can
 *  do useful work see the delivered modules like the lcms and oyIM ones in
 *  files like "oyranos_cmm_xxxx.c". They are linked as libraries and are
 *  installed in the "$cmmdir" and "$libdir/oyranos" paths. These paths are
 *  shown during the configuration process or through te provided oyranos-config
 *  tool.
 *
 *  The Filter API's are subdivided to allow for automatical combining of 
 *  preprocessing and processing stages. Especially in the case of expensive
 *  preprocessing data, like in CMM's, it makes sense to provide the means for
 *  combining general purpose libraries with hardware accelerated modules.
 *  This architecture allowes for combining by just providing enough interface
 *  information about their supported data formats.
 *  The following paragraphs provide a overview.
 *
 *  The oyCMMapi5_s module structure defines a meta module to load modules,
 *  from a to be defined directory with to be defined naming criterias. The 
 *  meta module loads or constructs all parts of a module, oyCMMapi4_s,
 *  oyCMMapi7_s, oyCMMapi6_s, oyCMMapi8_s and oyCMMapi9_s.
 *
 *  oyCMMapi7_s eighter deploys the context created in a oyCMMapi4_s filter, or
 *  simply processes the data of a oyFilterNode_s graph element. It is 
 *  responsible to request data from the graph and process them.
 *  Members are responsible to describe the filters capabilities for connecting
 *  to other filters in the graph. Modules can describe their own UI in 
 *  oyCMMapi4_s.
 *  oyCMMapi7_s is mandatory.
 *
 *  The oyCMMapi4_s is a structure to create a context for a oyCMMapi7_s
 *  processor. This context is a intermediate processing stage for all of the
 *  context data influencing options and input datas. The idea for tight 
 *  integration of the context functionality is to provide a well defined way
 *  of interaction for node modules with context modules. 
 *  The oyCMMapi4_s structure contains as well the GUI. oyCMMapi4_s is 
 *  mandatory because of its GUI parts.
 *  A oyCMMapi4_s without a oyCMMapi7_s is useless.
 *  oyCMMapi4_s must contain the same basic registration string like the 
 *  according oyCMMapi7_s except some keywords in the application section. This
 *  is explained more below in detail.
 *  It is assumed that a generated context is worth to be cached. If Oyranos 
 *  obtains a serialised data blob from the context generator it can be
 *  automatically cached.
 *  @dot
digraph Anatomy_A {
  bgcolor="transparent";
  nodesep=.1;
  ranksep=1.;
  rankdir=LR;
  graph [fontname=Helvetica, fontsize=14];
  node [shape=record,fontname=Helvetica, fontsize=11, width=.1];

  subgraph cluster_7 {
    label="One Context - Different Data Processors";
    color=white;
    clusterrank=global;

      node [width = 2.5, style=filled];

      api4_A [label="lcms ICC colour profile Module\n oyCMMapi4_s | <f>Context Creation \"oyDL\" | ... | XFORMS UI"];

      api6_A [label="lcms Context Converter\n oyCMMapi6_s\n \"oyDL\"-\>\"lcCC\" \(oyDL_lcCC\)"];
      api7_A [label="lcms colour conversion Data Processor\n oyCMMapi7_s\n C implementation needs \"lcCC\" context" width = 3.5];

      api6_C [label="ctl Context Converter\n oyCMMapi6_s\n \"oyDL\"-\>\"oCTL\" \(oyDL_oCTL\)"];
      api7_C [label="ctl colour conversion Data Processor\n oyCMMapi7_s\n LLVM implementation needs \"oCTL\" context" width = 3.5];

      api4_A; api6_A; api7_A;
      api7_C; api6_C;

      api4_A:f -> api6_A -> api7_A [arrowhead="open", color=black];
      api4_A:f -> api6_C -> api7_C [arrowhead="open", color=black];
  }
} @enddot
 *
 *  In case a oyCMMapi7_s function can not handle a certain provided context
 *  data format, Oyranos will try to convert it for the oyCMMapi7_s API through
 *  a fitting oyCMMapi6_s data convertor. oyCMMapi6_s is only required for 
 *  filters, which request incompatible contexts from a oyCMMapi4_s structure.
 *
 *  The oyCMMapi8_s handles configurations, like external module data
 *  and options. The @ref devices_handling deployes these modules.
 *
 *  oyCMMapi9_s can be used to plug in new object types, policy settings and
 *  a way to enforce the policies.
 *
 *  @section registration Registration
 *  Each filter API provides a \b registration member string.
 *  The registration member provides the means to later successfully select 
 *  the according filter. The string is separated into sections by a slash'/'.
 *  The sections can be subdivided by point'.' for additional attributes as 
 *  needed. This pattern follows the scheme of directories with attributes or
 *  XML elements with attributes.
 *  The sections are to be filled as follows:
 *  - top, e.g. OY_TOP_SHARED (::oyFILTER_REG_TOP)
 *  - vendor, e.g. OY_DOMAIN_STD (::oyFILTER_REG_DOMAIN)
 *  - filter type, e.g. OY_TYPE_STD
 *    (::oyFILTER_REG_TYPE)
 *  - filter name, e.g. "icc.lcms._NOACCEL._CPU" (::oyFILTER_REG_APPLICATION)
 *
 *  After that the options section follows (::oyFILTER_REG_OPTION).
 *
 *  The application registration string part should for general purpose modules
 *  contain a convention string. "icc" signals to process colours with the help
 *  of ICC style profiles, which can by convention be inserted into the options
 *  list.
 *
 *  @par Filter registration:
 *  A filter can add keywords but must omit the API number and the following
 *  matching rule sign. Recommended keywords for the application section are:
 *  - _ACCEL for acceleration, required
 *  - _NOACCEL for no acceleration or plain software, required
 *  - _GPU, _GLSL, _HLSL, _MMX, _SSE, _SSE2, _3DNow and so on for certain 
 *    hardware acceleration features \n
 *  
 *  \b Example: a complete module registration: \n
 *  "org/oyranos/openicc/icc.lcms._NOACCEL._CPU" registers a plain
 *  software CMM
 * 
 *  A underscore in front of a attribute makes the attribute optional during
 *  the matching process in oyFilterRegistrationMatch(). This is needed in case
 *  a registration string is used itself as a search pattern.
 *
 *  @par Registration search pattern:
 *  To explicitely select a different processor and context creator the
 *  according registration attribute must have a number and prefix,
 *  e.g. "4_lcms" "7_octl".
 *  A search pattern can add keywords.
 *  - a number followed by underscore, plus or minus signs the according API.
 *    The feature of interesst must then be appended, e.g. "7_GPU" preferes
 *    a GPU interpolator. This is useful to select a certain API of a module.
 *    - underscore '_' means preference
 *    - minus '-' means must skip
 *    - plus '+' means must have
 *  - "4[_,+,-]" - context+UI oyCMMapi4_s
 *  - "6[_,+,-]" - context convertor oyCMMapi6_s
 *  - "7[_,+,-]" - processor oyCMMapi7_s \n
 *  
 *  By default all attributes in a search pattern are considered mandatory.
 *  A level can be omitted, "//", or a attribute can be tagged by a '_'
 *  underscore in front for making it optional, or a '-' to indicate a 
 *  attibute must not match.
 *
 *  \b Example: a complete registration search pattern: \n
 *  "//openicc/4+icc.7+ACCEL.7_GPU.7_HLSL.7-GLSL" selects a accelerated CMM 
 *  interpolator with prefered GPU and HLSL but no GLSL support together with a
 *  ICC compliant context generator and options.
 *
 *  The oyFilterRegistrationToText() and oyFilterRegistrationMatch() functions
 *  might be useful for canonical processing Oyranos registration text strings.
 *  Many functions allow for passing a registration string. Matching can be 
 *  obtained by omitting sections like in the string "//openicc/icc", where the
 *  elements between slashes is o,itted. This string would result in a match 
 *  for any ICC compliant colour conversion filter.
 *
 *  The registration attributes ".front", ".advanced" and more are described in
 *  the objects_value::oyOPTIONATTRIBUTE_e enum.
 *
 *  See as well <a href="http://www.oyranos.org/wiki/index.php?title=Concepts#Elektra_namespace">Concepts#Elektra_namespace</a> on ColourWiki.
 *  @{
 */

/** @} *//* module_api */



/** \addtogroup misc Miscellaneous

 *  @{
 */


/** \addtogroup objects_generic

 *  @{
 */



/**
 *  @} *//* objects_generic
 */





/** @internal
 *  The lists are allocated one time and live until the application quits
 *  It contains the various caches for faster access of CPU intentsive data.\n
 *  We'd need a 3 dimensional table to map\n
 *    A: a function or resource type\n
 *    B: a CMM\n
 *    C: a hash value specifying under which conditions the resource was build\n
 *  The resulting cache entry is the result from the above 3 arguments. With the
 *  much implementation work and expectedly small to no speed advantage it is 
 *  not a good strategy. Even though argument C would be extensible.\n
 *  \n
 *  A different approach would use two values to map the search request to the 
 *  cache entry. The above hash map and the hash or combination of the two other *  values.\
 *  \n
 *  One point to consider is a readable string to end not with a anonymous list
 *  full of anonymous entries, where a user can pretty much nothing know.
 *  A transparent approach has to allow for easy identifying each entry.\n
 *  A help would be a function to compute both a md5 digest and a message from
 *  the 3 arguments outlined above. Probably it would allow much more arguments
 *  to add as we can not know how many optins and other influential parameters
 *  the cache entry depends on.\n
 *  \n
 *  A final implementation would consist of a
 *  - function to convert arbitrary (string) arguments to a single string and a
 *    hash sum 
 *  - a function to create a cache entry struct from above hash and a the 
 *    according description string plus the oyPointer_s struct. For simplicity
 *    the cache struct can be identical to the oyPointer_s, with the disadvantage
 *    of containing additional data not understandable for a CMM. We need to 
 *    mark these data (hash + description) as internal to Oyranos.
 *  - a list that hold above cache entry stucts
 *  - several functions to reference, release, maps a hash value to the
 *    cached resource
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 november 2007 (API 0.1.8)
 */
oyStructList_s * oy_cmm_cache_ = 0;

/** @internal
 *  @brief get always a Oyranos cache entry from the CMM's cache
 *
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyHash_s *   oyCMMCacheListGetEntry_ ( const char        * hash_text)
{
  if(!oy_cmm_cache_)
    oy_cmm_cache_ = oyStructList_New( 0 );

  return oyCacheListGetEntry_(oy_cmm_cache_, 0, hash_text);
}

/** @internal
 *  @brief get the Oyranos CMM cache
 *
 *
 *  @since Oyranos: version 0.1.8
 *  @date  17 december 2007 (API 0.1.8)
 */
oyStructList_s** oyCMMCacheList_()
{
  return &oy_cmm_cache_;
}

/** @internal
 *  @brief get the Oyranos CMM cache
 *
 *
 *  @since Oyranos: version 0.1.8
 *  @date  17 december 2007 (API 0.1.8)
 */
oyChar* oyCMMCacheListPrint_()
{
  oyStructList_s ** cache_list = oyCMMCacheList_();
  int n = oyStructList_Count( *cache_list ), i;
  oyChar * text = 0;
  oyChar refs[80];

  oySprintf_( refs,"%s:%d Oyranos CMM cache with %d entries:\n", 
              __FILE__,__LINE__, n);
  STRING_ADD( text, refs );

  for(i = 0; i < n ; ++i)
  {
    oyHash_s * compare = (oyHash_s*) oyStructList_GetType_((oyStructList_s_*)*cache_list, i,
                                                         oyOBJECT_HASH_S );

    if(compare)
    {
      oySprintf_(refs,"%d: ", compare->oy_->ref_);
      STRING_ADD( text, refs );
      STRING_ADD( text, oyObject_GetName(compare->oy_, oyNAME_NAME) );
      STRING_ADD( text, "\n" );
    }
  }

  return text;
}




/** \addtogroup objects_value Values Handling

 *  @{
 */


/** @internal
 *  @struct  oyConfDomain_s_
 *  @brief   a ConfDomain object
 *  @extends oyStruct_s
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
typedef struct {
  oyOBJECT_e           type_;          /**< struct type oyOBJECT_CONF_DOMAIN_S */ 
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  oyObject_s           oy_;            /**< base object */

  oyCMMapi8_s        * api8;
} oyConfDomain_s_;

oyConfDomain_s_ *
           oyConfDomain_New_         ( oyObject_s          object );
oyConfDomain_s_ *
           oyConfDomain_FromReg_     ( const char        * registration,
                                       oyObject_s          object );
oyConfDomain_s_ *
           oyConfDomain_Copy_        ( oyConfDomain_s_   * obj,
                                       oyObject_s          object);
int
           oyConfDomain_Release_     ( oyConfDomain_s_   **obj );

const char * oyConfDomain_GetText_   ( oyConfDomain_s_   * obj,
                                       const char        * name,
                                       oyNAME_e            type );
const char **oyConfDomain_GetTexts_  ( oyConfDomain_s_   * obj );

/* --- Public_API Begin --- */

/** Function oyConfDomain_New
 *  @memberof oyConfDomain_s
 *  @brief   allocate a new ConfDomain object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
OYAPI oyConfDomain_s * OYEXPORT
           oyConfDomain_FromReg      ( const char        * registration_domain,
                                       oyObject_s          object )
{
  oyConfDomain_s_ * obj = 0;

  obj = oyConfDomain_FromReg_( registration_domain, object );

  return (oyConfDomain_s*) obj;
}

/** Function oyConfDomain_Copy
 *  @memberof oyConfDomain_s
 *  @brief   copy or reference a ConfDomain object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
OYAPI oyConfDomain_s * OYEXPORT
           oyConfDomain_Copy         ( oyConfDomain_s    * obj,
                                       oyObject_s          object )
{
  oyConfDomain_s_ * s = (oyConfDomain_s_*) obj;

  if(s)
    oyCheckType__m( oyOBJECT_CONF_DOMAIN_S, return 0 );

  s = oyConfDomain_Copy_( s, (oyObject_s) object );

  return (oyConfDomain_s*) s;
}
 
/** Function oyConfDomain_Release
 *  @memberof oyConfDomain_s
 *  @brief   release and possibly deallocate a ConfDomain object
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
OYAPI int  OYEXPORT
           oyConfDomain_Release      ( oyConfDomain_s    **obj )
{
  oyConfDomain_s_ * s = 0;

  if(!obj || !*obj)
    return 0;

  s = (oyConfDomain_s_*) *obj;

  oyCheckType__m( oyOBJECT_CONF_DOMAIN_S, return 1 )

  *obj = 0;

  return oyConfDomain_Release_( &s );
}

/** Function oyConfDomain_GetText
 *  @memberof oyConfDomain_s
 *  @brief   obtain a UI text from a ConfDomain object
 *
 *  @param[in,out] obj                 struct object
 *  @param[in]     name                the category to return
 *  @param[in]     type                the type of string
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
OYAPI const char * OYEXPORT
           oyConfDomain_GetText      ( oyConfDomain_s    * obj,
                                       const char        * name,
                                       oyNAME_e            type )
{
  oyConfDomain_s_ * s = (oyConfDomain_s_*) obj;
  const char * text = 0;

  if(s)
    oyCheckType__m( oyOBJECT_CONF_DOMAIN_S, return 0 );

  text = oyConfDomain_GetText_( s, name, type );

  return text;
}

/** Function oyConfDomain_GetTexts
 *  @memberof oyConfDomain_s
 *  @brief   obtain a list of possible UI text from a ConfDomain object
 *
 *  @return                            zero terminated list of strings,
 *                                     Each string is a "name" option to
 *                                     oyConfDomain_GetText().
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
OYAPI const char ** OYEXPORT
           oyConfDomain_GetTexts     ( oyConfDomain_s    * obj )
{
  oyConfDomain_s_ * s = (oyConfDomain_s_*) obj;
  const char ** texts = 0;

  if(s)
    oyCheckType__m( oyOBJECT_CONF_DOMAIN_S, return 0 );

  texts = oyConfDomain_GetTexts_( s );

  return texts;
}

/* --- Public_API End --- */


/** @internal
 *  Function oyConfDomain_New_
 *  @memberof oyConfDomain_s_
 *  @brief   allocate a new ConfDomain object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
oyConfDomain_s_ * oyConfDomain_New_  ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CONF_DOMAIN_S;
# define STRUCT_TYPE oyConfDomain_s_
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = 0;

  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyConfDomain_Copy;
  s->release = (oyStruct_Release_f) oyConfDomain_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  s->api8 = 0;

  return s;
}

oyConfDomain_s_ * oyConfDomain_FromReg_(
                                       const char        * registration,
                                       oyObject_s          object )
{
  oyConfDomain_s_ * s = oyConfDomain_New_( object );
  int error = !s;
  oyCMMapi8_s * cmm_api8 = 0;

  if(error <= 0)
  {
    cmm_api8 = (oyCMMapi8_s*) oyCMMsGetFilterApi_( 0, registration,
                                                   oyOBJECT_CMM_API8_S );
    error = !cmm_api8;
  }

  if(error <= 0)
    s->api8 = cmm_api8;

  return s;
}

/** @internal
 *  Function oyConfDomain_Copy__
 *  @memberof oyConfDomain_s_
 *  @brief   real copy a ConfDomain object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
oyConfDomain_s_ * oyConfDomain_Copy__ (
                                       oyConfDomain_s_   * obj,
                                       oyObject_s          object )
{
  oyConfDomain_s_ * s = 0;
  int error = 0;

  if(!obj || !object)
    return s;

  s = oyConfDomain_New_( object );
  error = !s;

  if(!error)
  {

    if(obj->api8)
    {
      if(obj->api8->copy)
        s->api8 = (oyCMMapi8_s*) obj->api8->copy( (oyStruct_s*)s->api8,
                                                  object );
      else
        s->api8 = obj->api8;
    }
  }

  if(error)
    oyConfDomain_Release_( &s );

  return s;
}

/** @internal
 *  Function oyConfDomain_Copy_
 *  @memberof oyConfDomain_s_
 *  @brief   copy or reference a ConfDomain object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
oyConfDomain_s_ * oyConfDomain_Copy_ ( oyConfDomain_s_   * obj,
                                       oyObject_s          object )
{
  oyConfDomain_s_ * s = obj;

  if(!obj)
    return 0;

  if(obj && !object)
  {
    s = obj;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyConfDomain_Copy__( obj, object );

  return s;
}
 
/** @internal
 *  Function oyConfDomain_Release_
 *  @memberof oyConfDomain_s_
 *  @brief   release and possibly deallocate a ConfDomain object
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
int        oyConfDomain_Release_     ( oyConfDomain_s_   **obj )
{
  /* ---- start of common object destructor ----- */
  oyConfDomain_s_ * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */


  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    if(s->api8)
    {
      if(s->api8->release)
        s->api8->release( (oyStruct_s**) &s->api8 );
      else
        s->api8 = 0;
    }

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}

/** @internal
 *  Function oyConfDomain_GetText_
 *  @memberof oyConfDomain_s
 *  @brief   obtain a UI text from a ConfDomain object
 *
 *  @param[in,out] obj                 struct object
 *  @param[in]     name                the category to return
 *  @param[in]     type                the type of string
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
const char * oyConfDomain_GetText_   ( oyConfDomain_s_   * obj,
                                       const char        * name,
                                       oyNAME_e            type )
{
  const char * text = 0;
  oyConfDomain_s_ * s = obj;

  if(s->api8 && s->api8->ui && s->api8->ui->getText)
    text = s->api8->ui->getText( name, type, (oyStruct_s*)s->api8->ui );

  return text;
}

/** @internal
 *  Function oyConfDomain_GetTexts
 *  @memberof oyConfDomain_s
 *  @brief   obtain a list of possible UI text from a ConfDomain object
 *
 *  @return                            zero terminated list of strings,
 *                                     Each string is a "name" option to
 *                                     oyConfDomain_GetText().
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/30 (Oyranos: 0.1.10)
 *  @date    2009/12/30
 */
const char **oyConfDomain_GetTexts_  ( oyConfDomain_s_   * obj )
{
  oyConfDomain_s_ * s = obj;
  const char ** texts = 0;

  if(s->api8 && s->api8->ui)
    texts = s->api8->ui->texts;

  return texts;
}


/**
 *  @} *//* objects_value
 */

/** @} *//* misc */


/** \addtogroup devices_handling Device API
 *
 *  Devices are a special form of configurations. Their access is grouped
 *  for effective performance. Known devices are queried with
 *  oyDevicesGet(). oyConfigDomainList() provides a list of known device
 *  modules.
 *  A single device can be obtained by oyDeviceGet(). The \a
 *  device_type argument defaults to OY_TYPE_STD and can be omitted for this
 *  group. The \a device_class argument specifies a subgroup, e.g. 
 *  "monitor".
 *
 *  All other functions return a handle to the device. With this handle it is
 *  possible to get informations (oyDeviceGetInfo()), query it's current,
 *  possibly remote profile (oyDeviceAskProfile2()) or typical used get a 
 *  profile with fallbacks including the DB through (oyDeviceGetProfile()), 
 *  set the profile persistent (oyDeviceSetProfile()) or query the persistent
 *  stored profile (oyDeviceProfileFromDB()).
 *
 *  @{
 */

/** Function oyOption_FromDB
 *  @memberof oyOption_s
 *  @brief   new option with registration and value filled from DB if available
 *
 *  @param         registration        no or full qualified registration
 *  @param         object              the optional object
 *  @return                            the option
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/24 (Oyranos: 0.1.10)
 *  @date    2009/01/24
 */
oyOption_s *   oyOption_FromDB       ( const char        * registration,
                                       oyObject_s          object )
{
  int error = !registration;
  oyOption_s * o = 0;

  if(error <= 0)
  {
    /** This is merely a wrapper to oyOption_New() and
     *  oyOption_SetValueFromDB(). */
    o = oyOption_FromRegistration( registration, object );
    error = oyOption_SetFromText( o, 0, 0 );
    error = oyOption_SetValueFromDB( o );
    oyOption_SetSource( o, oyOPTIONSOURCE_DATA );
  }

  return o;
}

/** @internal
 *  Function oyOptions_ForFilter_
 *  @memberof oyOptions_s
 *  @brief   provide Oyranos behaviour settings
 *
 *  The returned options are read in from the Elektra settings and if thats not
 *  available from the inbuild defaults. The later can explicitely selected with
 *  oyOPTIONSOURCE_FILTER passed as flags argument.
 *  The key names map to the registration and XML syntax.
 *
 *  To obtain all front end options from a meta module use: @verbatim
    flags = oyOPTIONATTRIBUTE_ADVANCED |
            oyOPTIONATTRIBUTE_FRONT |
            OY_SELECT_COMMON @endverbatim
 *
 *  @param[in]     filter              the filter
 *  @param[in]     flags               for inbuild defaults |
 *                                     oyOPTIONSOURCE_FILTER;
 *                                     for options marked as advanced |
 *                                     oyOPTIONATTRIBUTE_ADVANCED |
 *                                     OY_SELECT_FILTER |
 *                                     OY_SELECT_COMMON
 *  @param         object              the optional object
 *  @return                            the options
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/08 (Oyranos: 0.1.9)
 *  @date    2009/07/27
 */
oyOptions_s *  oyOptions_ForFilter_  ( oyFilterCore_s    * filter,
                                       uint32_t            flags,
                                       oyObject_s          object )
{
  oyOptions_s * s = 0,
              * opts_tmp = 0,
              * opts_tmp2 = 0;
  oyOption_s * o = 0;
  int error = !filter || !filter->api4_;
  char * type_txt = oyFilterRegistrationToText( filter->registration_,
                                                oyFILTER_REG_TYPE, 0 );
  oyCMMapi5_s * api5 = 0;
  int i,n;

  /* by default we parse both sources */
  if(!(flags & OY_SELECT_FILTER) && !(flags & OY_SELECT_COMMON))
    flags |= OY_SELECT_FILTER | OY_SELECT_COMMON;

  if(!error)
  {
    /*
        Programm:
        1. get filter and its type
        2. get implementation for filter type
        3. parse static common options from meta module
        4. parse static options from filter 
        5. merge both
        6. get stored values from disk
     */

    /*  1. get filter */

    /*  2. get implementation for filter type */
    api5 = filter->api4_->api5_;

    /*  3. parse static common options from a policy module */
    if(api5 && flags & OY_SELECT_COMMON)
    {
      oyCMMapiFilters_s * apis;
      int apis_n = 0;
      uint32_t         * rank_list = 0;
      oyCMMapi9_s * cmm_api9 = 0;
      char * klass, * api_reg;

      klass = oyFilterRegistrationToText( filter->registration_,
                                          oyFILTER_REG_TYPE, 0 );
      api_reg = oyStringCopy_("//", oyAllocateFunc_ );
      STRING_ADD( api_reg, klass );
      oyFree_m_( klass );

      s = oyOptions_New( 0 );

      apis = oyCMMsGetFilterApis_( 0,0, api_reg,
                                   oyOBJECT_CMM_API9_S, 
                                   oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR,
                                   &rank_list, 0);
      apis_n = oyCMMapiFilters_Count( apis );
      for(i = 0; i < apis_n; ++i)
      {
        cmm_api9 = (oyCMMapi9_s*) oyCMMapiFilters_Get( apis, i );
        if(oyFilterRegistrationMatch( filter->registration_, cmm_api9->pattern,
                                      oyOBJECT_NONE ))
        {
          opts_tmp = oyOptions_FromText( cmm_api9->options, 0, object );
          oyOptions_AppendOpts( s, opts_tmp );
          oyOptions_Release( &opts_tmp );
        }
        if(cmm_api9->release)
          cmm_api9->release( (oyStruct_s**)&cmm_api9 );
      }
      oyCMMapiFilters_Release( &apis );
      oyFree_m_( api_reg );
      opts_tmp = s; s = 0;
    }
    /* requires step 2 */

    /*  4. parse static options from filter */
    if(flags & OY_SELECT_FILTER)
      opts_tmp2 = oyOptions_FromText( filter->api4_->ui->options, 0, object );

    /*  5. merge */
    s = oyOptions_FromBoolean( opts_tmp, opts_tmp2, oyBOOLEAN_UNION, object );

    oyOptions_Release( &opts_tmp );
    oyOptions_Release( &opts_tmp2 );

    /*  6. get stored values */
    n = oyOptions_Count( s );
    for(i = 0; i < n && error <= 0; ++i)
    {
      o = oyOptions_Get( s, i );
      oyOption_SetSource( o, oyOPTIONSOURCE_FILTER );
      /* ask Elektra */
      if(!(flags & oyOPTIONSOURCE_FILTER))
        error = oyOption_SetValueFromDB( o );
      oyOption_Release( &o );
    }
#if 0
    error = oyOptions_DoFilter ( s, flags, type_txt );
#endif
  }

  if(type_txt)
    oyDeAllocateFunc_( type_txt );

  return s;
}

/** Function oyOptions_SaveToDB
 *  @memberof oyOptions_s
 *  @brief   store a oyOptions_s in DB
 *
 *  @param[in]     options             the options
 *  @param[in]     registration        the registration
 *  @param[out]    new_reg             the new registration; optional
 *  @param[in]     alloc               the user allocator for new_reg; optional
 *  @return                            0 - good, 1 >= error
 *
 *  @version Oyranos: 0.3.0
 *  @since   2009/02/08 (Oyranos: 0.1.10)
 *  @date    2011/01/29
 */
OYAPI int  OYEXPORT
               oyOptions_SaveToDB    ( oyOptions_s       * options,
                                       const char        * registration,
                                       char             ** new_reg,
                                       oyAlloc_f           allocateFunc )
{
  int error = !options || !registration;
  oyOption_s * o = 0;
  int n,i;
  char * key_base_name = 0,
       * key_name = 0,
       * key_top = 0;

  DBG_PROG_START
  oyExportStart_(EXPORT_PATH | EXPORT_SETTING);

  if(error <= 0)
  {
    key_base_name = oySearchEmptyKeyname_( registration );
    error = !key_base_name;
    if(error <= 0)
    {
      STRING_ADD( key_base_name, OY_SLASH );
    }

    n = oyOptions_Count( options );
    for( i = 0; i < n; ++i )
    {
      o = oyOptions_Get( options, i );
      key_top = oyFilterRegistrationToText( oyOption_GetRegistration(o),
                                            oyFILTER_REG_MAX, 0 );


      STRING_ADD( key_name, key_base_name );
      STRING_ADD( key_name, key_top );
      if(oyOption_GetValueString(o,0))
        error = oyAddKey_valueComment_( key_name, oyOption_GetValueString(o,0),
                                        0 );
# if 0
      else if(o->value_type == oyVAL_STRUCT &&
              o->value && o->value->oy_struct->type_ == oyOBJECT_BLOB_S)
        error = 0;/*oyAddKeyBlobComment_();*/
#endif
      else
        WARNcc_S( (oyStruct_s*)o,
                    "Could not save non string / non binary option" );

      oyOption_Release( &o );
      oyFree_m_( key_name );
    }

    if(error <= 0 && new_reg && key_base_name)
    {
      key_base_name[strlen(key_base_name)-1] = '\000';
      *new_reg = oyStringCopy_(key_base_name, allocateFunc);
    }
    oyFree_m_( key_base_name );
  }

  oyExportEnd_();
  DBG_PROG_ENDE
  return error;
}

/** @internal
 *  Function oyOptions_SetDeviceTextKey_
 *  @brief   set a device option
 *
 *  @param[in,out] options             options for the device
 *  @param[in]     device_type         the device type ::oyFILTER_REG_TYPE,
 *                                     defaults to OY_TYPE_STD (optional)
 *  @param[in]     device_class        the device class, e.g. "monitor",
 *                                     ::oyFILTER_REG_APPLICATION
 *  @param[in]     key                 key_name to add at ::oyFILTER_REG_OPTION
 *  @param[in]     value               value of type ::oyVAL_STRING
 *  @return                            the new registration
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/30 (Oyranos: 0.1.10)
 *  @date    2009/01/30
 */
int    oyOptions_SetDeviceTextKey_   ( oyOptions_s       * options,
                                       const char        * device_type,
                                       const char        * device_class,
                                       const char        * key,
                                       const char        * value )
{
  char * text = 0;
  int error = 0;

  text = oyDeviceRegistrationCreate_( device_type, device_class,
                                          key, text );
  error = oyOptions_SetFromText( &options, text, value, OY_CREATE_NEW );

  oyFree_m_( text );

  return error;
}

/** @internal
 *  Function oyOptions_SetRegistrationTextKey_
 *  @brief   set a device option
 *
 *  @param[in,out] options             options for the device
 *  @param[in]     registration        the registration string
 *  @param[in]     key                 key_name to add at ::oyFILTER_REG_OPTION
 *  @param[in]     value               value of type ::oyVAL_STRING
 *  @return                            the new registration
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/09 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
int    oyOptions_SetRegistrationTextKey_(
                                       oyOptions_s       * options,
                                       const char        * registration,
                                       const char        * key,
                                       const char        * value )
{
  char * text = 0;
  int error = 0;

  STRING_ADD( text, registration );
  STRING_ADD( text, "/" );
  STRING_ADD( text, key );

  error = oyOptions_SetFromText( &options, text, value, OY_CREATE_NEW );

  oyFree_m_( text );

  return error;
}

icProfileClassSignature oyDeviceSigGet(oyConfig_s        * device )
{
  icProfileClassSignature deviceSignature = 0;
  if(oyFilterRegistrationMatch( oyConfigPriv_m(device)->registration, "monitor", 0 ))
    deviceSignature = icSigDisplayClass;
  else if(oyFilterRegistrationMatch( oyConfigPriv_m(device)->registration, "scanner", 0 ))
    deviceSignature = icSigInputClass;
  else if(oyFilterRegistrationMatch( oyConfigPriv_m(device)->registration, "raw-image", 0 ))
    deviceSignature = icSigInputClass;
  else if(oyFilterRegistrationMatch( oyConfigPriv_m(device)->registration, "printer", 0 ))
    deviceSignature = icSigOutputClass;

  return deviceSignature;
}
/**
 *  @} *//* devices_handling
 */


/** \addtogroup misc Miscellaneous

 *  @{
 */

/** \addtogroup objects_rectangle Rectangle Handling

 *  @{
 */


/**
 *  @} *//* objects_rectangle
 */



/**
 *  @brief oyDATATYPE_e to byte mapping
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 *  @date    2007/11/00
 */
int      oySizeofDatatype            ( oyDATATYPE_e        t )
{
  int n = 0;
  switch(t)
  {
    case oyUINT8:
         return 1;
    case oyUINT16:
    case oyHALF:
         return 2;
    case oyUINT32:
    case oyFLOAT:
         return 4;
    case oyDOUBLE:
         return 8;
  }
  return n;
}

/**
 *  @brief oyDATATYPE_e to string mapping
 *
 *  @since Oyranos: version 0.1.8
 *  @date  26 november 2007 (API 0.1.8)
 */

const char *   oyDatatypeToText      ( oyDATATYPE_e        t)
{
  const char * text = 0;
  switch(t)
  {
    case oyUINT8:
         text = "oyUINT8"; break;
    case oyUINT16:
         text = "oyUINT16"; break;
    case oyHALF:
         text = "oyHALF"; break;
    case oyUINT32:
         text = "oyUINT32"; break;
    case oyFLOAT:
         text = "oyFLOAT"; break;
    case oyDOUBLE:
         text = "oyDOUBLE"; break;
  }
  return text;
}


/** @} *//* misc */



/** \addtogroup objects_image Image API
 *
 *  @{
 */





/** Function oyDeviceProfileFromDB
 *  @brief   look up a profile of a device from DB
 *
 *  The function asks the module for a detailed and possible expensive list
 *  of device information and tries to find a matching configuration in the
 *  DB. The device informations are the same as for saving to DB.
 *
 *  @param[in]     device          a device
 *  @param[in]     profile_name        profile's name in DB
 *  @param[in]     allocateFunc        user allocator
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/21 (Oyranos: 0.1.10)
 *  @date    2009/02/09
 */
OYAPI int OYEXPORT oyDeviceProfileFromDB
                                     ( oyConfig_s        * device,
                                       char             ** profile_name,
                                       oyAlloc_f           allocateFunc )
{
  oyOption_s * o = 0;
  oyOptions_s * options = 0;
  int error = !device || !profile_name;
  const char * device_name = 0;
  char * tmp = 0, * tmp2 = 0;
  int32_t rank_value = 0;
  oyConfig_s * s = device;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 1 )

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  if(error <= 0)
  {
    o = oyConfig_Find( device, "profile_name" );
    device_name = oyConfig_FindString( device, "device_name", 0);

    /* 1. obtain detailed and expensive device informations */
    if( !oyConfig_FindString(s,"manufacturer",0) ||
        !oyConfig_FindString(s,"model",0) )
    { 
      /* 1.1 add "properties" call to module arguments */
      error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/command",
                                     "properties", OY_CREATE_NEW );
      error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/device_name",
                                     device_name, OY_CREATE_NEW );

      device_name = 0;

      /* 1.2 get device */
      if(error <= 0)
        error = oyDeviceBackendCall( device, options );

      oyOptions_Release( &options );

      /* renew outdated string */
      o = oyConfig_Find( device, "profile_name" );
      device_name = oyConfig_FindString( device, "device_name", 0);
      oyOption_Release( &o );
    }

    if(!o)
    {
      error = oyConfig_GetDB( device, &rank_value );
      o = oyConfig_Find( device, "profile_name" );
    }

    if(!o)
    {
      o = oyOptions_Get( device->db, 0 );
      if(o)
        tmp = oyStringCopy_(oyOption_GetRegistration(o), oyAllocateFunc_);
      if(tmp && oyStrrchr_( tmp, OY_SLASH_C))
      {
        tmp2 = oyStrrchr_( tmp, OY_SLASH_C);
        tmp2[0] = 0;
      }
      WARNc3_S( "\n Could not get a \"profile_name\" from %s\n"
                " registration: \"%s\" rank: %d", 
                oyNoEmptyString_m_(device_name), oyNoEmptyString_m_(tmp),
                (int)rank_value )
      if(tmp)
        oyFree_m_(tmp); tmp2 = 0;
      oyOption_Release( &o );
      error = -1;
    } else if(!oyOption_GetValueString(o,0))
    {
      WARNc1_S( "Could not get \"profile_name\" data from %s", 
                oyNoEmptyString_m_(device_name) )
      error = -1;
    } else
      *profile_name = oyOption_GetValueText( o, allocateFunc );

  } else
    WARNc_S( "missed argument(s)" );

  return error;
}



/** Function oyDeviceSelectSimiliar
 *  @brief   get similiar devices by a pattern from a list
 *
 *  The function takes a device and tries to find exact matches, which can be
 *  considered as belonging to the same device. The comparision can be 
 *  influenced by the flags.
 *  The option "profile_name" is ignored during the comparision.
 *
 *  @param[in]     pattern             Pass a device used as reference. String
 *                                     options of this object are compared to
 *                                     the objects in the heap argument
 *                                     depending on the flags argument.
 *                                     "profile_name" and other
 *                                     options from heap objects are ignored.
 *  @param[in]     heap                a list of device objects
 *  @param[in]     flags               - 0 yields exact match
 *                                     - 1 compare manufacturer model and serial
 *                                     - 2 compare only manufacturer and model
 *                                     - 4 compare only device_name
 *  @param[out]    matched_devices     the devices selected from heap
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/08/27 (Oyranos: 0.1.10)
 *  @date    2009/08/27
 */
OYAPI int OYEXPORT oyDeviceSelectSimiliar
                                     ( oyConfig_s        * pattern,
                                       oyConfigs_s       * heap,
                                       uint32_t            flags,
                                       oyConfigs_s      ** matched_devices )
{
  oyOption_s * odh = 0,
             * od = 0;
  int error  = !pattern || !matched_devices;
  char * od_key = 0;
  const char * od_val = 0,
             * odh_val = 0;
  oyConfig_s * s = pattern,
             * dh = 0;
  oyConfigs_s * matched = 0;
  int i,j,n,j_n;
  int match = 1;

  oyCheckType__m( oyOBJECT_CONFIG_S, return 1 )

  if(error <= 0)
  {
    n = oyConfigs_Count( heap );

    /* Make shure the pattern has as well manufacturer, model included.
     * If not try a "properties" command. */
    if((flags == 0 || flags == 1 || flags == 2) &&
       (!oyConfig_FindString(s,"manufacturer",0) ||
        !oyConfig_FindString(s,"model",0)))
    {
      oyOptions_s * options = 0;
      error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/command",
                                   "properties", OY_CREATE_NEW );
      oyDeviceBackendCall( s, options );
      oyOptions_Release( &options );
    }

    if((flags == 1 || flags == 2) &&
       (!oyConfig_FindString(s,"manufacturer",0) ||
        !oyConfig_FindString(s,"model",0)))
    {
      return 0;
    }

    matched = oyConfigs_New( 0 );

    for(i = 0; i < n; ++i)
    {
      match = 0;
      dh = oyConfigs_Get( heap, i );

      j_n = oyConfig_Count( pattern );
      for(j = 0; j < j_n; ++j)
      {
        match = 1;
        od = oyConfig_Get( pattern, j );
        od_key = oyFilterRegistrationToText( oyOption_GetRegistration(od),
                                             oyFILTER_REG_MAX, 0);

        od_val = oyOption_GetValueString( od, 0 );
        if(!od_val)
          /* ignore non text options */
          continue;

        /* handle selective flags */
        if(flags == 4 &&
           oyStrcmp_(od_key,"device_name") != 0
          )
          continue;
        else
        if(flags == 2 &&
           oyStrcmp_(od_key,"manufacturer") != 0 &&
           oyStrcmp_(od_key,"model") != 0
          )
          continue;
        else
        if(flags == 1 &&
           oyStrcmp_(od_key,"manufacturer") != 0 &&
           oyStrcmp_(od_key,"model") != 0 &&
           oyStrcmp_(od_key,"serial") != 0
          )
          continue;

        /* ignore a "profile_name" option */
        if(oyStrcmp_(od_key,"profile_name") == 0)
          continue;

        odh = oyOptions_Find( dh->db, od_key );

        odh_val = oyOption_GetValueString( odh, 0 );
        if( !odh_val )
          /* ignore non text options */
          match = 0;

        if(match && oyStrcmp_( od_val, odh_val ) != 0)
          match = 0;

        /*printf("pruefe: %s=%s match = %d flags=%d\n", od_key, od_val, match, flags);*/


        oyOption_Release( &od );

        oyOption_Release( &odh );

        if(match == 0)
          break;
      }

      if(match)
        oyConfigs_MoveIn( matched, &dh, -1 );
      else
        oyConfig_Release( &dh );
    }

    if(oyConfigs_Count( matched ))
      *matched_devices = matched;
    else
      oyConfigs_Release( &matched );

  } else
    WARNc_S( "missed argument(s)" );

  return error;
}

/**
 *  @} *//* devices_handling
 */


/** \addtogroup objects_profile Profile API
 *
 *  To open a profile exist several methods in the oyProfile_Fromxxx APIs.
 *  oyProfile_FromStd(), oyProfile_FromFile() and oyProfile_FromMem() are basic
 *  profile open functions. oyProfile_FromSignature() is used for creating a
 *  dummy profile during profile filtering.
 *  oyProfile_FromMD5() is a lookup function from a profile hash, e.g. as
 *  provided by a 'psid' tag. \n
 *  Profile properties can be obtained from oyProfile_GetSignature(), like
 *  colour spaces, date, magic number and so on.
 *  oyProfile_GetChannelNames(), oyProfile_GetText() and oyProfile_GetID()
 *  provide additional informations. \n
 *  The profile element functions have Tag in their names. They work together
 *  with the oyProfileTag_s APIs. \n
 *  oyProfile_GetFileName is a reverse lookup to obtain the name of a installed
 *  file from a profile, e.g. find the name of a reachable display profile.

 *  @{
 */


oyProfiles_s * oy_profile_list_cache_ = 0;
int oyLowerStrcmpWrap (const void * a_, const void * b_)
{
  const char * a = *(const char **)a_,
             * b = *(const char **)b_;
#ifdef HAVE_POSIX
  return strcasecmp(a,b);
#else
  return strcmp(a,b);
#endif
}



/** @} *//* objects_profile */




/** \addtogroup misc Miscellaneous

 *  @{
 */

/** \addtogroup objects_rectangle Rectangle Handling

 *  @{
 */


/**
 *  @} *//* objects_rectangle
 */



/**
 *  @brief oyDATATYPE_e to byte mapping
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 *  @date    2007/11/00
 */
int      oySizeofDatatype            ( oyDATATYPE_e        t )
{
  int n = 0;
  switch(t)
  {
    case oyUINT8:
         return 1;
    case oyUINT16:
    case oyHALF:
         return 2;
    case oyUINT32:
    case oyFLOAT:
         return 4;
    case oyDOUBLE:
         return 8;
  }
  return n;
}

/**
 *  @brief oyDATATYPE_e to string mapping
 *
 *  @since Oyranos: version 0.1.8
 *  @date  26 november 2007 (API 0.1.8)
 */

const char *   oyDatatypeToText      ( oyDATATYPE_e        t)
{
  const char * text = 0;
  switch(t)
  {
    case oyUINT8:
         text = "oyUINT8"; break;
    case oyUINT16:
         text = "oyUINT16"; break;
    case oyHALF:
         text = "oyHALF"; break;
    case oyUINT32:
         text = "oyUINT32"; break;
    case oyFLOAT:
         text = "oyFLOAT"; break;
    case oyDOUBLE:
         text = "oyDOUBLE"; break;
  }
  return text;
}


/** @} *//* misc */



/** \addtogroup objects_image Image API
 *
 *  @{
 */



char *         oyPixelLayoutPrint_   ( oyPixel_t           pixel_layout )
{
  return oyPixelPrint(pixel_layout, oyAllocateFunc_);
}

char   *           oyPixelPrint      ( oyPixel_t           pixel_layout,
                                       oyAlloc_f           allocateFunc )
{
  oyDATATYPE_e t = oyToDataType_m( pixel_layout );
  char * text = 0;

#define oyPixelLayoutPrint_FORMAT "channels: %d channel_offset: %d sample_type[%dByte]: %s planar: %d byte_swap %d colour_swap: %d flawor: %d"
#define oyPixelLayoutPrint_ARGS \
  oyToChannels_m( pixel_layout ), \
  oyToColourOffset_m( pixel_layout ), \
  oySizeofDatatype( t ), \
  oyDatatypeToText(t), \
  oyToPlanar_m( pixel_layout ), \
  oyToByteswap_m( pixel_layout), \
  oyToSwapColourChannels_m( pixel_layout ), \
  oyToFlavor_m( pixel_layout )

  /* describe the pixel layout and access */
  oyStringAddPrintf_(&text, allocateFunc, 0, 
                     oyPixelLayoutPrint_FORMAT, oyPixelLayoutPrint_ARGS);

  /*printf(oyPixelLayoutPrint_FORMAT,oyPixelLayoutPrint_ARGS);*/

#undef oyPixelLayoutPrint_FORMAT
#undef oyPixelLayoutPrint_ARGS
  return text;
}


/** @} *//* objects_image */ 


/** \addtogroup objects_conversion Conversion API's
 *  Colour conversion front end API's.
 *
 *  Colour conversions are realised by structures called acyclic graphs. A graph
 *  combines several processing elements and their according stages into one
 *  process model. A very simple graph will link elements just one by one to
 *  form a chain. More complex graphs are possible by branching and rejoining of
 *  data processing arms.
 *  Datas are requested always from the end. This makes it simple to create
 *  views at data. A observer tells the to be viewed plugin that it wants
 *  to connect and can request the data stream.
 *  In the other direction plug-ins can send events along the pipe through
 *  callbacks, e.g. for reporting errors or requesting updating of parameters.
 *  The @ref module_api explains how to create modules to plug into Oyranos.
 *
 *  \b About \b Graphs: \n
 *  The top object a user will handle is of type oyConversion_s. This 
 *  contains oyFilterNode_s objects, which describe the connections of each
 *  processing element, such as images with attached data and profile, and the
 *  filters. The Oyranos node connection concept is splitted into various
 *  structures.
 \dot
digraph G {
  bgcolor="transparent";
  rankdir=LR
  graph [fontname=Helvetica, fontsize=12];
  node [shape=record, fontname=Helvetica, fontsize=10, style="rounded,filled"];
  edge [fontname=Helvetica, fontsize=10];

  a [ label="{<plug> 0| Filter Node 1 == Input |<socket>}"];
  b [ label="{<plug> 1| Filter Node 2 |<socket>}"];
  c [ label="{<plug> 1| Filter Node 3 |<socket>}"];
  d [ label="{<plug> 2| Filter Node 4 == Output |<socket>}"];

  subgraph cluster_0 {
    label="Oyranos Filter Graph";
    color=gray;

    a:socket -> b:plug [arrowhead=none, arrowtail=normal];
    b:socket -> d:plug [arrowhead=none, arrowtail=normal];
    a:socket -> c:plug [arrowhead=none, arrowtail=normal];
    c:socket -> d:plug [arrowhead=none, arrowtail=normal];
  }
}
 \enddot
 *
 *  \b Connectors \b have \b tree \b missions:
 *  - The first is to tell others to about the 
 *  filters intention to provide a connection endity. This is done by the pure
 *  existence of the oyConnector_s inside the module filter structure
 *  (oyCMMapi4_s) and the oyFilterNode_s::sockets and 
 *  oyFilterNode_s::plugs members. \n
 *  - The second is to tell about the connectors capabilities, to allow for 
 *  automatic checking for fittness to other connectors. \n
 *  - The thierd task of this struct is to differenciate between \a input or
 *  \a plug and \a output or \a socket. This is delegated to the
 *  oyFilterSocket_s and oyFilterPlug_s structures.
 *  
 *
 *  \b Routing: \n
 *  The connector output, called socket, side is primarily passive. The data
 *  stream is requested or viewed by the input side, called plug. 
 *  Changes are propagated by events. This turns the acyclic graph into a 
 *  dual one. The events use the same graph just in the other direction.
 *  Events and data requests are distinct.
 *  A \a plug local to the filter or filter node can be connected to a remote
 *  \a socket connector and vice versa.
 \dot
digraph G {
  bgcolor="transparent";
  node[ shape=plaintext, fontname=Helvetica, fontsize=10 ];
  edge[ fontname=Helvetica, fontsize=10 ];
  rankdir=LR
  a [label=<
<table border="0" cellborder="1" cellspacing="4">
  <tr> <td>Filter Node A</td>
      <td bgcolor="red" width="10" port="s"> socket </td>
  </tr>
</table>>
  ]
  b [label=< 
<table border="0" cellborder="1" cellspacing="4">
  <tr><td bgcolor="lightblue" width="10" port="p"> plug </td>
      <td>Filter Node B</td>
  </tr>
</table>>
  ]
  subgraph { rank=min a }

  b:p->a:s [label=request];
} 
 \enddot
 * Status information can be passed from the input side to the output side by
 * callbacks.
 \dot
digraph G {
  bgcolor="transparent";
  node[ shape=plaintext, fontname=Helvetica, fontsize=10 ];
  edge[ fontname=Helvetica, fontsize=10 ];
  rankdir=LR
  a [label=<
<table border="0" cellborder="1" cellspacing="4">
  <tr> <td>Filter Node A</td>
      <td bgcolor="red" width="10" port="s"> socket </td>
  </tr>
</table>>
  ]
  b [label=< 
<table border="0" cellborder="1" cellspacing="4">
  <tr><td bgcolor="lightblue" width="10" port="p"> plug </td>
      <td>Filter Node B</td>
  </tr>
</table>>
  ]
  subgraph { rank=min a }

  b:p->a:s [arrowhead=none, arrowtail=normal, label=callback];
} 
 \enddot
 *
 * The data flows from the socket to the plug.
 \dot
digraph G {
  bgcolor="transparent";
  node[ shape=plaintext, fontname=Helvetica, fontsize=10 ];
  edge[ fontname=Helvetica, fontsize=10 ];
  rankdir=LR
  a [label=<
<table border="0" cellborder="1" cellspacing="4">
  <tr> <td>Filter Node A</td>
      <td bgcolor="red" width="10" port="s"> socket </td>
  </tr>
</table>>
  ]
  b [label=< 
<table border="0" cellborder="1" cellspacing="4">
  <tr><td bgcolor="lightblue" width="10" port="p"> plug </td>
      <td>Filter Node B</td>
  </tr>
</table>>
  ]
  subgraph { rank=min a }

  b:p->a:s [arrowhead=none, arrowtail=normal, label=data];
} 
 \enddot
 *
 * A oyFilterNode_s can have various oyFilterPlug_s ' to obtain data from
 * different sources. The required number is described in the oyCMMapi4_s 
 * structure, which is part of oyFilterCore_s. But each plug can only connect to
 * one socket.
 \dot
digraph G {
  bgcolor="transparent";
  rankdir=LR
  node [shape=record, fontname=Helvetica, fontsize=10, style="rounded"];
  edge [fontname=Helvetica, fontsize=10];

  b [ label="{<plug> | Filter Node 2 |<socket>}"];
  c [ label="{<plug> | Filter Node 3 |<socket>}"];
  d [ label="{<plug> 2| Filter Node 4 |<socket>}"];

  b:socket -> d:plug [arrowtail=normal, arrowhead=none];
  c:socket -> d:plug [arrowtail=normal, arrowhead=none];
}
 \enddot
 *
 * oyFilterSocket_s is designed to accept arbitrary numbers of connections 
 * to allow for viewing on a filters data output or observe its state changes.
 \dot
digraph G {
  bgcolor="transparent";
  rankdir=LR
  node [shape=record, fontname=Helvetica, fontsize=10, style="rounded"];
  edge [fontname=Helvetica, fontsize=10];

  a [ label="{<plug> | Filter Node 1 |<socket>}"];
  b [ label="{<plug> 1| Filter Node 2 |<socket>}"];
  c [ label="{<plug> 1| Filter Node 3 |<socket>}"];
  d [ label="{<plug> 1| Filter Node 4 |<socket>}"];
  e [ label="{<plug> 1| Filter Node 5 |<socket>}"];

  a:socket -> b:plug [arrowtail=normal, arrowhead=none];
  a:socket -> c:plug [arrowtail=normal, arrowhead=none];
  a:socket -> d:plug [arrowtail=normal, arrowhead=none];
  a:socket -> e:plug [arrowtail=normal, arrowhead=none];
}
 \enddot

 *  @{
 */

/** Function oyFilterNode_GetText
 *  @memberof oyFilterNode_s
 *  @brief   serialise filter node to text
 *
 *  Serialise into:
 *  - oyNAME_NICK: XML ID
 *  - oyNAME_NAME: XML from module
 *  - oyNAME_DESCRIPTION: ??
 *
 *  This function provides a complete description of the context. It might be 
 *  more adequate to use only a subset for hashing as not all data and options
 *  might have an effect to the context data result. 
 *  The oyCMMapi4_s::oyCMMFilterNode_GetText() function provides a way to let a
 *  module decide about what to place into a hash text.
 *
 *  @param[in,out] node                filter node
 *  @param[out]    name_type           the type
 *  @return                            the text
 *
 *  @version Oyranos: 0.3.3
 *  @since   2008/07/17 (Oyranos: 0.1.8)
 *  @date    2011/11/22
 */
const char * oyFilterNode_GetText    ( oyFilterNode_s    * node,
                                       oyNAME_e            name_type )
{
  const char * tmp = 0;
  char * hash_text = 0;
  oyFilterNode_s * s = node;

  oyStructList_s * in_datas = 0,
                 * out_datas = 0;

  if(!node)
    return 0;

  if( s->core && s->core->api4_ && s->core->api4_->oyCMMFilterNode_GetText &&
      name_type == oyNAME_NAME )
  {
    hash_text = s->core->api4_->oyCMMFilterNode_GetText( node, oyNAME_NICK,
                                                   oyAllocateFunc_ );
    if(hash_text)
    {
      oyObject_SetName( s->oy_, hash_text, oyNAME_NAME );

      oyDeAllocateFunc_( hash_text );
      hash_text = 0;

      hash_text = (oyChar*) oyObject_GetName( s->oy_, oyNAME_NAME );
      return hash_text;
    }
  }

  /* 1. create hash text */
  hashTextAdd_m( "<oyFilterNode_s>\n  " );

  /* the filter text */
  hashTextAdd_m( oyFilterCore_GetText( node->core, oyNAME_NAME ) );

  /* pick all plug (input) data */
  in_datas = oyFilterNode_DataGet_( node, 1 );

  /* pick all sockets (output) data */
  out_datas = oyFilterNode_DataGet_( node, 0 );

  /* make a description */
  tmp = oyContextCollectData_( (oyStruct_s*)node, s->core->options_,
                               in_datas, out_datas );
  hashTextAdd_m( tmp );

  hashTextAdd_m( "</oyFilterNode_s>\n" );


  oyObject_SetName( s->oy_, hash_text, oyNAME_NICK );

  if(s->oy_->deallocateFunc_)
    s->oy_->deallocateFunc_( hash_text );
  hash_text = 0;

  hash_text = (oyChar*) oyObject_GetName( s->oy_, oyNAME_NICK );

  return hash_text;
}

/** Function oyFilterNode_GetId
 *  @memberof oyFilterNode_s
 *  @brief   get the object Id
 *
 *  @param[in]     node                filter node
 *  @return                            the object Id
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/25 (Oyranos: 0.1.10)
 *  @date    2009/02/25
 */
OYAPI int  OYEXPORT
               oyFilterNode_GetId    ( oyFilterNode_s    * node )
{
  oyFilterNode_s * s = node;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return -1 )

  return oyObject_GetId( node->oy_ );
}


/** 
 *  @internal
 *  Info profilbody */
char info_profile_data[320] =
  {
/*0*/    0,0,1,64, 'o','y','r','a',
    2,48,0,0, 'n','o','n','e',
    'R','G','B',32, 'L','a','b',32,
    0,0,0,0,0,0,0,0,
/*32*/    0,0,0,0,97,99,115,112,
    '*','n','i','x',0,0,0,0,
    110,111,110,101,110,111,110,101,
    -64,48,11,8,-40,-41,-1,-65,
/*64*/    0,0,0,0,0,0,-10,-42,
    0,1,0,0,0,0,-45,45,
    'o','y','r','a',0,0,0,0,
    0,0,0,0,0,0,0,0,
/*96*/    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
/*128*/    0,0,0,3,'d','e','s','c',
    0,0,0,-88,0,0,0,33,
    'c','p','r','t',0,0,0,-52,
    0,0,0,29,'I','n','f','o',
/*160*/    0,0,0,-20,0,0,0,0,
    't','e','x','t',0,0,0,0,
    'F','i','l','t','e','r',' ','I',
    'n','f','o',' ','X','M','L',0,
/*192*/    0,0,0,0,0,0,0,0,
    0,0,0,0,'t','e','x','t',
    0,0,0,0,110,111,116,32,
    99,111,112,121,114,105,103,104,
/*224*/    116,101,100,32,100,97,116,97,
    0,0,0,0,'t','e','x','t',
    0,0,0,0,'s','t','a','r',
    't',0,0,0,0,0,0,0,
/*256*/    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0
  };

/** Function oyFilterNode_TextToInfo_
 *  @memberof oyFilterNode_s
 *  @brief   serialise filter node to binary
 *
 *  Serialise into a Oyranos specific ICC profile containers "Info" text tag.
 *  Not useable for binary contexts.
 *
 *  This function is currently a ICC only thing and yet just for debugging
 *  useful.
 *
 *  @param[in,out] node                filter node
 *  @param[out]    size                output size
 *  @param[in]     allocateFunc        memory allocator
 *  @return                            the profile container
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/17 (Oyranos: 0.1.8)
 *  @date    2008/07/18
 */
oyPointer    oyFilterNode_TextToInfo_( oyFilterNode_s    * node,
                                       size_t            * size,
                                       oyAlloc_f           allocateFunc )
{
  oyPointer ptr = 0;
  icHeader * header = 0;
  size_t len = 244, text_len = 0;
  char * text = 0;
  const char * temp = 0;
  uint32_t * mem = 0;

  if(!node)
    return 0;

  temp = oyFilterNode_GetText( node, oyNAME_NICK );

  text_len = strlen(temp) + 1;
  len += text_len + 1;
  len = len > 320 ? len : 320;
  ptr = allocateFunc(len);
  header = ptr;
  
  if(ptr)
  { 
    *size = len;
    memset(ptr,0,len);
    memcpy(ptr, info_profile_data, 320);

    text = ((char*)ptr)+244;
    sprintf(text, "%s", temp);
    header->size = oyValueUInt32( len );
    mem = ptr;
    mem[41] = oyValueUInt32( text_len + 8 );
  }

  return ptr;
}

/** Function oyFilterNode_DataGet
 *  @memberof oyFilterNode_s
 *  @brief   get process data from a filter socket
 *
 *  @param[in]     node                filter node
 *  @param[in]     socket_pos          position of socket
 *  @return                            the data
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/03/05 (Oyranos: 0.1.10)
 *  @date    2009/03/05
 */
oyStruct_s *   oyFilterNode_DataGet  ( oyFilterNode_s    * node,
                                       int                 socket_pos )
{
  oyFilterNode_s * s = node;
  oyStruct_s * data = 0;
  oyFilterSocket_s * socket = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return 0 );

  socket = oyFilterNode_GetSocket( node, socket_pos );
  if(socket)
    data = socket->data;

  return data;
}

/** Function oyFilterNode_DataSet
 *  @memberof oyFilterNode_s
 *  @brief   Set process data to a filter socket
 *
 *  @param[in,out] node                filter node
 *  @param[in]     data                data
 *  @param[in]     socket_pos          position of socket
 *  @param[in]     object              a object to not only set a reference
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/03/05 (Oyranos: 0.1.10)
 *  @date    2009/03/05
 */
int            oyFilterNode_DataSet  ( oyFilterNode_s    * node,
                                       oyStruct_s        * data,
                                       int                 socket_pos,
                                       oyObject_s        * object )
{
  oyFilterNode_s * s = node;
  oyFilterSocket_s * socket = 0;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return 0 );

  socket = oyFilterNode_GetSocket( node, socket_pos );
  
  if(socket)
  {
    if(socket->data && socket->data->release)
      socket->data->release( &socket->data );

    if(data && data->copy)
      socket->data = data->copy( data, object );
    else
      socket->data = data;
  } else
  {
    WARNc_S("Node has no socket. Can not assign data.");
    return -1;
  }

  return 0;
}


/**
 *  @internal
 *  Function: oyFilterNode_GetNextFromLinear_
 *  @memberof oyFilterNode_s
 *  @brief   get next node from a linear graph 
 *
 *  @param[in]     first               filter
 *  @return                            next node
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/11/01 (Oyranos: 0.1.8)
 *  @date    2008/11/01
 */
oyFilterNode_s *   oyFilterNode_GetNextFromLinear_ (
                                       oyFilterNode_s    * first )
{
      oyFilterNode_s * next = 0;
      oyFilterSocket_s * socket = 0;
      oyFilterPlug_s * plug = 0;

      {
        socket = first->sockets[0];

        if(socket)
          plug = oyFilterPlugs_Get( socket->requesting_plugs_, 0 );
        if(plug)
          next = plug->node;
        else
          next = 0;
        oyFilterPlug_Release( &plug );
      }

  return next;
}

/**
 *  @internal
 *  Function: oyFilterNode_GetLastFromLinear_
 *  @memberof oyFilterNode_s
 *  @brief   get last node from a linear graph 
 *
 *  @param[in]     first               filter
 *  @return                            last node
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/07/16 (Oyranos: 0.1.8)
 *  @date    2008/07/16
 */
oyFilterNode_s *   oyFilterNode_GetLastFromLinear_ (
                                       oyFilterNode_s    * first )
{
  oyFilterNode_s * next = 0,
                 * last = 0;

      next = last = first;

      while(next)
      {
        next = oyFilterNode_GetNextFromLinear_( next );

        if(next)
          last = next;
      }

  return last;
}

/**
 *  @internal
 *  Function oyFilterNode_DataGet_
 *  @memberof oyFilterNode_s
 *  @brief   get the processing data from a filter node
 *
 *  @param[in]     node                filter
 *  @param[in]     get_plug            1 get input, 0 get output data
 *  @return                            the data list
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/11/04 (Oyranos: 0.1.8)
 *  @date    2008/11/04
 */
oyStructList_s * oyFilterNode_DataGet_(oyFilterNode_s    * node,
                                       int                 get_plug )
{
  int error = !node;
  oyStructList_s * datas = 0;
  oyStruct_s * data = 0;
  int i, n;

  if(error <= 0)
  {
    datas = oyStructList_New(0);

    if(get_plug)
    {
          /* pick all plug (input) data */
          n = oyFilterNode_EdgeCount( node, 1, 0 );
          for( i = 0; i < n && error <= 0; ++i)
          if(node->plugs[i])
          {
            data = 0;
            if(node->plugs[i]->remote_socket_->data)
              data = node->plugs[i]->remote_socket_->data->copy( node->plugs[i]->remote_socket_->data, 0 );
            else
              data = (oyStruct_s*) oyOption_New(0);
            error = oyStructList_MoveIn( datas, &data, -1, 0 );
            ++i;
          }
    } else
    {
          /* pick all sockets (output) data */
          n = oyFilterNode_EdgeCount( node, 0, 0 );
          for( i = 0; i < n && error <= 0; ++i)
          if(node->sockets[i])
          {
            data = 0;
            if(node->sockets[i]->data)
              data = node->sockets[i]->data->copy( node->sockets[i]->data, 0 );
            else
              data = (oyStruct_s*) oyOption_New(0);
            error = oyStructList_MoveIn( datas, &data, -1, 0 );
            ++i;
          }

    }
  }

  return datas;
}

/** @internal
 *  @brief   wrapper for oyDeAllocateFunc_
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/27 (Oyranos: 0.1.10)
 *  @date    2008/12/27
 */
int oyPointerRelease                 ( oyPointer         * ptr )
{
  if(ptr && *ptr)
  {
    oyDeAllocateFunc_(*ptr);
    *ptr = 0;
    return 0;
  }
  return 1;
}


/**
 *  @internal
 *  Function oyFilterNode_ContextSet_
 *  @memberof oyFilterNode_s
 *  @brief   set module context in a filter 
 *
 *  The api4 data is passed to a interpolator specific transformer. The result
 *  of this transformer will on request be cached by Oyranos as well.
 *
 *  @param[in]     node                filter
 *  @param[in,out] blob                context to fill
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/11/02 (Oyranos: 0.1.8)
 *  @date    2009/06/12
 */
int          oyFilterNode_ContextSet_( oyFilterNode_s    * node,
                                       oyBlob_s          * blob )
{
  int error = 0;
  oyFilterCore_s * s = node->core;

  if(error <= 0)
  {
          size_t size = 0;
          oyHash_s * hash = 0,
                   * hash_out = 0;
          const char * hash_text_ = 0;
          char * hash_text = 0,
               * hash_temp = 0;
          int hash_text_len;
          oyPointer ptr = 0;
          oyPointer_s * cmm_ptr = 0,
                     * cmm_ptr_out = 0;


          /*  Cache Search
           *  1.     hash from input
           *  2.     query for hash in cache
           *  3.     check
           *  3a.       eighter take cache entry
           *  3b.       or ask CMM
           *  3b.1.                update cache entry
           */

          /* 1. create hash text */
          if(s->api4_->oyCMMFilterNode_GetText)
          {
            hash_temp = s->api4_->oyCMMFilterNode_GetText( node, oyNAME_NICK,
                                                           oyAllocateFunc_ );
            hash_text_ = hash_temp;
          } else
            hash_text_ = oyFilterNode_GetText( node, oyNAME_NICK );

          hash_text_len = oyStrlen_( hash_text_ );

          hash_text = oyAllocateFunc_(hash_text_len + 16);
          oySprintf_( hash_text, "%s:%s", node->api7_->context_type,
                                          hash_text_ );

          if(oy_debug == 1)
          {
            size = 0;
            ptr = oyFilterNode_TextToInfo_( node, &size, oyAllocateFunc_ );
            if(ptr)
              oyWriteMemToFile_( "test_dbg_colour.icc", ptr, size );
          }

          /* 2. query in cache for api7 */
          hash_out = oyCMMCacheListGetEntry_( hash_text );

          if(error <= 0)
          {
            /* 3. check and 3.a take*/
            cmm_ptr_out = (oyPointer_s*) oyHash_GetPointer( hash_out,
                                                        oyOBJECT_POINTER_S);

            if(!(cmm_ptr_out && oyPointer_GetPointer(cmm_ptr_out)) || blob)
            {
              oySprintf_( hash_text, "%s:%s", s->api4_->context_type, 
                                              hash_text_ );
              /* 2. query in cache for api4 */
              hash = oyCMMCacheListGetEntry_( hash_text );
              cmm_ptr = (oyPointer_s*) oyHash_GetPointer( hash,
                                                        oyOBJECT_POINTER_S);

              if(!cmm_ptr)
              {
                size = 0;
                cmm_ptr = oyPointer_New(0);
              }

              /* write the context to memory */
              if(blob)
              {

                error = oyOptions_SetFromText( &node->tags, "////verbose",
                                               "true", OY_CREATE_NEW );

                /* oy_debug is used to obtain a complete data set */
                ptr = s->api4_->oyCMMFilterNode_ContextToMem( node, &size,
                                                              oyAllocateFunc_ );
                oyBlob_SetFromData( blob, ptr, size, s->api4_->context_type );
                error = oyOptions_SetFromText( &node->tags, "////verbose",
                                               "false", 0 );

                goto clean;
              }

              if(!oyPointer_GetPointer(cmm_ptr))
              {
                /* 3b. ask CMM */
                ptr = s->api4_->oyCMMFilterNode_ContextToMem( node, &size,
                                                              oyAllocateFunc_ );

                if(!ptr || !size)
                {
                  oyMessageFunc_p( oyMSG_ERROR, (oyStruct_s*) node, 
                  OY_DBG_FORMAT_ "no device link for caching\n%s", OY_DBG_ARGS_,
                  oyFilterNode_GetText( node, oyNAME_NICK ));
                  error = 1;
                  oyPointer_Release( &cmm_ptr );
                }

                if(!error)
                {
                  error = oyPointer_Set( cmm_ptr, s->api4_->id_,
                                         s->api4_->context_type,
                                    ptr, "oyPointerRelease", oyPointerRelease);
                  oyPointer_SetSize( cmm_ptr, size );

                  /* 3b.1. update cache entry */
                  error = oyHash_SetPointer( hash, (oyStruct_s*) cmm_ptr);
                }
              }


              if(error <= 0 && cmm_ptr && oyPointer_GetPointer(cmm_ptr))
              {
                if(node->backend_data && node->backend_data->release)
                node->backend_data->release( (oyStruct_s**)&node->backend_data);

                if( oyStrcmp_( node->api7_->context_type,
                               s->api4_->context_type ) != 0 )
                {
                  cmm_ptr_out = oyPointer_New(0);
                  error = oyPointer_Set( cmm_ptr_out, node->api7_->id_,
                                         node->api7_->context_type, 0, 0, 0);

                  /* search for a convertor and convert */
                  oyPointer_ConvertData( cmm_ptr, cmm_ptr_out, node );
                  node->backend_data = cmm_ptr_out;
                  /* 3b.1. update cache entry */
                  error = oyHash_SetPointer( hash_out,
                                              (oyStruct_s*) cmm_ptr_out);

                } else
                  node->backend_data = oyPointer_Copy( cmm_ptr, 0 );
              }

              if(oy_debug == 1)
              {
                int id = oyFilterNode_GetId( node );
                char * file_name = 0;
                oyAllocHelper_m_( file_name, char, 80, 0, return 1 );
                sprintf( file_name, "test_dbg_colour_dl-%d.icc", id );
                if(ptr && size && node->backend_data)
                  oyWriteMemToFile_( file_name, ptr, size );
                oyFree_m_(file_name);
              }

            } else
              node->backend_data = cmm_ptr_out;

          }


    clean:
    if(hash_temp) oyDeAllocateFunc_(hash_temp);
    if(hash_text) oyDeAllocateFunc_(hash_text);
  }

  return error;
}


/** Function oyFilterNodes_New
 *  @memberof oyFilterNodes_s
 *  @brief   allocate a new FilterNodes list
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/28 (Oyranos: 0.1.10)
 *  @date    2009/02/28
 */
OYAPI oyFilterNodes_s * OYEXPORT
           oyFilterNodes_New         ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_FILTER_NODES_S;
# define STRUCT_TYPE oyFilterNodes_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = 0;

  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyFilterNodes_Copy;
  s->release = (oyStruct_Release_f) oyFilterNodes_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  s->list_ = oyStructList_Create( s->type_, 0, 0 );

  return s;
}

/** @internal
 *  Function oyFilterNodes_Copy_
 *  @memberof oyFilterNodes_s
 *  @brief   real copy a FilterNodes object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/28 (Oyranos: 0.1.10)
 *  @date    2009/02/28
 */
oyFilterNodes_s * oyFilterNodes_Copy_
                                     ( oyFilterNodes_s   * obj,
                                       oyObject_s          object )
{
  oyFilterNodes_s * s = 0;
  int error = 0;

  if(!obj || !object)
    return s;

  s = oyFilterNodes_New( object );
  error = !s;

  if(!error)
    s->list_ = oyStructList_Copy( obj->list_, s->oy_ );

  if(error)
    oyFilterNodes_Release( &s );

  return s;
}

/** Function oyFilterNodes_Copy
 *  @memberof oyFilterNodes_s
 *  @brief   copy or reference a FilterNodes list
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/28 (Oyranos: 0.1.10)
 *  @date    2009/02/28
 */
OYAPI oyFilterNodes_s * OYEXPORT
           oyFilterNodes_Copy        ( oyFilterNodes_s   * obj,
                                       oyObject_s          object )
{
  oyFilterNodes_s * s = obj;

  if(!obj)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_NODES_S, return 0 )

  if(obj && !object)
  {
    s = obj;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyFilterNodes_Copy_( obj, object );

  return s;
}
 
/** Function oyFilterNodes_Release
 *  @memberof oyFilterNodes_s
 *  @brief   release and possibly deallocate a FilterNodes list
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/28 (Oyranos: 0.1.10)
 *  @date    2009/02/28
 */
OYAPI int  OYEXPORT
           oyFilterNodes_Release     ( oyFilterNodes_s  ** obj )
{
  /* ---- start of common object destructor ----- */
  oyFilterNodes_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  oyCheckType__m( oyOBJECT_FILTER_NODES_S, return 1 )

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  oyStructList_Release( &s->list_ );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}


/** Function oyFilterNodes_MoveIn
 *  @memberof oyFilterNodes_s
 *  @brief   add a element to a FilterNodes list
 *
 *  @param[in]     list                list
 *  @param[in,out] obj                 list element
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/28 (Oyranos: 0.1.10)
 *  @date    2009/02/28
 */
OYAPI int  OYEXPORT
           oyFilterNodes_MoveIn      ( oyFilterNodes_s   * list,
                                       oyFilterNode_s   ** obj,
                                       int                 pos )
{
  oyFilterNodes_s * s = list;
  int error = 0;

  if(s)
    oyCheckType__m( oyOBJECT_FILTER_NODES_S, return 1 )

  if(obj && *obj && (*obj)->type_ == oyOBJECT_FILTER_NODE_S)
  {
    if(!s)
    {
      s = oyFilterNodes_New(0);
      error = !s;
    }                                  

    if(!error && !s->list_)
    {
      s->list_ = oyStructList_Create( s->type_, 0, 0 );
      error = !s->list_;
    }
      
    if(!error)
      error = oyStructList_MoveIn( s->list_, (oyStruct_s**)obj, pos,
                                   OY_OBSERVE_AS_WELL );
  }   
  
  return error;
}

/** Function oyFilterNodes_ReleaseAt
 *  @memberof oyFilterNodes_s
 *  @brief   release a element from a FilterNodes list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/28 (Oyranos: 0.1.10)
 *  @date    2009/02/28
 */
OYAPI int  OYEXPORT
           oyFilterNodes_ReleaseAt   ( oyFilterNodes_s   * list,
                                       int                 pos )
{ 
  int error = !list;
  oyFilterNodes_s * s = list;

  if(!error)
    oyCheckType__m( oyOBJECT_FILTER_NODES_S, return 1 )

  if(!error && list->type_ != oyOBJECT_FILTER_NODES_S)
    error = 1;
  
  if(!error)
    oyStructList_ReleaseAt( list->list_, pos );

  return error;
}

/** Function oyFilterNodes_Get
 *  @memberof oyFilterNodes_s
 *  @brief   get a element of a FilterNodes list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/28 (Oyranos: 0.1.10)
 *  @date    2009/02/28
 */
OYAPI oyFilterNode_s * OYEXPORT
           oyFilterNodes_Get         ( oyFilterNodes_s   * list,
                                       int                 pos )
{       
  int error = !list;
  oyFilterNodes_s * s = list;

  if(!error)
    oyCheckType__m( oyOBJECT_FILTER_NODES_S, return 0 )

  if(!error)
    return (oyFilterNode_s *) oyStructList_GetRefType( list->list_, pos, oyOBJECT_FILTER_NODE_S ); 
  else  
    return 0;
}   

/** Function oyFilterNodes_Count
 *  @memberof oyFilterNodes_s
 *  @brief   count the elements in a FilterNodes list
 *
 *  @param[in,out] list                the list
 *  @return                            element count
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/28 (Oyranos: 0.1.10)
 *  @date    2009/02/28
 */
OYAPI int  OYEXPORT
           oyFilterNodes_Count       ( oyFilterNodes_s   * list )
{       
  int error = !list;
  oyFilterNodes_s * s = list;

  if(!error)
    oyCheckType__m( oyOBJECT_FILTER_NODES_S, return 0 )

  if(!error)
    return oyStructList_Count( list->list_ );
  else return 0;
}


/** Function oyFilterGraph_New
 *  @memberof oyFilterGraph_s
 *  @brief   allocate a new FilterGraph object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/28 (Oyranos: 0.1.10)
 *  @date    2009/02/28
 */
OYAPI oyFilterGraph_s * OYEXPORT
           oyFilterGraph_New         ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_FILTER_GRAPH_S;
# define STRUCT_TYPE oyFilterGraph_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = 0;

  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyFilterGraph_Copy;
  s->release = (oyStruct_Release_f) oyFilterGraph_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  s->options = oyOptions_New( 0 );

  return s;
}

/** @internal
 *  Function oyAdjacencyListAdd_
 *  @brief   get a graphs adjazency list
 *
 *  Try to add an edge, if not yet found in the scanned graph.
 *  If the edge/plug was added, return success.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/25 (Oyranos: 0.1.10)
 *  @date    2009/03/06
 */
int    oyAdjacencyListAdd_           ( oyFilterPlug_s    * plug,
                                       oyFilterNodes_s   * nodes,
                                       oyFilterPlugs_s   * edges,
                                       const char        * selector,
                                       int                 flags )
{
  int added = 0, found = 0,
      i,n;
  oyFilterPlug_s * p = 0;
  oyFilterNode_s * node = 0;

  if(selector &&
     (oyOptions_FindString( plug->node->tags, selector, 0 ) == 0 &&
      oyOptions_FindString( plug->remote_socket_->node->tags, selector,0) == 0))
    return added;

  n = oyFilterPlugs_Count( edges );
  for(i = 0; i < n; ++i)
  {
    p = oyFilterPlugs_Get( edges, i );
    if(oyObject_GetId( p->oy_ ) == oyObject_GetId( plug->oy_ ))
      found = 1;
    oyFilterPlug_Release( &p );
  }

  if(!found)
  {
    p = oyFilterPlug_Copy(plug, 0);
    oyFilterPlugs_MoveIn( edges, &p, -1 );
    added = !found;


    found = 0;
    {
      n = oyFilterNodes_Count( nodes );
      for(i = 0; i < n; ++i)
      {
        node = oyFilterNodes_Get( nodes, i );
        if(oyObject_GetId( plug->node->oy_ ) == oyObject_GetId( node->oy_))
          found = 1;
        oyFilterNode_Release( &node );
      }
      if(!found)
      {
        node = oyFilterNode_Copy( plug->node, 0 );
        oyFilterNodes_MoveIn( nodes, &node, -1 );
      }
    }

    {
      found = 0;
      n = oyFilterNodes_Count( nodes );
      for(i = 0; i < n; ++i)
      {
        node = oyFilterNodes_Get( nodes, i );
        if(plug->remote_socket_ && plug->remote_socket_->node)
        {
          if(oyObject_GetId( plug->remote_socket_->node->oy_ ) ==
             oyObject_GetId( node->oy_ ))
            found = 1;

        } else
          found = 1;

        oyFilterNode_Release( &node );
      }
      if(!found)
      {
        node = oyFilterNode_Copy( plug->remote_socket_->node, 0 );
        oyFilterNodes_MoveIn( nodes, &node, -1 );
      }
    }
  }

  return added;
}

/** Function oyFilterGraph_FromNode
 *  @memberof oyFilterNode_s
 *  @brief   get a graphs adjazency list
 *
 *  @param[in]     node                filter node
 *  @param[in]     flags               - OY_INPUT omit input direction
 *                                     - OY_OUTPUT omit output direction
 *  @return                            the graph
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/25 (Oyranos: 0.1.10)
 *  @date    2009/10/28
 */
OYAPI oyFilterGraph_s * OYEXPORT
           oyFilterGraph_FromNode    ( oyFilterNode_s    * node,
                                       int                 flags )
{
  oyFilterNode_s * s = node;
  oyFilterGraph_s * graph = 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return 0 )

  {
    graph = oyFilterGraph_New( 0 );
    oyFilterGraph_SetFromNode( graph, s, 0, flags );
  }

  return graph;
}


/** @internal
 *  Function oyFilterGraph_Copy_
 *  @memberof oyFilterGraph_s
 *  @brief   real copy a FilterGraph object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/28 (Oyranos: 0.1.10)
 *  @date    2009/02/28
 */
oyFilterGraph_s * oyFilterGraph_Copy_
                                     ( oyFilterGraph_s   * obj,
                                       oyObject_s          object )
{
  oyFilterGraph_s * s = 0;
  int error = 0;

  if(!obj || !object)
    return s;

  s = oyFilterGraph_New( object );
  error = !s;

  s->nodes = oyFilterNodes_Copy( obj->nodes, 0 );
  s->edges = oyFilterPlugs_Copy( obj->edges, 0 );
  s->options = oyOptions_Copy( obj->options, object );

  if(error)
    oyFilterGraph_Release( &s );

  return s;
}

/** Function oyFilterGraph_Copy
 *  @memberof oyFilterGraph_s
 *  @brief   copy or reference a FilterGraph object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/28 (Oyranos: 0.1.10)
 *  @date    2009/02/28
 */
OYAPI oyFilterGraph_s * OYEXPORT
           oyFilterGraph_Copy        ( oyFilterGraph_s   * obj,
                                       oyObject_s          object )
{
  oyFilterGraph_s * s = obj;

  if(!obj)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_GRAPH_S, return 0 )

  if(obj && !object)
  {
    s = obj;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyFilterGraph_Copy_( obj, object );

  return s;
}
 
/** Function oyFilterGraph_Release
 *  @memberof oyFilterGraph_s
 *  @brief   release and possibly deallocate a FilterGraph object
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/28 (Oyranos: 0.1.10)
 *  @date    2009/02/28
 */
OYAPI int  OYEXPORT
           oyFilterGraph_Release     ( oyFilterGraph_s  ** obj )
{
  /* ---- start of common object destructor ----- */
  oyFilterGraph_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  oyCheckType__m( oyOBJECT_FILTER_GRAPH_S, return 1 )

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  oyFilterNodes_Release( &s->nodes );
  oyFilterPlugs_Release( &s->edges );
  oyOptions_Release( &s->options );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}


void oyShowGraph__( oyFilterGraph_s * s )
{
  char * ptr = 0;
  int error = 0;
  oyFilterGraph_s * adjacency_list = s;

  oyCheckType__m( oyOBJECT_FILTER_GRAPH_S, return )
  /*return;*/

  ptr = oyFilterGraph_ToText( adjacency_list, 0, 0,
                              "Oyranos Test Graph", 0, malloc );
  oyWriteMemToFile_( "test.dot", ptr, strlen(ptr) );
  error = system("dot -Tps test.dot -o test.ps; gv -spartan -antialias test.ps &");
  if(error)
    WARNc1_S("error during calling \"dot -Tps test.dot -o test.ps; gv -spartan -antialias test.ps &\": %d", error);

  free(ptr); ptr = 0;
}
void oyShowGraph_( oyFilterNode_s * s, const char * selector )
{
  oyFilterGraph_s * adjacency_list = 0;

  oyCheckType__m( oyOBJECT_FILTER_NODE_S, return )
  /*return;*/

  adjacency_list = oyFilterGraph_New( 0 );
  oyFilterGraph_SetFromNode( adjacency_list, s, selector, 0 );

  oyShowGraph__(adjacency_list);

  oyFilterGraph_Release( &adjacency_list );
}
void               oyShowConversion_ ( oyConversion_s    * conversion,
                                       uint32_t            flags )
{
  char * ptr = 0, * t = 0, * t2 = 0, * command = 0;
  int error = 0;
  oyConversion_s * s = conversion;
  oyCheckType__m( oyOBJECT_CONVERSION_S, return )
  /*return;*/

  ptr = oyConversion_ToText( s, "Conversion Graph", 0, oyAllocateFunc_ );

  oyStringAddPrintf_( &t, oyAllocateFunc_, oyDeAllocateFunc_,
                      "test-%d.dot",
                      oyStruct_GetId( (oyStruct_s*) conversion ) );
  oyStringAddPrintf_( &t2, oyAllocateFunc_, oyDeAllocateFunc_,
                      "test-%d.ps",
                      oyStruct_GetId( (oyStruct_s*) conversion ) );

  oyWriteMemToFile_( t, ptr, strlen(ptr) );
  if(!(flags & 0x01))
  {
    STRING_ADD( command, "dot -Tps ");
    STRING_ADD( command, t );
    STRING_ADD( command, " -o ");
    STRING_ADD( command, t2 );
    STRING_ADD( command, "; gv -spartan -antialias ");
    STRING_ADD( command, t2 );
    STRING_ADD( command, " &");
  } else
  {
    STRING_ADD( command, "dot -Tps ");
    STRING_ADD( command, t );
    STRING_ADD( command, " -o ");
    STRING_ADD( command, t2 );
    STRING_ADD( command, " &");
  }
  error = system(command);
  if(error)
    WARNc2_S("error during calling \"%s\": %d", command, error);

  oyFree_m_(ptr);
  oyFree_m_(t);
  oyFree_m_(t2);
  oyFree_m_(command);
}

/** @internal
 *  @brief oyCMMProgress_f
 *
 *  @since Oyranos: version 0.1.8
 *  @date  4 december 2007 (API 0.1.8)
 */
void             oyCMMProgress_      ( int                 ID,
                                       double              progress )
{
  printf("%s:%d id: %d %0.02f \n",__FILE__,__LINE__, ID, progress);
}



/** @} objects_conversion */


/** \addtogroup objects_single_colour Named Colour API
 *  Named Colour API's.

 *  @{
 */


/** @brief manage complex oyNamedColour_s inside Oyranos
 *  @memberof oyNamedColour_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 *
 *  @param[in]     chan                pointer to channel data with a number of elements specified in sig or channels_n, optional
 *  @param[in]     blob                CGATS or other reference data, optional
 *  @param[in]     blob_len            length of the data blob
 *  @param[in]     ref                 possibly a ICC profile
 *  @param         object              the optional object
 * 
 */
oyNamedColour_s*
oyNamedColour_Create( const double      * chan,
                      const char        * blob,
                      int                 blob_len,
                      oyProfile_s       * ref,
                      oyObject_s          object )
{
  int n = 0;
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_NAMED_COLOUR_S;
# define STRUCT_TYPE oyNamedColour_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = 0;
  
  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyNamedColour_Copy;
  s->release = (oyStruct_Release_f) oyNamedColour_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  if(error <= 0)
    s->profile_  = oyProfile_Copy( ref, 0 );

  n = oyProfile_GetChannelsCount( s->profile_ );
  if(n)
    s->channels_ = s->oy_->allocateFunc_( n * sizeof(double) );
  oyCopyColour( chan, &s->channels_[0], 1, ref, n );
  oyCopyColour( 0, &s->XYZ_[0], 1, 0, 0 );

  if(error <= 0 && blob && blob_len)
  {
    s->blob_ = s->oy_->allocateFunc_( blob_len );
    if(!s->blob_) error = 1;

    if(error <= 0)
      error = !memcpy( s->blob_, blob, blob_len );

    if(error <= 0)
      s->blob_len_ = blob_len;
  }

  return s;
}

/** Function: oyNamedColour_CreateWithName
 *  @memberof oyNamedColour_s
 *  @brief   create a oyNamedColour_s struct
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 december 2007 (API 0.1.8)
 *
 *  @param[in]  nick        very short, only a few letters
 *  @param[in]  name        of the colour
 *  @param[in]  description description
 *  @param[in]  chan        pointer to channel data with a number of elements specified in sig or channels_n
 *  @param[in]  XYZ         reference values
 *  @param[in]  blob        CGATS or other reference data
 *  @param[in]  blob_len    length of the data blob
 *  @param[in]  profile_ref profile
 *  @param[in]  object      memory management
 *  @return                 a oyNamedColour_s
 * 
 */
oyNamedColour_s*  oyNamedColour_CreateWithName (
                                       const char        * nick,
                                       const char        * name,
                                       const char        * description,
                                       const double      * chan,
                                       const double      * XYZ,
                                       const char        * blob,
                                       int                 blob_len,
                                       oyProfile_s       * profile_ref,
                                       oyObject_s          object )
{
  int error = 0;
  oyNamedColour_s * s = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );

  error = !s_obj;

  if(error <= 0)
    error = oyObject_SetNames( s_obj, nick, name, description );

  s = oyNamedColour_Create( chan, blob, blob_len, profile_ref, s_obj );
  error =!s;

  oyObject_Release( &s_obj );

  if(error <= 0 && XYZ)
    oyCopyColour( XYZ, s->XYZ_, 1, 0, 0 );

  return s;
}

/** @brief   copy
 *  @memberof oyNamedColour_s
 *
 *  
 *
 *  @param[in]     colour              address of Oyranos colour struct
 *  @param[in]     object              the optional base object
 *  @return                            copy
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
oyNamedColour_s*
oyNamedColour_Copy ( oyNamedColour_s   * colour,
                     oyObject_s          object )
{
  oyNamedColour_s * c = NULL,
                  * s = colour;

  if(!colour)
    return c;

  if(!object)
  {
    oyObject_Ref( s->oy_ );
    return s;
  }

  c = oyNamedColour_Create(
                     colour->channels_,
                     colour->blob_,
                     colour->blob_len_,
                     colour->profile_,
                     object );

  return c;
}

/** @brief release correctly
 *  @memberof oyNamedColour_s
 *
 *  set pointer to zero
 *
 *  @param[in]     obj                 address of Oyranos colour struct pointer
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
int 
oyNamedColour_Release( oyNamedColour_s ** obj )
{
  /* ---- start of common object destructor ----- */
  oyNamedColour_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  oyCheckType__m( oyOBJECT_NAMED_COLOUR_S, return 1 )

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  oyProfile_Release( &s->profile_ );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    if(s->channels_)
      deallocateFunc( s->channels_ ); s->channels_ = 0;

    if(s->blob_) /* s->blob_len */
      deallocateFunc( s->blob_ ); s->blob_ = 0; s->blob_len_ = 0;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}


/*  @return                 pointer to D50 Lab doubles with L 0.0...1.0 a/b -1.27...1.27 */

/** Function: oyNamedColour_GetSpaceRef
 *  @memberof oyNamedColour_s
 *  @brief   get a colour space reference
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 december 2007 (API 0.1.8)
 */
oyProfile_s *     oyNamedColour_GetSpaceRef ( oyNamedColour_s  * colour )
{
  if(!colour)
    return 0;

  return oyProfile_Copy( colour->profile_, 0 );
}


/** Function: oyNamedColour_SetChannels
 *  @memberof oyNamedColour_s
 *  @brief   set colour channels
 *
 *
 *  @param[in]     colour              Oyranos colour struct pointer
 *  @param[in]     channels            pointer to channel data
 *  @param[in]     flags               reserved
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
void
oyNamedColour_SetChannels( oyNamedColour_s  * colour,
                           const double     * channels,
                                       uint32_t            flags )
{
  int i, n;
  if(!colour)
    return;

  n = oyProfile_GetChannelsCount( colour->profile_ );
  if(channels)
    for(i = 0; i < n; ++i)
      colour->channels_[i] = channels[i];
}

/**
 *  @internal
 *  Function: oyConvertColour_
 *  @memberof oyNamedColour_s
 *  @brief   convert colours
 *
 *  The options are passed to oyConversion_CreateBasicPixels();
 *  The function does the lookups for the profiles and the modules contexts
 *  in the Oyranos cache on the fly. The allocated oyImage_s and
 *  oyConversion_s structures are not cheap as they are not cached.
 *
 *  @version Oyranos: 0.1.11
 *  @since   2007/12/23 (Oyranos: 0.1.8)
 *  @date    2010/09/10
 */
int  oyColourConvert_ ( oyProfile_s       * p_in,
                        oyProfile_s       * p_out,
                        oyPointer           buf_in,
                        oyPointer           buf_out,
                        oyDATATYPE_e        buf_type_in,
                        oyDATATYPE_e        buf_type_out,
                        oyOptions_s       * options,
                        int                 count )
{
  oyImage_s * in  = NULL,
            * out = NULL;
  oyConversion_s * conv = NULL;
  int error = 0;

  in    = oyImage_Create( count, 1,
                         buf_in ,
                         oyChannels_m(oyProfile_GetChannelsCount(p_in)) |
                          oyDataType_m(buf_type_in),
                         p_in,
                         0 );
  out   = oyImage_Create( count, 1,
                         buf_out ,
                         oyChannels_m(oyProfile_GetChannelsCount(p_out)) |
                          oyDataType_m(buf_type_out),
                         p_out,
                         0 );

  conv   = oyConversion_CreateBasicPixels( in,out, options, 0 );
  error  = oyConversion_RunPixels( conv, 0 );

  oyConversion_Release( &conv );
  oyImage_Release( &in );
  oyImage_Release( &out );

  return error;
}

/** Function: oyNamedColour_GetColour
 *  @memberof oyNamedColour_s
 *  @brief   convert a named colour to a standard colour space
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 december 2007 (API 0.1.8)
 */
int          oyNamedColour_GetColour ( oyNamedColour_s   * colour,
                                       oyProfile_s       * profile,
                                       oyPointer           buf,
                                       oyDATATYPE_e        buf_type,
                                       uint32_t            flags,
                                       oyOptions_s       * options )
{
  int error = !colour || !profile || !buf;
  oyProfile_s * p_in = colour->profile_;

  /* XYZ has priority */
  if(error <= 0 &&
     colour->XYZ_[0] != -1 && colour->XYZ_[1] != -1 && colour->XYZ_[2] != -1)
  {
    p_in = oyProfile_FromStd ( oyEDITING_XYZ, NULL );
    if(!profile)
      return 1;
 
    error = oyColourConvert_( p_in, profile,
                              colour->XYZ_, buf,
                              oyDOUBLE, buf_type, options, 1);

    oyProfile_Release ( &p_in );

  } else if(error <= 0)
    error = oyColourConvert_( p_in, profile,
                              colour->channels_, buf,
                              oyDOUBLE, buf_type, options, 1);

  return error;
}

/** Function: oyNamedColour_SetColourStd
 *  @memberof oyNamedColour_s
 *  @brief   set colour channels
 *
 *
 *  @param[in]     colour              Oyranos colour struct pointer
 *  @param[in]     colour_space        Oyranos standard colour space
 *  @param[in]     channels            pointer to channel data
 *  @param[in]     channels_type       data type
 *  @param[in]     flags               reserved for future use
 *  @param[in]     options             for filter node creation
 *  @return                            error
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 december 2007 (API 0.1.8)
 */
int               oyNamedColour_SetColourStd ( oyNamedColour_s * colour,
                                       oyPROFILE_e         colour_space,
                                       oyPointer           channels,
                                       oyDATATYPE_e        channels_type,
                                       uint32_t            flags,
                                       oyOptions_s       * options )
{
  oyNamedColour_s * s = colour;
  int error = !s || !colour_space || !channels;
  oyProfile_s * p_in = 0;
  oyProfile_s * p_out = 0;

  /* abreviate */
  if(error <= 0 && channels_type == oyDOUBLE)
  {
    if     (colour_space == oyEDITING_LAB)
    {
      oyLab2XYZ( (double*)channels, s->XYZ_ );
      return error;

    }
    else if(colour_space == oyEDITING_XYZ)
    {
      oyCopyColour( (double*)channels, s->XYZ_, 1, 0, 0 );
      return error;
    }
  }

  if(error <= 0)
  {
    p_in = oyProfile_FromStd ( colour_space, NULL );
    error = !p_in;
  }

  /* reset and allocate */
  if(error <= 0)
  {
    int n = oyProfile_GetChannelsCount( p_in );

    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    if(n > oyProfile_GetChannelsCount( s->profile_ ))
    {
      if(s->channels_)
        deallocateFunc(s->channels_); s->channels_ = 0;

      s->channels_ = s->oy_->allocateFunc_( n * sizeof(double) );
    }

    error = !memset( s->channels_, 0, sizeof(double) * n );

    s->XYZ_[0] = s->XYZ_[1] = s->XYZ_[2] = -1;

    if(deallocateFunc && s->blob_)
      deallocateFunc( s->blob_ ); s->blob_ = 0; s->blob_len_ = 0;
  }

  /* convert */
  if(error <= 0)
  {
    p_out = s->profile_;
    error = oyColourConvert_( p_in, p_out,
                              channels, s->channels_,
                              channels_type , oyDOUBLE, options, 1 );
    p_out = 0;
  }

  if(error <= 0)                               
  {  
    p_out = oyProfile_FromStd( oyEDITING_XYZ, 0 );
    error = oyColourConvert_( p_in, p_out,
                              channels, s->XYZ_,
                              channels_type , oyDOUBLE, options, 1 );
    oyProfile_Release ( &p_out );
  }

  return error;
}

/** Function: oyNamedColour_GetChannelsConst
 *  @memberof oyNamedColour_s
 *  @brief   get colour channels
 *
 *
 *  @param[in]     colour              Oyranos colour struct pointer
 *  @param[in]     flags               reserved
 *  @return                            pointer channels
 *
 *  @since Oyranos: version 0.1.8
 *  @date  october 2007 (API 0.1.8)
 */
const double *    oyNamedColour_GetChannelsConst ( oyNamedColour_s * colour,
                                       uint32_t          * flags )
{
  static double l[3] = {-1.f,-1.f,-1.f};
  if(colour)
    return colour->channels_;
  else
    return l;
}

/** Function: oyNamedColour_GetiXYZConst
 *  @memberof oyNamedColour_s
 *  @brief   get XYZ channels
 *
 *
 *  @param[in]  colour      Oyranos colour struct pointer
 *  @return                 pointer XYZ channels
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/02/18
 *  @since   2008/02/18 (Oyranos: 0.1.8)
 */
const double *    oyNamedColour_GetXYZConst      ( oyNamedColour_s * colour)
{
  static double l[3] = {-1.f,-1.f,-1.f};
  if(colour)
    return colour->XYZ_;
  else
    return l;
}

/** Function: oyNamedColour_GetColourStd
 *  @memberof oyNamedColour_s
 *  @brief   convert a named colour to a standard colour space
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 december 2007 (API 0.1.8)
 */
int               oyNamedColour_GetColourStd     ( oyNamedColour_s * colour,
                                       oyPROFILE_e         colour_space,
                                       oyPointer           buf,
                                       oyDATATYPE_e        buf_type,
                                       uint32_t          * flags,
                                       oyOptions_s       * options )
{                        
  int ret = 0;
  oyProfile_s * profile;

  if(!colour)
    return 1;

  /* abreviate */
  if(buf_type == oyDOUBLE &&
     colour->XYZ_[0] != -1 && colour->XYZ_[1] != -1 && colour->XYZ_[2] != -1)
  {
    if(colour_space == oyEDITING_LAB)
    {
      oyXYZ2Lab( colour->XYZ_, (double*)buf );
      return 0;
    }

    if(colour_space == oyEDITING_XYZ)
    {
      oyCopyColour( colour->XYZ_, (double*)buf, 1, 0, 0 );
      return 0;
    }
  }

  profile = oyProfile_FromStd ( colour_space, NULL );
  if(!profile)
    return 1;

  ret = oyNamedColour_GetColour ( colour, profile, buf, buf_type, 0, options );
  oyProfile_Release ( &profile );

  return ret;
}

/** Function: oyNamedColour_GetName
 *  @memberof oyNamedColour_s
 *  @brief   get colour channels
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 december 2007 (API 0.1.8)
 */
const oyChar *    oyNamedColour_GetName( oyNamedColour_s * s,
                                       oyNAME_e            type,
                                       uint32_t            flags )
{
  const oyChar * text = 0;
  if(!s)
    return 0;

  if(!s->oy_)
    return 0;

  text = oyObject_GetName( s->oy_, type );

  if(!text && flags)
  {
    const oyChar * tmp = 0;
    oyChar * txt = 0;
    double l[3];
    int i;
    icSignature sig = oyProfile_GetSignature( s->profile_,
                                              oySIGNATURE_COLOUR_SPACE );

    oyAllocHelper_m_( txt, oyChar, 1024, 0, return 0 );
    oyNamedColour_GetColourStd( s, oyEDITING_LAB, l, oyDOUBLE, 0, 0 );

    switch(type)
    {
      case oyNAME_DESCRIPTION:
           tmp = oyObject_GetName( s->oy_, oyNAME_NAME );
           if(!tmp)
             tmp = oyObject_GetName( s->oy_, oyNAME_NICK );

           if(tmp)
             oySprintf_(txt, "%s: CIE*Lab: ", tmp );
           else
             oySprintf_(txt, "CIE*Lab: ");
           for(i = 0; i < 3; ++i)
             oySprintf_( &txt[ oyStrlen_(txt) ], "%.02f ", l[i] );

           tmp = oyICCColourSpaceGetName( sig );
           if(tmp)
             oySprintf_( &txt[ oyStrlen_(txt) ], "; %s:", tmp );

           if( s->channels_ )
           for(i = 0; i < oyICCColourSpaceGetChannelCount( sig ); ++i)
             oySprintf_( &txt[ oyStrlen_(txt) ], "%.02f ", s->channels_[i] );
           break;
      default:
           break;
    }

    oyObject_SetName ( s->oy_, txt, type );

    oyFree_m_( txt );
    
    text = oyObject_GetName( s->oy_, type );
  }

  if(!text && type > oyNAME_NICK)
    text = oyObject_GetName( s->oy_, type - 2 );
  if(!text && type > oyNAME_NAME)
    text = oyObject_GetName( s->oy_, type - 1 );
  if(!text && type < oyNAME_NICK )
    text = oyObject_GetName( s->oy_, type + 2 );
  if(!text && type < oyNAME_DESCRIPTION )
    text = oyObject_GetName( s->oy_, type + 1 );
  if(!text)
    text = _("---");

  return text;
}









/** Function: oyNamedColours_New
 *  @memberof oyNamedColours_s
 *  @brief   create a list object for oyNamedColour_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 december 2007 (API 0.1.8)
 */
oyNamedColours_s* oyNamedColours_New ( oyObject_s       object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_NAMED_COLOURS_S;
# define STRUCT_TYPE oyNamedColours_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = 0;
  
  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyNamedColours_Copy;
  s->release = (oyStruct_Release_f) oyNamedColours_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  if(error <= 0)
    s->list_ = oyStructList_Create( s->type_, 0, object );

  return s;
}

/** Function: oyNamedColours_Copy
 *  @memberof oyNamedColours_s
 *  @brief   Copy
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 december 2007 (API 0.1.8)
 */
oyNamedColours_s* oyNamedColours_Copy( oyNamedColours_s  * colours,
                                       oyObject_s          obj )
{
  oyNamedColours_s * s = 0;
  int error = !colours;

  if(error)
    return s;

  if(colours && !obj)
  {
    s = colours;
    s->oy_ = oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyNamedColours_New( obj );

  s->list_ = oyStructList_Copy( colours->list_, 0 );

  return s;
}

/** Function: oyNamedColours_Release
 *  @memberof oyNamedColours_s
 *  @brief   release and probably free a named colour object
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 december 2007 (API 0.1.8)
 */
int               oyNamedColours_Release ( oyNamedColours_s** obj )
{
  int error = 0;
  /* ---- start of common object destructor ----- */
  oyNamedColours_s * s = 0;

  if(!obj || !*obj)
    return 1;

  s = *obj;

  oyCheckType__m( oyOBJECT_NAMED_COLOURS_S, return 1 )

  *obj = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  oyStructList_Release( &s->list_ );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    s->type_ = 0;

    deallocateFunc( s );
  }

  return error;
}

/** Function: oyNamedColours_Count
 *  @memberof oyNamedColours_s
 *  @brief   count in oyNamedColours_s
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 december 2007 (API 0.1.8)
 */
int               oyNamedColours_Count( oyNamedColours_s * obj )
{
  if(obj && obj->type_ == oyOBJECT_NAMED_COLOURS_S)
    return oyStructList_Count( obj->list_ );
  else
    return -1;
}

/** Function: oyNamedColours_Get
 *  @memberof oyNamedColours_s
 *  @brief   obtain a referenced patch from the list
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 december 2007 (API 0.1.8)
 */
oyNamedColour_s*  oyNamedColours_Get ( oyNamedColours_s  * obj,
                                       int                 position)
{
  int error = !obj;
  oyNamedColour_s * s = 0;

  if(error <= 0 && obj->type_ != oyOBJECT_NAMED_COLOURS_S)
    error = 1;

  if(error <= 0)
    s = (oyNamedColour_s*) oyStructList_GetRefType( obj->list_, position,
                                                 oyOBJECT_NAMED_COLOUR_S );

 
  return s;
}

/** Function: oyNamedColours_MoveIn
 *  @memberof oyNamedColours_s
 *  @brief   add a patch to the colours list
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 december 2007 (API 0.1.8)
 */
oyNamedColours_s * oyNamedColours_MoveIn ( oyNamedColours_s  * list,
                                       oyNamedColour_s  ** obj,
                                       int                 pos )
{
  int error = 0;
  oyNamedColours_s * s = list;

  if(obj && *obj && (*obj)->type_ == oyOBJECT_NAMED_COLOUR_S)
  {
    if(!s)
    {
      s = oyNamedColours_New(0);
      error = !s;
    }

    if(error <= 0 && !s->list_)
    {
      s->list_ = oyStructList_Create( s->type_, 0, 0 );
      error = !s->list_;
    }

    if(error <= 0)
      error = oyStructList_MoveIn( s->list_, (oyStruct_s**)obj, pos, 0 );
  }

  return s;
}

/** Function: oyNamedColours_ReleaseAt
 *  @memberof oyNamedColours_s
 *  @brief   release a patch from the list
 *
 *  @since Oyranos: version 0.1.8
 *  @date  22 december 2007 (API 0.1.8)
 */
int               oyNamedColours_ReleaseAt ( oyNamedColours_s * obj,
                                       int                 position )
{
  int error = !obj;

  if(error <= 0 && obj->type_ != oyOBJECT_NAMED_COLOURS_S)
    error = 1;

  if(error <= 0)
    oyStructList_ReleaseAt( obj->list_, position );

  return error; 
}


/** @} *//* objects_single_colour */


/** \addtogroup module_api

 *  @{
 */
const char *   oyModuleGetActual     ( const char        * type )
{
  oyExportStart_(EXPORT_CMMS);
  oyExportEnd_();

  if(oyFilterRegistrationMatch( type, "//" OY_TYPE_STD, oyOBJECT_CMM_API4_S ))
  return "lcms";
  else
  return OY_PROFILE_NONE;
}

uint32_t     oyCMMtoId               ( const char        * cmm )
{
  if(cmm)
    return oyValueUInt32( *(uint32_t*)cmm );
  else
    return 0;
}
int          oyIdToCMM               ( uint32_t            cmmId,
                                       char              * cmm )
{
  cmmId = oyValueUInt32( cmmId );

  if(cmm)
    return !memcpy( cmm, &cmmId, 4 );
  else
    return 0;
}

/** @} *//* module_api */



/** \addtogroup module_api

 *  @{
 */

/** Function oyCMMInfo_New
 *  @memberof oyCMMInfo_s
 *  @brief   allocate a new CMMInfo object
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/12/22 (Oyranos: 0.1.9)
 *  @date    2008/12/22
 */
OYAPI oyCMMInfo_s * OYEXPORT
                   oyCMMInfo_New ( oyObject_s          object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CMM_INFO_S;
# define STRUCT_TYPE oyCMMInfo_s
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  STRUCT_TYPE * s = 0;
  
  if(s_obj)
    s = (STRUCT_TYPE*)s_obj->allocateFunc_(sizeof(STRUCT_TYPE));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type = type;
  s->copy_ = (oyStruct_Copy_f) oyCMMInfo_Copy;
  s->release_ = (oyStruct_Release_f) oyCMMInfo_Release;

  s->oy_ = s_obj;

  error = !oyObject_SetParent( s_obj, type, (oyPointer)s );
# undef STRUCT_TYPE
  /* ---- end of common object constructor ------- */

  return s;
}

/** @internal
 *  Function oyCMMInfo_Copy_
 *  @memberof oyCMMInfo_s
 *  @brief   real copy a CMMInfo object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/12/22 (Oyranos: 0.1.9)
 *  @date    2008/12/22
 */
oyCMMInfo_s * oyCMMInfo_Copy_
                                     ( oyCMMInfo_s       * obj,
                                       oyObject_s          object )
{
  oyCMMInfo_s * s = 0;
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;

  if(!obj || !object)
    return s;

  s = oyCMMInfo_New( object );
  error = !s;

  if(error <= 0)
  {
    allocateFunc_ = s->oy_->allocateFunc_;

    error = !memcpy(s->cmm, obj->cmm, 8);

    if(obj->backend_version)
      s->backend_version = oyStringCopy_( obj->backend_version, allocateFunc_ );

    s->getText = obj->getText;
      
    s->oy_compatibility = obj->oy_compatibility;
  }

  if(error)
    oyCMMInfo_Release( &s );

  return s;
}

/** Function oyCMMInfo_Copy
 *  @memberof oyCMMInfo_s
 *  @brief   copy or reference a CMMInfo object
 *
 *  @param[in]     obj                 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/12/22 (Oyranos: 0.1.9)
 *  @date    2008/12/22
 */
OYAPI oyCMMInfo_s * OYEXPORT
                   oyCMMInfo_Copy ( oyCMMInfo_s       * obj,
                                       oyObject_s          object )
{
  oyCMMInfo_s * s = 0;

  if(!obj || obj->type != oyOBJECT_CMM_INFO_S)
    return s;

  if(obj && !object)
  {
    s = obj;
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyCMMInfo_Copy_( obj, object );

  return s;
}
 
/** Function oyCMMInfo_Release
 *  @memberof oyCMMInfo_s
 *  @brief   release and possibly deallocate a CMMInfo object
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/12/22 (Oyranos: 0.1.9)
 *  @date    2008/12/22
 */
OYAPI int  OYEXPORT
               oyCMMInfo_Release     ( oyCMMInfo_s      ** obj )
{
  /* ---- start of common object destructor ----- */
  oyCMMInfo_s * s = 0;

  if(!obj || !*obj)
    return 0;

  s = *obj;

  if(!(s && s->type == oyOBJECT_CMM_INFO_S && !s->release_))
  oyCheckType_m( oyOBJECT_CMM_INFO_S, return 1 )

  *obj = 0;

  
  if(!s->oy_ || oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    if(s->backend_version)
      deallocateFunc( s->backend_version );

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}

/** @internal
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/30 (Oyranos: 0.1.10)
 *  @date    2009/01/30
 */
int    oyIsOfTypeCMMapiFilter        ( oyOBJECT_e          type )
{
  return        type == oyOBJECT_CMM_API4_S ||
                type == oyOBJECT_CMM_API6_S ||
                type == oyOBJECT_CMM_API7_S ||
                type == oyOBJECT_CMM_API8_S ||
                type == oyOBJECT_CMM_API9_S ||
                type == oyOBJECT_CMM_API10_S;
}

/** @internal
 *  Function oyCMMapiNumberToChar
 *  @brief   convert a oyOBJECT_e to a char
 *
 *  @param         api_number          object type
 *  @return                            number as char or zero
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/06/02 (Oyranos: 0.1.10)
 *  @date    2009/06/02
 */
char   oyCMMapiNumberToChar          ( oyOBJECT_e          api_number )
{   
         if(api_number == oyOBJECT_CMM_API4_S)
      return '4';
    else if(api_number == oyOBJECT_CMM_API5_S)
      return '5';
    else if(api_number == oyOBJECT_CMM_API6_S)
      return '6';
    else if(api_number == oyOBJECT_CMM_API7_S)
      return '7';
    else if(api_number == oyOBJECT_CMM_API8_S)
      return '8';
    else if(api_number == oyOBJECT_CMM_API9_S)
      return '9';
  return 0;
}
/** @} *//* module_api */


/** \addtogroup misc Miscellaneous

 *  @{
 */

#define icValue_to_icUInt32Number_m(funkname, typ) \
typ \
funkname (typ val) \
{ \
  icUInt32Number i = (icUInt32Number) val; \
  return (typ) oyValueUInt32 (i); \
}

icValue_to_icUInt32Number_m( oyValueCSpaceSig, icColorSpaceSignature )
icValue_to_icUInt32Number_m( oyValuePlatSig, icPlatformSignature )
icValue_to_icUInt32Number_m( oyValueProfCSig, icProfileClassSignature )
icValue_to_icUInt32Number_m( oyValueTagSig, icTagSignature )




/** @} *//*misc */

/** @} *//*alpha
 */




/** \addtogroup monitor_api Monitor API
 *  @brief Hardware detection and profile selection for monitors

 *  The notation of screens for the X Window system is equal for traditional 
 *  and Xinerama screens. \n
 *  The X11 libraries will be loaded at runtime.
 *
 *  The functions can be studied by looking at the oyranos-monitor utiliy.
 *  The code for that tool is in oyranos_gamma.c .
 *
 *  @{
 */

/** Function: oyGetMonitorInfo
 *  @brief   brief pick up monitor information with Xlib
 *
 *  @deprecated because the functionality shall be integrated into a general framwork
 *
 *  @param      display_name  the display string
 *  @param[out] manufacturer  the manufacturer of the monitor device
 *  @param[out] model         the model of the monitor device
 *  @param[out] serial        the serial number of the monitor device
 *  @param[out] system_port   the system port name the monitor device is connectted to
 *  @param[out] display_geometry the display geometry description of the monitor
 *  @param[out] edid          the edid data blob of the monitor device
 *  @param      allocate_func the allocator for the above strings
 *  @return     error
 *
 *  @version Oyranos: 0.1.8
 *  @since   2005/00/00 (Oyranos: 0.1.x)
 *  @date    2008/10/24
 */
int      oyGetMonitorInfo            ( const char        * display_name,
                                       char             ** manufacturer,
                                       char             ** model,
                                       char             ** serial,
                                       char             ** system_port,
                                       char             ** display_geometry,
                                       oyBlob_s         ** edid,
                                       oyAlloc_f           allocate_func )
{
  int error = 0;

  error = 1;

  return error;
}

/** Function: oyGetScreenFromPosition
 *  @brief   screen number from position
 *
 *  @deprecated because the functionality shall be integrated into a general framwork
 *
 *  This function will hit exact results only with Xinerama. \n
 *  a platform specific function
 *
 *  @param      display_name  the display string
 *  @param      x             x position on screen
 *  @param      y             y position on screen
 *  @return                   screen
 *
 *  @version Oyranos: 0.1.8
 *  @since   2005/00/00 (Oyranos: 0.1.x)
 *  @date    2008/10/24
 */
int      oyGetScreenFromPosition     ( const char        * display_name,
                                       int                 x,
                                       int                 y )
{
  int screen = 0;

  return screen;
}

/** Function: oyGetDisplayNameFromPosition2
 *  @brief   display name from position
 *
 *  This function will hit exact results only with Xinerama.
 *
 *  @param[in]     device_type         the device type, e.g. OY_TYPE_STD,
 *                                     defaults to OY_TYPE_STD (optional)
 *  @param[in]     device_class        registration ::oyFILTER_REG_APPLICATION
 *                                     part, e.g. "monitor", mandatory
 *  @param      display_name  raw display string
 *  @param      x             x position on screen
 *  @param      y             y position on screen
 *  @param      allocateFunc  function used to allocate memory for the string
 *  @return                   display name
 *
 *  @version Oyranos: 0.1.13
 *  @since   2005/00/00 (Oyranos: 0.1.x)
 *  @date    2010/11/12
 */
char *   oyGetDisplayNameFromPosition2(const char        * device_type,
                                       const char        * device_class,
                                       const char        * display_name,
                                       int                 x,
                                       int                 y,
                                       oyAlloc_f           allocateFunc )
{
  int error = 0;
  char * text = 0;
  oyConfig_s * device = 0;
  oyConfigs_s * devices = 0;
  oyOptions_s * options = 0;
  oyOption_s * o = 0;
  oyRectangle_s * r = 0;
  int n, i;
  const char * device_name = 0;

  error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/command",
                                 "list", OY_CREATE_NEW );
  error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/device_rectangle",
                                 "true", OY_CREATE_NEW );
  /** we want a fuzzy look at our display, not the narrow "device_name" */
  error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/display_name",
                                 display_name, OY_CREATE_NEW );

  error = oyConfigs_FromDeviceClass ( device_type, device_class, options, &devices,
                                          0 );

  if(error>0)
    WARNc1_S("oyConfigs_FromDeviceClass() returned: %d", error);

  oyOptions_Release( &options );

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  n = oyConfigs_Count( devices );
  for( i = 0; i < n; ++i )
  {
    device = oyConfigs_Get( devices, i );
    o = oyConfig_Find( device, "device_rectangle" );

    r = (oyRectangle_s*) oyOption_StructGet( o, oyOBJECT_RECTANGLE_S );

    if(!device_name &&
       r && oyRectangle_PointIsInside( r, x,y ))
    {
      device_name = oyConfig_FindString( device, "device_name", 0 );
      text = oyStringCopy_( device_name, allocateFunc );
    }
    oyConfig_Release( &device );
  }
  oyConfigs_Release( &devices );

  return text;
}


/** Function: oyGetDisplayNameFromPosition
 *  @brief   display name from position
 *
 *  This function will hit exact results only with Xinerama.
 *
 *  @param      display_name  raw display string
 *  @param      x             x position on screen
 *  @param      y             y position on screen
 *  @param      allocateFunc  function used to allocate memory for the string
 *  @return                   display name
 *
 *  @version Oyranos: 0.1.8
 *  @since   2005/00/00 (Oyranos: 0.1.x)
 *  @date    2008/10/24
 */
char *   oyGetDisplayNameFromPosition( const char        * display_name,
                                       int                 x,
                                       int                 y,
                                       oyAlloc_f           allocateFunc )
{
  int error = 0;
  char * text = 0;
  oyConfig_s * device = 0;
  oyConfigs_s * devices = 0;
  oyOptions_s * options = 0;
  oyOption_s * o = 0;
  oyRectangle_s * r = 0;
  int n, i;
  const char * device_name = 0;

  error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/command",
                                 "list", OY_CREATE_NEW );
  error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/device_rectangle",
                                 "true", OY_CREATE_NEW );
  /** we want a fuzzy look at our display, not the narrow "device_name" */
  error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/display_name",
                                 display_name, OY_CREATE_NEW );

  error = oyConfigs_FromDeviceClass ( 0, "monitor", options, &devices,
                                          0 );

  if(error>0)
    WARNc1_S("oyConfigs_FromDeviceClass() returned: %d", error);

  oyOptions_Release( &options );

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  n = oyConfigs_Count( devices );
  for( i = 0; i < n; ++i )
  {
    device = oyConfigs_Get( devices, i );
    o = oyConfig_Find( device, "device_rectangle" );

    r = (oyRectangle_s*) oyOption_StructGet( o, oyOBJECT_RECTANGLE_S );

    if(!device_name &&
       r && oyRectangle_PointIsInside( r, x,y ))
    {
      device_name = oyConfig_FindString( device, "device_name", 0 );
      text = oyStringCopy_( device_name, allocateFunc );
    }
    oyConfig_Release( &device );
  }
  oyConfigs_Release( &devices );

  return text;
}


/** Function: oyGetMonitorProfile
 *  @brief   get the monitor profile from the server
 *
 *  @deprecated because the functionality shall be integrated into a general framwork
 *
 *  @param      device_name        the display string
 *  @param[out] size                   the size of profile
 *  @param      allocate_func          function used to allocate memory for the profile
 *  @return                            the memory block containing the profile
 *
 *  @version Oyranos: 0.1.8
 *  @since   2005/00/00 (Oyranos: 0.1.x)
 *  @date    2009/02/10
 */
char *   oyGetMonitorProfile         ( const char        * device_name,
                                       size_t            * size,
                                       oyAlloc_f           allocate_func )
{
  int error = 0;
  oyConfig_s * device = 0;
  oyOptions_s * options = 0;
  oyProfile_s * p = 0;
  char * block = 0;
  const char * device_type = OY_TYPE_STD,
             * device_class = "monitor";

  if(error <= 0)
  {
    error = oyDeviceGet( device_type, device_class, device_name,
                             options, &device );
    error = oyDeviceGetProfile( device, 0, &p );
    oyConfig_Release( &device );
  }

  if(error <= 0)
  {
    block = oyProfile_GetMem( p, size, 0, allocate_func );
    error = !block || !size;
  }

  return block;
}

/** Function: oyMonitorProfileNameFromDB
 *  @brief   get the monitor profile filename from the device
 *           database
 *
 *  @deprecated because the functionality shall be integrated into a general framwork
 *
 *  @param         device          the device
 *  @param         allocateFunc        user function used to allocate memory 
 *  @return                            the profile filename
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/02/08 (Oyranos: 0.1.10)
 *  @date    2009/02/08
 */
char *   oyMonitorProfileNameFromDB  ( oyConfig_s        * device,
                                       oyAlloc_f           allocateFunc )
{
  int error = !device;
  oyProfile_s * p = 0;
  char * profile_name = 0;
  char * text = 0;

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  if(error <= 0)
  {
    error = oyDeviceProfileFromDB( device, &profile_name, 0 );
  }

  if(error <= 0)
  {
    if(profile_name && oyStrrchr_( profile_name, OY_SLASH_C ) != 0)
      profile_name = oyStrrchr_( profile_name, OY_SLASH_C ) + 1;

    text = oyStringCopy_( profile_name, allocateFunc );
  }

  if(profile_name)
    oyFree_m_( profile_name );
  oyProfile_Release( &p );

  return text;
}

/** Function: oyGetMonitorProfileNameFromDB
 *  @brief   get the monitor profile filename from the device profile
 *           database
 *
 *  @deprecated because the functionality shall be integrated into a general framwork
 *
 *  @param      display_name           the display string
 *  @param      allocateFunc  function used to allocate memory for the string
 *  @return                   the profiles filename (if localy available)
 *
 *  @version Oyranos: 0.1.8
 *  @since   2005/00/00 (Oyranos: 0.1.x)
 *  @date    2008/10/24
 */
char *   oyGetMonitorProfileNameFromDB(const char        * display_name,
                                       oyAlloc_f           allocateFunc )
{
  int error = !display_name || !display_name[0];
  oyOptions_s * options = 0;
  oyConfig_s * device = 0;
  oyProfile_s * p = 0;
  char * profile_name = 0;
  char * text = 0;

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  if(error > 0)
  {
    display_name = getenv( "DISPLAY" );
    error = !display_name;
  }

  if(error > 0)
  {
    WARNc_S( "No display_name argument provided. Give up." );
    return 0;
  }

  /** 1. get monitor device */
  if(error <= 0)
    error = oyDeviceGet( OY_TYPE_STD, "monitor", display_name,
                             options, &device );

  oyOptions_Release( &options );

  /** 2. check for success of device detection */
  error = !device;
  if(error)
  {
    WARNc2_S( "%s: \"%s\"", _("Could not open device"), display_name );
    return 0;
  }

  if(error <= 0)
    error = oyDeviceProfileFromDB( device, &profile_name, 0 );

  if(error <= 0)
  {
    if(profile_name && oyStrrchr_( profile_name, OY_SLASH_C ) != 0)
      text = oyStringCopy_( oyStrrchr_( profile_name, OY_SLASH_C ) + 1,
                            allocateFunc );
    else
      text = oyStringCopy_( profile_name, allocateFunc );

  }

  if(profile_name)
    oyFree_m_( profile_name );
  oyProfile_Release( &p );
  oyConfig_Release( &device );

  return text;
}

/** Function: oySetMonitorProfile
 *  @brief   set the monitor profile by filename
 *
 *  @deprecated because the functionality shall be integrated into a general framwork
 *
 *  The profile_name argument does two things. Set to zero the function solely
 *  unsets the graphic card luts and the server stored profile. So pretty all
 *  server side information should go away. \n
 *  With a profile name given the function will lookup the monitor in the 
 *  Oyranos device database and stores the given profile there. \n
 *  To sum up, to set a new profile please call the following sequence:
 *  @verbatim
    // store new settings in the Oyranos data base
    oySetMonitorProfile( display_name, profile_name );
    // remove the server entries
    oySetMonitorProfile( display_name, 0 );
    // update the window server from the newly Oyranos data base settings
    oyActivateMonitorProfiles( display_name );
    @endverbatim
 *
 *  @param      display_name  the display string
 *  @param      profile_name  the file to use as monitor profile or 0 to unset
 *  @return                   error
 *
 *  @version Oyranos: 0.1.8
 *  @since   2005/00/00 (Oyranos: 0.1.x)
 *  @date    2009/01/28
 */
int      oySetMonitorProfile         ( const char        * display_name,
                                       const char        * profile_name )
{
  int error = !display_name || !display_name[0];
  oyOptions_s * options = 0;
  oyConfig_s * device = 0;

  if(error > 0)
  {
    WARNc1_S( "No display_name argument provided. Give up. %s",
              oyNoEmptyString_m_(profile_name) );
    return error;
  }

  /** 1. obtain detailed and expensive device informations */
  /** 1.1 add "properties" call to module arguments */
  error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/command",
                                 "properties", OY_CREATE_NEW );

  /** 1.2 get monitor device */
  if(error <= 0)
    error = oyDeviceGet( OY_TYPE_STD, "monitor", display_name,
                             options, &device );

  oyOptions_Release( &options );

  /** 2. check for success of device detection */
  error = !device;
  if(error)
  {
    WARNc2_S( "%s: \"%s\"", _("Could not open device"), display_name );
    return error;
  }

  if(profile_name)
    error = oyDeviceSetProfile( device, profile_name );
  else
    error = oyDeviceUnset( device );

  oyConfig_Release( &device );

  return error;
}


/** Function: oyActivateMonitorProfiles
 *  @brief   activate the monitor using the stored configuration
 *
 *  @deprecated because the functionality shall be integrated into a general framwork
 *
 *  Activate in case the appropriate profile is not yet setup in the server. \n
 *  To deactivate a profile in the server call 
 *  oySetMonitorProfile( device_name, 0 ).
 *
 *  @see oySetMonitorProfile for permanently configuring a monitor
 *
 *  @param   display_name              the device string
 *  @return                            error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2005/00/00 (Oyranos: 0.1.x)
 *  @date    2009/01/28
 */
int      oyActivateMonitorProfiles   ( const char        * display_name )
{
  int error = !display_name || !display_name[0];
  oyOptions_s * options = 0;
  oyConfig_s * device = 0;
  oyConfigs_s * devices = 0;
  const char * device_class = "monitor";
  int i, n;

  if(error > 0)
  {
    WARNc_S( "No device_name argument provided. Give up." );
    return error;
  }

  {
    /* 1. set a general request */
    error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/command",
                                   "list", OY_CREATE_NEW );
    /* we want a fuzzy look at our display, not as narrow as "device_name"*/
    error = oyOptions_SetFromText( &options, "//" OY_TYPE_STD "/config/display_name",
                                   display_name, OY_CREATE_NEW );
    error = oyConfigs_FromDeviceClass ( 0, device_class, options,
                                            &devices, 0 );

    n = oyConfigs_Count( devices );
    for(i = 0; i < n; ++i)
    {
      device = oyConfigs_Get( devices, i );
      oyDeviceSetup( device );
      oyConfig_Release( &device );
    }

    oyConfigs_Release( &devices );
    oyOptions_Release( &options );
  }

  return error;
}


/** @} *//* monitor_api */

extern oyStructList_s_ * oy_profile_s_file_cache_;

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
