/** @file oyConversion_s_.c

   [Template file inheritance graph]
   +-> oyConversion_s_.template.c
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



  
#include "oyConversion_s.h"
#include "oyConversion_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"


  

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif


static int oy_conversion_init_ = 0;
static char * oy_conversion_msg_text_ = NULL;
static int oy_conversion_msg_text_n_ = 0;
static const char * oyConversion_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyConversion_s_ * s = (oyConversion_s_*) obj;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if( oy_conversion_msg_text_ == NULL || oy_conversion_msg_text_n_ == 0 )
  {
    oy_conversion_msg_text_n_ = 512;
    oy_conversion_msg_text_ = (char*) alloc( oy_conversion_msg_text_n_ );
    if(oy_conversion_msg_text_)
      memset( oy_conversion_msg_text_, 0, oy_conversion_msg_text_n_ );
  }

  if( oy_conversion_msg_text_ == NULL || oy_conversion_msg_text_n_ == 0 )
    return "Memory problem";

  oy_conversion_msg_text_[0] = '\000';

  if(!(flags & 0x01))
    sprintf(oy_conversion_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  
  

  return oy_conversion_msg_text_;
}

static void oyConversion_StaticFree_           ( void )
{
  if(oy_conversion_init_)
  {
    oy_conversion_init_ = 0;
    if(oy_conversion_msg_text_)
      oyFree_m_(oy_conversion_msg_text_);
    if(oy_debug)
      fprintf(stderr, "%s() freeing static \"%s\" memory\n", "oyConversion_StaticFree_", "oyConversion_s" );
  }
}


/* Include "Conversion.private_custom_definitions.c" { */
/** Function    oyConversion_Release__Members
 *  @memberof   oyConversion_s
 *  @brief      Custom Conversion destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  conversion  the Conversion object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyConversion_Release__Members( oyConversion_s_ * conversion )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &conversion->member );
   */
  oyFilterGraph_s * g = oyFilterGraph_New( 0 );
  int i,n = 0;

  if(conversion->input) 
    oyFilterGraph_SetFromNode( g, (oyFilterNode_s*)conversion->input, 0, 0 );

  if(conversion->input) oyFilterNode_Release( (oyFilterNode_s**)&conversion->input );
  if(conversion->out_) oyFilterNode_Release( (oyFilterNode_s**)&conversion->out_ );

  if(g) n = oyFilterGraph_CountNodes( g, "", NULL );
  for(i = 0; i < n; ++i)
  {
    oyFilterNode_s * node = oyFilterGraph_GetNode( g, i, "", NULL ),
                   * temp;
    int ref = oyObject_GetRefCount( node->oy_ );
    if(ref > 1)
    {
      temp = node;
      oyFilterNode_Release( &temp );
    }
    oyFilterNode_Release( &node );
  }

  oyFilterGraph_Release( &g );
  conversion->input = NULL;
  conversion->out_ = NULL;

  if(conversion->oy_->deallocateFunc_)
  {
#if 0
    oyDeAlloc_f deallocateFunc = conversion->oy_->deallocateFunc_;
#endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( conversion->member );
     */
  }
}

/** Function    oyConversion_Init__Members
 *  @memberof   oyConversion_s
 *  @brief      Custom Conversion constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  conversion  the Conversion object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyConversion_Init__Members( oyConversion_s_ * conversion OY_UNUSED )
{
  return 0;
}

/** Function    oyConversion_Copy__Members
 *  @memberof   oyConversion_s
 *  @brief      Custom Conversion copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyConversion_s_ input object
 *  @param[out]  dst  the output oyConversion_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyConversion_Copy__Members( oyConversion_s_ * dst, oyConversion_s_ * src)
{
  int error = 0;
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
  dst->input = (oyFilterNode_s_*)oyFilterNode_Copy( (oyFilterNode_s*)src->input, dst->oy_ );

  return error;
}

/* } Include "Conversion.private_custom_definitions.c" */


