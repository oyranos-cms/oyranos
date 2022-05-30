/** @file oyPointer_s.c

   [Template file inheritance graph]
   +-> oyPointer_s.template.c
   |
   +-> Base_s.c
   |
   +-- oyStruct_s.template.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


  
#include "oyPointer_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"


#include "oyPointer_s_.h"

#include "oyHash_s.h"
  


/** Function oyPointer_New
 *  @memberof oyPointer_s
 *  @brief   allocate a new Pointer object
 */
OYAPI oyPointer_s * OYEXPORT
  oyPointer_New( oyObject_s object )
{
  oyObject_s s = object;
  oyPointer_s_ * pointer = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  pointer = oyPointer_New_( s );

  return (oyPointer_s*) pointer;
}

/** Function  oyPointer_Copy
 *  @memberof oyPointer_s
 *  @brief    Copy or Reference a Pointer object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     pointer                 Pointer struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyPointer_s* OYEXPORT
  oyPointer_Copy( oyPointer_s *pointer, oyObject_s object )
{
  oyPointer_s_ * s = (oyPointer_s_*) pointer;

  if(s)
  {
    oyCheckType__m( oyOBJECT_POINTER_S, return NULL )
  }
  else
    return NULL;

  s = oyPointer_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyPointer_s" );

  return (oyPointer_s*) s;
}
 
/** Function oyPointer_Release
 *  @memberof oyPointer_s
 *  @brief   release and possibly deallocate a oyPointer_s object
 *
 *  @param[in,out] pointer                 Pointer struct object
 */
OYAPI int OYEXPORT
  oyPointer_Release( oyPointer_s **pointer )
{
  oyPointer_s_ * s = 0;

  if(!pointer || !*pointer)
    return 0;

  s = (oyPointer_s_*) *pointer;

  oyCheckType__m( oyOBJECT_POINTER_S, return 1 )

  *pointer = 0;

  return oyPointer_Release_( &s );
}



/* Include "Pointer.public_methods_definitions.c" { */
/** Function oyPointer_Set
 *  @brief   set a oyPointer_s
 *  @ingroup module_api
 *  @memberof oyPointer_s
 *
 *  Use for initialising.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/07/22 (Oyranos: 0.1.10)
 *  @date    2009/08/16
 */
int          oyPointer_Set           ( oyPointer_s       * cmm_ptr,
                                       const char        * lib_name,
                                       const char        * resource,
                                       oyPointer           ptr,
                                       const char        * func_name,
                                       oyPointer_release_f ptrRelease )
{
  return oyPointer_Set_( (oyPointer_s_*)cmm_ptr, lib_name, resource, ptr, func_name, NULL, ptrRelease);
}

const char * oyPointer_GetFuncName    ( oyPointer_s        * cmm_ptr )
{
  oyPointer_s_ * c = (oyPointer_s_ *) cmm_ptr;
  if(c)
    return c->func_name;
  else
    return NULL;
}
const char * oyPointer_GetLibName     ( oyPointer_s        * cmm_ptr )
{
  oyPointer_s_ * c = (oyPointer_s_ *) cmm_ptr;
  if(c)
    return c->lib_name;
  else
    return NULL;
}
const char * oyPointer_GetResourceName( oyPointer_s        * cmm_ptr )
{
  oyPointer_s_ * c = (oyPointer_s_ *) cmm_ptr;
  if(c)
    return c->resource;
  else
    return NULL;
}
int          oyPointer_GetSize        ( oyPointer_s        * cmm_ptr )
{
  oyPointer_s_ * c = (oyPointer_s_ *) cmm_ptr;
  if(c)
    return c->size;
  else
    return 0;
}
int          oyPointer_SetSize        ( oyPointer_s        * cmm_ptr,
                                       int                 size )
{
  oyPointer_s_ * c = (oyPointer_s_ *) cmm_ptr;
  if(c)
    c->size = size;
  return 0;
}
oyPointer    oyPointer_GetPointer     ( oyPointer_s        * cmm_ptr )
{
  oyPointer_s_ * c = (oyPointer_s_ *) cmm_ptr;
  if(c)
    return c->ptr;
  else
    return NULL;
}
OYAPI const char *  OYEXPORT
                oyPointer_GetId         ( oyPointer_s       * cmm_ptr )
{
  oyPointer_s_ * c = (oyPointer_s_ *) cmm_ptr;
  if(c)
    return c->id;
  else
    return NULL;
}
OYAPI void OYEXPORT
                oyPointer_SetId         ( oyPointer_s       * cmm_ptr,
                                          const char        * id )
{
  oyPointer_s_ * s = (oyPointer_s_ *) cmm_ptr;
  int error = !s;
  oyAlloc_f alloc_func = oyStruct_GetAllocator( (oyStruct_s*) s );
  oyDeAlloc_f dealloc_func = oyStruct_GetDeAllocator( (oyStruct_s*) s );
  if(error <= 0 && id)
  {
    if(s->id)
      oyStringFree_( &s->id, dealloc_func );
    s->id = oyStringCopy_( id, alloc_func );
  }
}

/* } Include "Pointer.public_methods_definitions.c" */

