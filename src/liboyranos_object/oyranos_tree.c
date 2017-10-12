/** @file oyranos_tree.c
 *
 *  Oyranos is an open source Color Management System 
 *
 *  @par Copyright:
 *            2004-2016 (C) Kai-Uwe Behrmann
 *
 *  @brief    public Oyranos API's
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD <http://www.opensource.org/licenses/BSD-3-Clause>
 *  @since    2016/04/14
 */

#include <stddef.h>  /* ptrdiff_t size_t */

#include <oyranos_object_internal.h>

#include "oyObject_s.h"
#include "oyObject_s_.h"

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

/* get a objects directly owned references */
int                oyStruct_GetChildren (
                                       oyStruct_s        * obj,
                                       oyStruct_s      *** list )
{
  #define oy_c_max 120
  static oyStruct_s * c[oy_c_max];
  int n = 0, i = 0;

  if(list) *list = c;

#define CHECK_ASSIGN_STRUCT(struct_name) \
         if(s->struct_name) c[n++] = (oyStruct_s*)s->struct_name;
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
         if(s->value_type == oyVAL_STRUCT && s->value)
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
       {
         oyCMMui_s_ * s = (oyCMMui_s_*)obj;
         CHECK_ASSIGN_STRUCT( parent )
       }
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
         CHECK_ASSIGN_STRUCT( entry )
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
         oyObserver_s * s = (oyObserver_s*)obj;
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
    case oyOBJECT_MAX:
         break;
  }
#undef CHECK_ASSIGN_STRUCT 
  c[n] = NULL;

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
static const int oy_object_list_max_count_ = 1000000;
oyLeave_s ** oy_debug_leave_cache_ = NULL;

/* allocate object and oyLeave_s**children + oyStruct_s**obj arrays */
oyLeave_s *        oyLeave_NewWith   ( oyStruct_s        * obj,
                                       int                 id,
                                       oyLeave_s         * parent,
                                       oyLeave_s         * grandparent,
                                       int               * alloced )
{
  oyLeave_s * l = NULL;
 
  if(!oy_debug_leave_cache_)
  {
    oy_debug_leave_cache_ = (oyLeave_s**) myCalloc_m( sizeof( oyLeave_s* ), oy_object_list_max_count_ + 1 );
  }

  if(oy_debug_leave_cache_[id])
  {
    l = oy_debug_leave_cache_[id];
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

    oy_debug_leave_cache_[id] = l;
  }

  return l;
}

int                oyLeave_Release   ( oyLeave_s        ** leave )
{
  oyLeave_s * l = NULL;
  int error = 0;
 
  if(leave)
    l = *leave;

  if(l == NULL)
    return 1;

  if(oy_debug_leave_cache_[l->id] == NULL)
    error = 1;

  *leave = NULL;

  if(error)
    return error;

  oy_debug_leave_cache_[l->id] = NULL;
  if(oy_debug_memory)
    fprintf( stderr, "%d <- release oyLeave_s(id) " OY_PRINT_POINTER "\n", l->id, (ptrdiff_t)l );

  if(l->children)
    oyFree_m_(l->children);
  if(l->list)
    oyFree_m_(l->list);
  oyFree_m_(l);

  return 0;
}

/* @param          direction           search direction
 * - -1 : search down in children
 * -  0 : search in both directions
 * -  1 : search upward in parents */
int                oyObjectStructTreeContains (
                                       oyLeave_s         * l,
                                       int                 id,
                                       int                 direction )
{
  int i;

  if(l)
  {
    if(direction == 0)
    {
      if(oyObjectStructTreeContains(l, id, -1))
        return 1;
      if(oyObjectStructTreeContains(l, id, 1))
        return 1;
      return 0;
    }

    /* check this leave upward */
    if(direction < 0 && l->parent)
    {
      if(l->parent->id == id)
        return 1;
      if(l->parent &&
         oyObjectStructTreeContains(l->parent, id, -1))
        return 1;
    }

    /* check this leave downward */
    if(direction > 0)
    {
      for(i = 0; i < l->n; ++i)
      {
        if(!l->children[i])
          continue;
        if(l->children[i]->id == id)
          return 1;
        if(oyObjectStructTreeContains(l->children[i], id, 1))
          return 1;
      }
    }
  }

  return 0;
}

