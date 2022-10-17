/** @file oyranos_core.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2004-2019 (C) Kai-Uwe Behrmann
 *
 *  @brief    public Oyranos API's
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2004/11/25
 */


#include "oyranos_core.h" /* define HAVE_POSIX */

#if defined(__ANDROID__)
#include <android/log.h>
#endif

#include <sys/stat.h>
#ifdef HAVE_POSIX
#include <unistd.h>
#endif

#include "oyranos_config_internal.h" /* define HAVE_LANGINFO_H */
#if defined(HAVE_LANGINFO_H) && !defined(__ANDROID__)
#define USE_LANGINFO_H 1
#include <langinfo.h>
#endif

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "oyranos_debug.h"
#include "oyranos_helper.h"
#include "oyranos_internal.h"
#include "oyranos_icc.h"
#include "oyranos_io.h"
#include "oyranos_sentinel.h"
#include "oyranos_string.h"
#include "oyranos_texts.h"

#include "oyStruct_s.h"
#include "oyObject_s_.h"
#include "oyName_s_.h"

#include "oyArray2d_s_.h"
#include "oyRectangle_s_.h"
#include "oyOption_s_.h"

static oyStruct_RegisterStaticMessageFunc_f * oy_static_msg_funcs_ = NULL;
static oyStruct_RegisterStaticFreeFunc_f * oy_static_free_funcs_ = NULL;
static int oy_msg_func_n_ = 0;
char * oy_object_show_text_ = NULL;
char * oy_version_string_[] = {0,0,0,0,0,0};



/** Function oyStruct_RegisterStaticMessageFunc
 *  @memberof oyStruct_s
 *  @brief   register a function for verbosity
 *
 *  Hint: for custom object types, you should use a type of above oyOBJECT_MAX
 *
 *  @param[in]     type                the object oyOBJECT_e type id;
 *                                     keep lower than ::oyOBJECT_MAX_CUSTOM,
 *                                     as this number; oyStruct_s derived
 *                                     objects shall be lower than ::oyOBJECT_MAX_STRUCT
 *                                     will allocate a array of that length
 *  @param[in]     f                   the object string function;
 *                                     A custom type should consider to add
 *                                     - oyNAME_NICK : one word object name,
 *                                         e.g. "myIdCustom_s"
 *                                     - oyNAME_JSON+2 : ' ' space separated
 *                                         list of child object id's from
 *                                         oyObject_s::id_
 *                                     Internal defined types below oyOBJECT_MAX
 *                                     handle type enums by internal functions.
 *  @param[in]     object_type_init_var the object type initialisation variable address
 *  @return                            0 - success; >= 1 - error
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/10/14
 *  @since   2011/01/14
 */
int oyStruct_RegisterStaticMessageFunc (
                                       int                 type,
                                       oyStruct_RegisterStaticMessageFunc_f msg,
                                       oyStruct_RegisterStaticFreeFunc_f free_func )
{
  int error = 0;
  if((int)type >= oy_msg_func_n_)
  {
    int n = oy_msg_func_n_, size = sizeof(oyStruct_RegisterStaticMessageFunc_f);
    oyStruct_RegisterStaticMessageFunc_f * tmp = NULL;
    oyStruct_RegisterStaticFreeFunc_f * tmp_free = NULL;

    if(oy_msg_func_n_)
      n = type * 2;
    else
      n = (int) oyOBJECT_MAX;


    tmp = oyAllocateFunc_(size * n);
    tmp_free = oyAllocateFunc_(size * n);
    if(tmp && tmp_free)
    {
      memset( tmp, 0, size * n );
      memset( tmp_free, 0, size * n );
    }

    if(tmp && tmp_free && oy_msg_func_n_)
    {
      memcpy( tmp, oy_static_msg_funcs_, size * oy_msg_func_n_ );
      memcpy( tmp_free, oy_static_free_funcs_, size * oy_msg_func_n_ );
    }
    else if(!tmp || !tmp_free)
    {
      error = 1;
      if(tmp) oyDeAllocateFunc_(tmp);
      if(tmp_free) oyDeAllocateFunc_(tmp_free);
      return error;
    }

    if(oy_static_msg_funcs_)
      oyDeAllocateFunc_(oy_static_msg_funcs_);
    if(oy_static_free_funcs_)
      oyDeAllocateFunc_(oy_static_free_funcs_);
    oy_static_msg_funcs_ = tmp;
    oy_static_free_funcs_ = tmp_free;
    tmp = NULL;
    tmp_free = NULL;
    oy_msg_func_n_ = n;
  }

  oy_static_msg_funcs_[type] = msg;
  oy_static_free_funcs_[type] = free_func;

  return error;
}

void               oyLibCoreRelease  ( )
{
  int i;

  for(i = 0; i < 6; ++i)
    if(oy_version_string_[i])
    {
      oyFree_m_(oy_version_string_[i]);
      oy_version_string_[i] = NULL;
    }

  if(oy_static_msg_funcs_)
    oyDeAllocateFunc_(oy_static_msg_funcs_);
  oy_static_msg_funcs_ = NULL;

  for( i = 0; i < oy_msg_func_n_; ++i )
  {
    oyStruct_RegisterStaticFreeFunc_f f = oy_static_free_funcs_[i];
    if(f)
      f();
  }

  if(oy_static_free_funcs_)
    oyDeAllocateFunc_(oy_static_free_funcs_);
  oy_static_free_funcs_ = NULL;
  oy_msg_func_n_ = 0;

  if(oy_object_show_text_)
    oyFree_m_( oy_object_show_text_ );

  oyI18Nreset_();

  oyjlLibRelease();
}

