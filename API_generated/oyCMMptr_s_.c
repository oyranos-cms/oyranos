/** @file oyCMMptr_s_.c

   [Template file inheritance graph]
   +-> CMMptr_s_.template.c
   |
   +-- Base_s_.c

 *  Oyranos is an open source Colour Management System
 *
 *  @par Copyright:
 *            2004-2011 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @since    2011/02/02
 */



  
#include "oyCMMptr_s.h"
#include "oyCMMptr_s_.h"

#include "oyObject_s.h"
#include "oyranos_object_internal.h"
#include "oyranos_module_internal.h"

  


/* Include "CMMptr.private_custom_definitions.c" { */
/** Function    oyCMMptr_Release__Members
 *  @memberof   oyCMMptr_s
 *  @brief      Custom CMMptr destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmmptr  the CMMptr object
 *
 *  @version Oyranos: 0.3.0
 *  @since   2010/08/00 (Oyranos: 0.3.0)
 *  @date    2011/01/28
 */
void oyCMMptr_Release__Members( oyCMMptr_s_ * cmmptr )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmmptr->member );
   */

  if(cmmptr->oy_->deallocateFunc_)
  {
    /*oyDeAlloc_f deallocateFunc = cmmptr->oy_->deallocateFunc_;*/

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmmptr->member );
     */
    if(--cmmptr->ref)
      return;

    cmmptr->type_ = 0;

    if(cmmptr->ptr)
    {
      if(cmmptr->ptrRelease)
        cmmptr->ptrRelease( &cmmptr->ptr );
      else
      {
        oyDeAllocateFunc_( cmmptr->ptr );
        cmmptr->ptr = 0;
      }

      /*oyCMMdsoRelease_( cmmptr->lib_name );*/
    }
  }
}

/** Function    oyCMMptr_Init__Members
 *  @memberof   oyCMMptr_s
 *  @brief      Custom CMMptr constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmmptr  the CMMptr object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMptr_Init__Members( oyCMMptr_s_ * cmmptr )
{
  ++cmmptr->ref;

  return 0;
}

/** Function    oyCMMptr_Copy__Members
 *  @memberof   oyCMMptr_s
 *  @brief      Custom CMMptr copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyCMMptr_s_ input object
 *  @param[out]  dst  the output oyCMMptr_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMptr_Copy__Members( oyCMMptr_s_ * dst, oyCMMptr_s_ * src)
{
  oyAlloc_f allocateFunc_ = 0;
  oyDeAlloc_f deallocateFunc_ = 0;

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;
  deallocateFunc_ = dst->oy_->deallocateFunc_;

  /* Copy each value of src to dst here */
  dst->ref = src->ref;

  return 0;
}

/* } Include "CMMptr.private_custom_definitions.c" */



/** @internal
 *  Function oyCMMptr_New_
 *  @memberof oyCMMptr_s_
 *  @brief   allocate a new oyCMMptr_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMptr_s_ * oyCMMptr_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CMM_PTR_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyCMMptr_s_ * s = 0;

  if(s_obj)
    s = (oyCMMptr_s_*)s_obj->allocateFunc_(sizeof(oyCMMptr_s_));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(oyCMMptr_s_) );

  s->type_ = type;
  s->copy = (oyStruct_Copy_f) oyCMMptr_Copy;
  s->release = (oyStruct_Release_f) oyCMMptr_Release;

  s->oy_ = s_obj;
  /* ---- end of common object constructor ------- */

  
  
  /* ---- start of custom CMMptr constructor ----- */
  error = !oyObject_SetParent( s_obj, oyOBJECT_CMM_PTR_S, s );
  error = oyCMMptr_Init__Members( s );
  /* ---- end of custom CMMptr constructor ------- */
  
  
  
  
  

  return s;
}

/** @internal
 *  Function oyCMMptr_Copy__
 *  @memberof oyCMMptr_s_
 *  @brief   real copy a CMMptr object
 *
 *  @param[in]     cmmptr                 CMMptr struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMptr_s_ * oyCMMptr_Copy__ ( oyCMMptr_s_ *cmmptr, oyObject_s object )
{
  oyCMMptr_s_ *s = 0;
  int error = 0;

  if(!cmmptr || !object)
    return s;

  s = oyCMMptr_New_( object );
  error = !s;

  if(!error) {
    
    
    /* ---- start of custom CMMptr copy constructor ----- */
    error = oyCMMptr_Copy__Members( s, cmmptr );
    /* ---- end of custom CMMptr copy constructor ------- */
    
    
    
    
    
  }

  if(error)
    oyCMMptr_Release_( &s );

  return s;
}

/** @internal
 *  Function oyCMMptr_Copy_
 *  @memberof oyCMMptr_s_
 *  @brief   copy or reference a CMMptr object
 *
 *  @param[in]     cmmptr                 CMMptr struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMptr_s_ * oyCMMptr_Copy_ ( oyCMMptr_s_ *cmmptr, oyObject_s object )
{
  oyCMMptr_s_ *s = cmmptr;

  if(!cmmptr)
    return 0;

  if(cmmptr && !object)
  {
    s = cmmptr;
    
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyCMMptr_Copy__( cmmptr, object );

  return s;
}
 
/** @internal
 *  Function oyCMMptr_Release_
 *  @memberof oyCMMptr_s_
 *  @brief   release and possibly deallocate a CMMptr object
 *
 *  @param[in,out] cmmptr                 CMMptr struct object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyCMMptr_Release_( oyCMMptr_s_ **cmmptr )
{
  /* ---- start of common object destructor ----- */
  oyCMMptr_s_ *s = 0;

  if(!cmmptr || !*cmmptr)
    return 0;

  s = *cmmptr;

  *cmmptr = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */


  
  /* ---- start of custom CMMptr destructor ----- */
  oyCMMptr_Release__Members( s );
  /* ---- end of custom CMMptr destructor ------- */
  
  
  
  


  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "CMMptr.private_methods_definitions.c" { */
/** @internal
 *  @brief set oyCMMptr_s members
 *
 *  Has only a weak release behaviour. Use for initialising.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/11/26 (Oyranos: 0.1.8)
 *  @date    2008/12/27
 */
int                oyCMMptr_Set_     ( oyCMMptr_s_       * cmm_ptr,
                                       const char        * lib_name,
                                       const char        * resource,
                                       oyPointer           ptr,
                                       const char        * func_name,
                                       oyPointer_release_f ptrRelease )
{
  oyCMMptr_s_ * s = cmm_ptr;
  int error = !s;

  if(error <= 0 && lib_name)
    s->lib_name = oyStringCopy_( lib_name, oyAllocateFunc_ );

  if(error <= 0 && func_name)
    if(oyStrlen_(func_name) < 32)
      oySprintf_(s->func_name, "%s", func_name); 

  if(error <= 0 && ptr)
  {
    if(s->ptrRelease && s->ptr)
      s->ptrRelease( &ptr );
    s->ptr = ptr;
  }

  if(error <= 0 && resource)
    if(oyStrlen_(resource) < 5)
      oySprintf_(s->resource, "%s", resource); 

  if(error <= 0 && ptrRelease)
    s->ptrRelease = ptrRelease;

  return error;
}

/* } Include "CMMptr.private_methods_definitions.c" */