static int cntx = 0;
static int cntx_mem = 0;
oyLeave_s *          oyObjectIdListGetStructTree (
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
      int i_id = oyStruct_GetId(l->list[i]);

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
  for(i = 0; i < oy_object_list_max_count_; ++i)
    if(ids[i] > 0)
      ts[n++] = oyObjectIdListGetStructTree( i, 0, 0, ids, i, flags & 0x01 ? -oy_object_list_max_count_ : 0, func, user_data );

  /* TODO: release trees */
  oyFree_m_(ts);

  return n;
}


typedef struct oyTreeData_s {
  oyLeave_s * l;
  char * text;
  char * text2;
  int flags;
} oyTreeData_s;

void oyObjectTreePrintCallback       ( void              * user_data,
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
char * oyObjectTreeDotGraphCallbackGetDescription( oyStruct_s * s )
{
  const char * nick = oyStructTypeToText( s->type_ ),
             * text = oyStruct_GetText( s, oyNAME_DESCRIPTION, 2 );
  char * t, *t2, *t3, *desc = NULL;

  if(!text || strcmp(nick,text) == 0)
    return desc;

  t = oyStringReplace_( text, "\"", "'", 0,0 );
  t2 = oyStringReplace_( t, "\n", "\\n", 0,0 );
  t3 = oyStringReplace_( t2, "<", "\\<", 0,0 );
  desc = oyStringReplace_( t3, ">", "\\>", 0,0 );
  oyFree_m_( t );
  oyFree_m_( t2 );
  oyFree_m_( t3 );

  return desc;
}
void oyObjectTreeDotGraphCallback    ( void              * user_data,
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
  const char * node = "";

  /* Non identifyable objects map to different trees, which is ambiguous. */
  if(id < 0)
    return;

  if(!parent && oyObjectStructTreeContains( tree, id, -1 ))
    /* skip */
    return;

  if(graphs->flags & 0x02)
    desc = oyObjectTreeDotGraphCallbackGetDescription( current );

  /* emphasise with color */
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
    node = " fillcolor=\"SlateBlue\"";
  else
  if(current->type_ == oyOBJECT_IMAGE_S ||
     current->type_ == oyOBJECT_ARRAY2D_S)
    node = " fillcolor=\"orange\"";

  oyStringAddPrintf( &graphs[top].text2, 0,0, "%d [label=\"%s id=%d refs=%d%s%s\"%s];\n",
                     id, oyStructTypeToText( current->type_ ), id, oyObject_GetRefCount(current->oy_), desc?"\\n":"", desc?desc:"", node );
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

#define IS_EDGE_TYPES( t1,t2 ) ((current->type_ == t1 && children[i]->type_ == t2) || (current->type_ == t2 && children[i]->type_ == t1))
    if IS_EDGE_TYPES(oyOBJECT_PIXEL_ACCESS_S,oyOBJECT_FILTER_PLUG_S)
      edge = " [weight=\"3\" color=\"RoyalBlue4\" penwidth=\"3.0\"]"; else
    if IS_EDGE_TYPES(oyOBJECT_CONVERSION_S,oyOBJECT_FILTER_NODE_S)
      edge = " [color=\"RoyalBlue4\" penwidth=\"3.0\"]"; else
    if IS_EDGE_TYPES(oyOBJECT_FILTER_NODE_S,oyOBJECT_FILTER_SOCKET_S)
      edge = " [weight=\"5\" color=\"RoyalBlue4\" penwidth=\"3.0\"]"; else
    if IS_EDGE_TYPES(oyOBJECT_FILTER_SOCKET_S,oyOBJECT_FILTER_PLUGS_S)
      edge = " [weight=\"5\" color=\"RoyalBlue4\" penwidth=\"3.0\"]"; else
    if IS_EDGE_TYPES(oyOBJECT_FILTER_SOCKET_S,oyOBJECT_FILTER_PLUG_S)
      edge = " [weight=\"0\"]"; else
    if IS_EDGE_TYPES(oyOBJECT_FILTER_PLUGS_S,oyOBJECT_STRUCT_LIST_S)
      edge = " [weight=\"5\" color=\"RoyalBlue4\" penwidth=\"3.0\"]"; else
    if (IS_EDGE_TYPES(oyOBJECT_STRUCT_LIST_S,oyOBJECT_FILTER_PLUG_S) && parent && parent->type_ == oyOBJECT_FILTER_PLUGS_S && grandparent && grandparent->type_ == oyOBJECT_FILTER_SOCKET_S)
      edge = " [weight=\"5\" color=\"RoyalBlue4\" penwidth=\"3.0\"]"; else
    if IS_EDGE_TYPES(oyOBJECT_FILTER_PLUG_S,oyOBJECT_FILTER_NODE_S)
      edge = " [weight=\"5\" color=\"RoyalBlue4\" penwidth=\"3.0\"]";
    for(k = 0; k < level; ++k)
      oyStringAddPrintf( &graphs[top].text, 0,0, "- ");
    oyStringAddPrintf( &graphs[top].text, 0,0, "%d -> %d%s;\n",
                       id, i_id, edge );
    if(graphs->flags & 0x02)
      desc = oyObjectTreeDotGraphCallbackGetDescription( children[i] );
    oyStringAddPrintf( &graphs[top].text2, 0,0, "%d [label=\"%s id=%d refs=%d%s%s%s\"];\n",
                       i_id, oyStructTypeToText( children[i]->type_ ), i_id, oyObject_GetRefCount(children[i]->oy_), desc?"\\n\\\"":"", desc?desc:"", desc?"\\\"":"" );
    if(desc) oyFree_m_( desc );

    if(graphs[tree->id].l && graphs[tree->id].l != tree)
    {
      WARNc5_S( "The branch had already a different tree: %d old: %d " OY_PRINT_POINTER " new: %d " OY_PRINT_POINTER,
                top, tree->id, graphs[tree->id].l, tree->id, tree );
    }
    graphs[tree->id].l = tree;
  }
}

