/** @file oyCMMptr_s.c

   [Template file inheritance graph]
   +-> CMMptr_s.template.c
   |
   +-> Base_s.c
   |
   +-- Struct_s.template.c

 *  Oyranos is an open source Colour Management System
 *
 *  @par Copyright:
 *            2004-2011 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @since    2011/01/28
 */


  
#include "oyCMMptr_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyCMMptr_s_.h"
  


/** Function oyCMMptr_New
 *  @memberof oyCMMptr_s
 *  @brief   allocate a new CMMptr object
 */
OYAPI oyCMMptr_s * OYEXPORT
  oyCMMptr_New( oyObject_s object )
{
  oyObject_s s = object;
  oyCMMptr_s_ * cmmptr = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 );

  cmmptr = oyCMMptr_New_( s );

  return (oyCMMptr_s*) cmmptr;
}

/** Function oyCMMptr_Copy
 *  @memberof oyCMMptr_s
 *  @brief   copy or reference a CMMptr object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     cmmptr                 CMMptr struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triffers a real copy
 */
OYAPI oyCMMptr_s* OYEXPORT
  oyCMMptr_Copy( oyCMMptr_s *cmmptr, oyObject_s object )
{
  oyCMMptr_s_ * s = (oyCMMptr_s_*) cmmptr;

  if(s)
    oyCheckType__m( oyOBJECT_CMM_PTR_S, return 0 );

  s = oyCMMptr_Copy_( s, object );

  return (oyCMMptr_s*) s;
}
 
/** Function oyCMMptr_Release
 *  @memberof oyCMMptr_s
 *  @brief   release and possibly deallocate a oyCMMptr_s object
 *
 *  @param[in,out] cmmptr                 CMMptr struct object
 */
OYAPI int OYEXPORT
  oyCMMptr_Release( oyCMMptr_s **cmmptr )
{
  oyCMMptr_s_ * s = 0;

  if(!cmmptr || !*cmmptr)
    return 0;

  s = (oyCMMptr_s_*) *cmmptr;

  oyCheckType__m( oyOBJECT_CMM_PTR_S, return 1 )

  *cmmptr = 0;

  return oyCMMptr_Release_( &s );
}



/* Include "CMMptr.public_methods_definitions.c" { */
/** Function oyCMMptr_Set
 *  @brief   set a oyCMMptr_s
 *  @ingroup module_api
 *  @memberof oyCMMptr_s
 *
 *  Use for initialising.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/07/22 (Oyranos: 0.1.10)
 *  @date    2009/08/16
 */
int          oyCMMptr_Set            ( oyCMMptr_s        * cmm_ptr,
                                       const char        * lib_name,
                                       const char        * resource,
                                       oyPointer           ptr,
                                       const char        * func_name,
                                       oyPointer_release_f ptrRelease )
{
  return oyCMMptr_Set_( (oyCMMptr_s_*)cmm_ptr, lib_name, resource, ptr, func_name, ptrRelease);
}

const char * oyCMMptr_GetFuncName    ( oyCMMptr_s        * cmm_ptr )
{
  oyCMMptr_s_ * c = (oyCMMptr_s_ *) cmm_ptr;
  if(c)
    return c->func_name;
  else
    return NULL;
}
const char * oyCMMptr_GetLibName     ( oyCMMptr_s        * cmm_ptr )
{
  oyCMMptr_s_ * c = (oyCMMptr_s_ *) cmm_ptr;
  if(c)
    return c->lib_name;
  else
    return NULL;
}
const char * oyCMMptr_GetResourceName( oyCMMptr_s        * cmm_ptr )
{
  oyCMMptr_s_ * c = (oyCMMptr_s_ *) cmm_ptr;
  if(c)
    return c->resource;
  else
    return NULL;
}
int          oyCMMptr_GetSize        ( oyCMMptr_s        * cmm_ptr )
{
  oyCMMptr_s_ * c = (oyCMMptr_s_ *) cmm_ptr;
  if(c)
    return c->size;
  else
    return 0;
}
oyPointer    oyCMMptr_GetPointer     ( oyCMMptr_s        * cmm_ptr )
{
  oyCMMptr_s_ * c = (oyCMMptr_s_ *) cmm_ptr;
  if(c)
    return c->ptr;
  else
    return NULL;
}


/* } Include "CMMptr.public_methods_definitions.c" */

