/** @file oyCMMapis_s.c

   [Template file inheritance graph]
   +-> oyCMMapis_s.template.c
   |
   +-> BaseList_s.c
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


  
#include "oyCMMapis_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"



#include "oyCMMapis_s_.h"

#include "oyCMMapi_s_.h"

#include "oyranos_module_internal.h"
  



/** Function oyCMMapis_New
 *  @memberof oyCMMapis_s
 *  @brief   allocate a new CMMapis object
 */
OYAPI oyCMMapis_s * OYEXPORT
  oyCMMapis_New( oyObject_s object )
{
  oyObject_s s = object;
  oyCMMapis_s_ * cmmapis = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  cmmapis = oyCMMapis_New_( s );

  return (oyCMMapis_s*) cmmapis;
}

/** Function  oyCMMapis_Copy
 *  @memberof oyCMMapis_s
 *  @brief    Copy or Reference a CMMapis object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     cmmapis                 CMMapis struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyCMMapis_s* OYEXPORT
  oyCMMapis_Copy( oyCMMapis_s *cmmapis, oyObject_s object )
{
  oyCMMapis_s_ * s = (oyCMMapis_s_*) cmmapis;

  if(s)
  {
    oyCheckType__m( oyOBJECT_CMM_APIS_S, return NULL )
  }
  else
    return NULL;

  s = oyCMMapis_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyCMMapis_s" );

  return (oyCMMapis_s*) s;
}
 
/** Function oyCMMapis_Release
 *  @memberof oyCMMapis_s
 *  @brief   release and possibly deallocate a oyCMMapis_s object
 *
 *  @param[in,out] cmmapis                 CMMapis struct object
 */
OYAPI int OYEXPORT
  oyCMMapis_Release( oyCMMapis_s **cmmapis )
{
  oyCMMapis_s_ * s = 0;

  if(!cmmapis || !*cmmapis)
    return 0;

  s = (oyCMMapis_s_*) *cmmapis;

  oyCheckType__m( oyOBJECT_CMM_APIS_S, return 1 )

  *cmmapis = 0;

  return oyCMMapis_Release_( &s );
}

/** Function oyCMMapis_MoveIn
 *  @memberof oyCMMapis_s
 *  @brief   add an element to a CMMapis list
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
           oyCMMapis_MoveIn          ( oyCMMapis_s       * list,
                                       oyCMMapi_s       ** obj,
                                       int                 pos )
{
  oyCMMapis_s_ * s = (oyCMMapis_s_*)list;
  int error = 0;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_CMM_APIS_S, return 1 )

  if(obj && *obj &&
     (*obj)->type_ >= oyOBJECT_CMM_API1_S &&
     (*obj)->type_ <= oyOBJECT_CMM_API_MAX)
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

/** Function oyCMMapis_ReleaseAt
 *  @memberof oyCMMapis_s
 *  @brief   release a element from a CMMapis list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI int  OYEXPORT
           oyCMMapis_ReleaseAt       ( oyCMMapis_s       * list,
                                       int                 pos )
{ 
  int error = !list;
  oyCMMapis_s_ * s = (oyCMMapis_s_*)list;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_CMM_APIS_S, return 1 )

  if(!error && oyStructList_GetParentObjType(s->list_) != oyOBJECT_CMM_APIS_S)
    error = 1;
  
  if(!error)
    oyStructList_ReleaseAt( s->list_, pos );

  return error;
}

/** Function oyCMMapis_Get
 *  @memberof oyCMMapis_s
 *  @brief   get a element of a CMMapis list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI oyCMMapi_s * OYEXPORT
           oyCMMapis_Get             ( oyCMMapis_s       * list,
                                       int                 pos )
{       
  oyCMMapis_s_ * s = (oyCMMapis_s_*)list;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_CMM_APIS_S, return 0 )

  return (oyCMMapi_s *) oyStructList_GetRef( s->list_, pos );
}   

/** Function oyCMMapis_Count
 *  @memberof oyCMMapis_s
 *  @brief   count the elements in a CMMapis list
 *
 *  @param[in,out] list                the list
 *  @return                            element count
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI int  OYEXPORT
           oyCMMapis_Count           ( oyCMMapis_s       * list )
{       
  oyCMMapis_s_ * s = (oyCMMapis_s_*)list;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_CMM_APIS_S, return 0 )

  return oyStructList_Count( s->list_ );
}

/** Function oyCMMapis_Clear
 *  @memberof oyCMMapis_s
 *  @brief   clear the elements in a CMMapis list
 *
 *  @param[in,out] list                the list
 *  @return                            0 - success; otherwise - error
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/01 (Oyranos: 0.3.0)
 *  @date    2011/02/01
 */
OYAPI int  OYEXPORT
           oyCMMapis_Clear           ( oyCMMapis_s       * list )
{       
  oyCMMapis_s_ * s = (oyCMMapis_s_*)list;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_CMM_APIS_S, return 0 )

  return oyStructList_Clear( s->list_ );
}

/** Function oyCMMapis_Sort
 *  @memberof oyCMMapis_s
 *  @brief   sort a list according to a rank_list
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/01 (Oyranos: 0.3.0)
 *  @date    2011/02/01
 */
OYAPI int  OYEXPORT
           oyCMMapis_Sort            ( oyCMMapis_s       * list,
                                       int32_t           * rank_list )
{       
  oyCMMapis_s_ * s = (oyCMMapis_s_*)list;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_CMM_APIS_S, return 0 )

  return oyStructList_Sort( s->list_, rank_list );
}



/* Include "CMMapis.public_methods_definitions.c" { */

/* } Include "CMMapis.public_methods_definitions.c" */