int                oyObjectIdListTraverseStructTrees_ (
                                       int               * ids,
                                       oyObjectTreeCallback_f func,
                                       oyTreeData_s      * trees,
                                       int                 flags )
{
  int i, n = 0;
  for(i = 0; i < oy_object_list_max_count_; ++i)
    if(ids[i] > 0)
    {
      trees[i].l = oyObjectIdListGetStructTree( i, 0, 0, ids, i, flags & 0x01 ? -oy_object_list_max_count_ : 0, func, trees );
      ++n;
    }

  return n;
}

oyStruct_s *       oyStruct_FromId   ( int                 id )
{
  const oyObject_s * obs;

  if(id < 0 || oy_object_list_max_count_ < id) /* possibly static objects without oyObject part */
    return NULL;

  obs = oyObjectGetList( NULL );
  if(obs && obs[id])
    return obs[id]->parent_;
  else
    return NULL;
}

/** @brief    Print the current object trees to stderr.
 *  @ingroup  objects_generic
 * 
 *  Probably the most simple and still useful function from the Object tree debug APIs.
 *  It prints to stderr with OY_DEBUG_OBJECTS set.
 *
 *  @param        flags               - 0x01 show a graph
 *                                    - 0x02 include more object details
 *
 *  @version  Oyranos: 0.9.6
 *  @date     2016/04/17
 *  @since    2016/04/16 (Oyranos: 0.9.6)
 */
