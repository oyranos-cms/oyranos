/** @file oyNamedColors_s_.c

   [Template file inheritance graph]
   +-> oyNamedColors_s_.template.c
   |
   +-> oyList_s_.template.c
   |
   +-> BaseList_s_.c
   |
   +-- Base_s_.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2017 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */



  
#include "oyNamedColors_s.h"
#include "oyNamedColors_s_.h"


#include "oyList_s_.h"




#include "oyObject_s.h"
#include "oyranos_object_internal.h"


  


/* Include "NamedColors.private_custom_definitions.c" { */
/** Function    oyNamedColors_Release__Members
 *  @memberof   oyNamedColors_s
 *  @brief      Custom NamedColors destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  namedcolors  the NamedColors object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyNamedColors_Release__Members( oyNamedColors_s_ * namedcolors )
{
  oyNamedColors_s_ * s = namedcolors;
  if(s->prefix)
    oyObject_GetDeAlloc( s->oy_ )( s->prefix );
  if(s->suffix)
    oyObject_GetDeAlloc( s->oy_ )( s->suffix );
}

/** Function    oyNamedColors_Init__Members
 *  @memberof   oyNamedColors_s
 *  @brief      Custom NamedColors constructor
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  namedcolors  the NamedColors object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyNamedColors_Init__Members( oyNamedColors_s_ * namedcolors OY_UNUSED )
{
  return 0;
}

/** Function    oyNamedColors_Copy__Members
 *  @memberof   oyNamedColors_s
 *  @brief      Custom NamedColors copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyNamedColors_s_ input object
 *  @param[out]  dst  the output oyNamedColors_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyNamedColors_Copy__Members( oyNamedColors_s_ * dst, oyNamedColors_s_ * src)
{
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;

  if(!dst || !src)
    return 1;

  allocateFunc_ = oyObject_GetAlloc( dst->oy_ );

  /* Copy each value of src to dst here */
  if(src->prefix)
    dst->prefix = oyStringCopy_(src->prefix, allocateFunc_);

  return error;
}

/* } Include "NamedColors.private_custom_definitions.c" */



static int oy_namedcolors_init_ = 0;
static const char * oyNamedColors_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyNamedColors_s_ * s = (oyNamedColors_s_*) obj;
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
    text_n = 128;
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
/** @internal
 *  Function oyNamedColors_New_
 *  @memberof oyNamedColors_s_
 *  @brief   allocate a new oyNamedColors_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyNamedColors_s_ * oyNamedColors_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_NAMED_COLORS_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyNamedColors_s_ * s = 0;

  if(s_obj)
    s = (oyNamedColors_s_*)s_obj->allocateFunc_(sizeof(oyNamedColors_s_));
  else
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  if(!s)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(oyNamedColors_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyNamedColors_Copy;
  s->release = (oyStruct_Release_f) oyNamedColors_Release;

  s->oy_ = s_obj;

  
  
  /* ---- start of custom List constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_LIST_S, (oyPointer)s );
  /* ---- end of custom List constructor ------- */
  /* ---- start of custom NamedColors constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_NAMED_COLORS_S, (oyPointer)s );
  /* ---- end of custom NamedColors constructor ------- */
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  s->list_ = oyStructList_Create( s->type_, 0, 0 );


  
  
  /* ---- start of custom List constructor ----- */
  error += oyList_Init__Members( (oyList_s_*)s );
  /* ---- end of custom List constructor ------- */
  /* ---- start of custom NamedColors constructor ----- */
  error += oyNamedColors_Init__Members( s );
  /* ---- end of custom NamedColors constructor ------- */
  
  
  

  if(!oy_namedcolors_init_)
  {
    oy_namedcolors_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyNamedColors_StaticMessageFunc_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyNamedColors_Copy__
 *  @memberof oyNamedColors_s_
 *  @brief   real copy a NamedColors object
 *
 *  @param[in]     namedcolors                 NamedColors struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyNamedColors_s_ * oyNamedColors_Copy__ ( oyNamedColors_s_ *namedcolors, oyObject_s object )
{
  oyNamedColors_s_ *s = 0;
  int error = 0;

  if(!namedcolors || !object)
    return s;

  s = (oyNamedColors_s_*) oyNamedColors_New( object );
  error = !s;

  if(!error) {
    
    
    /* ---- start of custom List copy constructor ----- */
    error = oyList_Copy__Members( (oyList_s_*)s, (oyList_s_*)namedcolors );
    /* ---- end of custom List copy constructor ------- */
    /* ---- start of custom NamedColors copy constructor ----- */
    error = oyNamedColors_Copy__Members( s, namedcolors );
    /* ---- end of custom NamedColors copy constructor ------- */
    
    
    
    
    s->list_ = oyStructList_Copy( namedcolors->list_, s->oy_ );

  }

  if(error)
    oyNamedColors_Release_( &s );

  return s;
}

/** @internal
 *  Function oyNamedColors_Copy_
 *  @memberof oyNamedColors_s_
 *  @brief   copy or reference a NamedColors object
 *
 *  @param[in]     namedcolors                 NamedColors struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyNamedColors_s_ * oyNamedColors_Copy_ ( oyNamedColors_s_ *namedcolors, oyObject_s object )
{
  oyNamedColors_s_ *s = namedcolors;

  if(!namedcolors)
    return 0;

  if(namedcolors && !object)
  {
    s = namedcolors;
    
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyNamedColors_Copy__( namedcolors, object );

  return s;
}
 
/** @internal
 *  Function oyNamedColors_Release_
 *  @memberof oyNamedColors_s_
 *  @brief   release and possibly deallocate a NamedColors list
 *
 *  @param[in,out] namedcolors                 NamedColors struct object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyNamedColors_Release_( oyNamedColors_s_ **namedcolors )
{
  const char * track_name = NULL;
  /* ---- start of common object destructor ----- */
  oyNamedColors_s_ *s = 0;

  if(!namedcolors || !*namedcolors)
    return 0;

  s = *namedcolors;

  *namedcolors = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  if(oy_debug_objects >= 0)
  {
    const char * t = getenv(OY_DEBUG_OBJECTS);
    int id_ = -1;

    if(t)
      id_ = atoi(t);

    if((id_ >= 0 && s->oy_->id_ == id_) ||
       (t && s && strstr(oyStructTypeToText(s->type_), t) != 0) ||
       id_ == 1)
    {
      track_name = oyStructTypeToText(s->type_);
      fprintf( stderr, "%s[%d] untracking\n", track_name, s->oy_->id_);
    }
  }

  
  
  /* ---- start of custom List destructor ----- */
  oyList_Release__Members( (oyList_s_*)s );
  /* ---- end of custom List destructor ------- */
  /* ---- start of custom NamedColors destructor ----- */
  oyNamedColors_Release__Members( s );
  /* ---- end of custom NamedColors destructor ------- */
  
  
  

  oyStructList_Release( &s->list_ );


  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;
    int id = s->oy_->id_;

    oyObject_Release( &s->oy_ );
    if(track_name)
      fprintf( stderr, "%s[%d] untracked\n", track_name, id);

    deallocateFunc( s );
  }

  return 0;
}



/* Include "NamedColors.private_methods_definitions.c" { */

/* } Include "NamedColors.private_methods_definitions.c" */

