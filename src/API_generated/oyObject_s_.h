
/** @file oyObject_s_.h

   [Template file inheritance graph]
   +-- oyObject_s_.template.h

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


#ifndef OY_OBJECT_S__H
#define OY_OBJECT_S__H

#ifdef __cplusplus
/*namespace oyranos {*/
  extern "C" {
#endif /* __cplusplus */


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
 *
 *  @note New templates will not be created automaticly [notemplates]
 *
 *  @version Oyranos: 0.1.10
 *  @since   2007/10/00 (Oyranos: 0.1.8)
 *  @date    2009/03/01
 */

/* } Include "Object.dox" */

struct oyObject_s_ {
  oyOBJECT_e           type_;          /*!< @private struct type oyOBJECT_OBJECT_S*/
  oyObject_s         (*copy) (oyObject_s object); /**< copy function */
  int                (*release) (oyObject_s * object); /**< release function */
  int                  id_;            /**< @private identification for Oyranos */
  oyAlloc_f            allocateFunc_;  /**< @private data  allocator */
  oyDeAlloc_f          deallocateFunc_;/**< @private data release function */
  oyStruct_s         * parent_;        /*!< @private parent struct of parent_type */
  /** @private The first number in the array means the number of inherited 
   *  classes. It is the index to access top class. 
   *  parent_types_[parent_types_[0]] == actual_class */
  oyOBJECT_e         * parent_types_;  /**< @private the first number in the array means the number of inherited classes. */
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
int          oyGetNewObjectID          ( );
void         oyObjectIdRelease         ( );
/* object tracking */
int *              oyObjectGetCurrentObjectIdList( void );
int                oyObjectCountCurrentObjectIdList( void );
int *              oyObjectFindNewIds( int               * old_ids,
                                       int               * new_ids );
void               oyObjectReleaseCurrentObjectIdList(
                                       int              ** id_list );
const oyObject_s * oyObjectGetList   ( int               * max_count );
int                oyObjectIdListShowDiffAndRelease (
                                       int              ** ids_old,
                                       const char        * location );


/* } Include "Object.private_methods_declarations.h" */


#ifdef __cplusplus
} /* extern "C" */
/*}*/ /* namespace oyranos */
#endif /* __cplusplus */

#endif /* OY_OBJECT_S__H */
