/** @file oyNamedColors_s.c

   [Template file inheritance graph]
   +-> oyNamedColors_s.template.c
   |
   +-> BaseList_s.c
   |
   +-> Base_s.c
   |
   +-- oyStruct_s.template.c

 *  Oyranos is an open source Colour Management System
 *
 *  @par Copyright:
 *            2004-2013 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @date     2013/08/23
 */


  
#include "oyNamedColors_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"


#include "oyNamedColors_s_.h"

#include "oyNamedColor_s_.h"
  



/** Function oyNamedColors_New
 *  @memberof oyNamedColors_s
 *  @brief   allocate a new NamedColors object
 */
OYAPI oyNamedColors_s * OYEXPORT
  oyNamedColors_New( oyObject_s object )
{
  oyObject_s s = object;
  oyNamedColors_s_ * namedcolors = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  namedcolors = oyNamedColors_New_( s );

  return (oyNamedColors_s*) namedcolors;
}

/** Function oyNamedColors_Copy
 *  @memberof oyNamedColors_s
 *  @brief   copy or reference a NamedColors object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     namedcolors                 NamedColors struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyNamedColors_s* OYEXPORT
  oyNamedColors_Copy( oyNamedColors_s *namedcolors, oyObject_s object )
{
  oyNamedColors_s_ * s = (oyNamedColors_s_*) namedcolors;

  if(s)
    oyCheckType__m( oyOBJECT_NAMED_COLORS_S, return 0 )

  s = oyNamedColors_Copy_( s, object );

  return (oyNamedColors_s*) s;
}
 
/** Function oyNamedColors_Release
 *  @memberof oyNamedColors_s
 *  @brief   release and possibly deallocate a oyNamedColors_s object
 *
 *  @param[in,out] namedcolors                 NamedColors struct object
 */
OYAPI int OYEXPORT
  oyNamedColors_Release( oyNamedColors_s **namedcolors )
{
  oyNamedColors_s_ * s = 0;

  if(!namedcolors || !*namedcolors)
    return 0;

  s = (oyNamedColors_s_*) *namedcolors;

  oyCheckType__m( oyOBJECT_NAMED_COLORS_S, return 1 )

  *namedcolors = 0;

  return oyNamedColors_Release_( &s );
}

/** Function oyNamedColors_MoveIn
 *  @memberof oyNamedColors_s
 *  @brief   add an element to a NamedColors list
 *
 *  @param[in]     list                list
 *  @param[in,out] obj                 list element
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI int  OYEXPORT
           oyNamedColors_MoveIn          ( oyNamedColors_s       * list,
                                       oyNamedColor_s       ** obj,
                                       int                 pos )
{
  oyNamedColors_s_ * s = (oyNamedColors_s_*)list;
  int error = 0;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_NAMED_COLORS_S, return 1 )

  if(obj && *obj &&
     (*obj)->type_ == oyOBJECT_NAMED_COLOR_S)
  {
    if(!s->list_)
    {
      s->list_ = oyStructList_Create( s->type_, 0, 0 );
      error = !s->list_;
    }
      
    if(!error)
    error = oyStructList_MoveIn( s->list_, (oyStruct_s**)obj, pos, 0 );
  }   

  return error;
}

/** Function oyNamedColors_ReleaseAt
 *  @memberof oyNamedColors_s
 *  @brief   release a element from a NamedColors list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI int  OYEXPORT
           oyNamedColors_ReleaseAt       ( oyNamedColors_s       * list,
                                       int                 pos )
{ 
  int error = !list;
  oyNamedColors_s_ * s = (oyNamedColors_s_*)list;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_NAMED_COLORS_S, return 1 )

  if(!error && oyStructList_GetParentObjType(s->list_) != oyOBJECT_NAMED_COLORS_S)
    error = 1;
  
  if(!error)
    oyStructList_ReleaseAt( s->list_, pos );

  return error;
}

/** Function oyNamedColors_Get
 *  @memberof oyNamedColors_s
 *  @brief   get a element of a NamedColors list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI oyNamedColor_s * OYEXPORT
           oyNamedColors_Get             ( oyNamedColors_s       * list,
                                       int                 pos )
{       
  int error = !list;
  oyNamedColors_s_ * s = (oyNamedColors_s_*)list;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_NAMED_COLORS_S, return 0 )

  if(!error)
    return (oyNamedColor_s *) oyStructList_GetRefType( s->list_, pos, oyOBJECT_NAMED_COLOR_S);
  else
    return 0;
}   

/** Function oyNamedColors_Count
 *  @memberof oyNamedColors_s
 *  @brief   count the elements in a NamedColors list
 *
 *  @param[in,out] list                the list
 *  @return                            element count
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI int  OYEXPORT
           oyNamedColors_Count           ( oyNamedColors_s       * list )
{       
  int error = !list;
  oyNamedColors_s_ * s = (oyNamedColors_s_*)list;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_NAMED_COLORS_S, return 0 )

  if(!error)
    return oyStructList_Count( s->list_ );
  else return 0;
}

/** Function oyNamedColors_Clear
 *  @memberof oyNamedColors_s
 *  @brief   clear the elements in a NamedColors list
 *
 *  @param[in,out] list                the list
 *  @return                            0 - success; otherwise - error
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/01 (Oyranos: 0.3.0)
 *  @date    2011/02/01
 */
OYAPI int  OYEXPORT
           oyNamedColors_Clear           ( oyNamedColors_s       * list )
{       
  int error = !list;
  oyNamedColors_s_ * s = (oyNamedColors_s_*)list;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_NAMED_COLORS_S, return 0 )

  if(!error)
    return oyStructList_Clear( s->list_ );
  else return 0;
}

/** Function oyNamedColors_Sort
 *  @memberof oyNamedColors_s
 *  @brief   sort a list according to a rank_list
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/01 (Oyranos: 0.3.0)
 *  @date    2011/02/01
 */
OYAPI int  OYEXPORT
           oyNamedColors_Sort            ( oyNamedColors_s       * list,
                                       int32_t           * rank_list )
{       
  int error = !list;
  oyNamedColors_s_ * s = (oyNamedColors_s_*)list;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_NAMED_COLORS_S, return 0 )

  if(!error)
    return oyStructList_Sort( s->list_, rank_list );
  else return 0;
}



/* Include "NamedColors.public_methods_definitions.c" { */

/* } Include "NamedColors.public_methods_definitions.c" */

