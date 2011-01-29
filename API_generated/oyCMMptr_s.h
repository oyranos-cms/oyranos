/** @file oyCMMptr_s.h

   [Template file inheritance graph]
   +-> CMMptr_s.template.h
   |
   +-> Base_s.h
   |
   +-- Struct_s.template.h

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



#ifndef OY_CMM_PTR_S_H
#define OY_CMM_PTR_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


  
#include <oyranos_object.h>


#include "oyStruct_s.h"


typedef struct oyCMMptr_s oyCMMptr_s;
/* Include "CMMptr.public.h" { */

/* } Include "CMMptr.public.h" */


/* Include "CMMptr.dox" { */
/** @struct  oyCMMptr_s
 *  @ingroup module_api
 *  @extends oyStruct_s
 *  @brief   CMM pointer
 *  @internal
 *  
 *  The oyCMMptr_s is used internally and by modules.
 *  Memory management can be done by Oyranos' oyAllocateFunc_ and 
 *  oyDeallocateFunc_.
 *  A module can specify a own release function for the contained pointer.
 *  This is well handled for referencing oyCMMptr_s objects. As there is no
 *  requirement for a module to place a copy function for the pointer, it is
 *  not recommended to real copy oyCMMptr_s objects. Ask the module to create
 *  a new oyCMMptr_s object with the according options instead.
 *
 *  @version Oyranos: 0.1.8
 *  @since   2007/11/00 (Oyranos: 0.1.8)
 *  @date    2008/07/02
 */

/* } Include "CMMptr.dox" */

struct oyCMMptr_s {
/* Include "Struct.members.h" { */
oyOBJECT_e           type_;          /**< @private struct type */
oyStruct_Copy_f      copy;           /**< copy function */
oyStruct_Release_f   release;        /**< release function */
oyObject_s           oy_;            /**< @private features name and hash */

/* } Include "Struct.members.h" */
};


OYAPI oyCMMptr_s* OYEXPORT
  oyCMMptr_New( oyObject_s object );
OYAPI oyCMMptr_s* OYEXPORT
  oyCMMptr_Copy( oyCMMptr_s *cmmptr, oyObject_s obj );
OYAPI int OYEXPORT
  oyCMMptr_Release( oyCMMptr_s **cmmptr );



/* Include "CMMptr.public_methods_declarations.h" { */
oyCMMptr_s * oyCMMptrLookUpFromText  ( const char        * text,
                                       const char        * data_type );
oyCMMptr_s * oyCMMptrLookUpFromObject( oyStruct_s        * data,
                                       const char        * cmm );
int          oyCMMptr_Set            ( oyCMMptr_s        * cmm_ptr,
                                       const char        * lib_name,
                                       const char        * resource,
                                       oyPointer           ptr,
                                       const char        * func_name,
                                       oyPointer_release_f ptrRelease );
const char * oyCMMptr_GetFuncName    ( oyCMMptr_s        * cmm_ptr );
const char * oyCMMptr_GetLibName     ( oyCMMptr_s        * cmm_ptr );
const char * oyCMMptr_GetResourceName( oyCMMptr_s        * cmm_ptr );
int          oyCMMptr_GetSize        ( oyCMMptr_s        * cmm_ptr );
oyPointer    oyCMMptr_GetPointer     ( oyCMMptr_s        * cmm_ptr );

/* } Include "CMMptr.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_CMM_PTR_S_H */
