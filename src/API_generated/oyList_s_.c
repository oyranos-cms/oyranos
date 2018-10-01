/** @file oyList_s_.c

   [Template file inheritance graph]
   +-> oyList_s_.template.c
   |
   +-> BaseList_s_.c
   |
   +-- Base_s_.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2018 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */



  
#include "oyList_s.h"
#include "oyList_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyranos_generic_internal.h"
  


static int oy_list_init_ = 0;
static const char * oyList_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyList_s_ * s = (oyList_s_*) obj;
  static char * text = 0;
  static int text_n = 0;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if(s->oy_ && s->oy_->allocateFunc_)
    alloc = s->oy_->allocateFunc_;

  if( text == NULL || text_n == 0 )
  {
    text_n = 512;
    text = (char*) alloc( text_n );
    if(text)
      memset( text, 0, text_n );
  }

  if( text == NULL || text_n == 0 )
    return "Memory problem";

  text[0] = '\000';

  if(!(flags & 0x01))
    sprintf(text, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  
  

  return text;
}


/* Include "List.private_custom_definitions.c" { */
/** Function    oyList_Release__Members
 *  @memberof   oyList_s
 *  @brief      Custom List destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  list  the List object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyList_Release__Members( oyList_s_ * list OY_UNUSED )
{
}

/** Function    oyList_Init__Members
 *  @memberof   oyList_s
 *  @brief      Custom List constructor
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  list  the List object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyList_Init__Members( oyList_s_ * list OY_UNUSED )
{
  return 0;
}

/** Function    oyList_Copy__Members
 *  @memberof   oyList_s
 *  @brief      Custom List copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyList_s_ input object
 *  @param[out]  dst  the output oyList_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyList_Copy__Members( oyList_s_ * dst OY_UNUSED, oyList_s_ * src OY_UNUSED)
{
  return 0;
}

/* } Include "List.private_custom_definitions.c" */


/** @internal
 *  Function oyList_New_
 *  @memberof oyList_s_
 *  @brief   allocate a new oyList_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyList_s_ * oyList_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_LIST_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyList_s_ * s = 0;

  if(s_obj)
    s = (oyList_s_*)s_obj->allocateFunc_(sizeof(oyList_s_));
  else
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  if(!s)
  {
    if(s_obj)
      oyObject_Release( &s_obj );
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(oyList_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyList_Copy;
  s->release = (oyStruct_Release_f) oyList_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom List constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_LIST_S, (oyPointer)s );
  /* ---- end of custom List constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  s->list_ = oyStructList_Create( s->type_, 0, 0 );


  
  /* ---- start of custom List constructor ----- */
  error += oyList_Init__Members( s );
  /* ---- end of custom List constructor ------- */
  
  
  
  

  if(!oy_list_init_)
  {
    oy_list_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyList_StaticMessageFunc_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyList_Copy__
 *  @memberof oyList_s_
 *  @brief   real copy a List object
 *
 *  @param[in]     list                 List struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyList_s_ * oyList_Copy__ ( oyList_s_ *list, oyObject_s object )
{
  oyList_s_ *s = 0;
  int error = 0;

  if(!list || !object)
    return s;

  s = (oyList_s_*) oyList_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom List copy constructor ----- */
    error = oyList_Copy__Members( s, list );
    /* ---- end of custom List copy constructor ------- */
    
    
    
    
    
    s->list_ = oyStructList_Copy( list->list_, s->oy_ );

  }

  if(error)
    oyList_Release_( &s );

  return s;
}

