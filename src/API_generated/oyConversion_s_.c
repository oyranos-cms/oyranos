/** @file oyConversion_s_.c

   [Template file inheritance graph]
   +-> oyConversion_s_.template.c
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



  
#include "oyConversion_s.h"
#include "oyConversion_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"


  


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
  oyFilterNode_Release( (oyFilterNode_s**)&conversion->input );
  oyFilterNode_Release( (oyFilterNode_s**)&conversion->out_ );

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
int oyConversion_Init__Members( oyConversion_s_ * conversion )
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



static int oy_conversion_init_ = 0;
static const char * oyConversion_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyConversion_s_ * s = (oyConversion_s_*) obj;
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
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyConversion_s_ * s = 0;

  if(s_obj)
    s = (oyConversion_s_*)s_obj->allocateFunc_(sizeof(oyConversion_s_));

  if(!s || !s_obj)
  {
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
                                        oyConversion_StaticMessageFunc_ );
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
  oyConversion_s_ *s = conversion;

  if(!conversion)
    return 0;

  if(conversion && !object)
  {
    s = conversion;
    
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
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyConversion_Release_( oyConversion_s_ **conversion )
{
  const char * track_name = NULL;
  /* ---- start of common object destructor ----- */
  oyConversion_s_ *s = 0;

  if(!conversion || !*conversion)
    return 0;

  s = *conversion;

  *conversion = 0;

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

  
  /* ---- start of custom Conversion destructor ----- */
  oyConversion_Release__Members( s );
  /* ---- end of custom Conversion destructor ------- */
  
  
  
  



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



/* Include "Conversion.private_methods_definitions.c" { */

/* } Include "Conversion.private_methods_definitions.c" */

