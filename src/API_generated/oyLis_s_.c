/** @file oyLis_s_.c

   [Template file inheritance graph]
   +-> oyLis_s_.template.c
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



  
#include "oyLis_s.h"
#include "oyLis_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyranos_generic_internal.h"
  


/* Include "Lis.private_custom_definitions.c" { */
/** Function    oyLis_Release__Members
 *  @memberof   oyLis_s
 *  @brief      Custom Lis destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  lis  the Lis object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyLis_Release__Members( oyLis_s_ * lis )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &lis->member );
   */

  if(lis->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = lis->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( lis->member );
     */
  }
}

/** Function    oyLis_Init__Members
 *  @memberof   oyLis_s
 *  @brief      Custom Lis constructor
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  lis  the Lis object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyLis_Init__Members( oyLis_s_ * lis )
{
  return 0;
}

/** Function    oyLis_Copy__Members
 *  @memberof   oyLis_s
 *  @brief      Custom Lis copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyLis_s_ input object
 *  @param[out]  dst  the output oyLis_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyLis_Copy__Members( oyLis_s_ * dst, oyLis_s_ * src)
{
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;
  oyDeAlloc_f deallocateFunc_ = 0;

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;
  deallocateFunc_ = dst->oy_->deallocateFunc_;

  /* Copy each value of src to dst here */

  return error;
}

/* } Include "Lis.private_custom_definitions.c" */



static int oy_lis_init_ = 0;
static const char * oyLis_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyLis_s_ * s = (oyLis_s_*) obj;
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
 *  Function oyLis_New_
 *  @memberof oyLis_s_
 *  @brief   allocate a new oyLis_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyLis_s_ * oyLis_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_LIS_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyLis_s_ * s = 0;

  if(s_obj)
    s = (oyLis_s_*)s_obj->allocateFunc_(sizeof(oyLis_s_));
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

  error = !memset( s, 0, sizeof(oyLis_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyLis_Copy;
  s->release = (oyStruct_Release_f) oyLis_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom Lis constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_LIS_S, (oyPointer)s );
  /* ---- end of custom Lis constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom Lis constructor ----- */
  error += oyLis_Init__Members( s );
  /* ---- end of custom Lis constructor ------- */
  
  
  
  

  if(!oy_lis_init_)
  {
    oy_lis_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyLis_StaticMessageFunc_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyLis_Copy__
 *  @memberof oyLis_s_
 *  @brief   real copy a Lis object
 *
 *  @param[in]     lis                 Lis struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyLis_s_ * oyLis_Copy__ ( oyLis_s_ *lis, oyObject_s object )
{
  oyLis_s_ *s = 0;
  int error = 0;

  if(!lis || !object)
    return s;

  s = (oyLis_s_*) oyLis_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom Lis copy constructor ----- */
    error = oyLis_Copy__Members( s, lis );
    /* ---- end of custom Lis copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyLis_Release_( &s );

  return s;
}

/** @internal
 *  Function oyLis_Copy_
 *  @memberof oyLis_s_
 *  @brief   copy or reference a Lis object
 *
 *  @param[in]     lis                 Lis struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyLis_s_ * oyLis_Copy_ ( oyLis_s_ *lis, oyObject_s object )
{
  oyLis_s_ *s = lis;

  if(!lis)
    return 0;

  if(lis && !object)
  {
    s = lis;
    
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyLis_Copy__( lis, object );

  return s;
}
 
/** @internal
 *  Function oyLis_Release_
 *  @memberof oyLis_s_
 *  @brief   release and possibly deallocate a Lis object
 *
 *  @param[in,out] lis                 Lis struct object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyLis_Release_( oyLis_s_ **lis )
{
  const char * track_name = NULL;
  /* ---- start of common object destructor ----- */
  oyLis_s_ *s = 0;

  if(!lis || !*lis)
    return 0;

  s = *lis;

  *lis = 0;

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

  
  /* ---- start of custom Lis destructor ----- */
  oyLis_Release__Members( s );
  /* ---- end of custom Lis destructor ------- */
  
  
  
  



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



/* Include "Lis.private_methods_definitions.c" { */

/* } Include "Lis.private_methods_definitions.c" */