/** @internal
 *  Function oyList_Copy_
 *  @memberof oyList_s_
 *  @brief   copy or reference a List object
 *
 *  @param[in]     list                 List struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyList_s_ * oyList_Copy_ ( oyList_s_ *list, oyObject_s object )
{
  oyList_s_ *s = list;

  if(!list)
    return 0;

  if(list && !object)
  {
    s = list;
    
    if(oy_debug_objects >= 0 && s->oy_)
    {
      const char * t = getenv(OY_DEBUG_OBJECTS);
      int id_ = -1;

      if(t)
        id_ = atoi(t);
      else
        id_ = oy_debug_objects;

      if((id_ >= 0 && s->oy_->id_ == id_) ||
         (t && s && strstr(oyStructTypeToText(s->type_), t) != 0) ||
         id_ == 1)
      {
        oyStruct_s ** parents = NULL;
        int n = oyStruct_GetParents( (oyStruct_s*)s, &parents );
        if(n != s->oy_->ref_)
        {
          int i;
          const char * track_name = oyStructTypeToText(s->type_);
          fprintf( stderr, "%s[%d] tracking refs: %d parents: %d\n",
                   track_name, s->oy_->id_, s->oy_->ref_, n );
          for(i = 0; i < n; ++i)
          {
            track_name = oyStructTypeToText(parents[i]->type_);
            fprintf( stderr, "parent[%d]: %s[%d]\n", i,
                     track_name, parents[i]->oy_->id_ );
          }
        }
      }
    }
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyList_Copy__( list, object );

  return s;
}
 
/** @internal
 *  Function oyList_Release_
 *  @memberof oyList_s_
 *  @brief   release and possibly deallocate a List list
 *
 *  @param[in,out] list                 List struct object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyList_Release_( oyList_s_ **list )
{
  const char * track_name = NULL;
  int observer_refs = 0, i;
  /* ---- start of common object destructor ----- */
  oyList_s_ *s = 0;

  if(!list || !*list)
    return 0;

  s = *list;

  *list = 0;

  observer_refs = oyStruct_ObservedModelCount( (oyStruct_s*)s );

  if(oy_debug_objects >= 0 && s->oy_)
  {
    const char * t = getenv(OY_DEBUG_OBJECTS);
    int id_ = -1;

    if(t)
      id_ = atoi(t);
    else
      id_ = oy_debug_objects;

    if((id_ >= 0 && s->oy_->id_ == id_) ||
       (t && s && strstr(oyStructTypeToText(s->type_), t) != 0) ||
       id_ == 1)
    {
      oyStruct_s ** parents = NULL;
      int n = oyStruct_GetParents( (oyStruct_s*)s, &parents );
      if(n != s->oy_->ref_)
      {
        int i;
        track_name = oyStructTypeToText(s->type_);
        fprintf( stderr, "%s[%d] unref with refs: %d observers: %d parents: %d\n",
                 track_name, s->oy_->id_, s->oy_->ref_, observer_refs, n );
        for(i = 0; i < n; ++i)
        {
          track_name = oyStructTypeToText(parents[i]->type_);
          fprintf( stderr, "parent[%d]: %s[%d]\n", i,
                   track_name, parents[i]->oy_->id_ );
        }
      }
    }
  }

  
  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  if(oy_debug_objects >= 0)
  {
    const char * t = getenv(OY_DEBUG_OBJECTS);
    int id_ = -1;

    if(t)
      id_ = atoi(t);
    else
      id_ = oy_debug_objects;

    if((id_ >= 0 && s->oy_->id_ == id_) ||
       (t && s && strstr(oyStructTypeToText(s->type_), t) != 0) ||
       id_ == 1)
    {
      track_name = oyStructTypeToText(s->type_);
      fprintf( stderr, "%s[%d] destruct\n", track_name, s->oy_->id_);
    }
  }

  
  /* ---- start of custom List destructor ----- */
  oyList_Release__Members( s );
  /* ---- end of custom List destructor ------- */
  
  
  
  

  oyStructList_Release( &s->list_ );


  /* model and observer reference each other. So release the object two times.
   * The models and and observers are released later inside the
   * oyObject_s::handles. */
  for(i = 0; i < observer_refs; ++i)
  {
    oyObject_UnRef(s->oy_);
    oyObject_UnRef(s->oy_);
  }

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;
    int id = s->oy_->id_;
    int refs = s->oy_->ref_;

    if(refs > 1)
      fprintf( stderr, "!!!ERROR: node[%d]->object can not be untracked with refs: %d\n", id, refs);

    oyObject_Release( &s->oy_ );
    if(track_name)
      fprintf( stderr, "%s[%d] destructed\n", track_name, id );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "List.private_methods_definitions.c" { */

/* } Include "List.private_methods_definitions.c" */

