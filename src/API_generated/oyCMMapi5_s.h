/** @file oyCMMapi5_s.h

   [Template file inheritance graph]
   +-> oyCMMapi5_s.template.h
   |
   +-> oyCMMapi_s.template.h
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



#ifndef OY_CMM_API5_S_H
#define OY_CMM_API5_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyCMMapi5_s oyCMMapi5_s;



#include "oyStruct_s.h"

#include "oyFilterPlug_s.h"
#include "oyFilterSocket_s.h"


/* Include "CMMapi5.public.h" { */
#include "oyFilterSocket_s.h"

/* } Include "CMMapi5.public.h" */


/* Include "CMMapi5.dox" { */
/** @struct  oyCMMapi5_s
 *  @ingroup module_api
 *  @extends oyCMMapi_s
 *  @brief   Module or script loader
 *  @internal
 *  
 *  Filters can be provided in non library form, e.g. as text files. This API 
 *  allowes for registring of paths and file types to be recognised as filters.
 *  The API must provide the means to search, list, verify and open these 
 *  script filters through Oyranos. The filters are opened in Oyranos and passed
 *  as blobs to the API function for obtaining light wight informations, e.g.
 *  list the scanned filters in a user selection widget. Further the API is
 *  responsible to open the filter and create a oyFilter_s object.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/11/22 (Oyranos: 0.1.9)
 *  @date    2010/06/25
 */

/* } Include "CMMapi5.dox" */

struct oyCMMapi5_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};


OYAPI oyCMMapi5_s* OYEXPORT
  oyCMMapi5_New( oyObject_s object );
OYAPI oyCMMapi5_s* OYEXPORT
  oyCMMapi5_Copy( oyCMMapi5_s *cmmapi5, oyObject_s obj );
OYAPI int OYEXPORT
  oyCMMapi5_Release( oyCMMapi5_s **cmmapi5 );



/* Include "CMMapi5.public_methods_declarations.h" { */

/* } Include "CMMapi5.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CMM_API5_S_H */
