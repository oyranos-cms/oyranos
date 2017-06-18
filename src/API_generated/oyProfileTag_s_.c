/** @file oyProfileTag_s_.c

   [Template file inheritance graph]
   +-> oyProfileTag_s_.template.c
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



  
#include "oyProfileTag_s.h"
#include "oyProfileTag_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"


  


/* Include "ProfileTag.private_custom_definitions.c" { */
/** @internal
 *  Function    oyProfileTag_Release__Members
 *  @memberof   oyProfileTag_s
 *  @brief      Custom ProfileTag destructor
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  profiletag  the ProfileTag object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyProfileTag_Release__Members( oyProfileTag_s_ * profiletag )
{
  /* Nothing to deallocate here */
}

/** @internal
 *  Function    oyProfileTag_Init__Members
 *  @memberof   oyProfileTag_s
 *  @brief      Custom ProfileTag constructor 
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  profiletag  the ProfileTag object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyProfileTag_Init__Members( oyProfileTag_s_ * profiletag )
{
  return 0;
}

/** @internal
 *  Function    oyProfileTag_Copy__Members
 *  @memberof   oyProfileTag_s
 *  @brief      Custom ProfileTag copy constructor
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyProfileTag_s_ input object
 *  @param[out]  dst  the output oyProfileTag_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyProfileTag_Copy__Members( oyProfileTag_s_ * dst, oyProfileTag_s_ * src)
{
  if(!dst || !src)
    return 1;

  /* Copy each value of src to dst here */
  /* Copy the whole struct mem block as is - is this safe? */
  memcpy(dst, src, sizeof(oyProfileTag_s_));

  return 0;
}

/* } Include "ProfileTag.private_custom_definitions.c" */



static int oy_profiletag_init_ = 0;
static const char * oyProfileTag_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyProfileTag_s_ * s = (oyProfileTag_s_*) obj;
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

  

  
  /* allocate enough space */
  if(text_n < 1000)
  {
    oyDeAlloc_f dealloc = oyDeAllocateFunc_;
    if(s->oy_ && s->oy_->deallocateFunc_)
      dealloc = s->oy_->deallocateFunc_;
    if(text && text_n)
      dealloc( text );
    text_n = 1000;
    text = alloc(text_n);
    if(text)
      text[0] = '\000';
    else
      return "Memory Error";

    if(!(flags & 0x01))
      sprintf(text, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");
  }

  if(type == oyNAME_NICK && (flags & 0x01))
  {
    sprintf( &text[strlen(text)], "%s",
             oyICCTagName(s->use)
           );
  } else
  if(type == oyNAME_NAME)
    sprintf( &text[strlen(text)], "%s %s",
             oyICCTagName(s->use), oyICCTagTypeName(s->tag_type_)
           );
  else
  if((int)type >= oyNAME_DESCRIPTION)
    sprintf( &text[strlen(text)], "%s %s\noffset: %lu size: %lu/%lu",
             oyICCTagDescription(s->use), oyICCTagTypeName(s->tag_type_),
             s->offset_orig, s->size_, s->size_check_
           );


  return text;
}
/** @internal
 *  Function oyProfileTag_New_
 *  @memberof oyProfileTag_s_
 *  @brief   allocate a new oyProfileTag_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyProfileTag_s_ * oyProfileTag_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_PROFILE_TAG_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyProfileTag_s_ * s = 0;

  if(s_obj)
    s = (oyProfileTag_s_*)s_obj->allocateFunc_(sizeof(oyProfileTag_s_));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(oyProfileTag_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyProfileTag_Copy;
  s->release = (oyStruct_Release_f) oyProfileTag_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom ProfileTag constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_PROFILE_TAG_S, (oyPointer)s );
  /* ---- end of custom ProfileTag constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom ProfileTag constructor ----- */
  error += oyProfileTag_Init__Members( s );
  /* ---- end of custom ProfileTag constructor ------- */
  
  
  
  

  if(!oy_profiletag_init_)
  {
    oy_profiletag_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyProfileTag_StaticMessageFunc_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyProfileTag_Copy__
 *  @memberof oyProfileTag_s_
 *  @brief   real copy a ProfileTag object
 *
 *  @param[in]     profiletag                 ProfileTag struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyProfileTag_s_ * oyProfileTag_Copy__ ( oyProfileTag_s_ *profiletag, oyObject_s object )
{
  oyProfileTag_s_ *s = 0;
  int error = 0;

  if(!profiletag || !object)
    return s;

  s = (oyProfileTag_s_*) oyProfileTag_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom ProfileTag copy constructor ----- */
    error = oyProfileTag_Copy__Members( s, profiletag );
    /* ---- end of custom ProfileTag copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyProfileTag_Release_( &s );

  return s;
}

/** @internal
 *  Function oyProfileTag_Copy_
 *  @memberof oyProfileTag_s_
 *  @brief   copy or reference a ProfileTag object
 *
 *  @param[in]     profiletag                 ProfileTag struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyProfileTag_s_ * oyProfileTag_Copy_ ( oyProfileTag_s_ *profiletag, oyObject_s object )
{
  oyProfileTag_s_ *s = profiletag;

  if(!profiletag)
    return 0;

  if(profiletag && !object)
  {
    s = profiletag;
    
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyProfileTag_Copy__( profiletag, object );

  return s;
}
 
/** @internal
 *  Function oyProfileTag_Release_
 *  @memberof oyProfileTag_s_
 *  @brief   release and possibly deallocate a ProfileTag object
 *
 *  @param[in,out] profiletag                 ProfileTag struct object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyProfileTag_Release_( oyProfileTag_s_ **profiletag )
{
  /* ---- start of common object destructor ----- */
  oyProfileTag_s_ *s = 0;

  if(!profiletag || !*profiletag)
    return 0;

  s = *profiletag;

  *profiletag = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  
  /* ---- start of custom ProfileTag destructor ----- */
  oyProfileTag_Release__Members( s );
  /* ---- end of custom ProfileTag destructor ------- */
  
  
  
  



  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "ProfileTag.private_methods_definitions.c" { */

/* } Include "ProfileTag.private_methods_definitions.c" */