/** @internal
 *  Function oyConversion_New_
 *  @memberof oyConversion_s_
 *  @brief   allocate a new oyConversion_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyConversion_s_ * oyConversion_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CONVERSION_S;
  int error = 0, id = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object, "oyConversion_s" );
  oyConversion_s_ * s = 0;

  if(s_obj)
  {
    id = s_obj->id_;
    switch(id) /* give valgrind a glue, which object was created */
    {
      case 1: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 2: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 3: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 4: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 5: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 6: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 7: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 8: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 9: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 10: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 11: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 12: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 13: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 14: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 15: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 16: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 17: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 18: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 19: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 20: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 21: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 22: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 23: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 24: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 25: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 26: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 27: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 28: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 29: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 30: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 31: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 32: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 33: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 34: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 35: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 36: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 37: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 38: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      case 39: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_)); break;
      default: s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_));
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

  error = !memset( s, 0, sizeof(oyConversion_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyConversion_Copy;
  s->release = (oyStruct_Release_f) oyConversion_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom Conversion constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CONVERSION_S, (oyPointer)s );
  /* ---- end of custom Conversion constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom Conversion constructor ----- */
  error += oyConversion_Init__Members( s );
  /* ---- end of custom Conversion constructor ------- */
  
  
  
  

  if(!oy_conversion_init_)
  {
    oy_conversion_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyConversion_StaticMessageFunc_,
                                        oyConversion_StaticFree_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyConversion_Copy__
 *  @memberof oyConversion_s_
 *  @brief   real copy a Conversion object
 *
 *  @param[in]     conversion                 Conversion struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyConversion_s_ * oyConversion_Copy__ ( oyConversion_s_ *conversion, oyObject_s object )
{
  oyConversion_s_ *s = 0;
  int error = 0;

  if(!conversion || !object)
    return s;

  s = (oyConversion_s_*) oyConversion_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom Conversion copy constructor ----- */
    error = oyConversion_Copy__Members( s, conversion );
    /* ---- end of custom Conversion copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyConversion_Release_( &s );

  return s;
}

/** @internal
 *  Function oyConversion_Copy_
 *  @memberof oyConversion_s_
 *  @brief   copy or reference a Conversion object
 *
 *  @param[in]     conversion                 Conversion struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyConversion_s_ * oyConversion_Copy_ ( oyConversion_s_ *conversion, oyObject_s object )
{
  oyConversion_s_ * s = conversion;

  if(!conversion)
    return 0;

  if(conversion && !object)
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

  s = oyConversion_Copy__( conversion, object );

  return s;
}
 
/** @internal
 *  Function oyConversion_Release_
 *  @memberof oyConversion_s_
 *  @brief   release and possibly deallocate a Conversion object
 *
 *  @param[in,out] conversion                 Conversion struct object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/10/23
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyConversion_Release_( oyConversion_s_ **conversion )
{
  const char * track_name = NULL;
  int observer_refs = 0, id = 0, refs = 0, parent_refs = 0;
  /* ---- start of common object destructor ----- */
  oyConversion_s_ *s = 0;

  if(!conversion || !*conversion)
    return 0;

  s = *conversion;
  /* static object */
  if(!s->oy_)
    return 0;

  id = s->oy_->id_;
  refs = s->oy_->ref_;

  if(refs <= 0) /* avoid circular or double dereferencing */
    return 0;

  *conversion = 0;

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
    WARNc2_S( "oyConversion_s[%d]->object can not be untracked with refs: %d\n", id, refs );
    //oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ "refs:%d", OY_DBG_ARGS_, refs);
    return -1; /* issue */
  }

  
  /* ---- start of custom Conversion destructor ----- */
  oyConversion_Release__Members( s );
  /* ---- end of custom Conversion destructor ------- */
  
  
  
  



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
      fprintf( stderr, "!!!ERROR:%d oyConversion_s[%d]->object can not be untracked with refs: %d\n", __LINE__, id, refs);

    s->oy_ = NULL;
    oyObject_Release( &oy );
    if(track_name)
      fprintf( stderr, "%s[%d] destructed\n", (id == oy_debug_objects)?oyjlTermColor(oyjlRED, track_name):track_name, id );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "Conversion.private_methods_definitions.c" { */

/* } Include "Conversion.private_methods_definitions.c" */

