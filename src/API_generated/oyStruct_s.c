/** @file oyStruct_s.c

   [Template file inheritance graph]
   +-- oyStruct_s.template.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */


  
#include "oyStruct_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"
  




/* Include "Struct.public_methods_definitions.c" { */
#include "oyranos_generic.h"
#include "oyCMMapi9_s.h"
#include "oyCMMapiFilters_s.h"
#include "oyranos_types.h"/* uint64_t uintptr_t */
#include "oyranos_cmm.h" /* oyObjectInfoStatic_s */

const char * (*oyStruct_GetTextFromModule_p) (
                                       oyStruct_s        * obj,
                                       oyNAME_e            name_type,
                                       uint32_t            flags ) = NULL;

/** Function oyStruct_GetText
 *  @memberof oyStruct_s
 *  @brief   Get a text dump
 *
 *  Ask the object type module for text informations.
 *
 *  @param         obj                 the object
 *  @param         name_type           the text type
 *  @param         flags
 *                                     - 0: get object infos
 *                                     - 1: get object type infos
 *                                     - 2: skip object type infos
 *  @return                            the text
 *
 *  @version Oyranos: 0.9.6
 *  @date    2016/04/18
 *  @since   2009/09/14 (Oyranos: 0.1.10)
 */
const char * oyStruct_GetText        ( oyStruct_s        * obj,
                                       oyNAME_e            name_type,
                                       uint32_t            flags )
{
  int error = !obj;
  const char * text = 0;

  if(!error)
    text = oyObject_GetName( obj->oy_, name_type );

  if(!error && !text)
  {

    if(oyStruct_GetTextFromModule_p)
      text = oyStruct_GetTextFromModule_p(obj, name_type, flags);
#ifdef USE_MODULES /* FIXME move to oyStruct_GetTextFromModule_p */
    if(obj->type_)
    {
      oyCMMapiFilters_s * apis;
      int apis_n = 0, i,j;
      oyCMMapi9_s * cmm_api9 = 0;
      char * api_reg = 0;

      apis = oyCMMsGetFilterApis_( api_reg, oyOBJECT_CMM_API9_S,
                                   oyFILTER_REG_MODE_STRIP_IMPLEMENTATION_ATTR,
                                   0, 0);
      apis_n = oyCMMapiFilters_Count( apis );
      for(i = 0; i < apis_n; ++i)
      {
        cmm_api9 = (oyCMMapi9_s*) oyCMMapiFilters_Get( apis, i );

        j = 0;
        while( cmm_api9->object_types && cmm_api9->object_types[j] &&
               cmm_api9->object_types[j]->type == oyOBJECT_CMM_DATA_TYPES_S &&
               cmm_api9->object_types[j]->oyCMMobjectGetText &&
               cmm_api9->object_types[j]->id == obj->type_ )
        {
          text = cmm_api9->object_types[j]->oyCMMobjectGetText( flags ? 0 : obj,
                                                   name_type, 0 );
          if(text)
            break;
          ++j;
        }
        if(cmm_api9->release)
          cmm_api9->release( (oyStruct_s**)&cmm_api9 );

        if(text)
          break;
      }
      oyCMMapiFilters_Release( &apis );
    }
#endif
  }

  if(!error && !text)
  {
    text = oyStruct_GetInfo( obj, name_type, (flags&0x02) ? 0x01 : 0 );
    if(text && !text[0])
      text = 0;
  }

  if(!error && !text && !(flags & 0x02))
    text = oyStructTypeToText( obj->type_ );

  if(text && name_type >= oyNAME_JSON && oyjlDataFormat(text) != (int)name_type)
    return NULL;

  return text;
}

/** @internal
 *  Function oyStruct_TypeToText
 *  @brief   Objects type to small string
 *  @deprecated
 *
 *  @version Oyranos: 0.1.8
 *  @date    2008/06/24
 *  @since   2008/06/24 (Oyranos: 0.1.8)
 */
