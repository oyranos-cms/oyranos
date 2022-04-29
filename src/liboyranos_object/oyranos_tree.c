/** @file oyranos_tree.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2004-2018 (C) Kai-Uwe Behrmann
 *
 *  @brief    public Oyranos API's
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2016/04/14
 */

#include <stddef.h>  /* ptrdiff_t size_t */
#include <assert.h>

#include <oyranos_object_internal.h>

#include "oyObject_s.h"
#include "oyObject_s_.h"

#include "oyranos_threads.h"
#include "oyNamedColor_s_.h"
#include "oyNamedColors_s_.h"
#include "oyProfile_s_.h"
#include "oyProfileTag_s_.h"
#include "oyProfiles_s_.h"
#include "oyOption_s_.h"
#include "oyOptions_s_.h"
#include "oyImage_s_.h"
#include "oyArray2d_s_.h"
#include "oyFilterCore_s_.h"
#include "oyConversion_s_.h"
#include "oyFilterPlug_s_.h"
#include "oyFilterPlugs_s_.h"
#include "oyFilterSocket_s_.h"
#include "oyFilterNode_s_.h"
#include "oyFilterNodes_s_.h"
#include "oyFilterGraph_s_.h"
#include "oyPixelAccess_s_.h"
#include "oyCMMinfo_s_.h"
#include "oyranos_module_internal.h" /* oyCMMhandle_s */
#include "oyCMMapis_s_.h"
#include "oyCMMapi3_s_.h"
#include "oyCMMapi4_s_.h"
#include "oyCMMapi5_s_.h"
#include "oyCMMapi6_s_.h"
#include "oyCMMapi7_s_.h"
#include "oyCMMapi8_s_.h"
#include "oyCMMapi9_s_.h"
#include "oyCMMapi10_s_.h"
#include "oyCMMapiFilter_s_.h"
#include "oyCMMapiFilters_s_.h"
#include "oyCMMui_s_.h"
#include "oyHash_s_.h"
#include "oyStructList_s_.h"
#include "oyConfig_s_.h"
#include "oyConfigs_s_.h"

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif

#define PRINT_ID(id_)  ;//if(id_ == old_oy_debug_objects) fprintf(stderr, OY_DBG_FORMAT_ "ID: %d\n", OY_DBG_ARGS_, id_);
int old_oy_debug_objects = -1; /* be more silent */
static oyStruct_s *  oyStruct_FromId ( int                 id );

