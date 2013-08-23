/** @file oyNamedColors_s_.h

   [Template file inheritance graph]
   +-> oyNamedColors_s_.template.h
   |
   +-> BaseList_s_.h
   |
   +-- Base_s_.h

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


#ifndef OY_NAMED_COLORS_S__H
#define OY_NAMED_COLORS_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyNamedColorsPriv_m( var ) ((oyNamedColors_s_*) (var))

typedef struct oyNamedColors_s_ oyNamedColors_s_;

  
#include <oyranos_object.h>

#include "oyStructList_s.h"
  

#include "oyNamedColors_s.h"

/* Include "NamedColors.private.h" { */

/* } Include "NamedColors.private.h" */

/** @internal
 *  @struct   oyNamedColors_s_
 *  @brief      List of Single Color objects
 *  @ingroup  objects_single_color
 *  @extends  oyStruct_s
 */
struct oyNamedColors_s_ {

/* Include "Struct.members.h" { */
oyOBJECT_e           type_;          /**< @private struct type */
oyStruct_Copy_f      copy;           /**< copy function */
oyStruct_Release_f   release;        /**< release function */
oyObject_s           oy_;            /**< @private features name and hash */

/* } Include "Struct.members.h" */


  oyStructList_s     * list_;          /**< the list data */

};



oyNamedColors_s_*
  oyNamedColors_New_( oyObject_s object );
oyNamedColors_s_*
  oyNamedColors_Copy_( oyNamedColors_s_ *namedcolors, oyObject_s object);
oyNamedColors_s_*
  oyNamedColors_Copy__( oyNamedColors_s_ *namedcolors, oyObject_s object);
int
  oyNamedColors_Release_( oyNamedColors_s_ **namedcolors );

#if 0
/* FIXME Are these needed at all?? */
int
           oyNamedColors_MoveIn_         ( oyNamedColors_s_      * list,
                                       oyNamedColor_s       ** ptr,
                                       int                 pos );
int
           oyNamedColors_ReleaseAt_      ( oyNamedColors_s_      * list,
                                       int                 pos );
oyNamedColor_s *
           oyNamedColors_Get_            ( oyNamedColors_s_      * list,
                                       int                 pos );
int
           oyNamedColors_Count_          ( oyNamedColors_s_      * list );
#endif



/* Include "NamedColors.private_methods_declarations.h" { */

/* } Include "NamedColors.private_methods_declarations.h" */




#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_NAMED_COLORS_S__H */
