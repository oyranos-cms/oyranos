/** @file oyObject_s_.c

   [Template file inheritance graph]
   +-- oyObject_s_.template.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


#include <math.h>

#include <oyranos_cache.h>
#include <oyranos_object_internal.h>

#include "oyObject_s.h"
#include "oyObject_s_.h"


/* Include "Object.private_methods_definitions.c" { */
/** @internal
 *  @memberof oyObject_s
 *  @brief   custom object memory managers
 *
 *  @param[in]    object         the object to modify
 *  @param[in]    allocateFunc   zero for default or user memory allocator
 *  @param[in]    deallocateFunc zero for default or user memory deallocator
 *
 *  @since Oyranos: version 0.1.8
 *  @date  november 2007 (API 0.1.8)
 */
oyObject_s   oyObject_SetAllocators_  ( oyObject_s        object,
                                        oyAlloc_f         allocateFunc,
                                        oyDeAlloc_f       deallocateFunc )
{
  if(!object) return 0;

  /* we require a allocation function to be present */
  if(allocateFunc)
    object->allocateFunc_ = allocateFunc;
  else
    object->allocateFunc_ = oyAllocateFunc_;

  object->deallocateFunc_ = deallocateFunc;

  return object;
}

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif

/**
 *  @internal
 *  Function oyObject_Ref
 *  @memberof oyObject_s
 *  @brief   increase the ref counter and return the above zero ref value
 *
 *  @version Oyranos: 0.1.8
 *  @date    2011/02/02
 *  @since   2008/02/07 (Oyranos: 0.1.8)
 */
int          oyObject_Ref            ( oyObject_s          obj )
{
  oyObject_s s = obj;
  int error = !s;

  if(!s) return 1;

  if( s->type_ != oyOBJECT_OBJECT_S)
  {
    WARNc2_S("Attempt to manipulate a non oyObject_s object: %s[%d]",
             oyStructTypeToText( s->type_ ), s->id_)
    return 1;
  }

  if(error <= 0)
    oyObject_Lock( s, __FILE__, __LINE__ );

  if(error <= 0)
  {
    ++s->ref_;

    if(oy_debug_objects == 1 || oy_debug_objects == s->id_)
      OY_BACKTRACE_PRINT
    if(oy_debug_objects >= 0)
      /* track object */
      oyObject_GetId( obj );
  }
#   if DEBUG_OBJECT
    WARNc3_S("%s[%d] refs: %d",
             oyStructTypeToText( s->parent_types_[s->parent_types_[0]] ), s->id_, s->ref_)
#   endif

  if(obj->parent_types_[obj->parent_types_[0]] == oyOBJECT_NAMED_COLORS_S)
  {
    int e_a = error;
    error = pow(e_a,2.1);
    error = e_a;
  }

  if(error <= 0)
    oyObject_UnLock( s, __FILE__, __LINE__ );

  return s->ref_;
}

/** @internal
 *  Function oyObject_Hashed_
 *  @memberof oyObject_s
 *  @brief   check if a object has a hash sum computed
 *
 *  @version Oyranos: 0.3.0
 *  @date    2011/04/09
 *  @since   2008/11/02 (Oyranos: 0.1.8)
 */
int32_t      oyObject_Hashed_        ( oyObject_s          s )
{
  int32_t hashed = 0;
  if(s && s->type_ == oyOBJECT_OBJECT_S && s->hash_ptr_)
      if(*((uint32_t*)(&s->hash_ptr_[0])) ||
         *((uint32_t*)(&s->hash_ptr_[4])) ||
         *((uint32_t*)(&s->hash_ptr_[8])) ||
         *((uint32_t*)(&s->hash_ptr_[12])) )
        hashed = 1;
  return hashed;
}

/** @internal
 *  Function oyObject_HashSet
 *  @memberof oyObject_s
 *  @brief   check if a object has a hash allocated and set it
 *
 *  @param[in,out] s                   the object
 *  @param[in]     hash                the hash of max size 2 * OY_HASH_SIZE
 *  @return                            0 - fine, otherwise error
 *
 *  @version Oyranos: 0.1.10
 *  @date    2009/04/16
 *  @since   2009/04/16 (Oyranos: 0.1.10)
 */
