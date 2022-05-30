/** @file oyPointer_s.h

   [Template file inheritance graph]
   +-> oyPointer_s.template.h
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



#ifndef OY_POINTER_S_H
#define OY_POINTER_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>

typedef struct oyPointer_s oyPointer_s;


#include "oyStruct_s.h"


/* Include "Pointer.public.h" { */

/* } Include "Pointer.public.h" */


/* Include "Pointer.dox" { */
/** @struct  oyPointer_s
 *  @ingroup objects_generic
 *  @extends oyStruct_s
 *  @brief   custom pointer
 *  @internal
 *  
 *  The oyPointer_s is used internally and by modules.
 *  Memory management can be done by Oyranos' oyAllocateFunc_ and 
 *  oyDeallocateFunc_.
 *  A module can specify a own release function for the contained pointer.
 *  This is well handled for referencing oyPointer_s objects. As there is no
 *  requirement for a module to place a copy function for the pointer, it is
 *  not recommended to real copy oyPointer_s objects. Ask the module to create
 *  a new oyPointer_s object with the according options instead.
 *
 *  @version Oyranos: 0.3.0
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 *  @date    2011/02/15
 */

/* } Include "Pointer.dox" */

struct oyPointer_s {
/* Include "Struct.members.h" { */
const  oyOBJECT_e    type_;          /**< The struct type tells Oyranos how to interprete hidden fields. @brief Type of object */
oyStruct_Copy_f      copy;           /**< @brief Copy function */
oyStruct_Release_f   release;        /**< @brief Release function */
oyObject_s           oy_;            /**< Features name and hash. Do not change during object life time. @brief Oyranos internal object */

/* } Include "Struct.members.h" */
};


OYAPI oyPointer_s* OYEXPORT
  oyPointer_New( oyObject_s object );
OYAPI oyPointer_s* OYEXPORT
  oyPointer_Copy( oyPointer_s *pointer, oyObject_s obj );
OYAPI int OYEXPORT
  oyPointer_Release( oyPointer_s **pointer );



/* Include "Pointer.public_methods_declarations.h" { */
OYAPI int  OYEXPORT
                 oyPointer_Set           ( oyPointer_s       * cmm_ptr,
                                           const char        * lib_name,
                                           const char        * resource,
                                           oyPointer           ptr,
                                           const char        * func_name,
                                           oyPointer_release_f ptrRelease );
OYAPI const char *  OYEXPORT
                oyPointer_GetFuncName    ( oyPointer_s       * cmm_ptr );
OYAPI const char *  OYEXPORT
                oyPointer_GetLibName     ( oyPointer_s       * cmm_ptr );
OYAPI const char *  OYEXPORT
                oyPointer_GetResourceName(oyPointer_s       * cmm_ptr );
OYAPI int  OYEXPORT
                 oyPointer_GetSize       ( oyPointer_s       * cmm_ptr );
OYAPI int  OYEXPORT
                 oyPointer_SetSize       ( oyPointer_s       * cmm_ptr,
                                           int                 size );
OYAPI oyPointer  OYEXPORT
                 oyPointer_GetPointer    ( oyPointer_s       * cmm_ptr );
OYAPI oyPointer_s *  OYEXPORT
                oyPointer_LookUpFromObject (
                                          oyStruct_s        * data,
                                          const char        * data_type );
OYAPI oyPointer_s *  OYEXPORT
                oyPointer_LookUpFromText( const char        * text,
                                          const char        * data_type );
OYAPI const char *  OYEXPORT
                oyPointer_GetId         ( oyPointer_s       * cmm_ptr );
OYAPI void OYEXPORT
                oyPointer_SetId         ( oyPointer_s       * cmm_ptr,
                                          const char        * id_string );

/* } Include "Pointer.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_POINTER_S_H */
