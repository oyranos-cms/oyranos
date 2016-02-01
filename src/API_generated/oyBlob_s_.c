/** @file oyBlob_s_.c

   [Template file inheritance graph]
   +-> oyBlob_s_.template.c
   |
   +-- Base_s_.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2016 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */



  
#include "oyBlob_s.h"
#include "oyBlob_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"


  


/* Include "Blob.private_custom_definitions.c" { */
/** @internal
 *  Function    oyBlob_Release__Members
 *  @memberof   oyBlob_s
 *  @brief      Custom Blob destructor
 *   *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  blob  the Blob object
 *
 *  @version Oyranos: 0.2.1
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyBlob_Release__Members( oyBlob_s_ * blob )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &blob->member );
   */

  if(blob->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = blob->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( blob->member );
     */

    if(blob->ptr && !(blob->flags & 0x01))
      deallocateFunc( blob->ptr );
    blob->size = 0;
  }
}

/** @internal
 *  Function    oyBlob_Init__Members
 *  @memberof   oyBlob_s
 *  @brief      Custom Blob constructor 
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  blob  the Blob object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyBlob_Init__Members( oyBlob_s_ * blob )
{
  blob->ptr = NULL;

  return 0;
}

/** @internal
 *  Function    oyBlob_Copy__Members
 *  @memberof   oyBlob_s
 *  @brief      Custom Blob copy constructor
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyBlob_s_ input object
 *  @param[out]  dst  the output oyBlob_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyBlob_Copy__Members( oyBlob_s_ * dst, oyBlob_s_ * src)
{
  oyAlloc_f allocateFunc_ = 0;
  int error = 0;

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;

  /* Copy each value of src to dst here */
  if(error <= 0)
  {
    allocateFunc_ = dst->oy_->allocateFunc_;

    if(src->ptr && src->size && !(src->flags & 0x01))
    {
      dst->ptr = allocateFunc_( src->size );
      error = !dst->ptr;
      if(error <= 0)
        error = !memcpy( dst->ptr, src->ptr, src->size );
    } else
      dst->ptr = src->ptr;
  }

  if(error <= 0)
  {
    dst->size = src->size;
    dst->flags = src->flags;
    error = !memcpy( dst->type, src->type, 8 );
  }

  return error;
}

/* } Include "Blob.private_custom_definitions.c" */



/** @internal
 *  Function oyBlob_New_
 *  @memberof oyBlob_s_
 *  @brief   allocate a new oyBlob_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyBlob_s_ * oyBlob_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_BLOB_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyBlob_s_ * s = 0;

  if(s_obj)
    s = (oyBlob_s_*)s_obj->allocateFunc_(sizeof(oyBlob_s_));

  if(!s || !s_obj)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(oyBlob_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyBlob_Copy;
  s->release = (oyStruct_Release_f) oyBlob_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom Blob constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_BLOB_S, (oyPointer)s );
  /* ---- end of custom Blob constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom Blob constructor ----- */
  error += oyBlob_Init__Members( s );
  /* ---- end of custom Blob constructor ------- */
  
  
  
  

  if(error)
    WARNc1_S("%d", error);

  return s;
}

/** @internal
 *  Function oyBlob_Copy__
 *  @memberof oyBlob_s_
 *  @brief   real copy a Blob object
 *
 *  @param[in]     blob                 Blob struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyBlob_s_ * oyBlob_Copy__ ( oyBlob_s_ *blob, oyObject_s object )
{
  oyBlob_s_ *s = 0;
  int error = 0;

  if(!blob || !object)
    return s;

  s = (oyBlob_s_*) oyBlob_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom Blob copy constructor ----- */
    error = oyBlob_Copy__Members( s, blob );
    /* ---- end of custom Blob copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyBlob_Release_( &s );

  return s;
}

/** @internal
 *  Function oyBlob_Copy_
 *  @memberof oyBlob_s_
 *  @brief   copy or reference a Blob object
 *
 *  @param[in]     blob                 Blob struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyBlob_s_ * oyBlob_Copy_ ( oyBlob_s_ *blob, oyObject_s object )
{
  oyBlob_s_ *s = blob;

  if(!blob)
    return 0;

  if(blob && !object)
  {
    s = blob;
    
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyBlob_Copy__( blob, object );

  return s;
}
 
/** @internal
 *  Function oyBlob_Release_
 *  @memberof oyBlob_s_
 *  @brief   release and possibly deallocate a Blob object
 *
 *  @param[in,out] blob                 Blob struct object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyBlob_Release_( oyBlob_s_ **blob )
{
  /* ---- start of common object destructor ----- */
  oyBlob_s_ *s = 0;

  if(!blob || !*blob)
    return 0;

  s = *blob;

  *blob = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  
  /* ---- start of custom Blob destructor ----- */
  oyBlob_Release__Members( s );
  /* ---- end of custom Blob destructor ------- */
  
  
  
  



  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

    oyObject_Release( &s->oy_ );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "Blob.private_methods_definitions.c" { */

/* } Include "Blob.private_methods_definitions.c" */

