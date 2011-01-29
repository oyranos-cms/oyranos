/** @file oyName_s.c

   [Template file inheritance graph]
   +-- Name_s.template.c

 *  Oyranos is an open source Colour Management System
 *
 *  @par Copyright:
 *            2004-2011 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @since    2011/01/29
 */


#include "oyranos_object_internal.h"

#include "oyName_s.h"
#include "oyName_s_.h"

#include "oyObject_s.h"

/** @brief oyName_s new
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/08 (API 0.1.8)
 */
oyName_s *   oyName_new              ( oyObject_s          object )
{
  oyAlloc_f allocateFunc = oyAllocateFunc_;
  /* ---- start of object constructor ----- */
  oyOBJECT_e type = oyOBJECT_NAME_S;
# define STRUCT_TYPE oyName_s
  int error = 0;
  STRUCT_TYPE * s = 0;

  if(object)
    allocateFunc = object->allocateFunc_;

  s = (STRUCT_TYPE*)allocateFunc(sizeof(STRUCT_TYPE));

  if(!s)
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(STRUCT_TYPE) );

  s->type = type;

  s->copy = (oyStruct_Copy_f) oyName_copy;
  s->release = (oyStruct_Release_f) oyName_release;
# undef STRUCT_TYPE
  /* ---- end of object constructor ------- */

  return s;
}

/** @brief oyName_s copy
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/08 (API 0.1.8)
 */
oyName_s *   oyName_copy               ( oyName_s        * obj,
                                         oyObject_s        object )
{
  int error = 0;
  oyName_s * s = 0;

  if(!obj)
    return s;

  s = oyName_new( object );

  error = oyName_copy_( s, obj, object );

  if(error)
    oyName_release( &s );

  return s;
}

/** @brief oyName_s deallocation
 *
 *  @since Oyranos: version 0.1.8
 *  @date  2008/01/08 (API 0.1.8)
 */
int          oyName_release          ( oyName_s         ** obj )
{
  int error = 0;

  if(!obj || !*obj)
    return 0;

  error = oyName_release_(obj, oyDeAllocateFunc_);

  *obj = 0;

  return error;
}