int          oyObject_HashSet          ( oyObject_s        s,
                                         const unsigned char * hash )
{
  int error = !(s && s->type_ == oyOBJECT_OBJECT_S);

  if(!error && !s->hash_ptr_)
  {
    s->hash_ptr_ = s->allocateFunc_(OY_HASH_SIZE*2);
    error = !s->hash_ptr_;
  }

  if(!error)
  {
    if(hash)
      memcpy( s->hash_ptr_, hash, OY_HASH_SIZE*2 );
    else
      memset( s->hash_ptr_, 0, OY_HASH_SIZE*2 );
  }

  return error;
}

/** @internal
 *  Function oyObject_HashEqual
 *  @memberof oyObject_s
 *  @brief   check if two objects hash is equal
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/04/16 (Oyranos: 0.1.10)
 *  @date    2009/05/26
 */
int          oyObject_HashEqual        ( oyObject_s        s1,
                                         oyObject_s        s2 )
{
  int equal = 0;

  if(s1->hash_ptr_ &&
     s2->hash_ptr_)
  {
    if(memcmp(s1->hash_ptr_, s2->hash_ptr_, OY_HASH_SIZE*2) == 0)
      return 1;
  }

  return equal;
}

static oyPointer oy_object_id_mutex_ = NULL;

/** @internal
 *  @memberof oyObject_s
 *  @brief    get a object identification number
 *
 *  @version  Oyranos: 0.9.5
 *  @date     2014/02/04
 *  @since    2014/02/04 (Oyranos: 0.9.5)
 */
int oyGetNewObjectID()
{
  /* For the ::OY_DEBUG_OBJECTS variable starting with 2 is much easier. */
  static int oy_object_id_ = 2;
  int val = -1;
  if(!oy_object_id_mutex_)
    oy_object_id_mutex_ = oyStruct_LockCreateFunc_(NULL);

  oyLockFunc_(oy_object_id_mutex_,__FILE__,__LINE__);
  val = oy_object_id_++;
  oyUnLockFunc_(oy_object_id_mutex_,__FILE__,__LINE__);
  return val;
}
void oyObjectIdRelease() { if(oy_object_id_mutex_) oyLockReleaseFunc_( oy_object_id_mutex_, __FILE__,__LINE__ ); oy_object_id_mutex_ = NULL; }

#include <stddef.h>           /* size_t ptrdiff_t */
#define MAX_OBJECTS_TRACKED 1000000
/* private tracking API's start */
static oyObject_s * oy_obj_track_list = NULL;
void               oyObject_Track    ( oyObject_s          obj,
                                       const char        * name )
{
  const char * t = getenv(OY_DEBUG_OBJECTS);
  if(!oy_obj_track_list)
  {
    oy_obj_track_list = oyAllocateFunc_( sizeof(oyObject_s) * (MAX_OBJECTS_TRACKED + 1) );
    if(oy_obj_track_list)
      memset( oy_obj_track_list, 0, sizeof(oyObject_s) * (MAX_OBJECTS_TRACKED + 1) );
  }
  if(oy_obj_track_list && obj->id_ < MAX_OBJECTS_TRACKED)
    oy_obj_track_list[obj->id_] = obj;

  if( (t && (strstr(name, t) != NULL)) ||
      (oy_debug_objects == 1 || oy_debug_objects == obj->id_) )
  {
    OY_BACKTRACE_PRINT
    fprintf( stderr, "%s[%d] tracked (" OY_PRINT_POINTER ")\n", (oy_debug_objects == obj->id_)?oyjlTermColor(oyjlGREEN, name):name, obj->id_, (ptrdiff_t)obj );
  }
  if(oy_debug_objects <= -2)
  {
    //OY_BACKTRACE_PRINT
    //fprintf( stderr, "Object[%d] tracked %s:%d %d\n", obj->id_, __FILE__,__LINE__, oy_debug_objects);
  }
}
void               oyObject_UnTrack    ( oyObject_s          obj )
{
  if(obj->id_ <= 0) return; /* objects without ID are invisible by purpose */
  if(oy_obj_track_list && obj->id_ < MAX_OBJECTS_TRACKED)
    oy_obj_track_list[obj->id_] = NULL;
  if(oyObjectUsedByCache_( obj->id_ ))
    fprintf( stderr, "!!!ERROR: Object[%d] still in cache\n", obj->id_);
  if(obj->ref_ < -1 && (oy_debug_objects >= 0 || oy_debug))
  {
    const char * type = obj->parent_?oyStructTypeToText(obj->parent_->type_):"";
    OY_BACKTRACE_PRINT
    fprintf( stderr, OY_DBG_FORMAT_ "!!!ERROR: Object[%d] has unexpected reference counter: %d  %s\n", OY_DBG_ARGS_, obj->id_, obj->ref_, type );
  }
  if(oy_debug_objects <= -2)
  {
    //OY_BACKTRACE_PRINT
    //fprintf( stderr, "Object[%d] untracked %s:%d\n", obj->id_, __FILE__,__LINE__);
  }
}
/* private tracking API's end */