/* get a objects directly owned references */
int                oyStruct_GetChildren (
                                       oyStruct_s        * obj,
                                       oyStruct_s      *** list )
{
  #define oy_c_max 120
  static oyStruct_s * c[oy_c_max];
  int n = 0, i = 0;

  memset(c,0,sizeof(oyStruct_s *)*oy_c_max);
  if(list) *list = c;

#define CHECK_ASSIGN_OBJECT(struct_name) \
         if(s->oy_->struct_name) { \
                              c[n++] = (oyStruct_s*)s->oy_->struct_name; \
                              if(((oyStruct_s*)s->oy_->struct_name)->type_ > oyOBJECT_MAX_CUSTOM) \
                              { fprintf(stderr, OY_DBG_FORMAT_ "%s::%s failed\n", OY_DBG_ARGS_, oyStruct_GetText( (oyStruct_s*)s, oyNAME_DESCRIPTION, 0 ), #struct_name); \
                                goto gcassert; } \
                              if(oyStruct_GetText((oyStruct_s*)s->oy_->struct_name, oyNAME_DESCRIPTION, 2) == NULL && s->oy_) \
                              { \
                                char * t = NULL; oyjlStringAdd( &t, oyAllocateFunc_, oyDeAllocateFunc_, "parent: %s->oy_->" #struct_name, oyStructTypeToText(s->type_) );\
                                oyObject_SetName( s->oy_->struct_name->oy_, t, oyNAME_DESCRIPTION ); oyFree_m_(t); \
                              } \
         }
  if(obj->oy_)
  {
    oyStruct_s * s = (oyStruct_s*)obj;
    CHECK_ASSIGN_OBJECT( handles_ )
  }

#define CHECK_ASSIGN_STRUCT(struct_name) \
         if(s->struct_name) { c[n++] = (oyStruct_s*)s->struct_name; \
                              if(((oyStruct_s*)s->struct_name)->type_ > oyOBJECT_MAX) \
                              { fprintf(stderr, OY_DBG_FORMAT_ "%s::%s [%d] failed\n", OY_DBG_ARGS_, oyStruct_GetText( (oyStruct_s*)s, oyNAME_DESCRIPTION, 0 ), #struct_name, s->oy_?s->oy_->id_:-1); \
                                goto gcassert; } }
  switch(obj->type_)
  {
    case oyOBJECT_NONE:
    case oyOBJECT_OBJECT_S:  /* inherits oyStruct_s */
    case oyOBJECT_MONITOR_S: /* has no oyStruct_s children */
       break;
    case oyOBJECT_NAMED_COLOR_S:
       {
         oyNamedColor_s_ * s = (oyNamedColor_s_*)obj;
         CHECK_ASSIGN_STRUCT( profile_ )
       }
       break;
    case oyOBJECT_NAMED_COLORS_S:
       {
         oyNamedColors_s_ * s = (oyNamedColors_s_*)obj;
         CHECK_ASSIGN_STRUCT( list_ )
       }
       break;
    case oyOBJECT_PROFILE_S:
       {
         oyProfile_s_ * s = (oyProfile_s_*)obj;
         CHECK_ASSIGN_STRUCT( tags_ )
       }
       break;
    case oyOBJECT_PROFILE_TAG_S:
       break;
    case oyOBJECT_PROFILES_S:
       {
         oyProfiles_s_ * s = (oyProfiles_s_*)obj;
         CHECK_ASSIGN_STRUCT( list_ )
       }
       break;
    case oyOBJECT_OPTION_S:
       {
         oyOption_s_ * s = (oyOption_s_*)obj;
         if(s->value_type == oyVAL_STRUCT && s->value && s->value->oy_struct)
           c[n++] = (oyStruct_s*)s->value->oy_struct;
       }
       break;
    case oyOBJECT_OPTIONS_S:
       {
         oyOptions_s_ * s = (oyOptions_s_*)obj;
         CHECK_ASSIGN_STRUCT( list_ )
       }
       break;
    case oyOBJECT_RECTANGLE_S:
       break;
    case oyOBJECT_IMAGE_S:
       {
         oyImage_s_ * s = (oyImage_s_*)obj;
         CHECK_ASSIGN_STRUCT( viewport )
         CHECK_ASSIGN_STRUCT( tags )
         CHECK_ASSIGN_STRUCT( profile_ )
         CHECK_ASSIGN_STRUCT( pixel_data )
         CHECK_ASSIGN_STRUCT( user_data )
       }
       break;
    case oyOBJECT_ARRAY2D_S:
       {
         oyArray2d_s_ * s = (oyArray2d_s_*)obj;
         CHECK_ASSIGN_STRUCT( refs_ )
         CHECK_ASSIGN_STRUCT( refered_ )
       }
       break;
    case oyOBJECT_FILTER_CORE_S:
       {
         oyFilterCore_s_ * s = (oyFilterCore_s_*)obj;
         CHECK_ASSIGN_STRUCT( options_ )
         CHECK_ASSIGN_STRUCT( api4_ )
       }
       break;
    case oyOBJECT_FILTER_CORES_S:
       break;
    case oyOBJECT_CONVERSION_S:
       {
         oyConversion_s_ * s = (oyConversion_s_*)obj;
         CHECK_ASSIGN_STRUCT( input )
         CHECK_ASSIGN_STRUCT( out_ )
       }
       break;
    case oyOBJECT_CONNECTOR_S:
    case oyOBJECT_CONNECTOR_IMAGING_S:
    case oyOBJECT_CONNECTOR_MAX_S:
       break;
    case oyOBJECT_FILTER_PLUG_S:
       {
         oyFilterPlug_s_ * s = (oyFilterPlug_s_*)obj;
         CHECK_ASSIGN_STRUCT( node )
         CHECK_ASSIGN_STRUCT( remote_socket_ )
         CHECK_ASSIGN_STRUCT( pattern )
       }
       break;
    case oyOBJECT_FILTER_PLUGS_S:
       {
         oyFilterPlugs_s_ * s = (oyFilterPlugs_s_*)obj;
         CHECK_ASSIGN_STRUCT( list_ )
       }
       break;
    case oyOBJECT_FILTER_SOCKET_S:
       {
         oyFilterSocket_s_ * s = (oyFilterSocket_s_*)obj;
         CHECK_ASSIGN_STRUCT( node )
         CHECK_ASSIGN_STRUCT( requesting_plugs_ )
         CHECK_ASSIGN_STRUCT( data )
         CHECK_ASSIGN_STRUCT( pattern )
       }
       break;
    case oyOBJECT_FILTER_NODE_S:
       {
         oyFilterNode_s_ * s = (oyFilterNode_s_*)obj;
         if(s->plugs && s->plugs_n_)
         {
           i = 0;
           while(s->plugs[i]) ++i;
           if(i)
             memcpy(&c[n], s->plugs, sizeof(oyStruct_s*) * i);
           n += i;
         }
         if(s->sockets && s->sockets_n_)
         {
           i = 0;
           while(s->sockets[i]) ++i;
           if(i)
             memcpy(&c[n], s->sockets, sizeof(oyStruct_s*) * i);
           n += i;
         }
         CHECK_ASSIGN_STRUCT( core )
         CHECK_ASSIGN_STRUCT( tags )
         CHECK_ASSIGN_STRUCT( backend_data )
         CHECK_ASSIGN_STRUCT( api7_ )
       }
       break;
    case oyOBJECT_FILTER_NODES_S:
       {
         oyFilterNodes_s_ * s = (oyFilterNodes_s_*)obj;
         CHECK_ASSIGN_STRUCT( list_ )
       }
       break;
    case oyOBJECT_FILTER_GRAPH_S:
       {
         oyFilterGraph_s_ * s = (oyFilterGraph_s_*)obj;
         CHECK_ASSIGN_STRUCT( nodes )
         CHECK_ASSIGN_STRUCT( edges )
         CHECK_ASSIGN_STRUCT( options )
       }
       break;
    case oyOBJECT_PIXEL_ACCESS_S:
       {
         oyPixelAccess_s_ * s = (oyPixelAccess_s_*)obj;
         CHECK_ASSIGN_STRUCT( user_data )
         CHECK_ASSIGN_STRUCT( array )
         CHECK_ASSIGN_STRUCT( output_array_roi )
         CHECK_ASSIGN_STRUCT( output_image )
         CHECK_ASSIGN_STRUCT( graph )
         CHECK_ASSIGN_STRUCT( request_queue )
       }
       break;
    case oyOBJECT_CMM_HANDLE_S:
       {
         oyCMMhandle_s * s = (oyCMMhandle_s*)obj;
         CHECK_ASSIGN_STRUCT( info )
       }
       break;
    case oyOBJECT_POINTER_S:
       break;
    case oyOBJECT_CMM_INFO_S:
       {
         oyCMMinfo_s_ * s = (oyCMMinfo_s_*)obj;
         CHECK_ASSIGN_STRUCT( api )
         CHECK_ASSIGN_STRUCT( icon )
       }
       break;
    case oyOBJECT_CMM_API_S:
       break;
    case oyOBJECT_CMM_APIS_S:
       {
         oyCMMapis_s_ * s = (oyCMMapis_s_*)obj;
         CHECK_ASSIGN_STRUCT( list_ )
       }
       break;
    case oyOBJECT_CMM_API1_S:
    case oyOBJECT_CMM_API2_S:
       break;
    case oyOBJECT_CMM_API3_S:
       {
         oyCMMapi3_s_ * s = (oyCMMapi3_s_*)obj;
         CHECK_ASSIGN_STRUCT( next )
       }
       break;
    case oyOBJECT_CMM_API4_S:
       {
         oyCMMapi4_s_ * s = (oyCMMapi4_s_*)obj;
         CHECK_ASSIGN_STRUCT( next )
         CHECK_ASSIGN_STRUCT( api5_ )
         CHECK_ASSIGN_STRUCT( runtime_context )
         CHECK_ASSIGN_STRUCT( ui )
       }
       break;
    case oyOBJECT_CMM_API5_S:
       {
         oyCMMapi5_s_ * s = (oyCMMapi5_s_*)obj;
         CHECK_ASSIGN_STRUCT( next )
       }
       break;
    case oyOBJECT_CMM_API6_S:
       {
         oyCMMapi6_s_ * s = (oyCMMapi6_s_*)obj;
         CHECK_ASSIGN_STRUCT( next )
         CHECK_ASSIGN_STRUCT( api5_ )
         CHECK_ASSIGN_STRUCT( runtime_context )
       }
       break;
    case oyOBJECT_CMM_API7_S:
       {
         oyCMMapi7_s_ * s = (oyCMMapi7_s_*)obj;
         CHECK_ASSIGN_STRUCT( next )
         CHECK_ASSIGN_STRUCT( api5_ )
         CHECK_ASSIGN_STRUCT( runtime_context )
         if(s->plugs && s->plugs_n)
         {
           memcpy(&c[n], s->plugs, sizeof(oyStruct_s*)*s->plugs_n);
           n += s->plugs_n;
         }
         if(s->sockets && s->sockets_n)
         {
           memcpy(&c[n], s->sockets, sizeof(oyStruct_s*)*s->sockets_n);
           n += s->sockets_n;
         }
       }
       break;
    case oyOBJECT_CMM_API8_S:
       {
         oyCMMapi8_s_ * s = (oyCMMapi8_s_*)obj;
         CHECK_ASSIGN_STRUCT( next )
         CHECK_ASSIGN_STRUCT( api5_ )
         CHECK_ASSIGN_STRUCT( runtime_context )
         CHECK_ASSIGN_STRUCT( ui )
         CHECK_ASSIGN_STRUCT( icon )
       }
       break;
    case oyOBJECT_CMM_API9_S:
       {
         oyCMMapi9_s_ * s = (oyCMMapi9_s_*)obj;
         CHECK_ASSIGN_STRUCT( next )
         CHECK_ASSIGN_STRUCT( api5_ )
         CHECK_ASSIGN_STRUCT( runtime_context )
         while(s->object_types[i])
           c[n++] = (oyStruct_s*) s->object_types[i++];
       }
       break;
    case oyOBJECT_CMM_API10_S:
       {
         oyCMMapi10_s_ * s = (oyCMMapi10_s_*)obj;
         CHECK_ASSIGN_STRUCT( next )
         CHECK_ASSIGN_STRUCT( api5_ )
         CHECK_ASSIGN_STRUCT( runtime_context )
       }
       break;
    case oyOBJECT_CMM_DATA_TYPES_S:
       break;
    case oyOBJECT_CMM_API_FILTER_S:
       {
         oyCMMapiFilter_s_ * s = (oyCMMapiFilter_s_*)obj;
         CHECK_ASSIGN_STRUCT( next )
       }
       break;
    case oyOBJECT_CMM_API_FILTERS_S:
       {
         oyCMMapiFilters_s_ * s = (oyCMMapiFilters_s_*)obj;
         CHECK_ASSIGN_STRUCT( list_ )
       }
       break;
    case oyOBJECT_CMM_UI_S:
       /* The parent struct add just clutter, and might pose a
        * circular relation, which is hard to handle inside DAG. Skip! */
       /*{
         oyCMMui_s_ * s = (oyCMMui_s_*)obj;
         CHECK_ASSIGN_STRUCT( parent )
       }*/
       break;
    case oyOBJECT_CMM_OBJECT_TYPE_S:
    case oyOBJECT_CMM_API_MAX:
    case oyOBJECT_ICON_S:
    case oyOBJECT_JOB_S:
    case oyOBJECT_MODULE_S:
    case oyOBJECT_EXTERNFUNC_S:
    case oyOBJECT_NAME_S:
    case oyOBJECT_COMP_S_:
    case oyOBJECT_FILE_LIST_S_:
       break;
    case oyOBJECT_HASH_S:
       {
         oyHash_s_ * s = (oyHash_s_*)obj;
         CHECK_ASSIGN_STRUCT( oy_struct )
       }
       break;
    case oyOBJECT_STRUCT_LIST_S:
       {
         oyStructList_s_ * s = (oyStructList_s_*)obj;
         int n_max = (s->n_ > oy_c_max-1) ? oy_c_max-1 : s->n_;
         if(s->ptr_ && s->n_)
         {
           memcpy(&c[n], s->ptr_, sizeof(oyStruct_s*)*n_max);
           n += n_max;
         }
       }
       break;
    case oyOBJECT_BLOB_S:
       break;
    case oyOBJECT_CONFIG_S:
       {
         oyConfig_s_ * s = (oyConfig_s_*)obj;
         CHECK_ASSIGN_STRUCT( db )
         CHECK_ASSIGN_STRUCT( backend_core )
         CHECK_ASSIGN_STRUCT( data )
       }
       break;
    case oyOBJECT_CONFIGS_S:
       {
         oyConfigs_s_ * s = (oyConfigs_s_*)obj;
         CHECK_ASSIGN_STRUCT( list_ )
       }
       break;
    case oyOBJECT_UI_HANDLER_S:
    case oyOBJECT_FORMS_ARGS_S:
    case oyOBJECT_CALLBACK_S:
    case oyOBJECT_OBSERVER_S:
       {
         oyObserver_s_ * s = (oyObserver_s_*)obj;
         CHECK_ASSIGN_STRUCT( observer )
         CHECK_ASSIGN_STRUCT( model )
         CHECK_ASSIGN_STRUCT( user_data )
       }
       break;
    case oyOBJECT_CONF_DOMAIN_S:
    case oyOBJECT_INFO_STATIC_S:
    case oyOBJECT_LIST_S:
    case oyOBJECT_LIS_S:
    case oyOBJECT_DB_API_S:
    case oyOBJECT_MONITOR_HOOKS_S:
    case oyOBJECT_MONITOR_HOOKS2_S:
    case oyOBJECT_MONITOR_HOOKS3_S:
    case oyOBJECT_MAX:
         break;
  }
#undef CHECK_ASSIGN_STRUCT 
  c[n] = NULL;
  if(oyOBJECT_MAX < obj->type_ && obj->type_ < oyOBJECT_MAX_CUSTOM)
  {
    const char * ids = oyStruct_GetInfo( obj, oyNAME_JSON+2, 0x01 );
    char ** list;
    n = 0;
    list = oyStringSplit( ids, ' ', &n, oyAllocateFunc_ );
    for(i = 0; i < n; ++i)
    {
      int id = atoi( list[i] );
      oyStruct_s * s = oyStruct_FromId( id );
      if(s)
        c[i] = s;
      else
        fprintf( stderr, "Could not find struct for id: %s\n", list[i] );
    }
    oyStringListRelease( &list, n, oyDeAllocateFunc_ );
    c[n] = NULL;
  }

  {
    int i = 0;
    while(c[i] && i < (120-1)) ++i;
    if(i != n)
    {
      fprintf( stderr, OY_DBG_FORMAT_ "%s[%d] collected ambigous child count: %d == %d ", OY_DBG_ARGS_, oyStructTypeToText( obj->type_ ), oyStruct_GetId(obj), i,n );
      if(obj->type_ == oyOBJECT_STRUCT_LIST_S)
      {
        int j;
        oyStructList_s_ * s = (oyStructList_s_*)obj;
        fprintf( stderr, "\n%s\n", oyStruct_GetText(obj,oyNAME_DESCRIPTION,2) );
        for(j = 0; j < i && j < 5; ++j)
          fprintf( stderr, "[%d]", oyStruct_GetId(s->ptr_[j]) );
      }
      fprintf( stderr, "\n" );
    }
  }

  return n;

gcassert:
  return 0;
}

static void oyObjectTreeCallbackSearchParents (
                                       void              * user_data,
                                       int                 top_id OY_UNUSED,
                                       oyLeave_s         * tree OY_UNUSED,
                                       oyStruct_s        * grandparent OY_UNUSED,
                                       oyStruct_s        * parent,
                                       oyStruct_s        * current,
                                       oyStruct_s       ** children OY_UNUSED,
                                       int                 children_n OY_UNUSED,
                                       int                 level OY_UNUSED )
{
  oyStruct_s ** c = (oyStruct_s**)user_data,
             * ref = c[0];
  int id;
  int cid;
  int pid OY_UNUSED;
  int pos = 0;
  oyStruct_s * found = NULL;

  id = oyStruct_GetId(ref);
  cid = current->oy_ ? oyStruct_GetId(current) : -2;
  pid = parent && parent->oy_ ? oyStruct_GetId(parent) : -1;

  if(id == cid)
    found = parent;
  /* test children */
  else
  {
    static oyStruct_s ** c = NULL;
    int n = oyStruct_GetChildren(current, &c);
    for(pos = 0; pos < n; ++pos)
    {
      oyStruct_s * s = c[pos];
      int bid = oyStruct_GetId(s);
      if(bid == id)
        found = current;
    }
  }

  pos = 1;
  while(c[pos])
  {
    if(found == c[pos])
      found = NULL;
    ++pos;
  }

  if(found != NULL && pos < oy_c_max)
    c[pos] = found;
}


int                oyStruct_GetParents(oyStruct_s        * obj,
                                       oyStruct_s      *** list )
{
  static oyStruct_s * c[oy_c_max];
  int * ids = oyObjectGetCurrentObjectIdList( );
  int n = 0;

  memset(c,0,sizeof(oyStruct_s *)*oy_c_max);
  c[0] = obj;

  oyObjectIdListTraverseStructTrees( ids, oyObjectTreeCallbackSearchParents, c, 0 );

  oyObjectReleaseCurrentObjectIdList( &ids );

  while(c[n+1]) ++n;

  if(list) *list = &c[1];

  return n;
}

struct oyLeave_s {
  /* downward */
  int n;
  oyLeave_s ** children;
  oyStruct_s ** list;
  /* current */
  oyStruct_s * obj;
  int id;
  /* upper level */
  oyLeave_s * parent;
  oyLeave_s * grandparent;
};

#define myCalloc_m(x,n) myCalloc_(x,n); if(oy_debug_memory) printf(OY_DBG_FORMAT_" %lu * %d bytes\n",OY_DBG_ARGS_, x,n);
static void * myCalloc_( size_t size, size_t n )
{ void * mem = oyAllocateFunc_( size*n );
  memset(mem,0,size*n);
  return mem;
}
#define AD oyAllocateFunc_,oyDeAllocateFunc_
int oyObjectGetThreadIdDummy() { return 0; }
oyThreadId_f oyObjectGetThreadId = oyObjectGetThreadIdDummy;
static const int oy_object_list_max_count_ = 1000000;
oyLeave_s *** oy_debug_leave_cache_ = NULL;
#define OY_DEBUG_LEVEL_CACHE_THREADS_MAX 1024
oyLeave_s ** oyDebugLevelCacheGetForThread()
{
  int thread_id = oyObjectGetThreadId();
  if(!oy_debug_leave_cache_)
  { oy_debug_leave_cache_ = (oyLeave_s***) myCalloc_m( sizeof( oyLeave_s** ), OY_DEBUG_LEVEL_CACHE_THREADS_MAX ); }
  if(!oy_debug_leave_cache_[thread_id])
  { oy_debug_leave_cache_[thread_id] = (oyLeave_s**) myCalloc_m( sizeof( oyLeave_s* ), oy_object_list_max_count_ + 1 ); }
  return oy_debug_leave_cache_[thread_id];
}
void oyDebugLevelCacheRelease()
{
  int i;
  if(!oy_debug_leave_cache_) return;
  for(i = 0; i < OY_DEBUG_LEVEL_CACHE_THREADS_MAX; ++i)
  {
    if(oy_debug_leave_cache_[i])
      oyFree_m_(oy_debug_leave_cache_[i]);
  }
  oyFree_m_(oy_debug_leave_cache_);
}

/* allocate object and oyLeave_s**children + oyStruct_s**obj arrays */
static oyLeave_s * oyLeave_NewWith   ( oyStruct_s        * obj,
                                       int                 id,
                                       oyLeave_s         * parent,
                                       oyLeave_s         * grandparent,
                                       int               * alloced )
{
  oyLeave_s * l = NULL;
 
  oyLeave_s ** cache = oyDebugLevelCacheGetForThread( );

  if(id == old_oy_debug_objects)
  { const char * txt = oyStruct_GetText(obj, oyNAME_DESCRIPTION, 2);
    if(!txt)
      txt = oyStruct_GetText(obj, oyNAME_DESCRIPTION, 0);
    if(oy_debug_memory)
    {
      if(txt && (strlen(txt) < 80))
        fprintf(stderr, "%s ", txt);
      else
        fprintf(stderr, "%s ", oyStruct_TypeToText(obj));
    }
    PRINT_ID(id)
  }

  if(cache[id])
  {
    l = cache[id];
    *alloced = 0;
  }

  if(!l)
  {
    oyStruct_s ** slist;
    l = myCalloc_m( sizeof(oyLeave_s), 1 );
    *alloced = 1;
    l->obj = obj;
    l->id = id;
    l->parent = parent;
    if(parent && parent->obj && parent->obj->type_ == oyOBJECT_OBJECT_S)
      fprintf( stderr, "wrong type oyObject_s\n" );
    l->grandparent = grandparent;
    if(oy_debug_memory)
      fprintf( stderr, "%d <- new oyLeave_s(id) " OY_PRINT_POINTER "\n", id, (ptrdiff_t)l );

    l->n = oyStruct_GetChildren( obj, &slist );
    if(l->n)
    {
      l->list = myCalloc_m( sizeof(oyStruct_s*), l->n + 1 );
      memcpy( l->list, slist, sizeof(oyStruct_s*) * l->n );
      l->children = myCalloc_m( sizeof( oyLeave_s* ), l->n + 1 );
    }

    cache[id] = l;
  }

  return l;
}

static int         oyLeave_Release   ( oyLeave_s        ** leave )
{
  oyLeave_s * l = NULL;
  oyLeave_s ** cache = oyDebugLevelCacheGetForThread( );
  int error = 0;
 
  if(leave)
    l = *leave;

  if(l == NULL)
    return 1;

  PRINT_ID(l->id)

  if(cache[l->id] == NULL)
    error = 1;

  *leave = NULL;

  if(error)
    return error;

  cache[l->id] = NULL;
  if(oy_debug_memory)
    fprintf( stderr, "%d <- release oyLeave_s(id) " OY_PRINT_POINTER "\n", l->id, (ptrdiff_t)l );

  if(l->children)
    oyFree_m_(l->children);
  if(l->list)
    oyFree_m_(l->list);
  oyFree_m_(l);

  return 0;
}

static void oyDebugLevelCacheClean   ( void )
{
  oyLeave_s ** cache = oyDebugLevelCacheGetForThread( );
  int thread_id = oyObjectGetThreadId();
  if(cache)
  {
    int i;
    for(i = 0; i < oy_object_list_max_count_; ++i)
      if(cache[i] )
        oyLeave_Release( &cache[i] );
    oyFree_m_(cache);
  }
  oy_debug_leave_cache_[thread_id] = NULL;
}

static int         oyObjectStructTreeParentContains (
                                       oyLeave_s         * l,
                                       int                 id,
                                       int                 tolerate )
{
  int n = 0, lid,cid;
  oyLeave_s * c = l;
  if(!l || !l->parent) return tolerate;
  lid = l->id;
  /* put here a non recursive element, in order to avoid infinite loops. */
  while((c = c->parent) != NULL)
  { cid = c->id;
    if(lid == cid)
      --tolerate;
    if(tolerate <= 0)
      return tolerate; 
    ++n;
    if(n > 100)
    {
      if( id == old_oy_debug_objects ||
          l->id == old_oy_debug_objects )
        WARNc3_S( "[%d](%d) maximum loops reached: %d", l->id, cid, n );
      --tolerate;
      break;
    }
  }

  if(l->parent && l->parent->id == id)
    --tolerate;

  if(tolerate <= 0)
    return tolerate; 

  tolerate = oyObjectStructTreeParentContains(l->parent, id, tolerate);

  return tolerate;
}
static int contains_cache[10] = {0,0,0,0,0, 0,0,0,0,0};
static int contains_nested = 0;
#define CRETURN( code ) { --contains_nested; return code; }
/* @param          direction           search direction
 * -  1 : search down in children
 * -  0 : search in both directions
 * - -1 : search upward in parents */
static int oyObjectStructTreeContains( oyLeave_s         * l,
                                       int                 id,
                                       int                 direction )
{
  int i, tolerate = 1;
  ++contains_nested;

  if(l)
  {
    if(contains_nested > 100)
    {
      fprintf(stderr, OY_DBG_FORMAT_ "%s[%d] nested: %d (parents [%d]->[%d]->[%d]->[%d]->[%d]->[%d]->[%d]->[%d]->[%d])\n", OY_DBG_ARGS_, oyStructTypeToText( l->obj->type_ ), l->id, contains_nested, contains_cache[1], contains_cache[2], contains_cache[3], contains_cache[4], contains_cache[5], contains_cache[6], contains_cache[7], contains_cache[8], contains_cache[9]);
      CRETURN( 1 );
    }

    if(direction == 0)
    {
      if(oyObjectStructTreeContains(l, id, -1))
        CRETURN( 1 );
      if(oyObjectStructTreeContains(l, id, 1))
        CRETURN( 1 );
      CRETURN( 0 );
    }

    /* check this leave upward */
    if(direction < 0 && l->parent)
    {
      if(l->parent->id == id)
        CRETURN( 1 );
      if(l->parent &&
         oyObjectStructTreeParentContains(l->parent, id, tolerate) <= 0)
        CRETURN( 1 );
    }

    /* check this leave downward */
    if(direction > 0)
    {
      for(i = 0; i < l->n; ++i)
      {
        if(!l->children[i])
          continue;
        if(l->children[i]->id == id)
          CRETURN( 1 );
        PRINT_ID(l->id)
        {
          memmove(&contains_cache[1], &contains_cache[0], sizeof(int) * 9);
          contains_cache[0] = l->id;
#if 1
          int n = oyObjectStructTreeParentContains(l,id,tolerate);
          if(n <= 0)
#else
          if( (contains_cache[0] && (contains_cache[0] == contains_cache[1] || contains_cache[0] == contains_cache[2])) ||
              (contains_cache[1] && contains_cache[1] == c3) )
#endif
          {
            if( id == old_oy_debug_objects ||
                l->id == old_oy_debug_objects ||
                oy_debug )
              fprintf(stderr, OY_DBG_FORMAT_ "[%d] found circulars (parents [%d]->[%d]->[%d]->[%d]->[%d]->[%d]->[%d]->[%d]->[%d])\n", OY_DBG_ARGS_, l->id, contains_cache[1], contains_cache[2], contains_cache[3], contains_cache[4], contains_cache[5], contains_cache[6], contains_cache[7], contains_cache[8], contains_cache[9]);
            CRETURN( 0 );
          }
        }
        /* detect current ID upward and stop */
        if(oyObjectStructTreeContains(l->children[i], l->id, 1))
          CRETURN( 0 );
        if(oyObjectStructTreeContains(l->children[i], id, 1))
          CRETURN( 1 );
      }
    }
  }

  CRETURN( 0 );
}
#undef CRETURN

static int cntx = 0;
static int cntx_mem = 0;
static oyLeave_s * oyObjectIdListGetStructTree (
                                       int                 top,
                                       oyLeave_s         * grandparent,
                                       oyLeave_s         * parent,
                                       int               * ids,
                                       int                 id,
                                       int                 level,
                                       oyObjectTreeCallback_f func,
                                       void              * user_data )
{
  int max_count = 0, i, alloced = 0;
  const oyObject_s * obs;
  oyStruct_s * obj;
  oyLeave_s * l = NULL;

  if(id < 0) /* possibly static objects without oyObject part */
    return l;
  if(id >= oy_object_list_max_count_)
  {
    WARNc1_S( "id too big: %d", id );
    return l;
  }

  PRINT_ID(id)
  obs = oyObjectGetList( &max_count );
  if(!obs || !obs[id])
    return l;
  obj = obs[id]->parent_;
  l = oyLeave_NewWith( obj, id, parent, grandparent, &alloced );

  if(alloced)
    cntx_mem += sizeof(oyLeave_s);

  if(l->n && alloced)
  {
    cntx_mem += sizeof(oyLeave_s) * l->n;
    if(oy_debug_memory) printf("%d %d mem: %d\n", cntx++, level, cntx_mem);

    for(i = 0; i < l->n; ++i)
    {
      int i_id = -1;
      oyStruct_s * entry = l->list[i];
      if(entry && oyOBJECT_NONE < entry->type_ && entry->type_ < oyOBJECT_MAX_STRUCT && entry->oy_)
        i_id = entry->oy_->id_;

      PRINT_ID(i_id)
      if(i_id >= oy_object_list_max_count_)
      {
        WARNc5_S("l(%d)->list[%d]: " OY_PRINT_POINTER " %s[%d]", l->n, i, obj, oyStruct_GetText(obj, oyNAME_DESCRIPTION, 1),
                       oyStruct_GetId( obj ) );
        break;
      } else
        l->children[i] = oyObjectIdListGetStructTree( top, parent, l, ids, i_id, level+1, func, user_data );
      /* remember the parent to traverse the actual tree */
      if(l->children[i])
      {
        l->children[i]->parent = l;
        l->children[i]->grandparent = parent;
      }
    }
  }

  if(func) func( user_data, top, l, l->grandparent?l->grandparent->obj:NULL, l->parent?l->parent->obj:NULL, l->obj, l->list, l->n, level );

  return l;
}

int                oyObjectIdListTraverseStructTrees (
                                       int               * ids,
                                       oyObjectTreeCallback_f func,
                                       void              * user_data,
                                       int                 flags )
{
  int i, n = 0;
  oyLeave_s ** ts = myCalloc_m( sizeof( oyLeave_s* ), oy_object_list_max_count_ );

  old_oy_debug_objects = oy_debug_objects; /* be more silent */
  oy_debug_objects = -1;

  for(i = 0; i < oy_object_list_max_count_; ++i)
    if(ids[i] > 0)
      ts[n++] = oyObjectIdListGetStructTree( i, 0, 0, ids, i, flags & 0x01 ? -oy_object_list_max_count_ : 0, func, user_data );

  /* release trees */
  oyDebugLevelCacheClean();
  oyFree_m_(ts);

  oy_debug_objects = old_oy_debug_objects;

  return n;
}

int skip_ids[10] = {0,0,0,0,0, 0,0,0,0,0};
typedef struct oyTreeData_s {
  oyLeave_s * l;
  char * text;  /* node text */
  char * text2; /* top text */
  int flags;
} oyTreeData_s;

static void oyObjectTreePrintCallback( void              * user_data,
                                       int                 top,
                                       oyLeave_s         * tree,
                                       oyStruct_s        * grandparent OY_UNUSED,
                                       oyStruct_s        * parent,
                                       oyStruct_s        * current,
                                       oyStruct_s       ** children,
                                       int                 children_n,
                                       int                 level )
{
  oyTreeData_s * graphs = (oyTreeData_s*) user_data;
  int id = oyStruct_GetId(current), i,k;

  if(id < 0)
    return;

  if(!parent && oyObjectStructTreeContains( tree, id, -1 ))
    /* skip */
    return;

  PRINT_ID(id)
  for(i = 0; i < children_n; ++i)
  {
    if(!children[i])
      continue;
    for(k = 0; k < level; ++k)
#if 0
      oyStringAddPrintf( &graphs[top].text, 0,0, "- ");
    oyStringAddPrintf( &graphs[top].text, 0,0, "%s[%d] -> %s[%d]\n",
#else
    /* printf is much faster */
      printf("- ");
    printf( "%s[%d] -> %s[%d]\n",
#endif
                       oyStructTypeToText(current->type_), id, 
                       oyStruct_GetText( children[i], oyNAME_NICK, 1 ),
                       oyStruct_GetId( children[i] ) );
    if(graphs[top].l)
    {
      WARNc1_S( "The branch has already a tree: ", top );
    }
    graphs[top].l = tree;
  }
}
static char * oyObjectTreeDotGraphCallbackGetDescription( oyStruct_s * s )
{
  const char * nick = oyStructTypeToText( s->type_ ),
             * text = oyStruct_GetText( s, oyNAME_DESCRIPTION, 2 );
  char * t = oyjlStringCopy( text, oyAllocateFunc_ );

  if(!text || strcmp(nick,text) == 0)
    return NULL;

  oyjlStringReplace( &t, "\"", "'", AD );
  oyjlStringReplace( &t, "\n", "\\n", AD );
  oyjlStringReplace( &t, "<", "\\<", AD );
  oyjlStringReplace( &t, ">", "\\>", AD );
  oyjlStringReplace( &t, "{", "\\{", AD );
  oyjlStringReplace( &t, "}", "\\}", AD );

  return t;
}
static const char * oyDotNodeGetColor( oyStruct_s        * current,
                                       const char        * desc )
{
  const char * node = "";
  if(current->type_ == oyOBJECT_CONVERSION_S)
    node = " fillcolor=\"RoyalBlue2\"";
  else
  if(current->type_ == oyOBJECT_FILTER_NODE_S ||
     current->type_ == oyOBJECT_FILTER_CORE_S)
    node = " fillcolor=\"MediumSeaGreen\"";
  else
  if(current->type_ == oyOBJECT_CMM_API6_S ||
     current->type_ == oyOBJECT_CMM_API4_S ||
     current->type_ == oyOBJECT_CMM_API7_S)
    node = " fillcolor=\"white\" color=\"MediumSeaGreen\" style=\"filled,rounded,bold\"";
  else
  if(current->type_ == oyOBJECT_FILTER_PLUG_S ||
     current->type_ == oyOBJECT_FILTER_PLUGS_S ||
     (current->type_ == oyOBJECT_STRUCT_LIST_S &&
      desc && strstr(desc,"FilterPlug") != NULL))
    node = " fillcolor=\"LightBlue\"";
  else
  if(current->type_ == oyOBJECT_FILTER_SOCKET_S)
    node = " fillcolor=\"brown2\"";
  else
  if(current->type_ == oyOBJECT_PIXEL_ACCESS_S)
    node = " fillcolor=\"RoyalBlue3\"";
  else
  if(current->type_ == oyOBJECT_PROFILE_S ||
     current->type_ == oyOBJECT_PROFILES_S ||
     current->type_ == oyOBJECT_PROFILE_TAG_S ||
     (current->type_ == oyOBJECT_STRUCT_LIST_S &&
     desc && strstr(desc,"ProfileTag") != NULL))
    node = " fillcolor=\"SlateBlue\" fontcolor=\"white\"";
  else
  if(current->type_ == oyOBJECT_IMAGE_S ||
     current->type_ == oyOBJECT_ARRAY2D_S)
    node = " fillcolor=\"orange\"";
  return node;
}

static void  oyDotNodeAppend         ( char             ** text,
                                       oyStruct_s        * current,
                                       int                 id,
                                       const char        * desc )
{
  /* emphasise with color */
  const char * node = oyDotNodeGetColor( current, desc );
  const char * nick = oyStructTypeToText( current->type_ );
  if(!(nick && nick[0]))
    nick = oyStruct_GetInfo( current, oyNAME_NICK, 1 );
  oyStringAddPrintf( text, 0,0, "%d [label=\"%s id=%d refs=%d%s%s\"%s];\n",
                     id, nick, id, oyObject_GetRefCount(current->oy_), desc?"\\n":"", desc?desc:"", node );
  if(old_oy_debug_objects == id)
    printf("append: %d\n", id);
}

static void oyObjectTreeDotGraphCallback (
                                       void              * user_data,
                                       int                 top,
                                       oyLeave_s         * tree,
                                       oyStruct_s        * grandparent,
                                       oyStruct_s        * parent,
                                       oyStruct_s        * current,
                                       oyStruct_s       ** children,
                                       int                 children_n,
                                       int                 level )
{
  oyTreeData_s * graphs = (oyTreeData_s*) user_data;
  int id = oyStruct_GetId(current), i,k;
  char * desc = 0;

  /* Non identifyable objects map to different trees, which is ambiguous. */
  if(id < 0)
    return;

  if(!parent && oyObjectStructTreeContains( tree, id, -1 ))
    /* skip */
    return;

  if(graphs->flags & 0x02)
    desc = oyObjectTreeDotGraphCallbackGetDescription( current );

  if(skip_ids[0])
  { int i,
        keep = 0;
    if( current->type_ == oyOBJECT_IMAGE_S ||
        current->type_ == oyOBJECT_RECTANGLE_S ||
        current->type_ == oyOBJECT_ARRAY2D_S ||
        current->type_ == oyOBJECT_CONVERSION_S ||
        current->type_ == oyOBJECT_FILTER_NODE_S ||
        current->type_ == oyOBJECT_FILTER_CORE_S ||
        current->type_ == oyOBJECT_FILTER_PLUG_S ||
        current->type_ == oyOBJECT_FILTER_PLUGS_S ||
        current->type_ == oyOBJECT_FILTER_SOCKET_S ||
        current->type_ == oyOBJECT_CMM_API3_S ||
        current->type_ == oyOBJECT_CMM_API5_S ||
        current->type_ == oyOBJECT_CMM_API6_S ||
        current->type_ == oyOBJECT_CMM_API4_S ||
        current->type_ == oyOBJECT_CMM_API7_S ||
        current->type_ == oyOBJECT_CMM_API8_S ||
        current->type_ == oyOBJECT_CMM_API9_S ||
        ( current->type_ == oyOBJECT_STRUCT_LIST_S &&
          desc && strstr(desc,"FilterPlug") != NULL) ||
        ( current->type_ == oyOBJECT_PROFILE_S &&
          desc && strstr(desc,"oyImage_s") != NULL)
      )
      keep = 1;

    for(i = 0; i < 10; ++i)
    { int cid = skip_ids[i];
      if( cid &&
          ( id == cid ||
            oyObjectStructTreeParentContains( tree, cid, 1 ) == 0))
      { /* keep essential object types */
        if( keep )
        {
          if(oy_debug)
            fprintf(stderr, "keep %s[%d] %s\n", oyStructTypeToText(current->type_), id, desc?desc:"----" );
          break;
        }
        else
        {
          if(desc) oyFree_m_( desc );
          return;
        }
      }
    }
  }

  oyDotNodeAppend( &graphs[top].text2, current, id, desc );
  if(desc) oyFree_m_( desc );

  for(i = 0; i < children_n; ++i)
  {
    int i_id;
    const char * edge = "";
    if(!children[i])
      continue;

    i_id = oyStruct_GetId( children[i] );
    if(i_id < 0)
      continue;

#define IS_EDGE_TYPES( t1,t2 ) ((current->type_ == t1 && children[i]->type_ == t2) || (current->type_ == t1 && t2 == oyOBJECT_NONE) || (current->type_ == t2 && children[i]->type_ == t1))
    if IS_EDGE_TYPES(oyOBJECT_PIXEL_ACCESS_S,oyOBJECT_FILTER_PLUG_S)
      edge = " [weight=\"3\" color=\"RoyalBlue4\" penwidth=\"3.0\"]"; else
    if IS_EDGE_TYPES(oyOBJECT_CONVERSION_S,oyOBJECT_FILTER_NODE_S)
      edge = " [color=\"RoyalBlue4\" penwidth=\"3.0\"]"; else
    if (current->type_ == oyOBJECT_FILTER_NODE_S && children[i]->type_ == oyOBJECT_FILTER_SOCKET_S)
      edge = " [weight=\"5\" color=\"RoyalBlue4\" penwidth=\"3.0\"]"; else
    if (current->type_ == oyOBJECT_FILTER_SOCKET_S && children[i]->type_ == oyOBJECT_FILTER_NODE_S)
      edge = " [weight=\"5\" color=\"orange\" penwidth=\"3.0\"]"; else
    if IS_EDGE_TYPES(oyOBJECT_FILTER_SOCKET_S,oyOBJECT_FILTER_PLUGS_S)
      edge = " [weight=\"5\" color=\"RoyalBlue4\" penwidth=\"3.0\"]"; else
    if IS_EDGE_TYPES(oyOBJECT_FILTER_SOCKET_S,oyOBJECT_FILTER_PLUG_S)
      edge = " [weight=\"0\" color=\"orange\" penwidth=\"3.0\"]"; else
    if IS_EDGE_TYPES(oyOBJECT_FILTER_SOCKET_S,oyOBJECT_IMAGE_S)
      edge = " [color=\"orange\" penwidth=\"3.0\"]"; else
    if IS_EDGE_TYPES(oyOBJECT_FILTER_PLUGS_S,oyOBJECT_STRUCT_LIST_S)
      edge = " [weight=\"5\" color=\"RoyalBlue4\" penwidth=\"3.0\"]"; else
    if (IS_EDGE_TYPES(oyOBJECT_STRUCT_LIST_S,oyOBJECT_FILTER_PLUG_S) && parent && parent->type_ == oyOBJECT_FILTER_PLUGS_S && grandparent && grandparent->type_ == oyOBJECT_FILTER_SOCKET_S)
      edge = " [weight=\"5\" color=\"RoyalBlue4\" penwidth=\"3.0\"]"; else
    if (current->type_ == oyOBJECT_FILTER_NODE_S && children[i]->type_ == oyOBJECT_FILTER_PLUG_S)
      edge = " [weight=\"5\" color=\"orange\" penwidth=\"3.0\"]"; else
    if (current->type_ == oyOBJECT_FILTER_PLUG_S && children[i]->type_ == oyOBJECT_FILTER_NODE_S)
      edge = " [weight=\"5\" color=\"RoyalBlue4\" penwidth=\"3.0\"]"; else
    if IS_EDGE_TYPES(oyOBJECT_OBSERVER_S,oyOBJECT_NONE)
      edge = " [weight=\"-1\" color=\"DarkViolet\" penwidth=\"1.0\" style=\"dashed\"]";
    for(k = 0; k < level; ++k)
      oyStringAddPrintf( &graphs[top].text, 0,0, "- ");
    oyStringAddPrintf( &graphs[top].text, 0,0, "%d -> %d%s;\n",
                       id, i_id, edge );
    if(graphs->flags & 0x02)
      desc = oyObjectTreeDotGraphCallbackGetDescription( children[i] );

    oyDotNodeAppend( &graphs[top].text2, children[i], i_id, desc );
    if(desc) oyFree_m_( desc );

    if(graphs[tree->id].l && graphs[tree->id].l != tree)
    {
      WARNc5_S( "The branch had already a different tree: %d old: %d " OY_PRINT_POINTER " new: %d " OY_PRINT_POINTER,
                top, tree->id, graphs[tree->id].l, tree->id, tree );
    }
    graphs[tree->id].l = tree;
  }
}

static int oyObjectIdListTraverseStructTrees_ (
                                       int               * ids,
                                       oyObjectTreeCallback_f func,
                                       oyTreeData_s      * trees,
                                       int                 flags )
{
  int i, n = 0;

  /* scan upon a new cache */
  oyDebugLevelCacheClean();

  for(i = 0; i < oy_object_list_max_count_; ++i)
    if(ids[i] > 0)
    {
      trees[i].l = oyObjectIdListGetStructTree( i, 0, 0, ids, i, flags & 0x01 ? -oy_object_list_max_count_ : 0, func, trees );
      ++n;
    }

  return n;
}

static oyStruct_s *  oyStruct_FromId ( int                 id )
{
  const oyObject_s * obs;
  int max_count = 0;

  if(id < 0 || oy_object_list_max_count_ < id) /* possibly static objects without oyObject part */
    return NULL;

  obs = oyObjectGetList( &max_count );
  if(obs && obs[id])
  {
    if(obs[id]->type_ != oyOBJECT_OBJECT_S)
    {
      fprintf(stderr, "oyStruct_FromId(%d) out of range: %s\n", id, oyStructTypeToText(obs[id]->type_) );
      return NULL;
    }
    if(obs[id]->parent_ == NULL)
    {
      fprintf(stderr, "oyStruct_FromId(%d) no parent found\n", id );
      return NULL;
    }
    if(oyOBJECT_MAX < obs[id]->parent_->type_ && obs[id]->parent_->type_ < oyOBJECT_MAX_CUSTOM)
    {
      const char * nick = oyStructTypeToText(obs[id]->parent_->type_);
      if(!(nick && nick[0])) nick = oyStruct_GetInfo( obs[id]->parent_, oyNAME_NICK, 1 );
      if(!(nick && nick[0]))
      {
        fprintf(stderr, "oyStruct_FromId(%d) non reasonable type found: \"%s\"\nbacktrace:" OY_DBG_FORMAT_, id, nick, OY_DBG_ARGS_ );
        OY_BACKTRACE_PRINT
        return NULL;
      }
    }

    return obs[id]->parent_;
  }
  else
    return NULL;
}


/** @brief    Print the current object trees to stderr.
 *  @ingroup  objects_generic
 * 
 *  Probably the most simple and still useful function from the Object tree debug APIs.
 *  It prints to stderr with ::OY_DEBUG_OBJECTS/::oy_debug_objects set.
 *
 *  @param        flags               use 15 for all flags
 *                                    - 0x01 show a graph
 *                                    - 0x02 include more object details
 *                                    - 0x04 skip cmm caches to show a more functional graph
 *                                    - 0x08 include unconnected objects
 *  @param        comment             a title comment for the graph; optional
 *
 *  @version  Oyranos: 0.9.7
 *  @date     2019/10/15
 *  @since    2016/04/16 (Oyranos: 0.9.6)
 */
void               oyObjectTreePrint ( int                 flags,
                                       const char        * comment )
{
  if(oy_debug_objects >= 0 || oy_debug_objects <= -2)
  {
    int skip_cmm_caches_flag = getenv("OY_DEBUG_OBJECTS_SKIP_CMM_CACHES") ? 0x04 : 0;
    int * ids_old = oyObjectGetCurrentObjectIdList( );
    oyTreeData_s * trees = (oyTreeData_s*) myCalloc_m( sizeof( oyTreeData_s ), oy_object_list_max_count_ + 1 );
    int n, i, count = 0;
    char * dot = 0, * dot_edges = 0;

    old_oy_debug_objects = oy_debug_objects; /* be more silent */
    oy_debug_objects = -1;

    flags |= skip_cmm_caches_flag;

    if(flags)
      for(i = 0; i < oy_object_list_max_count_; ++i) trees[i].flags = flags;

    /* skip most of the caches */
    if(flags & 0x04)
    {
      int pos = 0;
      for(i = 0; i < oy_object_list_max_count_; ++i)
      {
        oyStruct_s * s = oyStruct_FromId(i);
        const char * text = oyStruct_GetText( s, oyNAME_DESCRIPTION, 2 );
        if(text &&
            ( strstr(text,"oy_cmm_") ||
              strstr(text,"oy_db_") ||
              strstr(text,"oy_profile_s_file_cache_")))
        {
          skip_ids[pos++] = i;
          if(strstr(text,"oy_db_cache"))
            skip_ids[pos++] = i-1;
          printf("ignore: %s[%d]\n", text, i);
        }
      }
    }

    if(flags & 0x01)
      n = oyObjectIdListTraverseStructTrees_( ids_old, oyObjectTreeDotGraphCallback, trees, flags );
    else
      n = oyObjectIdListTraverseStructTrees_( ids_old, oyObjectTreePrintCallback, trees, flags );

    for(i = 0; i < oy_object_list_max_count_; ++i)
    {
      if(old_oy_debug_objects == i)
      {
        oyStruct_s * s = oyStruct_FromId(i);
        if(s)
        {
          const char * nick = oyStructTypeToText( s->type_ ),
                     * text = oyStruct_GetText( s, oyNAME_DESCRIPTION, 2 );
          fprintf( stderr, "present: %s \"%s\" - %s\n", oyNoEmptyString_m_(nick), oyNoEmptyString_m_(text), oyNoEmptyString_m_(trees[i].text));
        }
      }
      if(i == old_oy_debug_objects)
        fprintf(stderr, OY_DBG_FORMAT_ "ID: %d\n", OY_DBG_ARGS_, i);
      if(trees[i].text || (trees[i].text2 && flags & 0x08))
      {
        if(flags & 0x01)
        {
          oyStringAddPrintf( &dot, 0,0, "    %s", trees[i].text2 );
          if(trees[i].text)
            oyStringAddPrintf( &dot_edges, 0,0, "    %s", trees[i].text );
          else
            oyStringAddPrintf( &dot_edges, 0,0, "    %d\n", i );
        } else
          fprintf(stderr, "%d: %s\n", i, trees[i].text);
        ++count;
      }
      if(trees[i].text)
        oyFree_m_(trees[i].text);
      if(trees[i].text2)
        oyFree_m_(trees[i].text2);
    }
    if(oy_debug)
      fprintf( stderr, "found/printed trees: %d/%d\n", n, count);

    oyFree_m_(trees);
    oyObjectReleaseCurrentObjectIdList( &ids_old );
    /* check if everything is released */
    oyDebugLevelCacheClean();

    /* remove double lines and priorise */
    {
      int lines_n = 0;
      char ** lines = oyStringSplit_( dot, '\n', &lines_n, 0 );
      char * tmp = 0;
      if(oy_debug)
        fprintf(stderr, "dot has number of lines %d\n", lines_n);
      oyStringListSetHeadingWhiteSpace( lines, lines_n, 4, 0,0 );

      /* give some item priority by moving the nodes up */
      for(i = 0; i < lines_n; ++i)
        if(strstr( lines[i], "oyConversion_s"))
          oyStringAddPrintf( &tmp, 0,0, "%s\n", lines[i] );
      for(i = 0; i < lines_n; ++i)
        if( strstr( lines[i], "oyFilterNode_s") &&
            !strstr( lines[i], "oyOption_s") &&
            !strstr( lines[i], "oyStructList_s"))
          oyStringAddPrintf( &tmp, 0,0, "%s\n", lines[i] );
      for(i = 0; i < lines_n; ++i)
        if(strstr( lines[i], "oyFilterSocket_s"))
          oyStringAddPrintf( &tmp, 0,0, "%s\n", lines[i] );
      for(i = 0; i < lines_n; ++i)
        if(strstr( lines[i], "oyImage_s"))
          oyStringAddPrintf( &tmp, 0,0, "%s\n", lines[i] );
      for(i = 0; i < lines_n; ++i)
        if(strstr( lines[i], "oyFilterPlug_s"))
          oyStringAddPrintf( &tmp, 0,0, "%s\n", lines[i] );
      if(dot)
      {
        oyStringAddPrintf( &tmp, 0,0, "\n%s", dot );
        oyFree_m_(dot);
      }

      oyStringListRelease_( &lines, lines_n, 0 );
      lines = oyStringSplit_( tmp, '\n', &lines_n, 0 );
      if(tmp) oyFree_m_(tmp);
      for(i = 0; i < lines_n; ++i)
      {
        int pos = 0;
        while(lines[i][pos] == ' ')
          pos++;
        oyStringAddPrintf( &dot, 0,0, "  %s\n", &lines[i][pos] );
      }
      oyStringListRelease_( &lines, lines_n, 0 );
      lines = oyStringSplit_( dot, '\n', &lines_n, 0 );
      if(dot) oyFree_m_(dot);
      oyStringListFreeDoubles( lines, &lines_n, 0 );
      for(i = 0; i < lines_n; ++i)
      {
        oyStringAddPrintf( &dot, 0,0, "%s\n", lines[i] );
      }
      if(oy_debug)
        fprintf(stderr, "dot has number of unique lines %d\n", lines_n);
      oyStringListRelease_( &lines, lines_n, 0 );

      lines_n = 0;
      lines = oyStringSplit_( dot_edges, '\n', &lines_n, 0 );
      if(oy_debug)
        fprintf(stderr, "dot_edges has number of lines %d\n", lines_n);
      oyStringListSetHeadingWhiteSpace( lines, lines_n, 4, 0,0 );
      oyStringListFreeDoubles( lines, &lines_n, 0 );
      if(oy_debug)
        fprintf(stderr, "dot_edges has number of unique lines %d\n", lines_n);
      for(i = 0; i < lines_n; ++i)
        oyStringAddPrintf( &tmp, 0,0, "%s\n", lines[i] );
      if(dot_edges) oyFree_m_(dot_edges);
      oyStringListRelease_( &lines, lines_n, 0 );
      dot_edges = tmp; tmp = 0;
    }

    if(flags & 0x01 && dot)
    {
      int r OY_UNUSED;
      char * graph = 0;
      oyStringAddPrintf( &graph,0,0,
                        "\
digraph G {\n\
bgcolor=\"transparent\"\n\
  rankdir=LR\n\
  graph [fontname=Helvetica, fontsize=12];\n\
  node [shape=record, fontname=Helvetica, fontsize=10, style=\"filled,rounded\"];\n\
  edge [fontname=Helvetica, fontsize=10];\n\
\n");

      if(dot)
        oyStringAddPrintf( &graph,0,0, "%s\n", dot);
      oyStringAddPrintf( &graph,0,0,"\
  subgraph cluster_0 {\n\
    label=\"%s\"\n\
    color=gray;\n", comment?comment:"Oyranos Object Trees Graph");
      if(dot_edges)
        oyStringAddPrintf( &graph,0,0,"%s", dot_edges);
      oyStringAddPrintf( &graph,0,0,"  }\n");
      oyStringAddPrintf( &graph,0,0,"}\n");

      /* write a dot file to disc */
      oyWriteMemToFile_( "oyranos_tree.dot", graph, strlen(graph)+1 );

      /* generate a SVG and send to firefox */
      if(flags & 0x02)
        r = system("dot -Tsvg oyranos_tree.dot -o oyranos_tree.svg && firefox oyranos_tree.svg ; sleep 1 &");
      else
        r = system("fdp -Tsvg oyranos_tree.dot -o oyranos_tree.svg && firefox oyranos_tree.svg ; sleep 1 &");

      if(graph) oyFree_m_( graph );
      if(dot) oyFree_m_( dot );
      if(dot_edges) oyFree_m_( dot_edges );
    } else
    if(flags & 0x01 && !dot)
      WARNc_S( "dot is empty" );
    oy_debug_objects = old_oy_debug_objects;
  }
}

