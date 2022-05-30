/** @file oyBlob_s.c

   [Template file inheritance graph]
   +-> oyBlob_s.template.c
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


  
#include "oyBlob_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyBlob_s_.h"
  


/** Function oyBlob_New
 *  @memberof oyBlob_s
 *  @brief   allocate a new Blob object
 */
OYAPI oyBlob_s * OYEXPORT
  oyBlob_New( oyObject_s object )
{
  oyObject_s s = object;
  oyBlob_s_ * blob = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  blob = oyBlob_New_( s );

  return (oyBlob_s*) blob;
}

/** Function  oyBlob_Copy
 *  @memberof oyBlob_s
 *  @brief    Copy or Reference a Blob object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     blob                 Blob struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyBlob_s* OYEXPORT
  oyBlob_Copy( oyBlob_s *blob, oyObject_s object )
{
  oyBlob_s_ * s = (oyBlob_s_*) blob;

  if(s)
  {
    oyCheckType__m( oyOBJECT_BLOB_S, return NULL )
  }
  else
    return NULL;

  s = oyBlob_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyBlob_s" );

  return (oyBlob_s*) s;
}
 
/** Function oyBlob_Release
 *  @memberof oyBlob_s
 *  @brief   release and possibly deallocate a oyBlob_s object
 *
 *  @param[in,out] blob                 Blob struct object
 */
OYAPI int OYEXPORT
  oyBlob_Release( oyBlob_s **blob )
{
  oyBlob_s_ * s = 0;

  if(!blob || !*blob)
    return 0;

  s = (oyBlob_s_*) *blob;

  oyCheckType__m( oyOBJECT_BLOB_S, return 1 )

  *blob = 0;

  return oyBlob_Release_( &s );
}



/* Include "Blob.public_methods_definitions.c" { */
/** Function oyBlob_SetFromData
 *  @memberof oyBlob_s
 *  @brief   set value from a data blob
 *
 *  @param[in]     blob                the data blob
 *  @param[in]     ptr                 copy the data into the blob object
 *  @param[in]     size                data size; 0 means the pointer is not owned by the object.
 *  @param[in]     type                data type; assuming 8 byte with typical
 *                                     4 byte content
 *  @return                            0 - success, 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/01/06 (Oyranos: 0.1.10)
 *  @date    2009/06/12
 */
int            oyBlob_SetFromData    ( oyBlob_s          * blob,
                                       oyPointer           ptr,
                                       size_t              size,
                                       const char        * type )
{
  oyBlob_s_ * s = (oyBlob_s_*)blob;
  int error = 0;

  oyCheckType__m( oyOBJECT_BLOB_S, return 1 )

  if(error <= 0)
  {
    if(s->ptr && !(s->flags & 0x01))
      s->oy_->deallocateFunc_( s->ptr );
    s->size = 0;

    if(size)
    {
      s->ptr = s->oy_->allocateFunc_( size );
      error = !s->ptr;
      s->flags = 0;
    } else
      s->flags = 0x01;
  }

  if(error <= 0)
  {
    if(size)
      error = !memcpy( s->ptr, ptr, size );
    else
      s->ptr = ptr;
  }

  if(error <= 0)
    s->size = size;

  if(error <= 0 && type)
    error = !memcpy( s->type, type, 8 );

  return error;
}

/** Function oyBlob_SetFromStatic
 *  @memberof oyBlob_s
 *  @brief   set value from a data blob
 *
 *  @param[in]     blob                the data blob
 *  @param[in]     ptr                 move the data into the blob object
 *  @param[in]     size                data size
 *  @param[in]     type                data type; assuming 8 byte with typical
 *                                     4 byte content
 *  @return                            0 - success, 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/07/26 (Oyranos: 0.1.10)
 *  @date    2009/07/26
 */
int            oyBlob_SetFromStatic  ( oyBlob_s          * blob,
                                       const oyPointer     ptr,
                                       size_t              size,
                                       const char        * type )
{
  oyBlob_s_ * s = (oyBlob_s_*)blob;
  int error = 0;

  oyCheckType__m( oyOBJECT_BLOB_S, return 1 )

  if(error <= 0)
  {
    if(s->ptr && !(s->flags & 0x01))
      s->oy_->deallocateFunc_( s->ptr );
    s->size = 0;

    s->ptr = ptr;
    error = !s->ptr;
    s->flags = 0x01;
  }

  if(error <= 0)
    s->size = size;

  if(error <= 0 && type)
    error = !memcpy( s->type, type, 8 );

  return error;
}

/** Function oyBlob_GetPointer
 *  @memberof oyBlob_s
 *  @brief   get value from a data blob
 *
 *  @param[in]     blob                the data blob
 *  @return                            the data pointer
 *
 *  @version Oyranos: 0.2.1
 *  @since   2011/01/17 (Oyranos: 0.2.1)
 *  @date    2011/01/17
 */
oyPointer          oyBlob_GetPointer ( oyBlob_s          * blob )
{
  oyBlob_s_ * s = (oyBlob_s_*)blob;

  oyCheckType__m( oyOBJECT_BLOB_S, return NULL )

  return s->ptr;
}

/** Function oyBlob_GetSize
 *  @memberof oyBlob_s
 *  @brief   get size from a data blob
 *
 *  @param[in]     blob                the data blob
 *  @return                            the data size
 *
 *  @version Oyranos: 0.2.1
 *  @since   2011/01/17 (Oyranos: 0.2.1)
 *  @date    2011/01/17
 */
size_t             oyBlob_GetSize    ( oyBlob_s          * blob )
{
  oyBlob_s_ * s = (oyBlob_s_*)blob;

  oyCheckType__m( oyOBJECT_BLOB_S, return 0 )

  return s->size;
}

/** Function oyBlob_GetType
 *  @memberof oyBlob_s
 *  @brief   get type from a data blob
 *
 *  @param[in]     blob                the data blob
 *  @return                            the data type
 *
 *  @version Oyranos: 0.2.1
 *  @since   2011/01/17 (Oyranos: 0.2.1)
 *  @date    2011/01/17
 */
const char *       oyBlob_GetType    ( oyBlob_s          * blob )
{
  oyBlob_s_ * s = (oyBlob_s_*)blob;

  oyCheckType__m( oyOBJECT_BLOB_S, return 0 )

  return s->type;
}


/* } Include "Blob.public_methods_definitions.c" */

