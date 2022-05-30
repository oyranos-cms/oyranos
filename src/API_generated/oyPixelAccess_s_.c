/** @file oyPixelAccess_s_.c

   [Template file inheritance graph]
   +-> oyPixelAccess_s_.template.c
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



  
#include "oyPixelAccess_s.h"
#include "oyPixelAccess_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"


  

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif


static int oy_pixelaccess_init_ = 0;
static char * oy_pixelaccess_msg_text_ = NULL;
static int oy_pixelaccess_msg_text_n_ = 0;
static const char * oyPixelAccess_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyPixelAccess_s_ * s = (oyPixelAccess_s_*) obj;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if( oy_pixelaccess_msg_text_ == NULL || oy_pixelaccess_msg_text_n_ == 0 )
  {
    oy_pixelaccess_msg_text_n_ = 512;
    oy_pixelaccess_msg_text_ = (char*) alloc( oy_pixelaccess_msg_text_n_ );
    if(oy_pixelaccess_msg_text_)
      memset( oy_pixelaccess_msg_text_, 0, oy_pixelaccess_msg_text_n_ );
  }

  if( oy_pixelaccess_msg_text_ == NULL || oy_pixelaccess_msg_text_n_ == 0 )
    return "Memory problem";

  oy_pixelaccess_msg_text_[0] = '\000';

  if(!(flags & 0x01))
    sprintf(oy_pixelaccess_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  
  

  return oy_pixelaccess_msg_text_;
}

static void oyPixelAccess_StaticFree_           ( void )
{
  if(oy_pixelaccess_init_)
  {
    oy_pixelaccess_init_ = 0;
    if(oy_pixelaccess_msg_text_)
      oyFree_m_(oy_pixelaccess_msg_text_);
    if(oy_debug)
      fprintf(stderr, "%s() freeing static \"%s\" memory\n", "oyPixelAccess_StaticFree_", "oyPixelAccess_s" );
  }
}


/* Include "PixelAccess.private_custom_definitions.c" { */
/** Function    oyPixelAccess_Release__Members
 *  @memberof   oyPixelAccess_s
 *  @brief      Custom PixelAccess destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  pixelaccess  the PixelAccess object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyPixelAccess_Release__Members( oyPixelAccess_s_ * pixelaccess )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &pixelaccess->member );
   */
  oyArray2d_Release( &pixelaccess->array );
  oyRectangle_Release( (oyRectangle_s**)&pixelaccess->output_array_roi );
  oyImage_Release( &pixelaccess->output_image );
  oyFilterGraph_Release( (oyFilterGraph_s**)&pixelaccess->graph );
  oyOptions_Release( &pixelaccess->request_queue );

  if(pixelaccess->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = pixelaccess->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( pixelaccess->member );
     */
    if(pixelaccess->user_data && pixelaccess->user_data->release)
        pixelaccess->user_data->release( &pixelaccess->user_data );
    if(pixelaccess->array_xy)
      deallocateFunc( pixelaccess->array_xy );
    pixelaccess->array_xy = 0;
  }
}

/** Function    oyPixelAccess_Init__Members
 *  @memberof   oyPixelAccess_s
 *  @brief      Custom PixelAccess constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  pixelaccess  the PixelAccess object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyPixelAccess_Init__Members( oyPixelAccess_s_ * pixelaccess )
{
  pixelaccess->output_array_roi = (oyRectangle_s_*)oyRectangle_NewFrom( 0, 0 );

  return 0;
}

/** Function    oyPixelAccess_Copy__Members
 *  @memberof   oyPixelAccess_s
 *  @brief      Custom PixelAccess copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyPixelAccess_s_ input object
 *  @param[out]  dst  the output oyPixelAccess_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyPixelAccess_Copy__Members( oyPixelAccess_s_ * dst, oyPixelAccess_s_ * src)
{
  int error = 0, len;
  oyAlloc_f allocateFunc_ = 0;
#if 0
  oyDeAlloc_f deallocateFunc_ = 0;
#endif

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;
#if 0
  deallocateFunc_ = dst->oy_->deallocateFunc_;
#endif

  /* Copy each value of src to dst here */
  dst->start_xy_old[0] = dst->start_xy[0] = src->start_xy[0];
  dst->start_xy_old[1] = dst->start_xy[1] = src->start_xy[1];
  dst->array_n = src->array_n;
  if(src->array_xy && src->array_n)
  {
    len = sizeof(int32_t) * 2 * src->array_n;
    dst->array_xy = allocateFunc_(len);
    error = !dst->array_xy;
    if(error <= 0)
      error = !memcpy(dst->array_xy, src->array_xy, len);
  }
  /* reset to properly initialise the new iterator */
  dst->index = 0;
  dst->pixels_n = src->pixels_n;
  dst->workspace_id = src->workspace_id;
  dst->output_array_roi = (oyRectangle_s_*)oyRectangle_Copy( (oyRectangle_s*)src->output_array_roi, dst->oy_ );
  dst->output_image = oyImage_Copy( src->output_image, 0 );
  dst->array = oyArray2d_Copy( src->array, 0 );
  if(src->user_data && src->user_data->copy)
    dst->user_data = src->user_data->copy( src->user_data, 0 );
  else
    dst->user_data = src->user_data;
  dst->graph = (oyFilterGraph_s_*)oyFilterGraph_Copy( (oyFilterGraph_s*)src->graph, 0 );

  return error;
}