int get_oy_msg_func_n_() { return oy_msg_func_n_; }

/** Function oyStruct_GetInfo
 *  @memberof oyStruct_s
 *  @brief   get a additional string from a object
 *
 *  The content can be provided by object authors by using
 *  oyStruct_RegisterStaticMessageFunc() typical at the first time of object
 *  creation.
 *
 *  Note: this function is a very low level version of oyStruct_GetText().
 *
 *  @param[in]     context_object      the object to get informations about
 *  @param[in]     type                request a suitable format
 *  @param[in]     flags               0x01 - skip trivial struct type name
 *  @return                            a string or NULL; The pointer might
 *                                     become invalid after further using the
 *                                     object pointed to by context.
 *  @version Oyranos: 0.9.6
 *  @date    2016/04/18
 *  @since   2011/01/15 (Oyranos: 0.2.1)
 */
const char *   oyStruct_GetInfo      ( oyPointer           context_object,
                                       oyNAME_e            type,
                                       int                 flags )
{
  const char * text = NULL;
  oyStruct_s * c = (oyStruct_s*) context_object;
  oyStruct_RegisterStaticMessageFunc_f f;

  if(!c)
    return NULL;

  if(oy_static_msg_funcs_ != NULL)
  {
    int n = oy_msg_func_n_;

    if(!oy_msg_func_n_)
      n = (int) oyOBJECT_MAX;

    if((int)c->type_ > n)
    {
      fprintf(stderr, "range check for oyStruct_s::type_ failed: %d > %d\n",
              (int)c->type_, n);
      return NULL;
    }

    f = oy_static_msg_funcs_[c->type_];
    if(f)
      text = f( c, type, flags );
  }

  if(text == NULL && !(flags & 0x01) )
    text = oyStructTypeToText( c->type_ );

  return text;
}


/** Function oyStructTypeToText
 *  @memberof oyStruct_s
 *  @brief   Objects type to small string
 *
 *  Give a basic description of inbuild object types.
 *
 *  @version Oyranos: 0.5.0
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 *  @date    2012/09/05
 */
