/** @file oyLis_s.c

   [Template file inheritance graph]
   +-> oyLis_s.template.c
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


  
#include "oyLis_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyLis_s_.h"
  


/** Function oyLis_New
 *  @memberof oyLis_s
 *  @brief   allocate a new Lis object
 */
OYAPI oyLis_s * OYEXPORT
  oyLis_New( oyObject_s object )
{
  oyObject_s s = object;
  oyLis_s_ * lis = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  lis = oyLis_New_( s );

  return (oyLis_s*) lis;
}

/** Function  oyLis_Copy
 *  @memberof oyLis_s
 *  @brief    Copy or Reference a Lis object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     lis                 Lis struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyLis_s* OYEXPORT
  oyLis_Copy( oyLis_s *lis, oyObject_s object )
{
  oyLis_s_ * s = (oyLis_s_*) lis;

  if(s)
  {
    oyCheckType__m( oyOBJECT_LIS_S, return NULL )
  }
  else
    return NULL;

  s = oyLis_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyLis_s" );

  return (oyLis_s*) s;
}
 
/** Function oyLis_Release
 *  @memberof oyLis_s
 *  @brief   release and possibly deallocate a oyLis_s object
 *
 *  @param[in,out] lis                 Lis struct object
 */
OYAPI int OYEXPORT
  oyLis_Release( oyLis_s **lis )
{
  oyLis_s_ * s = 0;

  if(!lis || !*lis)
    return 0;

  s = (oyLis_s_*) *lis;

  oyCheckType__m( oyOBJECT_LIS_S, return 1 )

  *lis = 0;

  return oyLis_Release_( &s );
}



/* Include "Lis.public_methods_definitions.c" { */

/* } Include "Lis.public_methods_definitions.c" */