/* } Include "PixelAccess.private_custom_definitions.c" */


/** @internal
 *  Function oyPixelAccess_New_
 *  @memberof oyPixelAccess_s_
 *  @brief   allocate a new oyPixelAccess_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyPixelAccess_s_ * oyPixelAccess_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_PIXEL_ACCESS_S;
  int error = 0, id = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object, "oyPixelAccess_s" );
  oyPixelAccess_s_ * s = 0;

  if(s_obj)
  {
    id = s_obj->id_;
    switch(id) /* give valgrind a glue, which object was created */
    {
      case 1: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 2: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 3: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 4: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 5: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 6: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 7: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 8: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 9: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 10: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 11: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 12: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 13: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 14: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 15: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 16: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 17: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 18: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 19: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 20: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 21: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 22: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 23: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 24: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 25: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 26: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 27: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 28: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 29: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 30: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 31: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 32: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 33: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 34: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 35: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 36: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 37: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 38: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      case 39: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_)); break;
      default: s = (oyPixelAccess_s_*)s_obj->allocateFunc_(sizeof(oyPixelAccess_s_));
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

  error = !memset( s, 0, sizeof(oyPixelAccess_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyPixelAccess_Copy;
  s->release = (oyStruct_Release_f) oyPixelAccess_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom PixelAccess constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_PIXEL_ACCESS_S, (oyPointer)s );
  /* ---- end of custom PixelAccess constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom PixelAccess constructor ----- */
  error += oyPixelAccess_Init__Members( s );
  /* ---- end of custom PixelAccess constructor ------- */
  
  
  
  

  if(!oy_pixelaccess_init_)
  {
    oy_pixelaccess_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyPixelAccess_StaticMessageFunc_,
                                        oyPixelAccess_StaticFree_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyPixelAccess_Copy__
 *  @memberof oyPixelAccess_s_
 *  @brief   real copy a PixelAccess object
 *
 *  @param[in]     pixelaccess                 PixelAccess struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyPixelAccess_s_ * oyPixelAccess_Copy__ ( oyPixelAccess_s_ *pixelaccess, oyObject_s object )
{
  oyPixelAccess_s_ *s = 0;
  int error = 0;

  if(!pixelaccess || !object)
    return s;

  s = (oyPixelAccess_s_*) oyPixelAccess_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom PixelAccess copy constructor ----- */
    error = oyPixelAccess_Copy__Members( s, pixelaccess );
    /* ---- end of custom PixelAccess copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyPixelAccess_Release_( &s );

  return s;
}

/** @internal
 *  Function oyPixelAccess_Copy_
 *  @memberof oyPixelAccess_s_
 *  @brief   copy or reference a PixelAccess object
 *
 *  @param[in]     pixelaccess                 PixelAccess struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyPixelAccess_s_ * oyPixelAccess_Copy_ ( oyPixelAccess_s_ *pixelaccess, oyObject_s object )
{
  oyPixelAccess_s_ * s = pixelaccess;

  if(!pixelaccess)
    return 0;

  if(pixelaccess && !object)
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

  s = oyPixelAccess_Copy__( pixelaccess, object );

  return s;
}
 
/** @internal
 *  Function oyPixelAccess_Release_
 *  @memberof oyPixelAccess_s_
 *  @brief   release and possibly deallocate a PixelAccess object
 *
 *  @param[in,out] pixelaccess                 PixelAccess struct object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/10/23
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyPixelAccess_Release_( oyPixelAccess_s_ **pixelaccess )
{
  const char * track_name = NULL;
  int observer_refs = 0, id = 0, refs = 0, parent_refs = 0;
  /* ---- start of common object destructor ----- */
  oyPixelAccess_s_ *s = 0;

  if(!pixelaccess || !*pixelaccess)
    return 0;

  s = *pixelaccess;
  /* static object */
  if(!s->oy_)
    return 0;

  id = s->oy_->id_;
  refs = s->oy_->ref_;

  if(refs <= 0) /* avoid circular or double dereferencing */
    return 0;

  *pixelaccess = 0;

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
    WARNc2_S( "oyPixelAccess_s[%d]->object can not be untracked with refs: %d\n", id, refs );
    //oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ "refs:%d", OY_DBG_ARGS_, refs);
    return -1; /* issue */
  }

  
  /* ---- start of custom PixelAccess destructor ----- */
  oyPixelAccess_Release__Members( s );
  /* ---- end of custom PixelAccess destructor ------- */
  
  
  
  



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
      fprintf( stderr, "!!!ERROR:%d oyPixelAccess_s[%d]->object can not be untracked with refs: %d\n", __LINE__, id, refs);

    s->oy_ = NULL;
    oyObject_Release( &oy );
    if(track_name)
      fprintf( stderr, "%s[%d] destructed\n", (id == oy_debug_objects)?oyjlTermColor(oyjlRED, track_name):track_name, id );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "PixelAccess.private_methods_definitions.c" { */

/* } Include "PixelAccess.private_methods_definitions.c" */