const char *     oyStructTypeToText  ( oyOBJECT_e          type )
{
  const char * text = "unknown";
  switch(type) {
    case oyOBJECT_NONE: text = "Zero - none"; break;
    case oyOBJECT_OBJECT_S: text = "oyObject_s"; break;
    case oyOBJECT_MONITOR_S: text = "oyMonitor_s"; break;
    case oyOBJECT_NAMED_COLOR_S: text = "oyNamedColor_s"; break;
    case oyOBJECT_NAMED_COLORS_S: text = "oyNamedColors_s"; break;
    case oyOBJECT_PROFILE_S: text = "oyProfile_s"; break;
    case oyOBJECT_PROFILE_TAG_S: text = "oyProfileTag_s"; break;
    case oyOBJECT_PROFILES_S: text = "oyProfiles_s"; break;
    case oyOBJECT_OPTION_S: text = "oyOption_s"; break;
    case oyOBJECT_OPTIONS_S: text = "oyOptions_s"; break;
    case oyOBJECT_RECTANGLE_S: text = "oyRectangle_s"; break;
    case oyOBJECT_IMAGE_S: text = "oyImage_s"; break;
    case oyOBJECT_ARRAY2D_S: text = "oyArray2d_s"; break;
    case oyOBJECT_FILTER_CORE_S: text = "oyFilterCore_s"; break;
    case oyOBJECT_FILTER_CORES_S: text = "oyFilterCores_s"; break;
    case oyOBJECT_CONVERSION_S: text = "oyConversion_s"; break;
    case oyOBJECT_CONNECTOR_S: text = "oyConnector_s"; break;
    case oyOBJECT_CONNECTOR_IMAGING_S: text = "oyConnectorImaging_s"; break;
    case oyOBJECT_CONNECTOR_MAX_S: text = "CONNECTOR MAX - not defined"; break;
    case oyOBJECT_FILTER_PLUG_S: text = "oyFilterPlug_s"; break;
    case oyOBJECT_FILTER_PLUGS_S: text = "oyFilterPlugs_s"; break;
    case oyOBJECT_FILTER_SOCKET_S: text = "oyFilterSocket_s"; break;
    case oyOBJECT_FILTER_NODE_S: text = "oyFilterNode_s"; break;
    case oyOBJECT_FILTER_NODES_S: text = "oyFilterNodes_s"; break;
    case oyOBJECT_FILTER_GRAPH_S: text = "oyFilterGraph_s"; break;
    case oyOBJECT_PIXEL_ACCESS_S: text = "oyPixelAccess_s"; break;
    case oyOBJECT_CMM_HANDLE_S: text = "oyCMMhandle_s"; break;
    case oyOBJECT_POINTER_S: text = "oyPointer_s"; break;
    case oyOBJECT_CMM_INFO_S: text = "oyCMMinfo_s"; break;
    case oyOBJECT_CMM_API_S: text = "oyCMMapi_s generic"; break;
    case oyOBJECT_CMM_APIS_S: text = "oyCMMapis_s generic"; break;
    case oyOBJECT_CMM_API1_S: text = "oyCMMapi1_s old CMM"; break;
    case oyOBJECT_CMM_API2_S: text = "oyCMMapi2_s Monitors"; break;
    case oyOBJECT_CMM_API3_S: text = "oyCMMapi3_s Profile tags"; break;
    case oyOBJECT_CMM_API4_S: text = "oyCMMapi4_s Filter"; break;
    case oyOBJECT_CMM_API5_S: text = "oyCMMapi5_s MetaFilter"; break;
    case oyOBJECT_CMM_API6_S: text = "oyCMMapi6_s Context convertor"; break;
    case oyOBJECT_CMM_API7_S: text = "oyCMMapi7_s Filter run"; break;
    case oyOBJECT_CMM_API8_S: text = "oyCMMapi8_s Devices"; break;
    case oyOBJECT_CMM_API9_S: text = "oyCMMapi9_s Graph Policies"; break;
    case oyOBJECT_CMM_API10_S: text = "oyCMMapi10_s generic command"; break;
    case oyOBJECT_CMM_DATA_TYPES_S: text = "oyCMMDataTypes_s Data types"; break;
    case oyOBJECT_CMM_API_FILTER_S: text="oyCMMapiFilter_s Filter";break;
    case oyOBJECT_CMM_API_FILTERS_S: text="oyCMMapiFilters_s Filter list";break;
    case oyOBJECT_CMM_UI_S: text = "oyCMMui_s UI part"; break;
    case oyOBJECT_CMM_OBJECT_TYPE_S: text = "oyCMMobjectType_s Custom object handler"; break;
    case oyOBJECT_CMM_API_MAX: text = "CMM_API MAX - not defined"; break;
    case oyOBJECT_ICON_S: text = "oyIcon_s"; break;
    case oyOBJECT_MODULE_S: text = "oyModule_s"; break;
    case oyOBJECT_EXTERNFUNC_S: text = "oyExternFunc_s"; break;
    case oyOBJECT_NAME_S: text = "oyName_s"; break;
    case oyOBJECT_COMP_S_: text = "oyComp_s_"; break;
    case oyOBJECT_FILE_LIST_S_: text = "oyFileList_s_"; break;
    case oyOBJECT_HASH_S: text = "oyHash_s"; break;
    case oyOBJECT_STRUCT_LIST_S: text = "oyStructList_s"; break;
    case oyOBJECT_BLOB_S: text = "oyBlob_s"; break;
    case oyOBJECT_CONFIG_S: text = "oyConfig_s"; break;
    case oyOBJECT_CONFIGS_S: text = "oyConfigs_s"; break;
    case oyOBJECT_UI_HANDLER_S: text = "oyUiHandler_s"; break;
    case oyOBJECT_FORMS_ARGS_S: text = "oyFormsArgs_s"; break;
    case oyOBJECT_CALLBACK_S: text = "oyCallback_s"; break;
    case oyOBJECT_OBSERVER_S: text = "oyObserver_s"; break;
    case oyOBJECT_CONF_DOMAIN_S: text = "oyConfDomain_s"; break;
    case oyOBJECT_INFO_STATIC_S: text = "oyObjectInfoStatic_s"; break;
    case oyOBJECT_LIST_S: text = "(oyList_s)"; break;
    case oyOBJECT_LIS_S: text = "(oyLis_s)"; break;
    case oyOBJECT_JOB_S: text = "(oyJob_s)"; break;
    case oyOBJECT_MONITOR_HOOKS_S: text = "(oyMonitorHooks_s)"; break;
    case oyOBJECT_MONITOR_HOOKS2_S: text = "(oyMonitorHooks2_s)"; break;
    case oyOBJECT_MONITOR_HOOKS3_S: text = "(oyMonitorHooks3_s)"; break;
    case oyOBJECT_MAX: text = "MAX - not defined"; break;
    default: { icUInt32Number i = oyValueUInt32(type);
               static char t[8];
               memcpy (t,(char*)&i, 4);
               t[4] = 0;
               text = &t[0];
               break;
             }
  }
  return text;
}


/** Function oyObject_GetId
 *  @ingroup objects_generic
 *  @brief   get the identification number of a object 
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/04/06
 *  @since   2008/07/10 (Oyranos: 0.1.8)
 */
int            oyObject_GetId        ( oyObject_s          object )
{
  struct oyObject_s_* obj = (struct oyObject_s_*)object;
  oyStruct_s * st = NULL;

  if(obj)
    st = obj->parent_;

  if(st && oy_debug_objects >= 0)
  {
    const char * t = getenv(OY_DEBUG_OBJECTS);
    int id_ = -1;
    if(t)
      id_ = atoi(t);
    else
      id_ = oy_debug_objects;

    if((id_ >= 0 && obj->id_ == id_) ||
       (t && strstr(oyStructTypeToText(st->type_), t) != 0) ||
       id_ == 1)
    {
      if(obj->ref_ >= 0)
        fputs( oyObject_Show( obj ), stderr );
      fflush( stderr );
    }
  } else if(obj && oy_debug_objects == obj->id_)
  {
    fprintf( stderr, OY_DBG_FORMAT_ "id[%d] refs: %d\n", OY_DBG_ARGS_, obj->id_, obj->ref_ );
    fflush( stderr );
  }

  if(obj)
    return obj->id_;
  else
    return -1;
}

/** Function oyObject_Show
 *  @ingroup  objects_generic
 *  @brief   Print object informations
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/04/06
 *  @since   2016/04/06 (Oyranos: 0.9.6)
 */