int *              oyObjectGetCurrentObjectIdList( void )
{
  int * id_list = oyAllocateFunc_( sizeof(int) * MAX_OBJECTS_TRACKED );
  int i;

  memset(id_list, 0, sizeof(int) * MAX_OBJECTS_TRACKED );

  if(id_list)
    for(i = 0; i < MAX_OBJECTS_TRACKED; ++i)
    {
      if(oy_obj_track_list && oy_obj_track_list[i] && oy_obj_track_list[i]->parent_)
        id_list[i] = oy_obj_track_list[i]->parent_->type_;
      else
        id_list[i] = -1;
    }
  return id_list;
}

const oyObject_s * oyObjectGetList   ( int               * max_count )
{
  if(max_count) *max_count = MAX_OBJECTS_TRACKED;
  else if(oy_obj_track_list)
    oyFree_m_(oy_obj_track_list);
  return oy_obj_track_list;
}

int *              oyObjectFindNewIds( int               * old,
                                       int               * new )
{
  int * id_list = oyAllocateFunc_( sizeof(int) * MAX_OBJECTS_TRACKED );
  int i;

  if(id_list)
    for(i = 0; i < MAX_OBJECTS_TRACKED; ++i)
    {
      if(old[i] == -1 && new[i] != -1)
        id_list[i] = new[i];
      else
        id_list[i] = -1;
    }
  return id_list;
}
void               oyObjectReleaseCurrentObjectIdList(
                                       int              ** id_list )
{ oyDeAllocateFunc_(*id_list); *id_list = NULL; }
int                oyObjectIdListShowDiffAndRelease (
                                       int              ** ids_old,
                                       const char        * location )
{
  int * ids_new = oyObjectGetCurrentObjectIdList(),
      * ids_remaining_new = oyObjectFindNewIds( *ids_old, ids_new ),
      max_count,i, count = 0;
  const oyObject_s * obs = oyObjectGetList( &max_count );

  for(i = 0; i < max_count; ++i)
    if(ids_remaining_new[i] != -1)
      ++count;
  if(count)
  {
    fprintf( stderr, "new allocated objects inside %s: %d\n", location, count );
    for(i = 0; i < max_count; ++i)
      if(ids_remaining_new[i] != -1)
        fputs( oyObject_Show( obs[i] ), stderr );
    fprintf( stderr, "... end new allocated objects inside %s\n", location );
    fflush( stderr );
  }

  oyObjectReleaseCurrentObjectIdList( ids_old );
  oyObjectReleaseCurrentObjectIdList( &ids_new );
  oyObjectReleaseCurrentObjectIdList( &ids_remaining_new );

  return count;
}

int                oyObjectCountCurrentObjectIdList( void )
{
  int * ids_new = oyObjectGetCurrentObjectIdList(),
      max_count = MAX_OBJECTS_TRACKED,i, count = 0;

  for(i = 0; i < max_count; ++i)
    if(ids_new[i] != -1)
      ++count;

  oyObjectReleaseCurrentObjectIdList( &ids_new );

  return count;
}


/* } Include "Object.private_methods_definitions.c" */

