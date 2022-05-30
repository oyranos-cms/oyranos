/** @file oyHash_s.c

   [Template file inheritance graph]
   +-> oyHash_s.template.c
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


  
#include "oyHash_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyHash_s_.h"
  


/** Function oyHash_New
 *  @memberof oyHash_s
 *  @brief   allocate a new Hash object
 */
OYAPI oyHash_s * OYEXPORT
  oyHash_New( oyObject_s object )
{
  oyObject_s s = object;
  oyHash_s_ * hash = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  hash = oyHash_New_( s );

  return (oyHash_s*) hash;
}

/** Function  oyHash_Copy
 *  @memberof oyHash_s
 *  @brief    Copy or Reference a Hash object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     hash                 Hash struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyHash_s* OYEXPORT
  oyHash_Copy( oyHash_s *hash, oyObject_s object )
{
  oyHash_s_ * s = (oyHash_s_*) hash;

  if(s)
  {
    oyCheckType__m( oyOBJECT_HASH_S, return NULL )
  }
  else
    return NULL;

  s = oyHash_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyHash_s" );

  return (oyHash_s*) s;
}
 
/** Function oyHash_Release
 *  @memberof oyHash_s
 *  @brief   release and possibly deallocate a oyHash_s object
 *
 *  @param[in,out] hash                 Hash struct object
 */
OYAPI int OYEXPORT
  oyHash_Release( oyHash_s **hash )
{
  oyHash_s_ * s = 0;

  if(!hash || !*hash)
    return 0;

  s = (oyHash_s_*) *hash;

  oyCheckType__m( oyOBJECT_HASH_S, return 1 )

  *hash = 0;

  return oyHash_Release_( &s );
}



/* Include "Hash.public_methods_definitions.c" { */
/** Function  oyHash_Create
 *  @memberof oyHash_s
 *  @brief    Create a new Oyranos cache entry
 *
 *  @version Oyranos: 0.9.0
 *  @date    2012/10/17
 *  @since   2007/11/24 (Oyranos: 0.1.8)
 */
oyHash_s *         oyHash_Create      ( const char        * hash_text,
                                        oyObject_s          object )
{
  return (oyHash_s*) oyHash_Get_( hash_text, object );
}

/** Function  oyHash_GetType
 *  @memberof oyHash_s
 *  @brief    get Hash entries type
 *
 *  @since    Oyranos: version 0.3.0
 *  @version  (API 0.3.0)
 */
oyOBJECT_e         oyHash_GetType    ( oyHash_s          * hash )
{
  oyHash_s_ * s = (oyHash_s_*)hash;
  if(s && s->oy_struct)
    return s->oy_struct->type_;
  else
    return oyOBJECT_NONE;
}

/** Function  oyHash_IsOf
 *  @memberof oyHash_s
 *  @brief    Hash is of type
 *
 *  @since Oyranos: version 0.3.0
 *  @date  3 december 2007 (API 0.1.8)
 */
int                oyHash_IsOf       ( oyHash_s          * hash,
                                       oyOBJECT_e          type )
{
  return oyHash_IsOf_((oyHash_s_*)hash, type);
}

/** Function  oyHash_SetPointer
 *  @memberof oyHash_s
 *
 *  @param[in,out] hash                the to be set hash
 *  @param[in,out] obj                 the to be referenced object
 *  @return                            0 - good; >= 1 - error; < 0 issue
 *
 *  @version Oyranos: 0.3.0
 *  @since   2007/12/03 (Oyranos: 0.3.0)
 *  @date    2011/01/05
 */
int                oyHash_SetPointer ( oyHash_s          * hash,
                                       oyStruct_s        * obj )
{
  return oyHash_SetPointer_((oyHash_s_*)hash, obj);
}

/** Function  oyHash_GetPointer
 *  @memberof oyHash_s
 *
 *  @version   Oyranos: 0.3.0
 *  @since     2011/01/05 (Oyranos: 0.3.0)
 */
oyStruct_s *       oyHash_GetPointer ( oyHash_s          * hash,
                                       oyOBJECT_e          type )
{
  return oyHash_GetPointer_((oyHash_s_*)hash, type);
}


/* } Include "Hash.public_methods_definitions.c" */