OYAPI const char * OYEXPORT  oyObject_Show (
                                       oyObject_s          object )
{
  struct oyObject_s_* obj = (struct oyObject_s_*)object;
  oyStruct_s * st = NULL;
  static int t_len = 1024;

  if(obj)
    st = obj->parent_;

  if(st)
  {
    if(!oy_object_show_text_)
      oy_object_show_text_ = malloc(t_len);

    if(oy_object_show_text_)
    {
      const char * tmp = oyStruct_GetInfo(st,oyNAME_NAME,0);
      int silen = tmp?strlen(tmp):0, len = 64 + silen;
      if(len > t_len + 1)
      {
        free(oy_object_show_text_);
        oy_object_show_text_ = malloc(len*2);
      }
      oy_object_show_text_[0] = 0;
      snprintf( oy_object_show_text_, 64, "\"%s", tmp );
      if(silen > 64) sprintf( &oy_object_show_text_[strlen(oy_object_show_text_)], " ..." );
      sprintf( &oy_object_show_text_[strlen(oy_object_show_text_)], "\"[%d] refs: %d", obj->id_, obj->ref_ );
      switch(st->type_)
      {
      case oyOBJECT_ARRAY2D_S:
        {
          oyArray2d_s_ * s = (oyArray2d_s_ *)st;
          sprintf( &oy_object_show_text_[strlen(oy_object_show_text_)], " %dx%d data_type: %d",
                   s->width, s->height, s->t);
          break;
        }
      case oyOBJECT_RECTANGLE_S:
        {
          oyRectangle_s_ * s = (oyRectangle_s_ *)st;
          sprintf( &oy_object_show_text_[strlen(oy_object_show_text_)], " %gx%g+%g+%g",
                   s->width, s->height, s->x, s->y);
          break;
        }
      case oyOBJECT_OPTION_S:
        {
          oyOption_s_ * s = (oyOption_s_ *)st;
          oyValue_u * v = s->value;
          const char * t = s->value_type==oyVAL_STRING?v->string:"";
          sprintf( &oy_object_show_text_[strlen(oy_object_show_text_)], " %s%s%s",
                   t?"\"":"", oyNoEmptyString_m_(t), t?"\"":"" );
          break;
        }
      default:
          break;
      }
      sprintf( &oy_object_show_text_[strlen(oy_object_show_text_)], "\n");
    }
  }

  if(oy_object_show_text_ && oy_object_show_text_[0])
    return oy_object_show_text_;
  else
    return "----";
}



/** Function oyMessageFormat
 *  @brief   default function to form a message string
 *
 *  This default message function is used as a message formatter.
 *  The resulting string can be placed anywhere, e.g. in a GUI.
 *
 *  @see the oyMessageFunc() needs just to replace the fprintf with your 
 *  favourite GUI call.
 *
 *  @version Oyranos: 0.2.1
 *  @since   2008/04/03 (Oyranos: 0.2.1)
 *  @date    2011/01/15
 */
int                oyMessageFormat   ( char             ** message_text,
                                       int                 code,
                                       const void        * context_object,
                                       const char        * string )
{
  char * text = 0, * t = 0;
  int i;
  const char * type_name = "";
  int id = -1;
#ifdef HAVE_POSIX
  pid_t pid = 0;
#else
  int pid = 0;
#endif
  FILE * fp = 0;
  const char * id_text = 0;
  char * id_text_tmp = 0;
  oyStruct_s * c = (oyStruct_s*) context_object;

  if(code == oyMSG_DBG && !oy_debug)
    return 0;

  if(c && oyOBJECT_NONE < c->type_)
  {
    type_name = oyStructTypeToText( c->type_ );
    if(c->type_ < oyOBJECT_MAX_STRUCT)
      id = oyObject_GetId( c->oy_ );
    id_text = oyStruct_GetInfo( (oyStruct_s*)c, oyNAME_NAME, 0x01 );
    if(id_text)
      id_text_tmp = strdup(id_text);
    id_text = id_text_tmp;
  }

  oyAllocHelper_m_(text, char, 256, malloc, if(id_text_tmp) free(id_text_tmp); return 1);

# define MAX_LEVEL 20
  if(level_PROG < 0)
    level_PROG = 0;
  if(level_PROG > MAX_LEVEL)
    level_PROG = MAX_LEVEL;
  for (i = 0; i < level_PROG; i++)
    oySprintf_( &text[oyStrlen_(text)], " ");

  STRING_ADD( t, text );

  text[0] = 0;

  switch(code)
  {
    case oyMSG_WARN:
         STRING_ADD( t, _("WARNING") );
         break;
    case oyMSG_ERROR:
         STRING_ADD( t, _("!!! ERROR"));
         break;
    case oyjlMSG_INFO:
         oyStringAddPrintf( &t, 0,0, "Oyjl%s: ", oyjlTermColor(oyjlGREEN,_("Info")));
         break;
    case oyjlMSG_CLIENT_CANCELED:
         oyStringAddPrintf( &t, 0,0, "Oyjl%s: ", oyjlTermColor(oyjlBLUE,_("Client Canceled")));
         break;
    case oyjlMSG_INSUFFICIENT_DATA:
         oyStringAddPrintf( &t, 0,0, "Oyjl%s: ", oyjlTermColor(oyjlRED,_("Insufficient data")));
         break;
    case oyjlMSG_ERROR:
         oyStringAddPrintf( &t, 0,0, "Oyjl%s: ", oyjlTermColor(oyjlRED,_("Usage Error")));
         break;
    case oyjlMSG_PROGRAM_ERROR:
         oyStringAddPrintf( &t, 0,0, "Oyjl%s: ", oyjlTermColor(oyjlRED,_("Program Error")));
         break;
    case oyjlMSG_SECURITY_ALERT:
         oyStringAddPrintf( &t, 0,0, "Oyjl%s: ", oyjlTermColor(oyjlRED,_("Security Alert")));
         break;
  }

  /* reduce output for non core messages */
  if( id > 0 || (oyMSG_ERROR <= code && code <= 399) )
  {
    oyStringAddPrintf_( &t, oyAllocateFunc_,oyDeAllocateFunc_,
                        " %03f: ", DBG_UHR_);
    oyStringAddPrintf_( &t, oyAllocateFunc_,oyDeAllocateFunc_,
                        "%s[%d]%s%s%s ", type_name, id,
             id_text ? "=\"" : "", id_text ? id_text : "", id_text ? "\"" : "");
  }

  STRING_ADD( t, string );

  if(oy_backtrace)
  {
#   define TMP_FILE "/tmp/oyranos_gdb_temp." OYRANOS_VERSION_NAME "txt"
#ifdef HAVE_POSIX
    pid = (int)getpid();
#endif
    fp = oyjlFopen( TMP_FILE, "w" );

    if(fp)
    {
      int r OY_UNUSED;
      fprintf(fp, "attach %d\n", pid);
      fprintf(fp, "thread 1\nbacktrace\n"/*thread 2\nbacktrace\nthread 3\nbacktrace\n*/"detach" );
      fclose(fp);
      fprintf( stderr, "GDB output:\n" );
      r = system("gdb -batch -x " TMP_FILE);
    } else
      fprintf( stderr, "could not open " TMP_FILE "\n" );
  }

  free( text ); text = NULL;
  if(id_text_tmp){ free(id_text_tmp); } id_text_tmp = 0;

  *message_text = t;

  return 0;
}