void               oyObjectTreePrint ( int                 flags )
{
  if(oy_debug_objects >= 0)
  {
    int * ids_old = oyObjectGetCurrentObjectIdList( );
    oyTreeData_s * trees = (oyTreeData_s*) myCalloc_m( sizeof( oyTreeData_s ), oy_object_list_max_count_ + 1 );
    int n, i, count = 0;
    char * dot = 0, * dot_edges = 0;

    if(flags)
      for(i = 0; i < oy_object_list_max_count_; ++i) trees[i].flags = flags;

    if(flags & 0x01)
      n = oyObjectIdListTraverseStructTrees_( ids_old, oyObjectTreeDotGraphCallback, trees, flags );
    else
      n = oyObjectIdListTraverseStructTrees_( ids_old, oyObjectTreePrintCallback, trees, flags );

    for(i = 0; i < oy_object_list_max_count_; ++i)
    {
      if(oy_debug_objects == i)
      {
        oyStruct_s * s = oyStruct_FromId(i);
        if(s)
        {
          const char * nick = oyStructTypeToText( s->type_ ),
                     * text = oyStruct_GetText( s, oyNAME_DESCRIPTION, 2 );
          fprintf( stderr, "present: %s \"%s\" - %s\n", nick, text, trees[i].text);
        }	  
      }
      if(trees[i].text)
      {
        int found = 0, j;
        for(j = 0; j < oy_object_list_max_count_; ++j)
        {
          if(i == j)
            continue;
          if(trees[j].text)
            found = oyObjectStructTreeContains( trees[j].l, i, 1 );
          if(found) break;
        }
        if(oy_debug_objects == i)
          fprintf( stderr, " %s %d in %d\n", found?"found":"found not",i,j);
        if(found == 0)
        {
          if(flags & 0x01)
          {
            oyStringAddPrintf( &dot, 0,0, "    %s", trees[i].text2 );
            oyStringAddPrintf( &dot_edges, 0,0, "    %s", trees[i].text );
          } else
            fprintf(stderr, "%d: %s\n", i, trees[i].text);
          ++count;
        }
      }
      if(trees[i].text)
        oyFree_m_(trees[i].text);
      if(trees[i].text2)
        oyFree_m_(trees[i].text2);
    }
    fprintf( stderr, "found/printed trees: %d/%d\n", n, count);

    oyFree_m_(trees);
    oyFree_m_(ids_old);
    /* check if everything is released */
    for(i = 0; i < oy_object_list_max_count_; ++i)
      if(oy_debug_leave_cache_[i] )
        oyLeave_Release( &oy_debug_leave_cache_[i] );
    oyFree_m_(oy_debug_leave_cache_);

    /* remove double lines */
    {
      int lines_n = 0;
      char ** lines = oyStringSplit_( dot, '\n', &lines_n, 0 );
      char * tmp = 0;
      fprintf(stderr, "dot has number of lines %d\n", lines_n);
      oyStringListFreeDoubles_( lines, &lines_n, 0 );
      fprintf(stderr, "dot has number of unique lines %d\n", lines_n);
      for(i = 0; i < lines_n; ++i)
        oyStringAdd_( &tmp, lines[i], 0,0 );
      oyFree_m_(dot);
      oyStringListRelease_( &lines, lines_n, 0 );
      dot = tmp; tmp = 0;

      lines_n = 0;
      lines = oyStringSplit_( dot_edges, '\n', &lines_n, 0 );
      fprintf(stderr, "dot_edges has number of lines %d\n", lines_n);
      oyStringListFreeDoubles_( lines, &lines_n, 0 );
      fprintf(stderr, "dot_edges has number of unique lines %d\n", lines_n);
      for(i = 0; i < lines_n; ++i)
        oyStringAdd_( &tmp, lines[i], 0,0 );
      oyFree_m_(dot_edges);
      oyStringListRelease_( &lines, lines_n, 0 );
      dot_edges = tmp; tmp = 0;
    }

    if(flags & 0x01)
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
    label=\"Oyranos Object Trees Graph\"\n\
    color=gray;\n");
      if(dot_edges)
        oyStringAddPrintf( &graph,0,0,"%s", dot_edges);
      oyStringAddPrintf( &graph,0,0,"  }\n");
      oyStringAddPrintf( &graph,0,0,"}\n");

      /* write a dot file to disc */
      oyWriteMemToFile_( "oyranos_tree.dot", graph, strlen(graph)+1 );

      /* generate a SVG and send to firefox */
      if(flags & 0x02)
        r = system("dot -Tsvg oyranos_tree.dot -o oyranos_tree.svg && firefox oyranos_tree.svg &");
      else
        r = system("fdp -Tsvg oyranos_tree.dot -o oyranos_tree.svg && firefox oyranos_tree.svg &");

      oyFree_m_( graph );
      oyFree_m_( dot );
      oyFree_m_( dot_edges );
    }
  }
}

