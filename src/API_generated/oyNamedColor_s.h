/** @file oyNamedColor_s.h

   [Template file inheritance graph]
   +-> oyNamedColor_s.template.h
   |
   +-> Base_s.h
   |
   +-- oyStruct_s.template.h

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */



#ifndef OY_NAMED_COLOR_S_H
#define OY_NAMED_COLOR_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */




#include "oyranos_image.h"
#include "oyProfile_s.h"
  
#include <oyranos_object.h>

typedef struct oyNamedColor_s oyNamedColor_s;


#include "oyStruct_s.h"


/* Include "NamedColor.public.h" { */

/* } Include "NamedColor.public.h" */


/* Include "NamedColor.dox" { */
/** @struct  oyNamedColor_s
 *  @ingroup objects_single_color
 *  @extends oyStruct_s
 *  @brief   Single Color object
 *
 *  @version Oyranos: 0.9.5
 *  @since   2007/10/00 (Oyranos: 0.1.8)
 *  @date    2013/08/15
 */

/* } Include "NamedColor.dox" */

struct oyNamedColor_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};


OYAPI oyNamedColor_s* OYEXPORT
  oyNamedColor_New( oyObject_s object );
OYAPI oyNamedColor_s* OYEXPORT
  oyNamedColor_Copy( oyNamedColor_s *namedcolor, oyObject_s obj );
OYAPI int OYEXPORT
  oyNamedColor_Release( oyNamedColor_s **namedcolor );



/* Include "NamedColor.public_methods_declarations.h" { */
oyNamedColor_s *   oyNamedColor_Create(const double      * chan,
                                       const char        * blob,
                                       int                 blob_len,
                                       oyProfile_s       * profile_ref,
                                       oyObject_s          object );
oyNamedColor_s *   oyNamedColor_CreateWithName (
                                       const char        * nick,
                                       const char        * name,
                                       const char        * description,
                                       const double      * chan,
                                       const double      * XYZ,
                                       const char        * blob,
                                       int                 blob_len,
                                       oyProfile_s       * profile_ref,
                                       oyObject_s          object );

oyProfile_s *      oyNamedColor_GetSpaceRef ( oyNamedColor_s  * color );
void               oyNamedColor_SetChannels (
                                       oyNamedColor_s    * color,
                                       const double      * channels,
                                       uint32_t            flags );
int                oyNamedColor_SetColorStd (
                                       oyNamedColor_s    * color,
                                       oyPROFILE_e         color_space,
                                       oyPointer           channels,
                                       oyDATATYPE_e        channels_type,
                                       uint32_t            flags,
                                       oyOptions_s       * options );
const double *    oyNamedColor_GetChannelsConst (
                                       oyNamedColor_s    * color,
                                       uint32_t            flags );
const double *     oyNamedColor_GetXYZConst ( oyNamedColor_s * color);
int                oyNamedColor_GetColorStd (
                                       oyNamedColor_s    * color,
                                       oyPROFILE_e         color_space,
                                       oyPointer           buf,
                                       oyDATATYPE_e        buf_type,
                                       uint32_t            flags,
                                       oyOptions_s       * options );
int                oyNamedColor_GetColor (
                                       oyNamedColor_s    * color,
                                       oyProfile_s       * profile,
                                       oyPointer           buf,
                                       oyDATATYPE_e        buf_type,
                                       uint32_t            flags,
                                       oyOptions_s       * options );
const char   *     oyNamedColor_GetName (
                                       oyNamedColor_s    * s,
                                       oyNAME_e            type,
                                       uint32_t            flags );


/* } Include "NamedColor.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_NAMED_COLOR_S_H */