/** Function oyMessageFunc
 *  @brief   default message function to console
 *
 *  The default message function is used as a message printer to the console 
 *  from library start.
 *
 *  @param         code                a message code understood be your message
 *                                     handler or oyMSG_e
 *  @param         context_object      a oyStruct_s is expected from Oyranos
 *  @param         format              the text format string for following args
 *  @param         ...                 the variable args fitting to format
 *  @return                            0 - success; 1 - error
 *
 *  @version Oyranos: 0.3.0
 *  @since   2008/04/03 (Oyranos: 0.1.8)
 *  @date    2009/07/20
 */
int oyMessageFunc( int code, const void * context_object, const char * format, ... )
{
  char * text = 0, * msg = 0;
  const char * message = NULL;
  int error = 0;
  va_list list;
  size_t sz = 0;
  int len = 0,
      l;
  oyStruct_s * c = (oyStruct_s*) context_object;

  va_start( list, format);
  len = vsnprintf( text, sz, format, list);
  va_end  ( list );

  {
    oyAllocHelper_m_(text, char, len + 1, oyAllocateFunc_, return 1);
    va_start( list, format);
    l = vsnprintf( text, len+1, format, list);
    if(l != len)
      fprintf(stderr, "vsnprintf lengths differ: %d %d\n", l,len );
    va_end  ( list );
  }

  error = oyMessageFormat( &msg, code, c, text );

  if(msg)
    message = msg;
  else if(error)
    message = format;

  if(message)
  {
    fprintf( stderr, "%s\n", message );
#if defined(__ANDROID__)
    __android_log_print(ANDROID_LOG_INFO, "oyMessageFunc", "%s", message );
#endif
  }

  oyDeAllocateFunc_( text ); text = 0;
  if(msg){ oyDeAllocateFunc_( msg ); } msg = 0;

  return error;
}

oyMessage_f     oyMessageFunc_p = oyMessageFunc;

void oyShowMessage(int type OY_UNUSED, const char * show_text, int show_gui)
{
        if(show_text)
        {
          if(show_gui)
          {
            int r OY_UNUSED;
            char * app = NULL,
                 * txt = NULL;
#if defined(__APPLE__)
            {
              /* use a simple apple script dialog */
              oyStringAddPrintf( &txt, 0,0,
                "echo 'display dialog \"%s\"' > $TMPDIR/a.scpt ; osacompile -o $TMPDIR/a.app $TMPDIR/a.scpt ; open $TMPDIR/a.app; sleep 1; #rm -rv $TMPDIR/a.app $TMPDIR/a.scpt", show_text );
              printf("%s\n", txt );
              app = oyStringCopy("osacompile", 0);
            }
#endif
            if(!app && (app = oyFindApplication( "notify-send" )) != NULL)
            {
              STRING_ADD( txt, "notify-send -i 'dialog-information' 'Oyranos' \"");
              STRING_ADD( txt, show_text );
              STRING_ADD( txt, "\"" );
              printf("%s\n", txt );
            }
            if(!app && getenv("KDE_FULL_SESSION") && (app = oyFindApplication( "kdialog" )) != NULL)
            {
              STRING_ADD( txt, "kdialog --passivepopup \"");
              STRING_ADD( txt, show_text );
              STRING_ADD( txt, "\" 5" );
            }
            if(!app && (app = oyFindApplication( "zenity" )) != NULL)
            {
              STRING_ADD( txt, "zenity --warning --text \"");
              STRING_ADD( txt, show_text );
              STRING_ADD( txt, "\"" );
              printf("%s\n", txt );
            }
            if(!app && (app = oyFindApplication( "dialog" )) != NULL)
            {
              STRING_ADD( txt, "xterm -e sh -c \"dialog --msgbox \\\"");
              STRING_ADD( txt, show_text );
              STRING_ADD( txt, "\\\" 5 70\"" );
              printf("%s\n", txt );
            }
            if(!app && (app = oyFindApplication( "xterm" )) != NULL)
            {
              STRING_ADD( txt, "xterm -e sh -c \"echo \\\"");
              STRING_ADD( txt, show_text );
              STRING_ADD( txt, "\\\"; sleep 10\"" );
              printf("%s\n", txt );
            }
            r = system(txt);
            oyFree_m_( txt );
            oyFree_m_( app );
          }

          fprintf(stderr, "%s\n", show_text );
        }
}

