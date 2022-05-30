/** @file oyList_s.c

   [Template file inheritance graph]
   +-> oyList_s.template.c
   |
   +-> Base_s.c
   |
   +-- oyStruct_s.template.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2020 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


  
#include "oyList_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"


#include "oyList_s_.h"

#include "oyLis_s_.h"
  



/** Function oyList_New
 *  @memberof oyList_s
 *  @brief   allocate a new List object
 */
OYAPI oyList_s * OYEXPORT
  oyList_New( oyObject_s object )
{
  oyObject_s s = object;
  oyList_s_ * list = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  list = oyList_New_( s );

  return (oyList_s*) list;
}

/** Function  oyList_Copy
 *  @memberof oyList_s
 *  @brief    Copy or Reference a List object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     list                 List struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyList_s* OYEXPORT
  oyList_Copy( oyList_s *list, oyObject_s object )
{
  oyList_s_ * s = (oyList_s_*) list;

  if(s)
  {
    oyCheckType__m( oyOBJECT_LIST_S, return NULL )
  }
  else
    return NULL;

  s = oyList_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyList_s" );

  return (oyList_s*) s;
}
 
/** Function oyList_Release
 *  @memberof oyList_s
 *  @brief   release and possibly deallocate a oyList_s object
 *
 *  @param[in,out] list                 List struct object
 */
OYAPI int OYEXPORT
  oyList_Release( oyList_s **list )
{
  oyList_s_ * s = 0;

  if(!list || !*list)
    return 0;

  s = (oyList_s_*) *list;

  oyCheckType__m( oyOBJECT_LIST_S, return 1 )

  *list = 0;

  return oyList_Release_( &s );
}

/** Function oyList_MoveIn
 *  @memberof oyList_s
 *  @brief   add an element to a List list
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
           oyList_MoveIn          ( oyList_s       * list,
                                       oyLis_s       ** obj,
                                       int                 pos )
{
  oyList_s_ * s = (oyList_s_*)list;
  int error = 0;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_LIST_S, return 1 )

  if(obj && *obj &&
     (*obj)->type_ == oyOBJECT_LIS_S)
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

/** Function oyList_ReleaseAt
 *  @memberof oyList_s
 *  @brief   release a element from a List list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI int  OYEXPORT
           oyList_ReleaseAt       ( oyList_s       * list,
                                       int                 pos )
{ 
  int error = !list;
  oyList_s_ * s = (oyList_s_*)list;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_LIST_S, return 1 )

  if(!error && oyStructList_GetParentObjType(s->list_) != oyOBJECT_LIST_S)
    error = 1;
  
  if(!error)
    oyStructList_ReleaseAt( s->list_, pos );

  return error;
}

/** Function oyList_Get
 *  @memberof oyList_s
 *  @brief   get a element of a List list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI oyLis_s * OYEXPORT
           oyList_Get             ( oyList_s       * list,
                                       int                 pos )
{       
  int error = !list;
  oyList_s_ * s = (oyList_s_*)list;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_LIST_S, return 0 )

  return (oyLis_s *) oyStructList_GetRefType( s->list_, pos, oyOBJECT_LIS_S);
}   

/** Function oyList_Count
 *  @memberof oyList_s
 *  @brief   count the elements in a List list
 *
 *  @param[in,out] list                the list
 *  @return                            element count
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI int  OYEXPORT
           oyList_Count           ( oyList_s       * list )
{       
  int error = !list;
  oyList_s_ * s = (oyList_s_*)list;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_LIST_S, return 0 )

  if(!error)
    return oyStructList_Count( s->list_ );
  else return 0;
}

/** Function oyList_Clear
 *  @memberof oyList_s
 *  @brief   clear the elements in a List list
 *
 *  @param[in,out] list                the list
 *  @return                            0 - success; otherwise - error
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/01 (Oyranos: 0.3.0)
 *  @date    2011/02/01
 */
OYAPI int  OYEXPORT
           oyList_Clear           ( oyList_s       * list )
{       
  int error = !list;
  oyList_s_ * s = (oyList_s_*)list;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_LIST_S, return 0 )

  return oyStructList_Clear( s->list_ );
}

/** Function oyList_Sort
 *  @memberof oyList_s
 *  @brief   sort a list according to a rank_list
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/01 (Oyranos: 0.3.0)
 *  @date    2011/02/01
 */
OYAPI int  OYEXPORT
           oyList_Sort            ( oyList_s       * list,
                                       int32_t           * rank_list )
{       
  int error = !list;
  oyList_s_ * s = (oyList_s_*)list;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_LIST_S, return 0 )

  return oyStructList_Sort( s->list_, rank_list );
}



/* Include "List.public_methods_definitions.c" { */

/* } Include "List.public_methods_definitions.c" */

