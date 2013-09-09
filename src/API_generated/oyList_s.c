/** @file oyLists_s.c

   [Template file inheritance graph]
   +-> oyLists_s.template.c
   |
   +-> Base_s.c
   |
   +-- oyStruct_s.template.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2013 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @date     2013/09/04
 */


  
#include "oyLists_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"


#include "oyLists_s_.h"

#include "oyList_s_.h"
  



/** Function oyLists_New
 *  @memberof oyLists_s
 *  @brief   allocate a new Lists object
 */
OYAPI oyLists_s * OYEXPORT
  oyLists_New( oyObject_s object )
{
  oyObject_s s = object;
  oyLists_s_ * lists = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  lists = oyLists_New_( s );

  return (oyLists_s*) lists;
}

/** Function oyLists_Copy
 *  @memberof oyLists_s
 *  @brief   copy or reference a Lists object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     lists                 Lists struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyLists_s* OYEXPORT
  oyLists_Copy( oyLists_s *lists, oyObject_s object )
{
  oyLists_s_ * s = (oyLists_s_*) lists;

  if(s)
    oyCheckType__m( oyOBJECT_LIST_S, return 0 )

  s = oyLists_Copy_( s, object );

  return (oyLists_s*) s;
}
 
/** Function oyLists_Release
 *  @memberof oyLists_s
 *  @brief   release and possibly deallocate a oyLists_s object
 *
 *  @param[in,out] lists                 Lists struct object
 */
OYAPI int OYEXPORT
  oyLists_Release( oyLists_s **lists )
{
  oyLists_s_ * s = 0;

  if(!lists || !*lists)
    return 0;

  s = (oyLists_s_*) *lists;

  oyCheckType__m( oyOBJECT_LIST_S, return 1 )

  *lists = 0;

  return oyLists_Release_( &s );
}

/** Function oyLists_MoveIn
 *  @memberof oyLists_s
 *  @brief   add an element to a Lists list
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
           oyLists_MoveIn          ( oyLists_s       * list,
                                       oyList_s       ** obj,
                                       int                 pos )
{
  oyLists_s_ * s = (oyLists_s_*)list;
  int error = 0;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_LIST_S, return 1 )

  if(obj && *obj &&
     (*obj)->type_ == oyOBJECT_LIST_S)
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

/** Function oyLists_ReleaseAt
 *  @memberof oyLists_s
 *  @brief   release a element from a Lists list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI int  OYEXPORT
           oyLists_ReleaseAt       ( oyLists_s       * list,
                                       int                 pos )
{ 
  int error = !list;
  oyLists_s_ * s = (oyLists_s_*)list;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_LIST_S, return 1 )

  if(!error && oyStructList_GetParentObjType(s->list_) != oyOBJECT_LIST_S)
    error = 1;
  
  if(!error)
    oyStructList_ReleaseAt( s->list_, pos );

  return error;
}

/** Function oyLists_Get
 *  @memberof oyLists_s
 *  @brief   get a element of a Lists list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI oyList_s * OYEXPORT
           oyLists_Get             ( oyLists_s       * list,
                                       int                 pos )
{       
  int error = !list;
  oyLists_s_ * s = (oyLists_s_*)list;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_LIST_S, return 0 )

  if(!error)
    return (oyList_s *) oyStructList_GetRefType( s->list_, pos, oyOBJECT_LIST_S);
  else
    return 0;
}   

/** Function oyLists_Count
 *  @memberof oyLists_s
 *  @brief   count the elements in a Lists list
 *
 *  @param[in,out] list                the list
 *  @return                            element count
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI int  OYEXPORT
           oyLists_Count           ( oyLists_s       * list )
{       
  int error = !list;
  oyLists_s_ * s = (oyLists_s_*)list;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_LIST_S, return 0 )

  if(!error)
    return oyStructList_Count( s->list_ );
  else return 0;
}

/** Function oyLists_Clear
 *  @memberof oyLists_s
 *  @brief   clear the elements in a Lists list
 *
 *  @param[in,out] list                the list
 *  @return                            0 - success; otherwise - error
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/01 (Oyranos: 0.3.0)
 *  @date    2011/02/01
 */
OYAPI int  OYEXPORT
           oyLists_Clear           ( oyLists_s       * list )
{       
  int error = !list;
  oyLists_s_ * s = (oyLists_s_*)list;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_LIST_S, return 0 )

  if(!error)
    return oyStructList_Clear( s->list_ );
  else return 0;
}

/** Function oyLists_Sort
 *  @memberof oyLists_s
 *  @brief   sort a list according to a rank_list
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/01 (Oyranos: 0.3.0)
 *  @date    2011/02/01
 */
OYAPI int  OYEXPORT
           oyLists_Sort            ( oyLists_s       * list,
                                       int32_t           * rank_list )
{       
  int error = !list;
  oyLists_s_ * s = (oyLists_s_*)list;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_LIST_S, return 0 )

  if(!error)
    return oyStructList_Sort( s->list_, rank_list );
  else return 0;
}



/* Include "Lists.public_methods_definitions.c" { */

/* } Include "Lists.public_methods_definitions.c" */

