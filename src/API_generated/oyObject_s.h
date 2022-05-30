
/** @file oyObject_s.h

   [Template file inheritance graph]
   +-- oyObject_s.template.h

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
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



/* oyObject_s common object functions { */
oyObject_s   oyObject_New            ( const char        * name );
oyObject_s   oyObject_NewWithAllocators(oyAlloc_f          allocateFunc,
                                       oyDeAlloc_f         deallocateFunc,
                                       const char        * name );
oyObject_s   oyObject_NewFrom        ( oyObject_s          object,
                                       const char        * name );
oyObject_s   oyObject_Copy           ( oyObject_s          object );
int          oyObject_Release        ( oyObject_s        * oy );
/* } oyObject_s common object functions */



/* Include "Object.public_methods_declarations.h" { */
OYAPI oyObject_s  OYEXPORT
                 oyObject_SetParent       ( oyObject_s        object,
                                        oyOBJECT_e        type,
                                        oyPointer         ptr );
/*oyPointer    oyObjectAlign            ( oyObject_s        oy,
                                        size_t          * size,
                                        oyAlloc_f         allocateFunc );*/

OYAPI int  OYEXPORT
                 oyObject_SetNames        ( oyObject_s        object,
                                        const char      * nick,
                                        const char      * name,
                                        const char      * description );
OYAPI int OYEXPORT oyObject_SetName  ( oyObject_s          object,
                                       const char        * text,
                                       oyNAME_e            type );
OYAPI int  OYEXPORT
                 oyObject_CopyNames       ( oyObject_s        dest,
                                        oyObject_s        src );
OYAPI const char * OYEXPORT oyObject_GetName (
                                       const oyObject_s    object,
                                       oyNAME_e            type );
/*oyPointer_s * oyObject_GetCMMPtr       ( oyObject_s        object,
                                        const char      * cmm );
oyObject_s   oyObject_SetCMMPtr       ( oyObject_s        object,
                                        oyPointer_s      * cmm_ptr );*/
OYAPI int  OYEXPORT
                 oyObject_Lock             ( oyObject_s        object,
                                         const char      * marker,
                                         int               line );
OYAPI int  OYEXPORT
                 oyObject_UnLock           ( oyObject_s        object,
                                         const char      * marker,
                                         int               line );
OYAPI int  OYEXPORT
                 oyObject_UnSetLocking     ( oyObject_s        object,
                                         const char      * marker,
                                         int               line );
OYAPI int  OYEXPORT
                 oyObject_GetId            ( oyObject_s        object );
OYAPI int  OYEXPORT
                 oyObject_GetRefCount      ( oyObject_s        object );
OYAPI int  OYEXPORT
                 oyObject_UnRef      ( oyObject_s          obj );
OYAPI oyAlloc_f  OYEXPORT
                 oyObject_GetAlloc   ( oyObject_s        object );
OYAPI oyDeAlloc_f  OYEXPORT
                 oyObject_GetDeAlloc ( oyObject_s        object );
OYAPI const char * OYEXPORT  oyObject_Show (
                                       oyObject_s          object );

/* debug API's */
int *              oyObjectGetCurrentObjectIdList( void );
void               oyObjectReleaseCurrentObjectIdList(
                                       int              ** id_list );
typedef struct oyLeave_s oyLeave_s;
typedef void (*oyObjectTreeCallback_f)(void              * user_data,
                                       int                 top_id,
                                       oyLeave_s         * tree,
                                       oyStruct_s        * grandparent,
                                       oyStruct_s        * parent,
                                       oyStruct_s        * current,
                                       oyStruct_s       ** children,
                                       int                 children_n,
                                       int                 level );
int                oyObjectIdListTraverseStructTrees (
                                       int               * id_list,
                                       oyObjectTreeCallback_f func,
                                       void              * user_data,
                                       int                 flags );
void               oyObjectTreePrint ( int                 flags,
                                       const char        * comment );
int                oyStruct_GetChildren (
                                       oyStruct_s        * obj,
                                       oyStruct_s      *** list );
int                oyStruct_GetParents(oyStruct_s        * obj,
                                       oyStruct_s      *** list );

/* } Include "Object.public_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_OBJECT_S_H */
