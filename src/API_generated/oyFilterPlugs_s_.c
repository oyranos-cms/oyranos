/** @file oyFilterPlugs_s_.c

   [Template file inheritance graph]
   +-> oyFilterPlugs_s_.template.c
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



  
#include "oyFilterPlugs_s.h"
#include "oyFilterPlugs_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"


  


/* Include "FilterPlugs.private_custom_definitions.c" { */
/** Function    oyFilterPlugs_Release__Members
 *  @memberof   oyFilterPlugs_s
 *  @brief      Custom FilterPlugs destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  filterplugs  the FilterPlugs object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyFilterPlugs_Release__Members( oyFilterPlugs_s_ * filterplugs )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &filterplugs->member );
   */

  if(filterplugs->oy_->deallocateFunc_)
  {
#if 0
    oyDeAlloc_f deallocateFunc = filterplugs->oy_->deallocateFunc_;
#endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( filterplugs->member );
     */
  }
}

/** Function    oyFilterPlugs_Init__Members
 *  @memberof   oyFilterPlugs_s
 *  @brief      Custom FilterPlugs constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  filterplugs  the FilterPlugs object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyFilterPlugs_Init__Members( oyFilterPlugs_s_ * filterplugs )
{
  return 0;
}

/** Function    oyFilterPlugs_Copy__Members
 *  @memberof   oyFilterPlugs_s
 *  @brief      Custom FilterPlugs copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyFilterPlugs_s_ input object
 *  @param[out]  dst  the output oyFilterPlugs_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyFilterPlugs_Copy__Members( oyFilterPlugs_s_ * dst, oyFilterPlugs_s_ * src)
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

/* } Include "FilterPlugs.private_custom_definitions.c" */



static int oy_filterplugs_init_ = 0;
static const char * oyFilterPlugs_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyFilterPlugs_s_ * s = (oyFilterPlugs_s_*) obj;
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
 *  Function oyFilterPlugs_New_
 *  @memberof oyFilterPlugs_s_
 *  @brief   allocate a new oyFilterPlugs_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyFilterPlugs_s_ * oyFilterPlugs_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_FILTER_PLUGS_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyFilterPlugs_s_ * s = 0;

  if(s_obj)
    s = (oyFilterPlugs_s_*)s_obj->allocateFunc_(sizeof(oyFilterPlugs_s_));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(oyFilterPlugs_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyFilterPlugs_Copy;
  s->release = (oyStruct_Release_f) oyFilterPlugs_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom FilterPlugs constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_FILTER_PLUGS_S, (oyPointer)s );
  /* ---- end of custom FilterPlugs constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  s->list_ = oyStructList_Create( s->type_, 0, 0 );


  
  /* ---- start of custom FilterPlugs constructor ----- */
  error += oyFilterPlugs_Init__Members( s );
  /* ---- end of custom FilterPlugs constructor ------- */
  
  
  
  

  if(!oy_filterplugs_init_)
  {
    oy_filterplugs_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyFilterPlugs_StaticMessageFunc_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyFilterPlugs_Copy__
 *  @memberof oyFilterPlugs_s_
 *  @brief   real copy a FilterPlugs object
 *
 *  @param[in]     filterplugs                 FilterPlugs struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyFilterPlugs_s_ * oyFilterPlugs_Copy__ ( oyFilterPlugs_s_ *filterplugs, oyObject_s object )
{
  oyFilterPlugs_s_ *s = 0;
  int error = 0;

  if(!filterplugs || !object)
    return s;

  s = (oyFilterPlugs_s_*) oyFilterPlugs_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom FilterPlugs copy constructor ----- */
    error = oyFilterPlugs_Copy__Members( s, filterplugs );
    /* ---- end of custom FilterPlugs copy constructor ------- */
    
    
    
    
    
    s->list_ = oyStructList_Copy( filterplugs->list_, s->oy_ );

  }

  if(error)
    oyFilterPlugs_Release_( &s );

  return s;
}

/** @internal
 *  Function oyFilterPlugs_Copy_
 *  @memberof oyFilterPlugs_s_
 *  @brief   copy or reference a FilterPlugs object
 *
 *  @param[in]     filterplugs                 FilterPlugs struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyFilterPlugs_s_ * oyFilterPlugs_Copy_ ( oyFilterPlugs_s_ *filterplugs, oyObject_s object )
{
  oyFilterPlugs_s_ *s = filterplugs;

  if(!filterplugs)
    return 0;

  if(filterplugs && !object)
  {
    s = filterplugs;
    
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyFilterPlugs_Copy__( filterplugs, object );

  return s;
}
 
/** @internal
 *  Function oyFilterPlugs_Release_
 *  @memberof oyFilterPlugs_s_
 *  @brief   release and possibly deallocate a FilterPlugs list
 *
 *  @param[in,out] filterplugs                 FilterPlugs struct object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyFilterPlugs_Release_( oyFilterPlugs_s_ **filterplugs )
{
  /* ---- start of common object destructor ----- */
  oyFilterPlugs_s_ *s = 0;

  if(!filterplugs || !*filterplugs)
    return 0;

  s = *filterplugs;

  *filterplugs = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  
  /* ---- start of custom FilterPlugs destructor ----- */
  oyFilterPlugs_Release__Members( s );
  /* ---- end of custom FilterPlugs destructor ------- */
  
  
  
  

  oyStructList_Release( &s->list_ );


  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "FilterPlugs.private_methods_definitions.c" { */

/* } Include "FilterPlugs.private_methods_definitions.c" */

