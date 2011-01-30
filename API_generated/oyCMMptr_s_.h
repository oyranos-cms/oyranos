/** @file oyCMMptr_s_.h

   [Template file inheritance graph]
   +-> CMMptr_s_.template.h
   |
   +-- Base_s_.h

 *  Oyranos is an open source Colour Management System
 *
 *  @par Copyright:
 *            2004-2011 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @since    2011/01/30
 */


#ifndef OY_CMM_PTR_S__H
#define OY_CMM_PTR_S__H

#define oyCMMptrPriv_m( var ) ((oyCMMptr_s_*) (var))

  
#include <oyranos_object.h>
  

#include "oyCMMptr_s.h"

typedef struct oyCMMptr_s_ oyCMMptr_s_;
/* Include "CMMptr.private.h" { */

/* } Include "CMMptr.private.h" */

/** @internal
 *  @struct   oyCMMptr_s_
 *  @brief      CMM pointer
 *  @ingroup  module_api
 *  @extends  oyStruct_s
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
struct oyCMMptr_s_ {

/* Include "Struct.members.h" { */
oyOBJECT_e           type_;          /**< @private struct type */
oyStruct_Copy_f      copy;           /**< copy function */
oyStruct_Release_f   release;        /**< release function */
oyObject_s           oy_;            /**< @private features name and hash */

/* } Include "Struct.members.h" */



/* Include "CMMptr.members.h" { */
  char               * lib_name;       /*!< the CMM */
  char                 func_name[32];  /*!< optional the CMM's function name */
  oyPointer            ptr;            /*!< a CMM's data pointer */
  int                  size;           /**< -1 && 0 - unknown, > 1 size of ptr*/
  char                 resource[5];    /**< the resource type, e.g. oyCMM_PROFILE, oyCMM_COLOUR_CONVERSION */
  oyPointer_release_f  ptrRelease;     /*!< CMM's deallocation function */
  int                  ref;            /**< Oyranos reference counter */

/* } Include "CMMptr.members.h" */

};


oyCMMptr_s_*
  oyCMMptr_New_( oyObject_s object );
oyCMMptr_s_*
  oyCMMptr_Copy_( oyCMMptr_s_ *cmmptr, oyObject_s object);
oyCMMptr_s_*
  oyCMMptr_Copy__( oyCMMptr_s_ *cmmptr, oyObject_s object);
int
  oyCMMptr_Release_( oyCMMptr_s_ **cmmptr );



/* Include "CMMptr.private_methods_declarations.h" { */
int                oyCMMptr_Set_     ( oyCMMptr_s_       * cmm_ptr,
                                       const char        * lib_name,
                                       const char        * resource,
                                       oyPointer           ptr,
                                       const char        * func_name,
                                       oyPointer_release_f ptrRelease );

/* } Include "CMMptr.private_methods_declarations.h" */



void oyCMMptr_Release__Members( oyCMMptr_s_ * cmmptr );
int oyCMMptr_Init__Members( oyCMMptr_s_ * cmmptr );
int oyCMMptr_Copy__Members( oyCMMptr_s_ * dst, oyCMMptr_s_ * src);


#endif /* OY_CMM_PTR_S__H */
