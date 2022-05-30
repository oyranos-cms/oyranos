/** @file oyFilterNodes_s.c

   [Template file inheritance graph]
   +-> oyFilterNodes_s.template.c
   |
   +-> BaseList_s.c
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


  
#include "oyFilterNodes_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"


#include "oyFilterNodes_s_.h"

#include "oyFilterNode_s_.h"
  



/** Function oyFilterNodes_New
 *  @memberof oyFilterNodes_s
 *  @brief   allocate a new FilterNodes object
 */
OYAPI oyFilterNodes_s * OYEXPORT
  oyFilterNodes_New( oyObject_s object )
{
  oyObject_s s = object;
  oyFilterNodes_s_ * filternodes = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  filternodes = oyFilterNodes_New_( s );

  return (oyFilterNodes_s*) filternodes;
}

/** Function  oyFilterNodes_Copy
 *  @memberof oyFilterNodes_s
 *  @brief    Copy or Reference a FilterNodes object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     filternodes                 FilterNodes struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyFilterNodes_s* OYEXPORT
  oyFilterNodes_Copy( oyFilterNodes_s *filternodes, oyObject_s object )
{
  oyFilterNodes_s_ * s = (oyFilterNodes_s_*) filternodes;

  if(s)
  {
    oyCheckType__m( oyOBJECT_FILTER_NODES_S, return NULL )
  }
  else
    return NULL;

  s = oyFilterNodes_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyFilterNodes_s" );

  return (oyFilterNodes_s*) s;
}
 
/** Function oyFilterNodes_Release
 *  @memberof oyFilterNodes_s
 *  @brief   release and possibly deallocate a oyFilterNodes_s object
 *
 *  @param[in,out] filternodes                 FilterNodes struct object
 */
OYAPI int OYEXPORT
  oyFilterNodes_Release( oyFilterNodes_s **filternodes )
{
  oyFilterNodes_s_ * s = 0;

  if(!filternodes || !*filternodes)
    return 0;

  s = (oyFilterNodes_s_*) *filternodes;

  oyCheckType__m( oyOBJECT_FILTER_NODES_S, return 1 )

  *filternodes = 0;

  return oyFilterNodes_Release_( &s );
}

/** Function oyFilterNodes_MoveIn
 *  @memberof oyFilterNodes_s
 *  @brief   add an element to a FilterNodes list
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
           oyFilterNodes_MoveIn          ( oyFilterNodes_s       * list,
                                       oyFilterNode_s       ** obj,
                                       int                 pos )
{
  oyFilterNodes_s_ * s = (oyFilterNodes_s_*)list;
  int error = 0;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_FILTER_NODES_S, return 1 )

  if(obj && *obj &&
     (*obj)->type_ == oyOBJECT_FILTER_NODE_S)
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

/** Function oyFilterNodes_ReleaseAt
 *  @memberof oyFilterNodes_s
 *  @brief   release a element from a FilterNodes list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI int  OYEXPORT
           oyFilterNodes_ReleaseAt       ( oyFilterNodes_s       * list,
                                       int                 pos )
{ 
  int error = !list;
  oyFilterNodes_s_ * s = (oyFilterNodes_s_*)list;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_FILTER_NODES_S, return 1 )

  if(!error && oyStructList_GetParentObjType(s->list_) != oyOBJECT_FILTER_NODES_S)
    error = 1;
  
  if(!error)
    oyStructList_ReleaseAt( s->list_, pos );

  return error;
}

/** Function oyFilterNodes_Get
 *  @memberof oyFilterNodes_s
 *  @brief   get a element of a FilterNodes list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI oyFilterNode_s * OYEXPORT
           oyFilterNodes_Get             ( oyFilterNodes_s       * list,
                                       int                 pos )
{       
  oyFilterNodes_s_ * s = (oyFilterNodes_s_*)list;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_NODES_S, return 0 )

  return (oyFilterNode_s *) oyStructList_GetRefType( s->list_, pos, oyOBJECT_FILTER_NODE_S);
}   

/** Function oyFilterNodes_Count
 *  @memberof oyFilterNodes_s
 *  @brief   count the elements in a FilterNodes list
 *
 *  @param[in,out] list                the list
 *  @return                            element count
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI int  OYEXPORT
           oyFilterNodes_Count           ( oyFilterNodes_s       * list )
{       
  oyFilterNodes_s_ * s = (oyFilterNodes_s_*)list;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_NODES_S, return 0 )

  return oyStructList_Count( s->list_ );
}

/** Function oyFilterNodes_Clear
 *  @memberof oyFilterNodes_s
 *  @brief   clear the elements in a FilterNodes list
 *
 *  @param[in,out] list                the list
 *  @return                            0 - success; otherwise - error
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/01 (Oyranos: 0.3.0)
 *  @date    2011/02/01
 */
OYAPI int  OYEXPORT
           oyFilterNodes_Clear           ( oyFilterNodes_s       * list )
{       
  oyFilterNodes_s_ * s = (oyFilterNodes_s_*)list;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_FILTER_NODES_S, return 0 )

  return oyStructList_Clear( s->list_ );
}

/** Function oyFilterNodes_Sort
 *  @memberof oyFilterNodes_s
 *  @brief   sort a list according to a rank_list
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/01 (Oyranos: 0.3.0)
 *  @date    2011/02/01
 */
OYAPI int  OYEXPORT
           oyFilterNodes_Sort            ( oyFilterNodes_s       * list,
                                       int32_t           * rank_list )
{       
  oyFilterNodes_s_ * s = (oyFilterNodes_s_*)list;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_FILTER_NODES_S, return 0 )

  return oyStructList_Sort( s->list_, rank_list );
}



/* Include "FilterNodes.public_methods_definitions.c" { */

/* } Include "FilterNodes.public_methods_definitions.c" */

