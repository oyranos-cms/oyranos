/** @file oyCMMapiFilter_s.c

   [Template file inheritance graph]
   +-> oyCMMapiFilter_s.template.c
   |
   +-> oyCMMapi_s.template.c
   |
   +-> Base_s.c
   |
   +-- oyStruct_s.template.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2012 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @date     2012/12/13
 */


  
#include "oyCMMapiFilter_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyCMMapiFilter_s_.h"
  


/** Function oyCMMapiFilter_New
 *  @memberof oyCMMapiFilter_s
 *  @brief   allocate a new CMMapiFilter object
 */
OYAPI oyCMMapiFilter_s * OYEXPORT
  oyCMMapiFilter_New( oyObject_s object )
{
  oyObject_s s = object;
  oyCMMapiFilter_s_ * cmmapifilter = 0;

  if(s)
    oyCheckType__m( oyOBJECT_OBJECT_S, return 0 )

  cmmapifilter = oyCMMapiFilter_New_( s );

  return (oyCMMapiFilter_s*) cmmapifilter;
}

/** Function oyCMMapiFilter_Copy
 *  @memberof oyCMMapiFilter_s
 *  @brief   copy or reference a CMMapiFilter object
 *
 *  The function is for copying and for referencing. The reference is the most
 *  often used way, which saves resourcs and time.
 *
 *  @param[in]     cmmapifilter                 CMMapiFilter struct object
 *  @param         object              NULL - means reference,
 *                                     the optional object triggers a real copy
 */
OYAPI oyCMMapiFilter_s* OYEXPORT
  oyCMMapiFilter_Copy( oyCMMapiFilter_s *cmmapifilter, oyObject_s object )
{
  oyCMMapiFilter_s_ * s = (oyCMMapiFilter_s_*) cmmapifilter;

  if(s)
    oyCheckType__m( oyOBJECT_CMM_API_FILTER_S, return 0 )

  s = oyCMMapiFilter_Copy_( s, object );

  return (oyCMMapiFilter_s*) s;
}
 
/** Function oyCMMapiFilter_Release
 *  @memberof oyCMMapiFilter_s
 *  @brief   release and possibly deallocate a oyCMMapiFilter_s object
 *
 *  @param[in,out] cmmapifilter                 CMMapiFilter struct object
 */
OYAPI int OYEXPORT
  oyCMMapiFilter_Release( oyCMMapiFilter_s **cmmapifilter )
{
  oyCMMapiFilter_s_ * s = 0;

  if(!cmmapifilter || !*cmmapifilter)
    return 0;

  s = (oyCMMapiFilter_s_*) *cmmapifilter;

  oyCheckType__m( oyOBJECT_CMM_API_FILTER_S, return 1 )

  *cmmapifilter = 0;

  return oyCMMapiFilter_Release_( &s );
}



/* Include "CMMapiFilter.public_methods_definitions.c" { */

/* } Include "CMMapiFilter.public_methods_definitions.c" */

