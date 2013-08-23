/** @file oyNamedColor_s_.h

   [Template file inheritance graph]
   +-> oyNamedColor_s_.template.h
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


#ifndef OY_NAMED_COLOR_S__H
#define OY_NAMED_COLOR_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyNamedColorPriv_m( var ) ((oyNamedColor_s_*) (var))

typedef struct oyNamedColor_s_ oyNamedColor_s_;



#include "oyranos_colour.h"
#include "oyranos_image.h"

#include "oyProfile_s.h"
#include "oyImage_s.h"
#include "oyConversion_s.h"
  
#include <oyranos_object.h>
  

#include "oyNamedColor_s.h"

/* Include "NamedColor.private.h" { */

/* } Include "NamedColor.private.h" */

/** @internal
 *  @struct   oyNamedColor_s_
 *  @brief      Single Color object
 *  @ingroup  objects_single_color
 *  @extends  oyStruct_s
 */
struct oyNamedColor_s_ {

/* Include "Struct.members.h" { */
oyOBJECT_e           type_;          /**< @private struct type */
oyStruct_Copy_f      copy;           /**< copy function */
oyStruct_Release_f   release;        /**< release function */
oyObject_s           oy_;            /**< @private features name and hash */

/* } Include "Struct.members.h" */



/* Include "NamedColor.members.h" { */
  double             * channels_;      /**< eigther parsed or calculated otherwise */
  double               XYZ_[3];        /**< CIE*XYZ representation */
  char               * blob_;          /**< advanced : CGATS / ICC ? */
  size_t               blob_len_;      /**< advanced : CGATS / ICC ? */
  oyProfile_s        * profile_;       /**< ICC */


/* } Include "NamedColor.members.h" */

};


oyNamedColor_s_*
  oyNamedColor_New_( oyObject_s object );
oyNamedColor_s_*
  oyNamedColor_Copy_( oyNamedColor_s_ *namedcolor, oyObject_s object);
oyNamedColor_s_*
  oyNamedColor_Copy__( oyNamedColor_s_ *namedcolor, oyObject_s object);
int
  oyNamedColor_Release_( oyNamedColor_s_ **namedcolor );



/* Include "NamedColor.private_methods_declarations.h" { */
void              oyCopyColor        ( const double      * from,
                                       double            * to,
                                       int                 n,
                                       oyProfile_s       * profile_ref,
                                       int                 channels_n );

/* } Include "NamedColor.private_methods_declarations.h" */



void oyNamedColor_Release__Members( oyNamedColor_s_ * namedcolor );
int oyNamedColor_Init__Members( oyNamedColor_s_ * namedcolor );
int oyNamedColor_Copy__Members( oyNamedColor_s_ * dst, oyNamedColor_s_ * src);


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_NAMED_COLOR_S__H */