int oy_level_prog = 0;

int oyGuiMessageFunc( int code, const void * c, const char * format, ... )
{
  oyStruct_s * context = (oyStruct_s*) c;
  char* text = 0, *pos = 0;
  va_list list;
  const char * type_name = "";
  int id = -1, i;
  int pid = 0;
  FILE * fp = 0;

  if(code == oyMSG_DBG && !oy_debug)
    return 0;


  if(context && oyOBJECT_NONE < context->type_)
  {
    type_name = oyStructTypeToText( context->type_ );
    id = oyObject_GetId( context->oy_ );
  }

  text = (char*)calloc(sizeof(char), 4096);
  if(!text)
  {
    if(code == oyMSG_ERROR) oyShowMessage(code,format,1);
    else fprintf( stderr, "%d %s\n", code, format );
    return 1;
  }
  text[0] = 0;

  if(format && strlen(format) > 6)
  {
    if(strncasecmp("Start:", format, 6 ) == 0)
      ++oy_level_prog;
    if(strncasecmp("  End:", format, 6 ) == 0)
      --oy_level_prog;
  }

# define MAX_LEVEL 20
  if(oy_level_prog < 0)
    oy_level_prog = 0;
  if(oy_level_prog > MAX_LEVEL)
    oy_level_prog = MAX_LEVEL;
  for (i = 0; i < oy_level_prog; i++)
    sprintf( &text[strlen(text)], " ");


  if(type_name && type_name[0])
    snprintf( &text[strlen(text)], 4096 - strlen(text), " %03f %s[%d] ", 
              (double)clock()/(double)CLOCKS_PER_SEC, type_name,id );
  else
    snprintf( &text[strlen(text)], 4096 - strlen(text), " " );

  va_start( list, format);
  vsnprintf( &text[strlen(text)], 4096 - strlen(text), format, list);
  va_end  ( list );


  pos = &text[strlen(text)];
  *pos = '\n';
  pos++;
  *pos = 0;

  if(code == oyMSG_ERROR)
    oyShowMessage(code,text,1);
  /* for debugging it is better to see messages on the console rather than
     getting lost during a crash */
  fprintf( stderr, "%d %s", code, text );

  free( text );

  if(oy_backtrace)
  {
#   define TMP_FILE "/tmp/oyranos_gdb_temp." OYRANOS_VERSION_NAME "txt"
#ifdef HAVE_POSIX
    pid = (int)getpid();
#endif
    fp = oyjlFopen( TMP_FILE, "w" );

    if(fp)
    {
      int r OY_UNUSED;
      fprintf(fp, "attach %d\n", pid);
      fprintf(fp, "thread 1\nbacktrace\n"/*thread 2\nbacktrace\nthread 3\nbacktrace\n*/"detach" );
      fclose(fp);
      fprintf( stderr, "GDB output:\n" );
      r = system("gdb -batch -x " TMP_FILE);
    } else
      fprintf( stderr, "could not open " TMP_FILE "\n" );
  }

  return 0;
}

/** Function oyMessageFuncSet
 *  @brief
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/04/03
 *  @since   2008/04/03 (Oyranos: 0.1.8)
 */
int            oyMessageFuncSet      ( oyMessage_f         message_func )
{
  if(message_func)
    oyMessageFunc_p = message_func;
  return 0;
}

/** Function oyObjectDebugMessage_
 *  @internal for debugging objects
 *  @brief   used with _Copy() macros
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/04/16
 *  @since   2016/04/16 (Oyranos: 0.9.6)
 */
void         oyObjectDebugMessage_   ( void              * object,
                                       const char        * function_name OY_UNUSED,
                                       const char        * struct_name )
{
  struct oyObject_s_* obj = (struct oyObject_s_*)object;
  oyStruct_s * st = NULL;

  if(obj)
    st = obj->parent_;

  if(st && oy_debug_objects >= 0)
  {
    const char * t = getenv(OY_DEBUG_OBJECTS);
    int id_ = -1;
    if(t)
      id_ = atoi(t);
    else
      id_ = oy_debug_objects;

    if((id_ >= 0 && obj->id_ == id_) ||
       (t && strstr(struct_name, t) != 0) ||
       id_ == 1)
    {
      if(oy_debug)
        fprintf(stderr, "copied %s[%d] refs: %d\n", struct_name, obj->id_, obj->ref_ );
      fflush( stderr );
    }
  }
}

/* --- internal API decoupling --- */



/** \addtogroup misc Miscellaneous
 *  @brief Miscellaneous stuff.

 *  @{ *//* misc */