const char * oyStruct_TypeToText     ( const oyStruct_s  * st )
{ return oyStructTypeToText( st->type_ ); }

/** Function oyStruct_GetAllocator
 *  @brief   get the memory allocator of a object
 *
 *  @version Oyranos: 0.3.0
 *  @date    2011/01/30
 *  @since   2011/01/30 (Oyranos: 0.3.0)
 */
oyAlloc_f    oyStruct_GetAllocator   ( oyStruct_s        * obj )
{
  if(obj && obj->type_ != oyOBJECT_NONE && obj->oy_)
    return obj->oy_->allocateFunc_;
  else
    return 0;
}
/** Function oyStruct_GetDeAllocator
 *  @brief   get the memory allocator of a object
 *
 *  @version Oyranos: 0.3.0
 *  @date    2011/01/30
 *  @since   2011/01/30 (Oyranos: 0.3.0)
 */
oyDeAlloc_f  oyStruct_GetDeAllocator ( oyStruct_s        * obj )
{
  if(obj && obj->type_ != oyOBJECT_NONE && obj->oy_)
    return obj->oy_->deallocateFunc_;
  else
    return 0;
}

int          oyStruct_GetId          ( oyStruct_s        * st )
{
  int id = -1;

  if(st && st->oy_)
  {
    if(oyOBJECT_NONE < st->type_ && st->type_ < oyOBJECT_MAX_STRUCT)
      id = oyObject_GetId(st->oy_);
    else
      WARNc2_S( "type too non default - skip: %d (max: %d)", st->type_, oyOBJECT_MAX_STRUCT );
  }

  return id;
}

/** Function oyStruct_Allocate
 *  @brief   let a object allocate some memory
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/12/00 (Oyranos: 0.1.10)
 *  @date    2008/12/00
 */
OYAPI oyPointer  OYEXPORT
                 oyStruct_Allocate       ( oyStruct_s        * st,
                                           size_t              size )
{
  oyAlloc_f allocateFunc = oyAllocateFunc_;

  if(st && st->oy_ && st->oy_->allocateFunc_)
    allocateFunc = st->oy_->allocateFunc_;

  return allocateFunc( size );
}
/** Function oyStruct_DeAllocate
 *  @brief   let a object deallocate some memory
 *
 *  @version Oyranos: 0.9.7
 *  @since   2017/10/23 (Oyranos: 0.9.7)
 *  @date    2017/10/23
 */
OYAPI void OYEXPORT  oyStruct_DeAllocate ( oyStruct_s        * st,
                                           oyPointer           ptr )
{
  oyDeAlloc_f deallocateFunc = oyDeAllocateFunc_;

  if(st && st->oy_ && st->oy_->deallocateFunc_)
    deallocateFunc = st->oy_->deallocateFunc_;

  deallocateFunc( ptr );
}


/* Locking function definitions { */
oyPointer  oyStruct_LockCreateDummy_   ( oyStruct_s      * obj OY_UNUSED)    {return 0;}
void       oyLockReleaseDummy_         ( oyPointer         lock OY_UNUSED,
                                         const char      * marker OY_UNUSED,
                                         int               line OY_UNUSED )   {;}
void       oyLockDummy_                ( oyPointer         lock OY_UNUSED,
                                         const char      * marker OY_UNUSED,
                                         int               line OY_UNUSED )   {;}
void       oyUnLockDummy_              ( oyPointer         look OY_UNUSED,
                                         const char      * marker OY_UNUSED,
                                         int               line OY_UNUSED ) {;}

oyStruct_LockCreate_f   oyStruct_LockCreateFunc_ = oyStruct_LockCreateDummy_;
oyLockRelease_f         oyLockReleaseFunc_ = oyLockReleaseDummy_;
oyLock_f                oyLockFunc_        = oyLockDummy_;
oyUnLock_f              oyUnLockFunc_      = oyUnLockDummy_;

/** Function: oyThreadLockingSet
 *  @ingroup threads
 *  @brief set locking functions for threaded applications
 *
 *  @since Oyranos: version 0.1.8
 *  @date  14 january 2008 (API 0.1.8)
 */
