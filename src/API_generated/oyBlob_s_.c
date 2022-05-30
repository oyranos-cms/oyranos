/** @file oyBlob_s_.c

   [Template file inheritance graph]
   +-> oyBlob_s_.template.c
   |
   +-- Base_s_.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */



  
#include "oyBlob_s.h"
#include "oyBlob_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"


  

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif


static int oy_blob_init_ = 0;
static char * oy_blob_msg_text_ = NULL;
static int oy_blob_msg_text_n_ = 0;
static const char * oyBlob_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyBlob_s_ * s = (oyBlob_s_*) obj;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if( oy_blob_msg_text_ == NULL || oy_blob_msg_text_n_ == 0 )
  {
    oy_blob_msg_text_n_ = 512;
    oy_blob_msg_text_ = (char*) alloc( oy_blob_msg_text_n_ );
    if(oy_blob_msg_text_)
      memset( oy_blob_msg_text_, 0, oy_blob_msg_text_n_ );
  }

  if( oy_blob_msg_text_ == NULL || oy_blob_msg_text_n_ == 0 )
    return "Memory problem";

  oy_blob_msg_text_[0] = '\000';

  if(!(flags & 0x01))
    sprintf(oy_blob_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  

  
  if(type == oyNAME_NICK && (flags & 0x01))
  {
    sprintf( &oy_blob_msg_text_[strlen(oy_blob_msg_text_)], "%s",
             s->type
           );
  } else
  if(type == oyNAME_NAME)
    sprintf( &oy_blob_msg_text_[strlen(oy_blob_msg_text_)], "%s %lu",
             s->type, (long unsigned int)s->size
           );
  else
  if((int)type >= oyNAME_DESCRIPTION)
    sprintf( &oy_blob_msg_text_[strlen(oy_blob_msg_text_)], "type: %s size: %lu flags: %d",
             s->type, (long unsigned int)s->size, s->flags
           );


  return oy_blob_msg_text_;
}

static void oyBlob_StaticFree_           ( void )
{
  if(oy_blob_init_)
  {
    oy_blob_init_ = 0;
    if(oy_blob_msg_text_)
      oyFree_m_(oy_blob_msg_text_);
    if(oy_debug)
      fprintf(stderr, "%s() freeing static \"%s\" memory\n", "oyBlob_StaticFree_", "oyBlob_s" );
  }
}


/* Include "Blob.private_custom_definitions.c" { */
/** @internal
 *  Function    oyBlob_Release__Members
 *  @memberof   oyBlob_s
 *  @brief      Custom Blob destructor
 *   *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  blob  the Blob object
 *
 *  @version Oyranos: 0.2.1
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyBlob_Release__Members( oyBlob_s_ * blob )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &blob->member );
   */

  if(blob->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = blob->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( blob->member );
     */

    if(blob->ptr && !(blob->flags & 0x01))
      deallocateFunc( blob->ptr );
    blob->size = 0;
  }
}

/** @internal
 *  Function    oyBlob_Init__Members
 *  @memberof   oyBlob_s
 *  @brief      Custom Blob constructor 
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  blob  the Blob object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyBlob_Init__Members( oyBlob_s_ * blob )
{
  blob->ptr = NULL;

  return 0;
}

/** @internal
 *  Function    oyBlob_Copy__Members
 *  @memberof   oyBlob_s
 *  @brief      Custom Blob copy constructor
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyBlob_s_ input object
 *  @param[out]  dst  the output oyBlob_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyBlob_Copy__Members( oyBlob_s_ * dst, oyBlob_s_ * src)
{
  oyAlloc_f allocateFunc_ = 0;
  int error = 0;

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;

  /* Copy each value of src to dst here */
  if(error <= 0)
  {
    allocateFunc_ = dst->oy_->allocateFunc_;

    if(src->ptr && src->size && !(src->flags & 0x01))
    {
      dst->ptr = allocateFunc_( src->size );
      error = !dst->ptr;
      if(error <= 0)
        error = !memcpy( dst->ptr, src->ptr, src->size );
    } else
      dst->ptr = src->ptr;
  }

  if(error <= 0)
  {
    dst->size = src->size;
    dst->flags = src->flags;
    error = !memcpy( dst->type, src->type, 8 );
  }

  return error;
}

/* } Include "Blob.private_custom_definitions.c" */


