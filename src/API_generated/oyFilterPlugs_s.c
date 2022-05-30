/** @file oyFilterPlugs_s.c

   [Template file inheritance graph]
   +-> oyFilterPlugs_s.template.c
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


  
#include "oyFilterPlugs_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"


#include "oyFilterPlugs_s_.h"

#include "oyFilterPlug_s_.h"
  



/** Function oyFilterPlugs_New
 *  @memberof oyFilterPlugs_s
 *  @brief   allocate a new FilterPlugs object
 */
OYAPI oyFilterPlugs_s * OYEXPORT
  oyFilterPlugs_New( oyObject_s object )
{
  oyObject_s s = object;
  oyFilterPlugs_s_ * filterplugs = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  filterplugs = oyFilterPlugs_New_( s );

  return (oyFilterPlugs_s*) filterplugs;
}

/** Function  oyFilterPlugs_Copy
 *  @memberof oyFilterPlugs_s
 *  @brief    Copy or Reference a FilterPlugs object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     filterplugs                 FilterPlugs struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyFilterPlugs_s* OYEXPORT
  oyFilterPlugs_Copy( oyFilterPlugs_s *filterplugs, oyObject_s object )
{
  oyFilterPlugs_s_ * s = (oyFilterPlugs_s_*) filterplugs;

  if(s)
  {
    oyCheckType__m( oyOBJECT_FILTER_PLUGS_S, return NULL )
  }
  else
    return NULL;

  s = oyFilterPlugs_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyFilterPlugs_s" );

  return (oyFilterPlugs_s*) s;
}
 
/** Function oyFilterPlugs_Release
 *  @memberof oyFilterPlugs_s
 *  @brief   release and possibly deallocate a oyFilterPlugs_s object
 *
 *  @param[in,out] filterplugs                 FilterPlugs struct object
 */
OYAPI int OYEXPORT
  oyFilterPlugs_Release( oyFilterPlugs_s **filterplugs )
{
  oyFilterPlugs_s_ * s = 0;

  if(!filterplugs || !*filterplugs)
    return 0;

  s = (oyFilterPlugs_s_*) *filterplugs;

  oyCheckType__m( oyOBJECT_FILTER_PLUGS_S, return 1 )

  *filterplugs = 0;

  return oyFilterPlugs_Release_( &s );
}

/** Function oyFilterPlugs_MoveIn
 *  @memberof oyFilterPlugs_s
 *  @brief   add an element to a FilterPlugs list
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
           oyFilterPlugs_MoveIn          ( oyFilterPlugs_s       * list,
                                       oyFilterPlug_s       ** obj,
                                       int                 pos )
{
  oyFilterPlugs_s_ * s = (oyFilterPlugs_s_*)list;
  int error = 0;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_FILTER_PLUGS_S, return 1 )

  if(obj && *obj &&
     (*obj)->type_ == oyOBJECT_FILTER_PLUG_S)
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

/** Function oyFilterPlugs_ReleaseAt
 *  @memberof oyFilterPlugs_s
 *  @brief   release a element from a FilterPlugs list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI int  OYEXPORT
           oyFilterPlugs_ReleaseAt       ( oyFilterPlugs_s       * list,
                                       int                 pos )
{ 
  int error = !list;
  oyFilterPlugs_s_ * s = (oyFilterPlugs_s_*)list;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_FILTER_PLUGS_S, return 1 )

  if(!error && oyStructList_GetParentObjType(s->list_) != oyOBJECT_FILTER_PLUGS_S)
    error = 1;
  
  if(!error)
    oyStructList_ReleaseAt( s->list_, pos );

  return error;
}

/** Function oyFilterPlugs_Get
 *  @memberof oyFilterPlugs_s
 *  @brief   get a element of a FilterPlugs list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI oyFilterPlug_s * OYEXPORT
           oyFilterPlugs_Get             ( oyFilterPlugs_s       * list,
                                       int                 pos )
{       
  oyFilterPlugs_s_ * s = (oyFilterPlugs_s_*)list;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_PLUGS_S, return 0 )

  return (oyFilterPlug_s *) oyStructList_GetRefType( s->list_, pos, oyOBJECT_FILTER_PLUG_S);
}   

/** Function oyFilterPlugs_Count
 *  @memberof oyFilterPlugs_s
 *  @brief   count the elements in a FilterPlugs list
 *
 *  @param[in,out] list                the list
 *  @return                            element count
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI int  OYEXPORT
           oyFilterPlugs_Count           ( oyFilterPlugs_s       * list )
{       
  oyFilterPlugs_s_ * s = (oyFilterPlugs_s_*)list;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_FILTER_PLUGS_S, return 0 )

  return oyStructList_Count( s->list_ );
}

/** Function oyFilterPlugs_Clear
 *  @memberof oyFilterPlugs_s
 *  @brief   clear the elements in a FilterPlugs list
 *
 *  @param[in,out] list                the list
 *  @return                            0 - success; otherwise - error
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/01 (Oyranos: 0.3.0)
 *  @date    2011/02/01
 */
OYAPI int  OYEXPORT
           oyFilterPlugs_Clear           ( oyFilterPlugs_s       * list )
{       
  oyFilterPlugs_s_ * s = (oyFilterPlugs_s_*)list;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_FILTER_PLUGS_S, return 0 )

  return oyStructList_Clear( s->list_ );
}

/** Function oyFilterPlugs_Sort
 *  @memberof oyFilterPlugs_s
 *  @brief   sort a list according to a rank_list
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/01 (Oyranos: 0.3.0)
 *  @date    2011/02/01
 */
OYAPI int  OYEXPORT
           oyFilterPlugs_Sort            ( oyFilterPlugs_s       * list,
                                       int32_t           * rank_list )
{       
  oyFilterPlugs_s_ * s = (oyFilterPlugs_s_*)list;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_FILTER_PLUGS_S, return 0 )

  return oyStructList_Sort( s->list_, rank_list );
}



/* Include "FilterPlugs.public_methods_definitions.c" { */

/* } Include "FilterPlugs.public_methods_definitions.c" */

