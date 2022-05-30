/** @file oyCMMapiFilters_s.c

   [Template file inheritance graph]
   +-> oyCMMapiFilters_s.template.c
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


  
#include "oyCMMapiFilters_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"



#include "oyCMMapiFilters_s_.h"

#include "oyCMMapiFilter_s_.h"

#include "oyranos_module_internal.h"
  



/** Function oyCMMapiFilters_New
 *  @memberof oyCMMapiFilters_s
 *  @brief   allocate a new CMMapiFilters object
 */
OYAPI oyCMMapiFilters_s * OYEXPORT
  oyCMMapiFilters_New( oyObject_s object )
{
  oyObject_s s = object;
  oyCMMapiFilters_s_ * cmmapifilters = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  cmmapifilters = oyCMMapiFilters_New_( s );

  return (oyCMMapiFilters_s*) cmmapifilters;
}

/** Function  oyCMMapiFilters_Copy
 *  @memberof oyCMMapiFilters_s
 *  @brief    Copy or Reference a CMMapiFilters object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     cmmapifilters                 CMMapiFilters struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyCMMapiFilters_s* OYEXPORT
  oyCMMapiFilters_Copy( oyCMMapiFilters_s *cmmapifilters, oyObject_s object )
{
  oyCMMapiFilters_s_ * s = (oyCMMapiFilters_s_*) cmmapifilters;

  if(s)
  {
    oyCheckType__m( oyOBJECT_CMM_API_FILTERS_S, return NULL )
  }
  else
    return NULL;

  s = oyCMMapiFilters_Copy_( s, object );

  if(oy_debug_objects >= 0)
    oyObjectDebugMessage_( s?s->oy_:NULL, __func__, "oyCMMapiFilters_s" );

  return (oyCMMapiFilters_s*) s;
}
 
/** Function oyCMMapiFilters_Release
 *  @memberof oyCMMapiFilters_s
 *  @brief   release and possibly deallocate a oyCMMapiFilters_s object
 *
 *  @param[in,out] cmmapifilters                 CMMapiFilters struct object
 */
OYAPI int OYEXPORT
  oyCMMapiFilters_Release( oyCMMapiFilters_s **cmmapifilters )
{
  oyCMMapiFilters_s_ * s = 0;

  if(!cmmapifilters || !*cmmapifilters)
    return 0;

  s = (oyCMMapiFilters_s_*) *cmmapifilters;

  oyCheckType__m( oyOBJECT_CMM_API_FILTERS_S, return 1 )

  *cmmapifilters = 0;

  return oyCMMapiFilters_Release_( &s );
}

/** Function oyCMMapiFilters_MoveIn
 *  @memberof oyCMMapiFilters_s
 *  @brief   add an element to a CMMapiFilters list
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
           oyCMMapiFilters_MoveIn          ( oyCMMapiFilters_s       * list,
                                       oyCMMapiFilter_s       ** obj,
                                       int                 pos )
{
  oyCMMapiFilters_s_ * s = (oyCMMapiFilters_s_*)list;
  int error = 0;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_CMM_API_FILTERS_S, return 1 )

  if(obj && *obj &&
     oyIsOfTypeCMMapiFilter( (*obj)->type_ ))
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

/** Function oyCMMapiFilters_ReleaseAt
 *  @memberof oyCMMapiFilters_s
 *  @brief   release a element from a CMMapiFilters list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI int  OYEXPORT
           oyCMMapiFilters_ReleaseAt       ( oyCMMapiFilters_s       * list,
                                       int                 pos )
{ 
  int error = !list;
  oyCMMapiFilters_s_ * s = (oyCMMapiFilters_s_*)list;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_CMM_API_FILTERS_S, return 1 )

  if(!error && oyStructList_GetParentObjType(s->list_) != oyOBJECT_CMM_API_FILTERS_S)
    error = 1;
  
  if(!error)
    oyStructList_ReleaseAt( s->list_, pos );

  return error;
}

/** Function oyCMMapiFilters_Get
 *  @memberof oyCMMapiFilters_s
 *  @brief   get a element of a CMMapiFilters list
 *
 *  @param[in,out] list                the list
 *  @param         pos                 position
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI oyCMMapiFilter_s * OYEXPORT
           oyCMMapiFilters_Get             ( oyCMMapiFilters_s       * list,
                                       int                 pos )
{       
  oyCMMapiFilters_s_ * s = (oyCMMapiFilters_s_*)list;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_CMM_API_FILTERS_S, return 0 )

  return (oyCMMapiFilter_s *) oyStructList_GetRef( s->list_, pos );
}   

/** Function oyCMMapiFilters_Count
 *  @memberof oyCMMapiFilters_s
 *  @brief   count the elements in a CMMapiFilters list
 *
 *  @param[in,out] list                the list
 *  @return                            element count
 *
 *  @version Oyranos: 0.1.10
 *  @since   2010/07/05 (Oyranos: 0.1.10)
 *  @date    2010/07/05
 */
OYAPI int  OYEXPORT
           oyCMMapiFilters_Count           ( oyCMMapiFilters_s       * list )
{       
  oyCMMapiFilters_s_ * s = (oyCMMapiFilters_s_*)list;

  if(!s)
    return 0;

  oyCheckType__m( oyOBJECT_CMM_API_FILTERS_S, return 0 )

  return oyStructList_Count( s->list_ );
}

/** Function oyCMMapiFilters_Clear
 *  @memberof oyCMMapiFilters_s
 *  @brief   clear the elements in a CMMapiFilters list
 *
 *  @param[in,out] list                the list
 *  @return                            0 - success; otherwise - error
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/01 (Oyranos: 0.3.0)
 *  @date    2011/02/01
 */
OYAPI int  OYEXPORT
           oyCMMapiFilters_Clear           ( oyCMMapiFilters_s       * list )
{       
  oyCMMapiFilters_s_ * s = (oyCMMapiFilters_s_*)list;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_CMM_API_FILTERS_S, return 0 )

  return oyStructList_Clear( s->list_ );
}

/** Function oyCMMapiFilters_Sort
 *  @memberof oyCMMapiFilters_s
 *  @brief   sort a list according to a rank_list
 *
 *  @version Oyranos: 0.3.0
 *  @since   2011/02/01 (Oyranos: 0.3.0)
 *  @date    2011/02/01
 */
OYAPI int  OYEXPORT
           oyCMMapiFilters_Sort            ( oyCMMapiFilters_s       * list,
                                       int32_t           * rank_list )
{       
  oyCMMapiFilters_s_ * s = (oyCMMapiFilters_s_*)list;

  if(!s)
    return -1;

  oyCheckType__m( oyOBJECT_CMM_API_FILTERS_S, return 0 )

  return oyStructList_Sort( s->list_, rank_list );
}



/* Include "CMMapiFilters.public_methods_definitions.c" { */

/* } Include "CMMapiFilters.public_methods_definitions.c" */