/** \addtogroup i18n i18n
 *  @brief      Internationalisation helpers for translating UI texts

    Internationalisation is handled for Oyranos by a external library. 
    To sync locale settings with the Oyranos use the 
    standard C infaces. For debugging purposes the internal settings are 
    exported read only.

    A external application might want a different code set than Oyranos'
    default UTF-8. Thus a ::oy_domain_codeset variable is available and can be 
    set to match the applications needs.

 *  @{ *//* i18n */

/** @brief  get language code
 *
 *  @since Oyranos: version 0.1.8
 *  @date  26 november 2007 (API 0.1.8)
 */
const char *   oyLanguage            ( void )
{
  const char * text = 0;

  DBG_PROG_START
  oyInit_();

  text = oyLanguage_();

  oyExportEnd_();
  DBG_PROG_ENDE

  return text;
}

/** @brief  get country code
 *
 *  @since Oyranos: version 0.1.8
 *  @date  26 november 2007 (API 0.1.8)
 */
const char *   oyCountry             ( void )
{
  const char * text = 0;

  DBG_PROG_START
  oyInit_();

  text = oyCountry_();

  oyExportEnd_();
  DBG_PROG_ENDE

  return text;
}

/** @brief  get LANG code/variable
 *
 *  @since Oyranos: version 0.1.8
 *  @date  26 november 2007 (API 0.1.8)
 */
const char *   oyLang                ( void )
{
  const char * text = 0;

  DBG_PROG_START
  oyInit_();

  text = oyLang_();

  oyExportEnd_();
  DBG_PROG_ENDE

  return text;
}

/** @brief   reset i18n language and  country variables
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/05 (Oyranos: 0.1.10)
 *  @date    2009/01/05
 */
void           oyI18Nreset           ( void )
{
  DBG_PROG_START
  oyI18Nreset_();
  oyInit_();
  oyExportEnd_();
  DBG_PROG_ENDE
}

/** @} *//* i18n */

/** \addtogroup string Strings
 *  @brief String handling.
 *
 *  These functions prefere Oyranos oyAllocateFunc_ and oyDeAllocateFunc_ over
 *  plain malloc/free. They are wrappers around the libOyjlCore equivalents.
 *
 *  @{ *//* string */

#undef oyStringAddPrintf
/** @brief  Append a string and handle memory */
int                oyStringAddPrintf ( char             ** string,
                                       oyAlloc_f           alloc,
                                       oyDeAlloc_f         deAlloc,
                                       const char        * format,
                                                           ... )
{
  char * text_copy = NULL;
  char * text = 0;
  va_list list;
  int len;
  size_t sz = 0;

  void*(* allocate)(size_t size) = alloc?alloc: oyAllocateFunc_;
  void (* deAllocate)(void * data ) = deAlloc?deAlloc: oyDeAllocateFunc_;

  va_start( list, format);
  len = vsnprintf( text, sz, format, list );
  va_end  ( list );

  {
    oyAllocHelper_m_(text, char, len + 1, allocate, return 1);
    va_start( list, format);
    len = vsnprintf( text, len+1, format, list );
    va_end  ( list );
  }

  if(string && *string)
  {
    int l = strlen(*string);
    text_copy = allocate( len + l + 1 );
    strcpy( text_copy, *string );
    strcpy( &text_copy[l], text );

    deAllocate(*string);
    *string = text_copy;

    deAllocate(text);

  } else if(string)
    *string = text;
  else if(text)
    deAllocate( text );

  return 0;
}

#undef oyStringSplit
/** @brief  Create a array of strings */
char**             oyStringSplit     ( const char        * text,
                                       const char          delimiter,
                                       int               * count,
                                       oyAlloc_f           allocateFunc )
{
  return oyjlStringSplit( text, delimiter, count, allocateFunc?allocateFunc : oyAllocateFunc_ );
}
#undef oyStringListRelease
/** @brief  Release a array of strings */
void               oyStringListRelease(char            *** l,
                                       int                 size,
                                       oyDeAlloc_f         deallocFunc )
{
  oyjlStringListRelease( l, size, deallocFunc?deallocFunc : oyDeAllocateFunc_ );
}
#undef oyStringCopy
/** @brief  Copy with allocator */
char*              oyStringCopy      ( const char        * text,
                                       oyAlloc_f           allocateFunc )
{
  return oyjlStringCopy( text, allocateFunc?allocateFunc : oyAllocateFunc_ );
}
/** @} *//* string */

/** @brief  give the compiled in library version
 *
 *  @param[in]  type           0 - Oyranos API
 *                             1 - start month
 *                             2 - start year
 *                             3 - development last month
 *                             4 - development last year
 *
 *  @return                    OYRANOS_VERSION at library compile time
 */
int            oyVersion             ( int                 type )
{
  if(type == 1)
    return OYRANOS_START_MONTH;
  if(type == 2)
    return OYRANOS_START_YEAR;
  if(type == 3)
    return OYRANOS_DEVEL_MONTH;
  if(type == 4)
    return OYRANOS_DEVEL_YEAR;

  return OYRANOS_VERSION;
}

#include "config.log.h"
#include "oyranos_git_version.h"
#ifndef OY_GIT_VERSION
#define OY_GIT_VERSION ""
#endif
/** @brief  give the configure options for Oyranos
 *
 *  @param[in] type
                               - 1  OYRANOS_VERSION_NAME;
                               - 2  git master hash, deprecated
                               - 3  OYRANOS_CONFIG_DATE, deprecated
                               - 4  development period
 *  @return                    Oyranos configure output
 *
 *  @since     Oyranos: version 0.1.8
 *  @date      18 december 2007 (API 0.1.8)
 */
