/** @file oyPointer_s_.c

   [Template file inheritance graph]
   +-> oyPointer_s_.template.c
   |
   +-- Base_s_.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2013 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @date     2013/03/12
 */



  
#include "oyPointer_s.h"
#include "oyPointer_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyranos_generic_internal.h"
  


/* Include "Pointer.private_custom_definitions.c" { */
/** @internal
 *  Function    oyPointer_Release__Members
 *  @memberof   oyPointer_s
 *  @brief      Custom Pointer destructor
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmmptr  the Pointer object
 *
 *  @version Oyranos: 0.3.0
 *  @since   2010/08/00 (Oyranos: 0.3.0)
 *  @date    2011/02/13
 */
void oyPointer_Release__Members( oyPointer_s_ * cmmptr )
{
  oyPointer_s_ * s = cmmptr;
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmmptr->member );
   */

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmmptr->member );
     */
    if(--s->ref)
      return;

    s->type_ = 0;

    if(s->ptr)
    {
      if(s->ptrRelease)
        s->ptrRelease( &s->ptr );
      else
      {
        oyDeAllocateFunc_( s->ptr );
        s->ptr = 0;
      }

      if(s->lib_name)
      deallocateFunc( s->lib_name ); s->lib_name = 0;
      if(s->func_name)
      deallocateFunc( s->func_name ); s->func_name = 0;
      if(s->resource)
      deallocateFunc( s->resource ); s->resource = 0;
      if(s->id)
      deallocateFunc( s->id ); s->id = 0;

      /*oyCMMdsoRelease_( cmmptr->lib_name );*/
    }
  }
}

/** @internal
 *  Function    oyPointer_Init__Members
 *  @memberof   oyPointer_s
 *  @brief      Custom Pointer constructor 
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmmptr  the Pointer object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyPointer_Init__Members( oyPointer_s_ * cmmptr )
{
  ++cmmptr->ref;

  return 0;
}

/** @internal
 *  Function    oyPointer_Copy__Members
 *  @memberof   oyPointer_s
 *  @brief      Custom Pointer copy constructor
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyPointer_s_ input object
 *  @param[out]  dst  the output oyPointer_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyPointer_Copy__Members( oyPointer_s_ * dst, oyPointer_s_ * src)
{
  oyAlloc_f allocateFunc_ = 0;

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;

#define COPY_MEMBER_STRING(variable_name) { \
  if(allocateFunc_) \
    dst->variable_name = oyStringCopy_( src->variable_name, allocateFunc_ ); \
  else \
    dst->variable_name = src->variable_name; }

  /* Copy each value of src to dst here */
  dst->ref = src->ref;
  COPY_MEMBER_STRING( lib_name )
  COPY_MEMBER_STRING( func_name )
  COPY_MEMBER_STRING( resource )
  COPY_MEMBER_STRING( id )

  return 0;
}

/* } Include "Pointer.private_custom_definitions.c" */



/** @internal
 *  Function oyPointer_New_
 *  @memberof oyPointer_s_
 *  @brief   allocate a new oyPointer_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyPointer_s_ * oyPointer_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_POINTER_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyPointer_s_ * s = 0;

  if(s_obj)
    s = (oyPointer_s_*)s_obj->allocateFunc_(sizeof(oyPointer_s_));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(oyPointer_s_) );
  if(error)
    WARNc_S( "memset failed" );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyPointer_Copy;
  s->release = (oyStruct_Release_f) oyPointer_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom Pointer constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_POINTER_S, (oyPointer)s );
  /* ---- end of custom Pointer constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom Pointer constructor ----- */
  error += oyPointer_Init__Members( s );
  /* ---- end of custom Pointer constructor ------- */
  
  
  
  

  if(error)
    WARNc1_S("%d", error);

  return s;
}

/** @internal
 *  Function oyPointer_Copy__
 *  @memberof oyPointer_s_
 *  @brief   real copy a Pointer object
 *
 *  @param[in]     pointer                 Pointer struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyPointer_s_ * oyPointer_Copy__ ( oyPointer_s_ *pointer, oyObject_s object )
{
  oyPointer_s_ *s = 0;
  int error = 0;

  if(!pointer || !object)
    return s;

  s = (oyPointer_s_*) oyPointer_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom Pointer copy constructor ----- */
    error = oyPointer_Copy__Members( s, pointer );
    /* ---- end of custom Pointer copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyPointer_Release_( &s );

  return s;
}

/** @internal
 *  Function oyPointer_Copy_
 *  @memberof oyPointer_s_
 *  @brief   copy or reference a Pointer object
 *
 *  @param[in]     pointer                 Pointer struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyPointer_s_ * oyPointer_Copy_ ( oyPointer_s_ *pointer, oyObject_s object )
{
  oyPointer_s_ *s = pointer;

  if(!pointer)
    return 0;

  if(pointer && !object)
  {
    s = pointer;
    
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyPointer_Copy__( pointer, object );

  return s;
}
 
/** @internal
 *  Function oyPointer_Release_
 *  @memberof oyPointer_s_
 *  @brief   release and possibly deallocate a Pointer object
 *
 *  @param[in,out] pointer                 Pointer struct object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyPointer_Release_( oyPointer_s_ **pointer )
{
  /* ---- start of common object destructor ----- */
  oyPointer_s_ *s = 0;

  if(!pointer || !*pointer)
    return 0;

  s = *pointer;

  *pointer = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  
  /* ---- start of custom Pointer destructor ----- */
  oyPointer_Release__Members( s );
  /* ---- end of custom Pointer destructor ------- */
  
  
  
  



  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "Pointer.private_methods_definitions.c" { */
/** @internal
 *  @brief set oyPointer_s members
 *
 *  Has only a weak release behaviour. Use for initialising.
 *
 *  @version Oyranos: 0.3.0
 *  @since   2007/11/26 (Oyranos: 0.1.8)
 *  @date    2011/02/15
 */
int                oyPointer_Set_    ( oyPointer_s_      * cmm_ptr,
                                       const char        * lib_name,
                                       const char        * resource,
                                       oyPointer           ptr,
                                       const char        * func_name,
                                       const char        * id,
                                       oyPointer_release_f ptrRelease )
{
  oyPointer_s_ * s = cmm_ptr;
  int error = !s;
  oyAlloc_f alloc_func = oyStruct_GetAllocator( (oyStruct_s*) s );
  oyDeAlloc_f dealloc_func = oyStruct_GetDeAllocator( (oyStruct_s*) s );

    
  if(error <= 0 && lib_name)
  {
    if(s->lib_name)
      oyStringFree_( &s->lib_name, dealloc_func );
    s->lib_name = oyStringCopy_( lib_name, alloc_func );
  }

  if(error <= 0 && func_name)
  {
    if(s->func_name)
      oyStringFree_( &s->func_name, dealloc_func );
    s->func_name = oyStringCopy_( func_name, alloc_func );
  }

  if(error <= 0 && resource)
  {
    if(s->resource)
      oyStringFree_( &s->resource, dealloc_func );
    s->resource = oyStringCopy_( resource, alloc_func );
  }

  if(error <= 0 && id)
  {
    if(s->id)
      oyStringFree_( &s->id, dealloc_func );
    s->id = oyStringCopy_( id, alloc_func );
  }

  if(error <= 0 && ptr)
  {
    if(s->ptrRelease && s->ptr)
      s->ptrRelease( &ptr );
    s->ptr = ptr;
  }

  if(error <= 0 && ptrRelease)
    s->ptrRelease = ptrRelease;

  return error;
}

/* } Include "Pointer.private_methods_definitions.c" */

