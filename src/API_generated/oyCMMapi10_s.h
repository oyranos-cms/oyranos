/** @file oyCMMapi10_s.h

   [Template file inheritance graph]
   +-> oyCMMapi10_s.template.h
   |
   +-> oyCMMapiFilter_s.template.h
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



#ifndef OY_CMM_API10_S_H
#define OY_CMM_API10_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyCMMapi10_s oyCMMapi10_s;



#include "oyStruct_s.h"

#include "oyPointer_s.h"


/* Include "CMMapi10.public.h" { */

/* } Include "CMMapi10.public.h" */


/* Include "CMMapi10.dox" { */
/** @struct  oyCMMapi10_s
 *  @ingroup module_api
 *  @extends oyCMMapiFilter_s
 *  @brief   Generic data exchange
 *  @internal
 *  
 *  This API provides a very generic interface to exchange data between Oyranos
 *  core and a module.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/11 (Oyranos: 0.1.10)
 *  @date    2009/12/11
 */

/* } Include "CMMapi10.dox" */

struct oyCMMapi10_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};



OYAPI oyCMMapi10_s* OYEXPORT
  oyCMMapi10_New( oyObject_s object );
OYAPI oyCMMapi10_s* OYEXPORT
  oyCMMapi10_Copy( oyCMMapi10_s *cmmapi10, oyObject_s obj );
OYAPI int OYEXPORT
  oyCMMapi10_Release( oyCMMapi10_s **cmmapi10 );

OYAPI int  OYEXPORT
           oyCMMapi10_SetBackendContext          ( oyCMMapi10_s       * api,
                                       oyPointer_s       * ptr );
OYAPI oyPointer_s * OYEXPORT
           oyCMMapi10_GetBackendContext          ( oyCMMapi10_s       * api );



/* Include "CMMapi10.public_methods_declarations.h" { */
#include "oyOptions_s.h"
/** typedef  oyMOptions_Handle_f
 *  @brief   generic data exchange
 *  @ingroup module_api
 *  @memberof oyCMMapi10_s
 *
 *  The provided data passed with oyOptions_s shall be processed according to
 *  the rules of a given command. The result is again a generic oyOptions_s
 *  object.
 *
 *  This module type works similiar to the device modules, which are defined in
 *  oyCMMapi8_s. While the module interface is simple it has minimal meaning to
 *  the intented work or data exchange. The rules are to be defined by the
 *  communicating parties, the host and the module. They use the plug-in
 *  mechanism only for their convinience.
 *
 *  For speed up the "can_handle.xxx" command must be implemented. "xxx" means
 *  in the previous command string the intented command to be performed. The
 *  "can_handle" command shall not produce any output in result, but shall
 *  indicate with zero that it could handle the request. This command is
 *  intented to lightwight select a capable module.
 *
 *  @param[in]     options             the input data; It is typical not
 *                                     changed.
 *  @param[in]     command             a string describing the intention of the
 *                                     caller; Allowed are alphanumerical signs.
 *                                     lower bar '_', minus '-', plus '+' and
 *                                     point '.' . other signs including white
 *                                     space must not occure.
 *                                     Strings are separated by point '.' .
 *                                     The first string is the actual command.
 *                                     The following strings are merely
 *                                     modifiers.
 *  @param[out]    result              the created data
 *  @return                            0 - indifferent, >= 1 - error,
 *                                     <= -1 - issue,
 *                                     + a message should be sent
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/12/11 (Oyranos: 0.1.10)
 *  @date    2009/12/11
 */
typedef int  (*oyMOptions_Handle_f)  ( oyOptions_s       * options,
                                       const char        * command,
                                       oyOptions_s      ** result );
#include "oyCMMapi_s.h"
OYAPI oyCMMapi10_s*  OYEXPORT
                  oyCMMapi10_Create  ( oyCMMInit_f         init,
                                       oyCMMReset_f        reset,
                                       oyCMMMessageFuncSet_f msg_set,
                                       const char        * registration,
                                       int32_t             version[3],
                                       int32_t             module_api[3],
                                       oyCMMGetText_f      getText,
                                       const char       ** texts,
                                       oyMOptions_Handle_f oyMOptions_Handle,
                                       oyObject_s          object );

/* } Include "CMMapi10.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CMM_API10_S_H */
