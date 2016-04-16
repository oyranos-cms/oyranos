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
/* get a objects directly owned references */
int                oyStruct_GetChildren (
                                       oyStruct_s        * obj,
                                       oyStruct_s      *** list )
{
  static oyStruct_s * c[32];
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
    case oyOBJECT_CMM_API_FILTER_S:
    case oyOBJECT_CMM_API_FILTERS_S:
    case oyOBJECT_CMM_UI_S:
    case oyOBJECT_CMM_OBJECT_TYPE_S:
    case oyOBJECT_CMM_API_MAX:
    case oyOBJECT_ICON_S:
    case oyOBJECT_MODULE_S:
    case oyOBJECT_EXTERNFUNC_S:
    case oyOBJECT_NAME_S:
    case oyOBJECT_COMP_S_:
    case oyOBJECT_FILE_LIST_S_:
    case oyOBJECT_HASH_S:
    case oyOBJECT_STRUCT_LIST_S:
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

typedef struct leave_s leave_s;
struct leave_s {
  /* top down */
  int n;
  leave_s ** children;
  oyStruct_s ** list;
  /* current */
  oyStruct_s * obj;
  int id;
  /* upper level */
  leave_s * parent;
};

int                oyObjectStructTreeContains (
                                       leave_s           * l,
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

static const int oy_object_list_max_count_ = 1000000;
leave_s *          oyObjectIdListGetStructTree (
                                       leave_s           * parent,
                                       int               * ids,
                                       int                 id,
                                       int                 level )
{
  int max_count = 0, i;
  const oyObject_s * obs;
  oyStruct_s * obj,
            ** slist;
  leave_s * l = calloc( sizeof(leave_s), 1 );

  if(id < 0) /* possibly static objects without oyObject part */
    return l;

  obs = oyObjectGetList( &max_count );
  obj = obs[id]->parent_;
  l = calloc( sizeof(leave_s), 1 );
  l->obj = obj;
  l->id = id;
  l->parent = parent;

  if(oyObjectStructTreeContains(l, id, -1))
  {
    free(l);
    l  = NULL;
    return l;
  }

  l->n = oyStruct_GetChildren( obj, &slist );
  if(l->n)
  {
    l->list = calloc( sizeof(oyStruct_s*), l->n + 1 );
    memcpy( l->list, slist, sizeof(oyStruct_s*) * l->n );
    l->children = calloc( sizeof( leave_s* ), l->n + 1 );
    for(i = 0; i < l->n; ++i)
    {
      int i_id = oyStruct_GetId(l->list[i]);

      l->children[i] = oyObjectIdListGetStructTree( l, ids, i_id, level+1 );
      /* remember the parent to traverse the actual tree */
      if(l->children[i])
      {
        int k;
        l->children[i]->parent = l;
        for(k = 0; k < level; ++k)
          printf("- ");
        printf("%s[%d] -> %s[%d]\n", oyStructTypeToText(obj->type_), id, oyStruct_GetText(l->list[i], oyNAME_NICK, 1), i_id);
      }
    }
  }
  return l;
}

int                oyObjectIdListGetStructTrees (
                                       int               * ids,
                                       leave_s         *** trees )
{
  int i, n = 0;
  leave_s ** ts = calloc( sizeof( leave_s* ), oy_object_list_max_count_ );
  for(i = 0; i < oy_object_list_max_count_; ++i)
    if(ids[i] > 0)
      ts[n++] = oyObjectIdListGetStructTree( 0, ids, i, 0 );

  *trees = ts;
  return n;
}


