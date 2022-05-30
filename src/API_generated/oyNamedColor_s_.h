/** @file oyNamedColor_s_.h

   [Template file inheritance graph]
   +-> oyNamedColor_s_.template.h
   |
   +-- Base_s_.h

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


#ifndef OY_NAMED_COLOR_S__H
#define OY_NAMED_COLOR_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define oyNamedColorPriv_m( var ) ((oyNamedColor_s_*) (var))

typedef struct oyNamedColor_s_ oyNamedColor_s_;



#include "oyranos_color.h"
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
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

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