void         oyThreadLockingSet        ( oyStruct_LockCreate_f  createLockFunc,
                                         oyLockRelease_f   releaseLockFunc,
                                         oyLock_f          lockFunc,
                                         oyUnLock_f        unlockFunc )
{
  oyStruct_LockCreateFunc_ = createLockFunc;
  oyLockReleaseFunc_ = releaseLockFunc;
  oyLockFunc_ = lockFunc;
  oyUnLockFunc_ = unlockFunc;

  if(!oyStruct_LockCreateFunc_ && !oyLockReleaseFunc_ &&
     !oyLockFunc_ && !oyUnLockFunc_)
  {
    oyStruct_LockCreateFunc_ = oyStruct_LockCreateDummy_;
    oyLockReleaseFunc_ = oyLockReleaseDummy_;
    oyLockFunc_ = oyLockDummy_;
    oyUnLockFunc_ = oyUnLockDummy_;
  }
}

/** Function: oyThreadLockingReset
 *  @ingroup threads
 *  @brief unset locking functions for threaded applications
 *
 *  Use this after threads ended to remove resources.
 *
 *  @version Oyranos: 0.9.7
 *  @since   2020/07/07 (Oyranos: 0.9.7)
 *  @date    2020/07/07
 */
void         oyThreadLockingReset      ( )
{
  oyStruct_LockCreateFunc_ = oyStruct_LockCreateDummy_;
  oyLockReleaseFunc_ = oyLockReleaseDummy_;
  oyLockFunc_ = oyLockDummy_;
  oyUnLockFunc_ = oyUnLockDummy_;
  if(oy_debug) fprintf( stderr, "oyThreadLockingReset(): done\n" );
}

/**
 *  @brief tell if thread locking is ready
 *  @ingroup threads
 *
 *  @version Oyranos: 0.9.5
 *  @date    2014/02/04
 *  @since   2014/02/04 (Oyranos: 0.9.5)
 */
int          oyThreadLockingReady    ( void )
{
  return oyStruct_LockCreateFunc_ && oyStruct_LockCreateFunc_ != oyStruct_LockCreateDummy_;
}

/** Function oyStruct_CheckType
 *  @brief   check if the object is a class or is inherited of a class
 *
 *  @version Oyranos: 0.3.0
 *  @date    2011/02/15
 *  @since   2011/02/02 (Oyranos: 0.3.0)
 */
int          oyStruct_CheckType      ( oyStruct_s        * obj,
                                       oyOBJECT_e          type )
{
  int error = 1;
  if(obj && obj->type_ != oyOBJECT_NONE)
  {
    /* check dynamic objects */
    if(obj->type_ != oyOBJECT_OBJECT_S && obj->oy_)
    {
      int n, i;
      oyOBJECT_e * inheritance = 0;

      if(obj->oy_->type_ == oyOBJECT_OBJECT_S)
        inheritance = obj->oy_->parent_types_;
      else if(obj->oy_->type_ == oyOBJECT_INFO_STATIC_S)
      {
        oyObjectInfoStatic_s * static_object = (oyObjectInfoStatic_s*)obj->oy_;
        inheritance = static_object->inheritance;
      } else
        return 2;

      if((uintptr_t)inheritance < (uintptr_t)oyOBJECT_MAX)
      {
        WARNc1_S( "non plausible inheritance pointer: %s", 
                  oyStruct_GetInfo(obj, oyNAME_NAME,0) );
        return 2;
      }

      n = inheritance[0];
      for(i = 1; i <= n; ++i)
        if(inheritance[i] == type)
        {
          if(inheritance[0] == (size_t)i)
            error = 0;
          else
            error = -1;
          break;
        }
    } else
    /* check static objects */
      if(obj->type_ == type)
        error = 0;
  } else
    error = 2;

  return error;
}
/* } Locking function definitions */

/* } Include "Struct.public_methods_definitions.c" */

