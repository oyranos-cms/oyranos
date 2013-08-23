/** @file oyNamedColors_s.h

   [Template file inheritance graph]
   +-> oyNamedColors_s.template.h
   |
   +-> BaseList_s.h
   |
   +-> Base_s.h
   |
   +-- oyStruct_s.template.h

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



#ifndef OY_NAMED_COLORS_S_H
#define OY_NAMED_COLORS_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyNamedColors_s oyNamedColors_s;



#include "oyStruct_s.h"

#include "oyNamedColor_s.h"


/* Include "NamedColors.public.h" { */

/* } Include "NamedColors.public.h" */


/* Include "NamedColors.dox" { */
/** @struct  oyNamedColors_s
 *  @ingroup objects_single_color
 *  @extends oyStruct_s
 *  @brief   List of Single Color objects
 *
 *  @note This class holds a list of oyNamedColor_s objects [list]
 *
 *  @version Oyranos: 0.9.5
 *  @since   2007/10/00 (Oyranos: 0.1.8)
 *  @date    2013/08/15
 */

/* } Include "NamedColors.dox" */

struct oyNamedColors_s {
/* Include "Struct.members.h" { */
oyOBJECT_e           type_;          /**< @private struct type */
oyStruct_Copy_f      copy;           /**< copy function */
oyStruct_Release_f   release;        /**< release function */
oyObject_s           oy_;            /**< @private features name and hash */

/* } Include "Struct.members.h" */
};



OYAPI oyNamedColors_s* OYEXPORT
  oyNamedColors_New( oyObject_s object );
OYAPI oyNamedColors_s* OYEXPORT
  oyNamedColors_Copy( oyNamedColors_s *namedcolors, oyObject_s obj );
OYAPI int OYEXPORT
  oyNamedColors_Release( oyNamedColors_s **namedcolors );

OYAPI int  OYEXPORT
           oyNamedColors_MoveIn          ( oyNamedColors_s       * list,
                                       oyNamedColor_s       ** ptr,
                                       int                 pos );
OYAPI int  OYEXPORT
           oyNamedColors_ReleaseAt       ( oyNamedColors_s       * list,
                                       int                 pos );
OYAPI oyNamedColor_s * OYEXPORT
           oyNamedColors_Get             ( oyNamedColors_s       * list,
                                       int                 pos );
OYAPI int  OYEXPORT
           oyNamedColors_Count           ( oyNamedColors_s       * list );
OYAPI int  OYEXPORT
           oyNamedColors_Clear           ( oyNamedColors_s       * list );
OYAPI int  OYEXPORT
           oyNamedColors_Sort            ( oyNamedColors_s       * list,
                                       int32_t           * rank_list );



/* Include "NamedColors.public_methods_declarations.h" { */

/* } Include "NamedColors.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_NAMED_COLORS_S_H */
