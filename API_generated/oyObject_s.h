
/** @file oyObject_s.h

   [Template file inheritance graph]
   +-- Object_s.template.h

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


#ifndef OY_OBJECT_S_H
#define OY_OBJECT_S_H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


#define OY_HASH_SIZE 16

#include <oyranos_object.h>

#include "oyStruct_s.h"
#include "oyOptions_s.h"
#include "oyName_s.h"


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


/* oyObject_s common object functions { */
oyObject_s   oyObject_New             ( void );
oyObject_s   oyObject_NewWithAllocators(oyAlloc_f         allocateFunc,
                                        oyDeAlloc_f       deallocateFunc );
oyObject_s   oyObject_NewFrom         ( oyObject_s        object );
oyObject_s   oyObject_Copy            ( oyObject_s        object );
int          oyObject_Release         ( oyObject_s      * oy );
/* } oyObject_s common object functions */



/* Include "Object.public_methods_declarations.h" { */
oyObject_s   oyObject_SetParent       ( oyObject_s        object,
                                        oyOBJECT_e        type,
                                        oyPointer         ptr );
/*oyPointer    oyObjectAlign            ( oyObject_s        oy,
                                        size_t          * size,
                                        oyAlloc_f         allocateFunc );*/

int          oyObject_SetNames        ( oyObject_s        object,
                                        const char      * nick,
                                        const char      * name,
                                        const char      * description );
int          oyObject_SetName         ( oyObject_s        object,
                                        const char      * name,
                                        oyNAME_e          type );
const char  *oyObject_GetName         ( const oyObject_s  object,
                                        oyNAME_e          type );
/*oyCMMptr_s * oyObject_GetCMMPtr       ( oyObject_s        object,
                                        const char      * cmm );
oyObject_s   oyObject_SetCMMPtr       ( oyObject_s        object,
                                        oyCMMptr_s      * cmm_ptr );*/
int          oyObject_Lock             ( oyObject_s        object,
                                         const char      * marker,
                                         int               line );
int          oyObject_UnLock           ( oyObject_s        object,
                                         const char      * marker,
                                         int               line );
int          oyObject_UnSetLocking     ( oyObject_s        object,
                                         const char      * marker,
                                         int               line );
int          oyObject_GetId            ( oyObject_s        object );
int          oyObject_GetRefCount      ( oyObject_s        object );
int          oyObject_UnRef          ( oyObject_s          obj );

/* } Include "Object.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_OBJECT_S_H */
