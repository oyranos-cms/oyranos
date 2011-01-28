/** @file oyStruct_s.c

   [Template file inheritance graph]
   +-- Struct_s.template.c

 *  Oyranos is an open source Colour Management System
 *
 *  @par Copyright:
 *            2004-2011 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/bsd-license.php
 *  @since    2011/01/28
 */


  
#include "oyStruct_s.h"
#include "oyObject_s.h"
#include "oyranos_object_internal.h"
  




/* Include "Struct.public_methods_definitions.c" { */
const char * (*oyStruct_GetTextFromModule_p) (
                                       oyStruct_s        * obj,
                                       oyNAME_e            name_type,
                                       uint32_t            flags ) = NULL;

/** Function oyStruct_GetText
 *  @memberof oyStruct_s
 *  @brief   get a text dump
 *
 *  Ask the object type module for text informations.
 *
 *  @param         obj                 the object
 *  @param         name_type           the text type
 *  @param         flags
 *                                     - 0: get object infos
 *                                     - 1: get object type infos
 *  @return                            the text
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/09/14 (Oyranos: 0.1.10)
 *  @date    2009/09/15
 */
const char * oyStruct_GetText        ( oyStruct_s        * obj,
                                       oyNAME_e            name_type,
                                       uint32_t            flags )
{
  int error = !obj;
  const char * text = 0;
  oyOBJECT_e type = oyOBJECT_NONE;

  if(!error)
    text = oyObject_GetName( obj->oy_, oyNAME_NICK );

  if(!error && !text)
  {
    type = obj->type_;

    if(oyStruct_GetTextFromModule_p)
      text = oyStruct_GetTextFromModule_p(obj, name_type, flags);
#ifdef USE_MODULES /* FIXME move to oyStruct_GetTextFromModule_p */
    if(type)
    {
      oyCMMapiFilters_s * apis;
      int apis_n = 0, i,j;
      oyCMMapi9_s * cmm_api9 = 0;
      char * api_reg = 0;

      apis = oyCMMsGetFilterApis_( 0,0, api_reg, oyOBJECT_CMM_API9_S,
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
    text = oyStructTypeToText( obj->type_ );

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


/* Locking function definitions { */
oyPointer  oyStruct_LockCreateDummy_   ( oyStruct_s      * obj )    {return 0;}
void       oyLockReleaseDummy_         ( oyPointer         lock,
                                         const char      * marker,
                                         int               line )   {;}
void       oyLockDummy_                ( oyPointer         lock,
                                         const char      * marker,
                                         int               line )   {;}
void       oyUnLockDummy_              ( oyPointer         look,
                                         const char      * marker,
                                         int               line ) {;}

oyStruct_LockCreate_f   oyStruct_LockCreateFunc_ = oyStruct_LockCreateDummy_;
oyLockRelease_f         oyLockReleaseFunc_ = oyLockReleaseDummy_;
oyLock_f                oyLockFunc_        = oyLockDummy_;
oyUnLock_f              oyUnLockFunc_      = oyUnLockDummy_;

/** Function: oyThreadLockingSet
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
/* } Locking function definitions */

/* } Include "Struct.public_methods_definitions.c" */

