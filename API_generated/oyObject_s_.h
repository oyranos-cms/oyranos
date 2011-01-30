
/** @file oyObject_s_.h

   [Template file inheritance graph]
   +-- Object_s_.template.h

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


#ifndef OY_OBJECT_S__H
#define OY_OBJECT_S__H

#include <oyranos_object.h>
#include <oyName_s_.h>

#include "oyObject_s.h"


/* Include "Object.dox" { */
/** @struct  oyObject_s
 *  @brief   Oyranos structure base
 *  @ingroup objects_generic
 *
 *  The base object of Oyranos object system is self contained. It can be
 *  handled by the belonging function set. Complex objects for user interaction
 *  should almost all be derived from this struct.\n
 *  The memory management can be controlled by the user and will affect internal
 *  and derived data.
 *  Do not automaticly create templates [notemplates]
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/10/00 (Oyranos: 0.1.8)
 *  @date    2009/03/01
 */

/* } Include "Object.dox" */

struct oyObject_s_ {
  oyOBJECT_e           type_;          /*!< @private struct type oyOBJECT_OBJECT_S*/
  oyStruct_Copy_f      copy;           /**< copy function */
  oyStruct_Release_f   release;        /**< release function */
  int                  id_;            /**< @private identification for Oyranos */
  oyAlloc_f            allocateFunc_;  /**< @private data  allocator */
  oyDeAlloc_f          deallocateFunc_;/**< @private data release function */
  oyPointer            parent_;        /*!< @private parent struct of parent_type */
  oyOBJECT_e           parent_type_;   /*!< @private parents struct type */
  oyPointer            backdoor_;      /*!< @private allow non breaking extensions */
  oyOptions_s        * handles_;       /**< @private addational data and infos*/
  oyName_s           * name_;          /*!< @private naming feature */
  int                  ref_;           /*!< @private reference counter */
  int                  version_;       /*!< @private OYRANOS_VERSION */
  unsigned char      * hash_ptr_;      /**< @private 2*OY_HASH_SIZE */
  oyPointer            lock_;          /**< @private the user provided lock */
};


/* Include "Object.private_methods_declarations.h" { */
oyObject_s   oyObject_SetAllocators_  ( oyObject_s        object,
                                        oyAlloc_f         allocateFunc,
                                        oyDeAlloc_f       deallocateFunc );
int          oyObject_Ref            ( oyObject_s          obj );
int32_t      oyObject_Hashed_        ( oyObject_s          s );
int          oyObject_HashSet          ( oyObject_s        s,
                                         const unsigned char * hash );
int          oyObject_HashEqual        ( oyObject_s        s1,
                                         oyObject_s        s2 );

/* } Include "Object.private_methods_declarations.h" */


#endif /* OY_OBJECT_S__H */
