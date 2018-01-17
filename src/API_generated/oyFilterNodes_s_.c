/** @file oyFilterNodes_s_.c

   [Template file inheritance graph]
   +-> oyFilterNodes_s_.template.c
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



  
#include "oyFilterNodes_s.h"
#include "oyFilterNodes_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"


  


/* Include "FilterNodes.private_custom_definitions.c" { */
/** Function    oyFilterNodes_Release__Members
 *  @memberof   oyFilterNodes_s
 *  @brief      Custom FilterNodes destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  filternodes  the FilterNodes object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyFilterNodes_Release__Members( oyFilterNodes_s_ * filternodes )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &filternodes->member );
   */

  if(filternodes->oy_->deallocateFunc_)
  {
#if 0
    oyDeAlloc_f deallocateFunc = filternodes->oy_->deallocateFunc_;
#endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( filternodes->member );
     */
  }
}

/** Function    oyFilterNodes_Init__Members
 *  @memberof   oyFilterNodes_s
 *  @brief      Custom FilterNodes constructor
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  filternodes  the FilterNodes object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyFilterNodes_Init__Members( oyFilterNodes_s_ * filternodes OY_UNUSED )
{
  return 0;
}

/** Function    oyFilterNodes_Copy__Members
 *  @memberof   oyFilterNodes_s
 *  @brief      Custom FilterNodes copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyFilterNodes_s_ input object
 *  @param[out]  dst  the output oyFilterNodes_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyFilterNodes_Copy__Members( oyFilterNodes_s_ * dst, oyFilterNodes_s_ * src)
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

  return error;
}

/* } Include "FilterNodes.private_custom_definitions.c" */



static int oy_filternodes_init_ = 0;
static const char * oyFilterNodes_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyFilterNodes_s_ * s = (oyFilterNodes_s_*) obj;
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
 *  Function oyFilterNodes_New_
 *  @memberof oyFilterNodes_s_
 *  @brief   allocate a new oyFilterNodes_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyFilterNodes_s_ * oyFilterNodes_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_FILTER_NODES_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyFilterNodes_s_ * s = 0;

  if(s_obj)
    s = (oyFilterNodes_s_*)s_obj->allocateFunc_(sizeof(oyFilterNodes_s_));
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

  error = !memset( s, 0, sizeof(oyFilterNodes_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyFilterNodes_Copy;
  s->release = (oyStruct_Release_f) oyFilterNodes_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom FilterNodes constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_FILTER_NODES_S, (oyPointer)s );
  /* ---- end of custom FilterNodes constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  s->list_ = oyStructList_Create( s->type_, 0, 0 );


  
  /* ---- start of custom FilterNodes constructor ----- */
  error += oyFilterNodes_Init__Members( s );
  /* ---- end of custom FilterNodes constructor ------- */
  
  
  
  

  if(!oy_filternodes_init_)
  {
    oy_filternodes_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyFilterNodes_StaticMessageFunc_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyFilterNodes_Copy__
 *  @memberof oyFilterNodes_s_
 *  @brief   real copy a FilterNodes object
 *
 *  @param[in]     filternodes                 FilterNodes struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyFilterNodes_s_ * oyFilterNodes_Copy__ ( oyFilterNodes_s_ *filternodes, oyObject_s object )
{
  oyFilterNodes_s_ *s = 0;
  int error = 0;

  if(!filternodes || !object)
    return s;

  s = (oyFilterNodes_s_*) oyFilterNodes_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom FilterNodes copy constructor ----- */
    error = oyFilterNodes_Copy__Members( s, filternodes );
    /* ---- end of custom FilterNodes copy constructor ------- */
    
    
    
    
    
    s->list_ = oyStructList_Copy( filternodes->list_, s->oy_ );

  }

  if(error)
    oyFilterNodes_Release_( &s );

  return s;
}

/** @internal
 *  Function oyFilterNodes_Copy_
 *  @memberof oyFilterNodes_s_
 *  @brief   copy or reference a FilterNodes object
 *
 *  @param[in]     filternodes                 FilterNodes struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyFilterNodes_s_ * oyFilterNodes_Copy_ ( oyFilterNodes_s_ *filternodes, oyObject_s object )
{
  oyFilterNodes_s_ *s = filternodes;

  if(!filternodes)
    return 0;

  if(filternodes && !object)
  {
    s = filternodes;
    
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyFilterNodes_Copy__( filternodes, object );

  return s;
}
 
/** @internal
 *  Function oyFilterNodes_Release_
 *  @memberof oyFilterNodes_s_
 *  @brief   release and possibly deallocate a FilterNodes list
 *
 *  @param[in,out] filternodes                 FilterNodes struct object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyFilterNodes_Release_( oyFilterNodes_s_ **filternodes )
{
  const char * track_name = NULL;
  /* ---- start of common object destructor ----- */
  oyFilterNodes_s_ *s = 0;

  if(!filternodes || !*filternodes)
    return 0;

  s = *filternodes;

  *filternodes = 0;

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

  
  /* ---- start of custom FilterNodes destructor ----- */
  oyFilterNodes_Release__Members( s );
  /* ---- end of custom FilterNodes destructor ------- */
  
  
  
  

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



/* Include "FilterNodes.private_methods_definitions.c" { */

/* } Include "FilterNodes.private_methods_definitions.c" */

