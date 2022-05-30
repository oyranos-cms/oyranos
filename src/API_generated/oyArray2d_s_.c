/** @file oyArray2d_s_.c

   [Template file inheritance graph]
   +-> oyArray2d_s_.template.c
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



  
#include "oyArray2d_s.h"
#include "oyArray2d_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"


  

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif


static int oy_array2d_init_ = 0;
static char * oy_array2d_msg_text_ = NULL;
static int oy_array2d_msg_text_n_ = 0;
static const char * oyArray2d_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyArray2d_s_ * s = (oyArray2d_s_*) obj;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if( oy_array2d_msg_text_ == NULL || oy_array2d_msg_text_n_ == 0 )
  {
    oy_array2d_msg_text_n_ = 512;
    oy_array2d_msg_text_ = (char*) alloc( oy_array2d_msg_text_n_ );
    if(oy_array2d_msg_text_)
      memset( oy_array2d_msg_text_, 0, oy_array2d_msg_text_n_ );
  }

  if( oy_array2d_msg_text_ == NULL || oy_array2d_msg_text_n_ == 0 )
    return "Memory problem";

  oy_array2d_msg_text_[0] = '\000';

  if(!(flags & 0x01))
    sprintf(oy_array2d_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  

  
  if(type == oyNAME_NICK && (flags & 0x01))
    sprintf( &oy_array2d_msg_text_[strlen(oy_array2d_msg_text_)], "%dx%d", s->width, s->height);
  else
  if(type == oyNAME_NAME)
    sprintf( &oy_array2d_msg_text_[strlen(oy_array2d_msg_text_)], "(%dx%d)%dt", s->width, s->height, s->t);
  else
  if((int)type >= oyNAME_DESCRIPTION)
    sprintf( &oy_array2d_msg_text_[strlen(oy_array2d_msg_text_)], "(%dx%d)%s", s->width, s->height, oyDataTypeToText(s->t));


  return oy_array2d_msg_text_;
}

static void oyArray2d_StaticFree_           ( void )
{
  if(oy_array2d_init_)
  {
    oy_array2d_init_ = 0;
    if(oy_array2d_msg_text_)
      oyFree_m_(oy_array2d_msg_text_);
    if(oy_debug)
      fprintf(stderr, "%s() freeing static \"%s\" memory\n", "oyArray2d_StaticFree_", "oyArray2d_s" );
  }
}


/* Include "Array2d.private_custom_definitions.c" { */
#include "oyranos_image_internal.h"

/** Function    oyArray2d_Release__Members
 *  @memberof   oyArray2d_s
 *  @brief      Custom Array2d destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  array2d  the Array2d object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyArray2d_Release__Members( oyArray2d_s_ * array2d )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &array2d->member );
   */
  oyArray2d_ReleaseArray_( (oyArray2d_s*)array2d );

  if(array2d->oy_->deallocateFunc_)
  {
#if 0
    oyDeAlloc_f deallocateFunc = array2d->oy_->deallocateFunc_;
#endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( array2d->member );
     */
  }
}

/** Function    oyArray2d_Init__Members
 *  @memberof   oyArray2d_s
 *  @brief      Custom Array2d constructor
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  array2d  the Array2d object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyArray2d_Init__Members( oyArray2d_s_ * array2d OY_UNUSED )
{
  return 0;
}

/** Function    oyArray2d_Copy__Members
 *  @memberof   oyArray2d_s
 *  @brief      Custom Array2d copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyArray2d_s_ input object
 *  @param[out]  dst  the output oyArray2d_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyArray2d_Copy__Members( oyArray2d_s_ * dst, oyArray2d_s_ * src)
{
  int error = 0, i, size;
#if 0
  oyAlloc_f allocateFunc_ = 0;
  oyDeAlloc_f deallocateFunc_ = 0;
#endif

  if(!dst || !src)
    return 1;

#if 0
  allocateFunc_ = dst->oy_->allocateFunc_;
  deallocateFunc_ = dst->oy_->deallocateFunc_;
#endif

  /* Copy each value of src to dst here */

  dst->own_lines = 2;
  for(i = 0; i < dst->height; ++i)
  {
    size = dst->width * oyDataTypeGetSize( dst->t );
    oyStruct_AllocHelper_m_( dst->array2d[i], unsigned char, size,
                             dst, error = 1; break );
    error = !memcpy( dst->array2d[i], src->array2d[i], size );
  }

  return error;
}

