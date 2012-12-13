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

#include "oyranos_config_internal.h"

#include "oyCMMapis_s.h"
#include "oyCMMapi6_s.h"
#include "oyCMMapi8_s.h"
#include "oyCMMapi9_s.h"
#include "oyCMMui_s.h"
#include "oyConfig_s.h"
#include "oyConfigs_s.h"
#include "oyConfig_s_.h"
#include "oyConversion_s.h"
#include "oyObserver_s.h"
#include "oyOption_s.h"
#include "oyOptions_s.h"
#include "oyPointer_s.h"
#include "oyProfile_s_.h"
#include "oyStructList_s_.h"
#include "oyranos_devices.h"

#include "oyranos_types.h"
#include "oyranos_alpha.h"
#include "oyranos_colour.h"
#include "oyranos_alpha_internal.h"
#include "oyranos_object_internal.h"
#include "oyranos_generic_internal.h"
#include "oyranos_cmm.h"
#include "oyranos_elektra.h"
#include "oyranos_helper.h"
#include "oyranos_i18n.h"
#include "oyranos_icc.h"
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




/** @} *//* misc */



/** \addtogroup objects_image Image API
 *
 *  @{
 */


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



/** @} *//* misc */




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
/** @} *//* module_api */



/** \addtogroup module_api

 *  @{
 */

/** @} *//* module_api */


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

    r = (oyRectangle_s*) oyOption_GetStruct( o, oyOBJECT_RECTANGLE_S );

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

    r = (oyRectangle_s*) oyOption_GetStruct( o, oyOBJECT_RECTANGLE_S );

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

