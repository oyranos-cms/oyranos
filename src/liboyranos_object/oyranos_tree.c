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
         int n_max = (s->n_ > oy_c_max) ? oy_c_max : s->n_;
         if(s->ptr_ && s->n_)
         {
           memcpy(&c[n], s->ptr_, sizeof(oyStruct_s*)*n_max);
           n += n_max;
         }
       }
       break;
    case oyOBJECT_BLOB_S:
    case oyOBJECT_CONFIG_S:
    case oyOBJECT_CONFIGS_S:
    case oyOBJECT_UI_HANDLER_S:
    case oyOBJECT_FORMS_ARGS_S:
    case oyOBJECT_CALLBACK_S:
    case oyOBJECT_OBSERVER_S:
    case oyOBJECT_CONF_DOMAIN_S:
    case oyOBJECT_INFO_STATIC_S:
    case oyOBJECT_LIST_S:
    case oyOBJECT_LIS_S:
    case oyOBJECT_MAX:
         break;
  }
#undef CHECK_ASSIGN_STRUCT 
  c[n] = NULL;

  return n;
}

typedef struct oyLeave_s oyLeave_s;
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
};

int                oyObjectStructTreeContains (
                                       oyLeave_s         * l,
                                       int                 id,
                                       int                 direction )
{
  int i;

  if(l)
  {
    /* check this leave upward */
    if(direction <= 0 && l->parent)
    {
      if(l->parent->id == id)
        return 1;
      if(l->parent &&
         oyObjectStructTreeContains(l->parent, id, -1))
        return 1;
    }

    /* check this leave downward */
    if(direction >= 0)
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
typedef void (*oyObjectTreeCallback_f)(void              * user_data,
                                       int                 top,
                                       oyLeave_s         * tree,
                                       oyStruct_s        * parent,
                                       oyStruct_s        * current,
                                       oyStruct_s       ** children,
                                       int                 children_n,
                                       int                 level );
#define myCalloc(x,n) myCalloc_(x,n); if(oy_debug_memory) printf(OY_DBG_FORMAT_" %lu * %d bytes\n",OY_DBG_ARGS_, x,n);
static void * myCalloc_( size_t size, size_t n )
{ void * mem = oyAllocateFunc_( size*n );
  memset(mem,0,size*n);
  return mem;
}
static const int oy_object_list_max_count_ = 1000000;
static int cntx = 0;
static int cntx_mem = 0;
oyLeave_s *          oyObjectIdListGetStructTree (
                                       int                 top,
                                       oyLeave_s         * parent,
                                       int               * ids,
                                       int                 id,
                                       int                 level,
                                       oyObjectTreeCallback_f func,
                                       void              * user_data )
{
  int max_count = 0, i;
  const oyObject_s * obs;
  oyStruct_s * obj,
            ** slist;
  oyLeave_s * l = 0;

  if(id < 0) /* possibly static objects without oyObject part */
    return l;

  obs = oyObjectGetList( &max_count );
  obj = obs[id]->parent_;
  l = myCalloc( sizeof(oyLeave_s), 1 );
  l->obj = obj;
  l->id = id;
  l->parent = parent;

  cntx_mem += sizeof(oyLeave_s);
  if(oy_debug_memory) printf("%d %d mem: %d\n", cntx++, level, cntx_mem);

  if(oyObjectStructTreeContains(l, id, -1))
  {
    oyDeAllocateFunc_(l);
    l  = NULL;
    return l;
  }

  l->n = oyStruct_GetChildren( obj, &slist );
  if(l->n)
  {
    l->list = myCalloc( sizeof(oyStruct_s*), l->n + 1 );
    memcpy( l->list, slist, sizeof(oyStruct_s*) * l->n );
    cntx_mem += sizeof(oyLeave_s) * l->n;
    l->children = myCalloc( sizeof( oyLeave_s* ), l->n + 1 );
    for(i = 0; i < l->n; ++i)
    {
      int i_id = oyStruct_GetId(l->list[i]);

      l->children[i] = oyObjectIdListGetStructTree( top, l, ids, i_id, level+1, func, user_data );
      /* remember the parent to traverse the actual tree */
      if(l->children[i])
        l->children[i]->parent = l;
    }
  }
  if(func) func( user_data, top, l, l->parent?l->parent->obj:NULL, l->obj, l->list, l->n, level );
  return l;
}

int                oyObjectIdListTraverseStructTrees (
                                       int               * ids,
                                       oyObjectTreeCallback_f func,
                                       void              * user_data,
                                       int                 flags )
{
  int i, n = 0;
  oyLeave_s ** ts = myCalloc( sizeof( oyLeave_s* ), oy_object_list_max_count_ );
  for(i = 0; i < oy_object_list_max_count_; ++i)
    if(ids[i] > 0)
      ts[n++] = oyObjectIdListGetStructTree( i, 0, ids, i, flags & 0x01 ? -oy_object_list_max_count_ : 0, func, user_data );

  /* TODO: release trees */

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
    graphs[top].l = tree;
  }
}
char * oyObjectTreeDotGraphCallbackGetDescription( oyStruct_s * s )
{
  const char * nick = oyStructTypeToText( s->type_ ),
             * text = oyStruct_GetText( s, oyNAME_NICK, 1 );
  char * t, *t2, *t3, *desc = NULL;

  if(strstr(nick,"Conversion"))
    /* make it *big* */
    oyStringAddPrintf( &desc, 0,0, "\\n\\n"
                                   "############################\\n"
                                   "########## Start ###########\\n"
                                   "############################\\n" );

  if(strcmp(nick,text) == 0)
    return desc;

  t = oyStringReplace_( text, "\"", "'", 0 );
  t2 = oyStringReplace_( t, "\n", "\\n", 0 );
  t3 = oyStringReplace_( t2, "<", "\\<", 0 );
  desc = oyStringReplace_( t3, ">", "\\>", 0 );
  oyFree_m_( t );
  oyFree_m_( t2 );
  oyFree_m_( t3 );

  return desc;
}
void oyObjectTreeDotGraphCallback    ( void              * user_data,
                                       int                 top,
                                       oyLeave_s         * tree,
                                       oyStruct_s        * parent,
                                       oyStruct_s        * current,
                                       oyStruct_s       ** children,
                                       int                 children_n,
                                       int                 level )
{
  oyTreeData_s * graphs = (oyTreeData_s*) user_data;
  int id = oyStruct_GetId(current), i,k;
  char * desc = 0;
  const char * color = "";

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
    color = " color=\"white\"";

  oyStringAddPrintf( &graphs[top].text2, 0,0, "%d [label=\"%s id=%d refs=%d%s%s\"%s];\n",
                     id, oyStructTypeToText( current->type_ ), id, oyObject_GetRefCount(current->oy_), desc?"\\n":"", desc?desc:"", color );
  if(desc) oyFree_m_( desc );

  for(i = 0; i < children_n; ++i)
  {
    int i_id;
    if(!children[i])
      continue;

    i_id = oyStruct_GetId( children[i] );
    if(i_id < 0)
      continue;

    for(k = 0; k < level; ++k)
      oyStringAddPrintf( &graphs[top].text, 0,0, "- ");
    oyStringAddPrintf( &graphs[top].text, 0,0, "%d -> %d;\n",
                       id, i_id );
    if(graphs->flags & 0x02)
      desc = oyObjectTreeDotGraphCallbackGetDescription( children[i] );
    oyStringAddPrintf( &graphs[top].text2, 0,0, "%d [label=\"%s id=%d refs=%d%s%s%s\"];\n",
                       i_id, oyStructTypeToText( children[i]->type_ ), i_id, oyObject_GetRefCount(children[i]->oy_), desc?"\\n\\\"":"", desc?desc:"", desc?"\\\"":"" );
    if(desc) oyFree_m_( desc );
    graphs[top].l = tree;
  }
}


/** @brief    Print the current object trees to stderr.
 *  @ingroup  objects_generic
 * 
 *  Probably the most simple and still useful function from the Object tree debug APIs.
 *  It prints to stderr with OY_DEBUG_OBJECTS set.
 *
 *  @params       flags               - 0x01 show a graph
 *                                    - 0x02 include more object details
 *
 *  @version  Oyranos: 0.9.6
 *  @date     2016/04/17
 *  @since    2016/04/16 (Oyranos: 0.9.6)
 */
void               oyObjectTreePrint ( int                 flags )
{
  if(oy_debug_objects)
  {
    int * ids_old = oyObjectGetCurrentObjectIdList( );
    oyTreeData_s * trees = (oyTreeData_s*) myCalloc( sizeof( oyTreeData_s ), oy_object_list_max_count_ + 1 );
    int n, i, count = 0;
    char * dot = 0, * dot_edges = 0;

    if(flags)
      for(i = 0; i < oy_object_list_max_count_; ++i) trees[i].flags = flags;

    if(flags & 0x01)
      n = oyObjectIdListTraverseStructTrees( ids_old, oyObjectTreeDotGraphCallback, trees, flags );
    else
      n = oyObjectIdListTraverseStructTrees( ids_old, oyObjectTreePrintCallback, trees, flags );

    for(i = 0; i < oy_object_list_max_count_; ++i)
      if(trees[i].text)
      {
        int found = 0, j;
        for(j = 0; j < oy_object_list_max_count_; ++j)
        {
          if(trees[j].text)
            found = oyObjectStructTreeContains( trees[j].l, i, 0 );
          if(found) break;
        }
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
    fprintf( stderr, "found/printed trees: %d/%d\n", n, count);

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

      oyStringAddPrintf( &graph,0,0, "%s\n", dot);
      oyStringAddPrintf( &graph,0,0,"\
  subgraph cluster_0 {\n\
    label=\"Oyranos Object Trees Graph\"\n\
    color=gray;\n");
      oyStringAddPrintf( &graph,0,0,"%s", dot_edges);
      oyStringAddPrintf( &graph,0,0,"  }\n");
      oyStringAddPrintf( &graph,0,0,"}\n");

      /* write a dot file to disc */
      oyWriteMemToFile_( "oyranos_tree.dot", graph, strlen(graph)+1 );

      /* generate a SVG and send to firefox */
      if(flags & 0x02)
        system("dot -Tsvg oyranos_tree.dot -o oyranos_tree.svg && firefox oyranos_tree.svg &");
      else
        system("fdp -Tsvg oyranos_tree.dot -o oyranos_tree.svg && firefox oyranos_tree.svg &");

      oyFree_m_( graph );
      oyFree_m_( dot );
      oyFree_m_( dot_edges );
    }
  }
}
