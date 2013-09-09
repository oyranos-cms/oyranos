/** @file oyConversion_s_.c

   [Template file inheritance graph]
   +-> oyConversion_s_.template.c
   |
   +-- Base_s_.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2012 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @date     2012/10/24
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

  s->type_ = type;
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
  
  
  
  

  if(error)
    WARNc1_S("%d", error);

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
  /* ---- start of common object destructor ----- */
  oyConversion_s_ *s = 0;

  if(!conversion || !*conversion)
    return 0;

  s = *conversion;

  *conversion = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  
  /* ---- start of custom Conversion destructor ----- */
  oyConversion_Release__Members( s );
  /* ---- end of custom Conversion destructor ------- */
  
  
  
  



  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "Conversion.private_methods_definitions.c" { */

/* } Include "Conversion.private_methods_definitions.c" */