/** @internal
 *  Function oyBlob_New_
 *  @memberof oyBlob_s_
 *  @brief   allocate a new oyBlob_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyBlob_s_ * oyBlob_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_BLOB_S;
  int error = 0, id = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object, "oyBlob_s" );
  oyBlob_s_ * s = 0;

  if(s_obj)
  {
    id = s_obj->id_;
    switch(id) /* give valgrind a glue, which object was created */
    {
      case 1: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 2: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 3: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 4: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 5: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 6: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 7: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 8: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 9: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 10: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 11: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 12: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 13: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 14: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 15: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 16: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 17: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 18: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 19: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 20: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 21: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 22: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 23: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 24: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 25: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 26: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 27: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 28: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 29: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 30: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 31: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 32: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 33: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 34: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 35: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 36: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 37: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 38: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      case 39: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_)); break;
      default: s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_));
    }
  }
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

  error = !memset( s, 0, sizeof(oyBlob_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyBlob_Copy;
  s->release = (oyStruct_Release_f) oyBlob_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom Blob constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_BLOB_S, (oyPointer)s );
  /* ---- end of custom Blob constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom Blob constructor ----- */
  error += oyBlob_Init__Members( s );
  /* ---- end of custom Blob constructor ------- */
  
  
  
  

  if(!oy_blob_init_)
  {
    oy_blob_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyBlob_StaticMessageFunc_,
                                        oyBlob_StaticFree_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyBlob_Copy__
 *  @memberof oyBlob_s_
 *  @brief   real copy a Blob object
 *
 *  @param[in]     blob                 Blob struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyBlob_s_ * oyBlob_Copy__ ( oyBlob_s_ *blob, oyObject_s object )
{
  oyBlob_s_ *s = 0;
  int error = 0;

  if(!blob || !object)
    return s;

  s = (oyBlob_s_*) oyBlob_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom Blob copy constructor ----- */
    error = oyBlob_Copy__Members( s, blob );
    /* ---- end of custom Blob copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyBlob_Release_( &s );

  return s;
}

/** @internal
 *  Function oyBlob_Copy_
 *  @memberof oyBlob_s_
 *  @brief   copy or reference a Blob object
 *
 *  @param[in]     blob                 Blob struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyBlob_s_ * oyBlob_Copy_ ( oyBlob_s_ *blob, oyObject_s object )
{
  oyBlob_s_ * s = blob;

  if(!blob)
    return 0;

  if(blob && !object)
  {
    if(oy_debug_objects >= 0 && s->oy_)
    {
      const char * t = getenv(OY_DEBUG_OBJECTS);
      int id_ = -1;

      if(t)
        id_ = atoi(t);
      else
        id_ = oy_debug_objects;

      if((id_ >= 0 && s->oy_->id_ == id_) ||
         (t && s && (strstr(oyStructTypeToText(s->type_), t) != 0)) ||
         id_ == 1)
      {
        oyStruct_s ** parents = NULL;
        int n = oyStruct_GetParents( (oyStruct_s*)s, &parents ),
            observer_refs = oyStruct_ObservationCount( (oyStruct_s*)s, 0 ),
            i;
        const char * track_name = oyStructTypeToText(s->type_);
        if(!(track_name && track_name[0]))
          track_name = oyStruct_GetInfo( s, oyNAME_NICK, 0x01 );
        if(s->oy_->id_ == id_)
          for( i = 0; i < s->oy_->ref_ - observer_refs - n; ++i)
            fprintf( stderr, "  " );
        fprintf( stderr, "%s[%d] tracking refs: %d++ observers: %d parents: %d\n",
                 (s->oy_->id_ == id_)?oyjlTermColor(oyjlGREEN, track_name):track_name, s->oy_->id_, s->oy_->ref_, observer_refs, n );
        for(i = 0; i < n; ++i)
        {
          track_name = oyStructTypeToText(parents[i]->type_);
          if(!(track_name && track_name[0]))
            track_name = oyStruct_GetInfo( parents[i], oyNAME_NICK, 0x01 );
          if(s->oy_->id_ == id_)
          {
            int i;
            for( i = 0; i < s->oy_->ref_ - observer_refs - n; ++i)
              fprintf( stderr, "  " );
          }
          fprintf( stderr, "parent[%d]: %s[%d]\n", i,
                   track_name, parents[i]->oy_->id_ );
        }
      }
    }
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyBlob_Copy__( blob, object );

  return s;
}
 
/** @internal
 *  Function oyBlob_Release_
 *  @memberof oyBlob_s_
 *  @brief   release and possibly deallocate a Blob object
 *
 *  @param[in,out] blob                 Blob struct object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/10/23
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyBlob_Release_( oyBlob_s_ **blob )
{
  const char * track_name = NULL;
  int observer_refs = 0, id = 0, refs = 0, parent_refs = 0;
  /* ---- start of common object destructor ----- */
  oyBlob_s_ *s = 0;

  if(!blob || !*blob)
    return 0;

  s = *blob;
  /* static object */
  if(!s->oy_)
    return 0;

  id = s->oy_->id_;
  refs = s->oy_->ref_;

  if(refs <= 0) /* avoid circular or double dereferencing */
    return 0;

  *blob = 0;

  observer_refs = oyStruct_ObservationCount( (oyStruct_s*)s, 0 );

  if(oy_debug_objects >= 0)
  {
    const char * t = getenv(OY_DEBUG_OBJECTS);
    int id_ = -1;

    if(t)
      id_ = atoi(t);
    else
      id_ = oy_debug_objects;

    if((id_ >= 0 && s->oy_->id_ == id_) ||
       (t && (strstr(oyStructTypeToText(s->type_), t) != 0)) ||
       id_ == 1)
    {
      oyStruct_s ** parents = NULL;
      int n = oyStruct_GetParents( (oyStruct_s*)s, &parents );
      {
        int i;
        track_name = oyStructTypeToText(s->type_);
        if(!(track_name && track_name[0]))
          track_name = oyStruct_GetInfo( s, oyNAME_NICK, 0x01 );
        if(s->oy_->id_ == id_)
          for( i = 0; i < s->oy_->ref_ - observer_refs - n; ++i)
            fprintf( stderr, "  " );
        fprintf( stderr, "%s[%d] unref with refs: %d-- observers: %d parents: %d\n",
                 (s->oy_->id_ == id_)?oyjlTermColor(oyjlRED, track_name):track_name, s->oy_->id_, s->oy_->ref_, observer_refs, n );
        for(i = 0; i < n; ++i)
        {
          track_name = oyStructTypeToText(parents[i]->type_);
          if(!(track_name && track_name[0]))
            track_name = oyStruct_GetInfo( parents[i], oyNAME_NICK, 0x01 );
          if(s->oy_->id_ == id_)
          {
            int i;
            for( i = 0; i < s->oy_->ref_ - observer_refs - n; ++i)
              fprintf( stderr, "  " );
          }
          fprintf( stderr, "parent[%d]: %s[%d]\n", i,
                   track_name, parents[i]->oy_->id_ );
        }
      }
    }
  }

  
  if((oyObject_UnRef(s->oy_) - parent_refs - 2*observer_refs) > 0)
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
       (t && s && (strstr(oyStructTypeToText(s->type_), t) != 0)) ||
       id_ == 1)
    {
      track_name = oyStructTypeToText(s->type_);
      if(!(track_name && track_name[0]))
        track_name = oyStruct_GetInfo( s, oyNAME_NICK, 0x01 );
      fprintf( stderr, "%s[%d] destruct\n", (s->oy_->id_ == id_)?oyjlTermColor(oyjlRED, track_name):track_name, s->oy_->id_);
    }
  }

  refs = s->oy_->ref_;
  if(refs < 0)
  {
    WARNc2_S( "oyBlob_s[%d]->object can not be untracked with refs: %d\n", id, refs );
    //oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ "refs:%d", OY_DBG_ARGS_, refs);
    return -1; /* issue */
  }

  
  /* ---- start of custom Blob destructor ----- */
  oyBlob_Release__Members( s );
  /* ---- end of custom Blob destructor ------- */
  
  
  
  



  /* remove observer edges */
  oyOptions_Release( &s->oy_->handles_ );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;
    oyObject_s oy = s->oy_;

    refs = s->oy_->ref_;

    if(track_name)
      fprintf( stderr, "%s[%d] destructing\n", (s->oy_->id_ == oy_debug_objects)?oyjlTermColor(oyjlRED, track_name):track_name, id );

    if(refs > 1)
      fprintf( stderr, "!!!ERROR:%d oyBlob_s[%d]->object can not be untracked with refs: %d\n", __LINE__, id, refs);

    s->oy_ = NULL;
    oyObject_Release( &oy );
    if(track_name)
      fprintf( stderr, "%s[%d] destructed\n", (id == oy_debug_objects)?oyjlTermColor(oyjlRED, track_name):track_name, id );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "Blob.private_methods_definitions.c" { */

/* } Include "Blob.private_methods_definitions.c" */

