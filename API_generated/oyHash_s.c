/** @file oyHash_s.c

   [Template file inheritance graph]
   +-> Hash_s.template.c
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
 *  @since    2011/01/27
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
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 );

  hash = oyHash_New_( s );

  return (oyHash_s*) hash;
}

/** Function oyHash_Copy
 *  @memberof oyHash_s
 *  @brief   copy or reference a Hash object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     hash                 Hash struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triffers a real copy
 */
OYAPI oyHash_s* OYEXPORT
  oyHash_Copy( oyHash_s *hash, oyObject_s object )
{
  oyHash_s_ * s = (oyHash_s_*) hash;

  if(s)
    oyCheckType__m( oyOBJECT_HASH_S, return 0 );

  s = oyHash_Copy_( s, object );

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

/* } Include "Hash.public_methods_definitions.c" */