const char *       oyVersionString         ( int                 type )
{
  char * text = 0, * tmp = 0;
  char temp[24];
  char * git = OY_GIT_VERSION;

  if( 1 < type && type <= 4 &&
      oy_version_string_[type]  )
    return oy_version_string_[type];
  if( (type < 1 || 4 < type) &&
      oy_version_string_[5] )
    return oy_version_string_[5];

  if(type == 1)
    return oy_version_string_[1] = oyStringCopy_(OYRANOS_VERSION_NAME, oyAllocateFunc_);
  if(type == 2)
  {
    if(oy_debug) fprintf( stderr, "OY_GIT_VERSION: %s\n", OY_GIT_VERSION );
    if(git[0])
      return oy_version_string_[2] = oyStringCopy_(git, oyAllocateFunc_);
    else
      return 0;
  }
  if(type == 3)
    return oy_version_string_[3] = oyStringCopy_("", oyAllocateFunc_);

  if(type == 4)
  {
#ifdef USE_LANGINFO_H
    oyStringAdd_( &text, nl_langinfo(MON_1-1+oyVersion(1)),
                                            oyAllocateFunc_, oyDeAllocateFunc_);
#endif
    oySprintf_( temp, " %d - ", oyVersion(2) );
    oyStringAdd_( &text, temp, oyAllocateFunc_, oyDeAllocateFunc_);
#ifdef USE_LANGINFO_H
    oyStringAdd_( &text, nl_langinfo(MON_1-1+oyVersion(3)),
                                            oyAllocateFunc_, oyDeAllocateFunc_);
#endif
    oySprintf_( temp, " %d", oyVersion(4) );
    oyStringAdd_( &text, temp, oyAllocateFunc_, oyDeAllocateFunc_);

    tmp = oyStringCopy_( text , oyAllocateFunc_);
    oyDeAllocateFunc_(text);
    return oy_version_string_[4] = tmp;
  }

  return oy_version_string_[5] = oyStringCopy_("----", oyAllocateFunc_);
}

int                oyBigEndian       ( void )
{
  int big = 0;
  char testc[2] = {0,0};
  uint16_t *testu = (uint16_t*)testc;
  *testu = 1;
  big = testc[1];
  return big;
}


/** @brief convert to network byte order on little endian machines */
icUInt16Number oyValueUInt16 (icUInt16Number val)
{
  if(!oyBigEndian())
  {
  # define BYTES 2
  # define KORB  4
    unsigned char        *temp  = (unsigned char*) &val;
    unsigned char  korb[KORB];
    int i;
    for (i = 0; i < KORB ; i++ )
      korb[i] = (int) 0;  /* empty */

    {
    int klein = 0,
        gross = BYTES - 1;
    for (; klein < BYTES ; klein++ ) {
      korb[klein] = temp[gross--];
    }
    }

    {
    unsigned int *erg = (unsigned int*) &korb[0];

  # undef BYTES
  # undef KORB
    return (long)*erg;
    }
  } else
  return (long)val;
}

/** @brief convert to network byte order */
icUInt32Number oyValueUInt32 (icUInt32Number val)
{
  if(!oyBigEndian())
  {
    unsigned char        *temp = (unsigned char*) &val;

    unsigned char  uint32[4];

    uint32[0] = temp[3];
    uint32[1] = temp[2];
    uint32[2] = temp[1];
    uint32[3] = temp[0];

    {
    unsigned int *erg = (unsigned int*) &uint32[0];


    return (icUInt32Number) *erg;
    }
  } else
    return (icUInt32Number)val;
}


/** @brief convert to network byte order */
icS15Fixed16Number      oyValueInt32    (icS15Fixed16Number val)
{
  if(!oyBigEndian())
  {
    unsigned char        *temp = (unsigned char*) &val;

    unsigned char  uint32[4];

    uint32[0] = temp[3];
    uint32[1] = temp[2];
    uint32[2] = temp[1];
    uint32[3] = temp[0];

    {
    int *erg = (int*) &uint32[0];


    return (icS15Fixed16Number) *erg;
    }
  } else
    return (icS15Fixed16Number)val;
}

/** @brief convert to network byte order */
unsigned long oyValueUInt64 (icUInt64Number val)
{
  if(!oyBigEndian())
  {
    unsigned char        *temp  = (unsigned char*) &val;

    unsigned char  c8[8];
    int little = 0,
        big    = 8-1;

    for (; little < 8 ; little++ ) {
      c8[little] = temp[big--];
    }

    {
    unsigned long *erg = (unsigned long*) &c8[0];

    return (unsigned long)*erg;
    }
  } else
  return (long)val;
}

/** @brief swap byte order */
uint16_t oyByteSwapUInt16            ( uint16_t            v )
{
  uint8_t c[2], *vp = (uint8_t*)&v;

  c[0] = vp[1];
  c[1] = vp[0];
  vp = &c[0];
  v = *(uint16_t*)vp;

  return v;
}
/** @brief swap byte order */
uint32_t oyByteSwapUInt32            ( uint32_t            v )
{
  uint8_t c[4], *vp = (uint8_t*)&v;

  c[0] = vp[3];
  c[1] = vp[2];
  c[2] = vp[1];
  c[3] = vp[0];
  vp = &c[0];
  v = *(uint32_t*)vp;

  return v;
}

/** @} *//* misc */



/* deprecated function to reduce dlopen warnings after API break in 0.3.0; 
   these APIs are not deselected after dlopen */
int oyFilterMessageFunc() { return 1; }