/* } Include "Array2d.private_custom_definitions.c" */


/** @internal
 *  Function oyArray2d_New_
 *  @memberof oyArray2d_s_
 *  @brief   allocate a new oyArray2d_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyArray2d_s_ * oyArray2d_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_ARRAY2D_S;
  int error = 0, id = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object, "oyArray2d_s" );
  oyArray2d_s_ * s = 0;

  if(s_obj)
  {
    id = s_obj->id_;
    switch(id) /* give valgrind a glue, which object was created */
    {
      case 1: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 2: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 3: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 4: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 5: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 6: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 7: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 8: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 9: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 10: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 11: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 12: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 13: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 14: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 15: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 16: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 17: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 18: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 19: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 20: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 21: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 22: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 23: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 24: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 25: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 26: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 27: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 28: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 29: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 30: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 31: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 32: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 33: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 34: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 35: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 36: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 37: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 38: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      case 39: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_)); break;
      default: s = (oyArray2d_s_*)s_obj->allocateFunc_(sizeof(oyArray2d_s_));
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

  error = !memset( s, 0, sizeof(oyArray2d_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyArray2d_Copy;
  s->release = (oyStruct_Release_f) oyArray2d_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom Array2d constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_ARRAY2D_S, (oyPointer)s );
  /* ---- end of custom Array2d constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom Array2d constructor ----- */
  error += oyArray2d_Init__Members( s );
  /* ---- end of custom Array2d constructor ------- */
  
  
  
  

  if(!oy_array2d_init_)
  {
    oy_array2d_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyArray2d_StaticMessageFunc_,
                                        oyArray2d_StaticFree_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyArray2d_Copy__
 *  @memberof oyArray2d_s_
 *  @brief   real copy a Array2d object
 *
 *  @param[in]     array2d                 Array2d struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyArray2d_s_ * oyArray2d_Copy__ ( oyArray2d_s_ *array2d, oyObject_s object )
{
  oyArray2d_s_ *s = 0;
  int error = 0;

  if(!array2d || !object)
    return s;

  s = (oyArray2d_s_*)oyArray2d_Create( 0, array2d->height, 0, array2d->t, object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom Array2d copy constructor ----- */
    error = oyArray2d_Copy__Members( s, array2d );
    /* ---- end of custom Array2d copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyArray2d_Release_( &s );

  return s;
}

/** @internal
 *  Function oyArray2d_Copy_
 *  @memberof oyArray2d_s_
 *  @brief   copy or reference a Array2d object
 *
 *  @param[in]     array2d                 Array2d struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyArray2d_s_ * oyArray2d_Copy_ ( oyArray2d_s_ *array2d, oyObject_s object )
{
  oyArray2d_s_ * s = array2d;

  if(!array2d)
    return 0;

  if(array2d && !object)
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

  s = oyArray2d_Copy__( array2d, object );

  return s;
}
 
/** @internal
 *  Function oyArray2d_Release_
 *  @memberof oyArray2d_s_
 *  @brief   release and possibly deallocate a Array2d object
 *
 *  @param[in,out] array2d                 Array2d struct object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/10/23
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyArray2d_Release_( oyArray2d_s_ **array2d )
{
  const char * track_name = NULL;
  int observer_refs = 0, id = 0, refs = 0, parent_refs = 0;
  /* ---- start of common object destructor ----- */
  oyArray2d_s_ *s = 0;

  if(!array2d || !*array2d)
    return 0;

  s = *array2d;
  /* static object */
  if(!s->oy_)
    return 0;

  id = s->oy_->id_;
  refs = s->oy_->ref_;

  if(refs <= 0) /* avoid circular or double dereferencing */
    return 0;

  *array2d = 0;

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
    WARNc2_S( "oyArray2d_s[%d]->object can not be untracked with refs: %d\n", id, refs );
    //oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ "refs:%d", OY_DBG_ARGS_, refs);
    return -1; /* issue */
  }

  
  /* ---- start of custom Array2d destructor ----- */
  oyArray2d_Release__Members( s );
  /* ---- end of custom Array2d destructor ------- */
  
  
  
  



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
      fprintf( stderr, "!!!ERROR:%d oyArray2d_s[%d]->object can not be untracked with refs: %d\n", __LINE__, id, refs);

    s->oy_ = NULL;
    oyObject_Release( &oy );
    if(track_name)
      fprintf( stderr, "%s[%d] destructed\n", (id == oy_debug_objects)?oyjlTermColor(oyjlRED, track_name):track_name, id );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "Array2d.private_methods_definitions.c" { */
/** Function  oyArray2d_Init_
 *  @memberof oyArray2d_s
 *  @brief    Initialise Array
 *
 *  @version  Oyranos: 0.9.0
 *  @since    2012/10/20 (Oyranos: 0.9.0)
 *  @date     2012/10/20
 */
int                oyArray2d_Init_   ( oyArray2d_s_      * s,
                                       int                 width,
                                       int                 height,
                                       oyDATATYPE_e        data_type )
{
  int error = !(width || height);
  if(error <= 0)
  {
    int y_len = sizeof(unsigned char *) * (height + 1);
    oyOBJECT_e rtype = oyOBJECT_RECTANGLE_S;

    s->width = width;
    s->height = height;
    s->t = data_type;
    memcpy( &s->data_area, &rtype, sizeof(oyOBJECT_e) );
    oyRectangle_SetGeo( (oyRectangle_s*)&s->data_area, 0,0, width, height );
    oyAllocHelper_m_( s->array2d, unsigned char *, height + 1, s->oy_->allocateFunc_, return 1 );
    error = !memset( s->array2d, 0, y_len );
    s->own_lines = oyNO;

    if(oy_debug)
    {
      const char * txt = oyStruct_GetInfo( (oyStruct_s*)s, oyNAME_DESCRIPTION, 0x01 );
      oyObject_SetName( s->oy_, txt, oyNAME_DESCRIPTION );
    }
  }
  return error;
}

/** Function  oyArray2d_Create_
 *  @memberof oyArray2d_s
 *  @brief    Allocate and initialise a oyArray2d_s object widthout pixel
 *  @internal
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/08/23 (Oyranos: 0.1.8)
 *  @date    2008/10/03
 */
oyArray2d_s_ *
                   oyArray2d_Create_ ( int                 width,
                                       int                 height,
                                       oyDATATYPE_e        data_type,
                                       oyObject_s          object )
{
  oyArray2d_s_ * s = 0;
  int error = 0;

  if(!width || !height)
    return s;

  s = (oyArray2d_s_*)oyArray2d_New( object );
  error = !s;

  if(error <= 0)
  {
    error = oyArray2d_Init_( s, width, height, data_type );
    if(error)
      oyArray2d_Release( (oyArray2d_s**)&s );
  }

  return s;
}

/** Function  oyArray2d_ReleaseArray_
 *  @memberof oyArray2d_s
 *  @brief    Release Array2d::array member
 *
 *  @param[in,out] obj                 struct object
 *
 *  @version Oyranos: 0.9.1
 *  @date    2012/11/02
 *  @since   2010/09/07 (Oyranos: 0.1.11)
 */
int          oyArray2d_ReleaseArray_ ( oyArray2d_s       * obj )
{
  int error = 0;
  oyArray2d_s_ * s = (oyArray2d_s_*)obj;

  if(s && s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;
    int y, y_max = s->data_area.height + s->data_area.y;
    size_t dsize = oyDataTypeGetSize( s->t );

    if(oy_debug > 3)
    {
      int32_t channels = 1;
      oyOptions_FindInt( s->oy_->handles_, "channels", 0, &channels );
      oyMessageFunc_p( oyMSG_DBG, (oyStruct_s*)s,
                       OY_DBG_FORMAT_ "%s", OY_DBG_ARGS_,
                       oyArray2d_Show( (oyArray2d_s*)s, channels) );
    }

    for( y = s->data_area.y; y < y_max; ++y )
    {
      if((s->own_lines == 1 && y == s->data_area.y) ||
         s->own_lines == 2)
        deallocateFunc( &s->array2d[y][dsize * (int)OY_ROUND(s->data_area.x)] );
      s->array2d[y] = 0;
    }
    deallocateFunc( s->array2d + (int)OY_ROUND(s->data_area.y) );
    s->array2d = 0;
  }

  return error;
}
 
/** Function  oyArray2d_ToPPM_
 *  @memberof oyArray2d_s
 *  @brief    Dump array to a netppm file 
 *  @internal
 *
 *  @param[in]     array               the array to fill read from
 *  @param[in]     file_name           rectangle of interesst in samples
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/06/18 (Oyranos: 0.1.10)
 *  @date    2009/06/18
 */
int              oyArray2d_ToPPM_    ( oyArray2d_s_      * array,
                                       const char        * file_name )
{
  oyArray2d_s_ * s = array;
  int error = 0, i,j, len, shift;
  size_t size,
         byteps;
  char * buf,
       * data;

  if(!array || !file_name || !file_name[0])
    return 1;

  oyCheckType__m( oyOBJECT_ARRAY2D_S, return 1 )

  if(!error)
  {
    if(s->array2d[0] == NULL)
    {
      printf("oyArray2d_s[%d] is not yet used/allocated\n",
             oyObject_GetId(s->oy_));
      return 1;
    }

    byteps = oyDataTypeGetSize(s->t); /* byte per sample */
    size = s->width * s->height * byteps;
    buf = oyAllocateFunc_(size + 1024);

    if(buf && size)
    {
      switch(s->t) {
      case oyUINT8:     /*  8-bit integer */
           sprintf( buf, "P5\n#%s:%d oyArray2d_s[%d]\n%d %d\n255\n", 
                    __FILE__,__LINE__, oyObject_GetId(s->oy_),
                    s->width, s->height );
           break;
      case oyUINT16:    /* 16-bit integer */
      case oyUINT32:    /* 32-bit integer */
           sprintf( buf, "P5\n#%s:%d oyArray2d_s[%d]\n%d %d\n65535\n", 
                    __FILE__,__LINE__, oyObject_GetId(s->oy_),
                    s->width, s->height );
           break;
      case oyHALF:      /* 16-bit floating point number */
      case oyFLOAT:     /* IEEE floating point number */
      case oyDOUBLE:    /* IEEE double precission floating point number */
           sprintf( buf, "Pf\n#%s:%d oyArray2d_s[%d]\n%d %d\n%s\n", 
                    __FILE__,__LINE__, oyObject_GetId(s->oy_),
                    s->width, s->height,
                    oyBigEndian()? "1.0" : "-1.0" );
           break;
      default: return 1;
      }

      len = oyStrlen_(buf);
      data = &buf[len];
      shift = oyBigEndian() ? 0 : 1;

      switch(s->t) {
      case oyUINT8:     /*  8-bit integer */
      case oyFLOAT:     /* IEEE floating point number */
           for(i = 0; i < s->height; ++i)
             memcpy( &data[i * s->width * byteps],
                     s->array2d[i],
                     s->width * byteps );
           break;
      case oyUINT16:    /* 16-bit integer */
           for(i = 0; i < s->height; ++i)
             memcpy( &data[i * s->width * byteps + shift],
                     s->array2d[i],
                     s->width * byteps );
           break;
      case oyUINT32:    /* 32-bit integer */
           for(i = 0; i < s->height; ++i)
             for(j = 0; j < s->width; ++j)
               ((uint16_t*)&data[i*s->width*2])[j] =
                                       *((uint32_t*)&s->array2d[i][j*byteps]) /
                                                     65537;
           break;
      case oyHALF:      /* 16-bit floating point number */
           for(i = 0; i < s->height; ++i)
             for(j = 0; j < s->width; ++j)
               ((uint16_t*)&data[i*s->width*2])[j] = 
                                       *((uint16_t*)&s->array2d[i][j*byteps]);
           break;
      case oyDOUBLE:    /* IEEE double precission floating point number */
           for(i = 0; i < s->height; ++i)
             for(j = 0; j < s->width; ++j)
               ((float*)&data[i*s->width*2])[j] =
                                       *((double*)&s->array2d[i][j*byteps]);
           break;
      default: return 1;
      }

      error = oyWriteMemToFile_( file_name, buf, len + size );
    }

    if(buf) oyDeAllocateFunc_(buf);
      size = 0;
  }

  return error;
}

/* } Include "Array2d.private_methods_definitions.c" */

