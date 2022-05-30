/** @file oyCMMobjectType_s.h

   [Template file inheritance graph]
   +-> oyCMMobjectType_s.template.h
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



#ifndef OY_CMM_OBJECT_TYPE_S_H
#define OY_CMM_OBJECT_TYPE_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyCMMobjectType_s oyCMMobjectType_s;


#include "oyStruct_s.h"


/* Include "CMMobjectType.public.h" { */

/** typedef oyCMMobjectGetText_f
 *  @brief   build a text string from a given object
 *  @ingroup module_api
 *  @memberof oyCMMobjectType_s
 *
 *  Serialise a object into:
 *  - oyNAME_NICK: XML ID
 *  - oyNAME_NAME: XML
 *  - oyNAME_DESCRIPTION: ??
 *
 *  For type information the object argument is omitted. Then the function shall
 *  programatically tell in oyNAME_NICK about the object type,
 *  e.g. "oyProfile_s",
 *  in oyNAME_NAME translated about its intented usage,
 *  e.g. i18n("ICC profile") and give with oyNAME_DESCRIPTION some further long
 *  informations.
 *
 *  Note: Dynamically generated informations can be stored in the
 *  oyStruct_s::oy::name_  member and then returned by the function. Oyranos
 *  will typical look first at that oyObject_s member and then ask this
 *  function to get the information. @see oyObject_SetName()
 *
 *  @param[in]     object              the object, omit to get a general text
 *  @param[out]    type                the string type
 *  @param[in]     flags               for future use
 *  @return                            0 on success; error >= 1; unknown < 0
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/24 (Oyranos: 0.1.10)
 *  @date    2009/09/14
 */
typedef const char *   (*oyCMMobjectGetText_f) (
                                       oyStruct_s        * object,
                                       oyNAME_e            type,
                                       int                 flags );


/* } Include "CMMobjectType.public.h" */


/* Include "CMMobjectType.dox" { */
/** @struct  oyCMMobjectType_s
 *  @ingroup module_api
 *  @extends oyStruct_s
 *  @brief   Custom object handler
 *  @internal
 *  
 *  @version Oyranos: 0.1.10
 *  @since   2008/11/23 (Oyranos: 0.1.9)
 *  @date    2009/09/14
 */

/* } Include "CMMobjectType.dox" */

struct oyCMMobjectType_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};


OYAPI oyCMMobjectType_s* OYEXPORT
  oyCMMobjectType_New( oyObject_s object );
OYAPI oyCMMobjectType_s* OYEXPORT
  oyCMMobjectType_Copy( oyCMMobjectType_s *cmmobjecttype, oyObject_s obj );
OYAPI int OYEXPORT
  oyCMMobjectType_Release( oyCMMobjectType_s **cmmobjecttype );



/* Include "CMMobjectType.public_methods_declarations.h" { */
oyOBJECT_e         oyCMMobjectType_GetId
                                     ( oyCMMobjectType_s * obj );
const char *       oyCMMobjectType_GetPaths
                                     ( oyCMMobjectType_s * obj );
oyCMMobjectGetText_f
                   oyCMMobjectType_GetTextF
                                     ( oyCMMobjectType_s * obj );

/* } Include "CMMobjectType.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CMM_OBJECT_TYPE_S_H */
