/** @file oyCMMinfo_s_.c

   [Template file inheritance graph]
   +-> oyCMMinfo_s_.template.c
   |
   +-- Base_s_.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2022 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */



  
#include "oyCMMinfo_s.h"
#include "oyCMMinfo_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"
#include "oyranos_module_internal.h"

  

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif


static int oy_cmminfo_init_ = 0;
static char * oy_cmminfo_msg_text_ = NULL;
static int oy_cmminfo_msg_text_n_ = 0;
static const char * oyCMMinfo_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyCMMinfo_s_ * s = (oyCMMinfo_s_*) obj;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if( oy_cmminfo_msg_text_ == NULL || oy_cmminfo_msg_text_n_ == 0 )
  {
    oy_cmminfo_msg_text_n_ = 512;
    oy_cmminfo_msg_text_ = (char*) alloc( oy_cmminfo_msg_text_n_ );
    if(oy_cmminfo_msg_text_)
      memset( oy_cmminfo_msg_text_, 0, oy_cmminfo_msg_text_n_ );
  }

  if( oy_cmminfo_msg_text_ == NULL || oy_cmminfo_msg_text_n_ == 0 )
    return "Memory problem";

  oy_cmminfo_msg_text_[0] = '\000';

  if(!(flags & 0x01))
    sprintf(oy_cmminfo_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  

  
  /* allocate enough space */
  if(oy_cmminfo_msg_text_n_ < 1000)
  {
    oyDeAlloc_f dealloc = oyDeAllocateFunc_;
    if(oy_cmminfo_msg_text_ && oy_cmminfo_msg_text_n_)
      dealloc( oy_cmminfo_msg_text_ );
    oy_cmminfo_msg_text_n_ = 1024;
    oy_cmminfo_msg_text_ = alloc(oy_cmminfo_msg_text_n_);
    if(oy_cmminfo_msg_text_)
      oy_cmminfo_msg_text_[0] = '\000';
    else
      return "Memory Error";

    if(!(flags & 0x01))
      sprintf(oy_cmminfo_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");
  }

  if(type == oyNAME_NICK && (flags & 0x01))
  {
    sprintf( &oy_cmminfo_msg_text_[strlen(oy_cmminfo_msg_text_)], "%s",
             s->cmm
           );
  } else
  if(type == oyNAME_NAME)
    sprintf( &oy_cmminfo_msg_text_[strlen(oy_cmminfo_msg_text_)], "%s %s",
             s->cmm, s->backend_version
           );
  else
  if((int)type >= oyNAME_DESCRIPTION)
  {
    sprintf( &oy_cmminfo_msg_text_[strlen(oy_cmminfo_msg_text_)], "%s %s %d",
             s->cmm, s->backend_version, s->oy_compatibility
           );
  }


  return oy_cmminfo_msg_text_;
}

static void oyCMMinfo_StaticFree_           ( void )
{
  if(oy_cmminfo_init_)
  {
    oy_cmminfo_init_ = 0;
    if(oy_cmminfo_msg_text_)
      oyFree_m_(oy_cmminfo_msg_text_);
    if(oy_debug)
      fprintf(stderr, "%s() freeing static \"%s\" memory\n", "oyCMMinfo_StaticFree_", "oyCMMinfo_s" );
  }
}


/* Include "CMMinfo.private_custom_definitions.c" { */
/** Function    oyCMMinfo_Release__Members
 *  @memberof   oyCMMinfo_s
 *  @brief      Custom CMMinfo destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmminfo  the CMMinfo object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2017/10/09
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 */
void oyCMMinfo_Release__Members( oyCMMinfo_s_ * cmminfo )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmminfo->member );
   */
  oyCMMapi_s * api = NULL,
             * old_api = cmminfo->api;
  while(old_api && (api = oyCMMapi_GetNext(old_api)) != NULL)
  {
    if(old_api->release)
      old_api->release( (oyStruct_s**)&old_api );
    old_api = api;
  }

  if(cmminfo->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = cmminfo->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmminfo->member );
     */

    if(cmminfo->backend_version)
      deallocateFunc( cmminfo->backend_version );
  }
}

/** Function    oyCMMinfo_Init__Members
 *  @memberof   oyCMMinfo_s
 *  @brief      Custom CMMinfo constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmminfo  the CMMinfo object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMinfo_Init__Members( oyCMMinfo_s_ * cmminfo OY_UNUSED )
{
  return 0;
}

/** Function    oyCMMinfo_Copy__Members
 *  @memberof   oyCMMinfo_s
 *  @brief      Custom CMMinfo copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyCMMinfo_s_ input object
 *  @param[out]  dst  the output oyCMMinfo_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMinfo_Copy__Members( oyCMMinfo_s_ * dst, oyCMMinfo_s_ * src)
{
  oyAlloc_f allocateFunc_ = 0;
#if 0
  oyDeAlloc_f deallocateFunc_ = 0;
#endif
  int error = 0;

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;
#if 0
  deallocateFunc_ = dst->oy_->deallocateFunc_;
#endif

  /* Copy each value of src to dst here */
  error = !memcpy(dst->cmm, src->cmm, 8);

  if(src->backend_version)
    dst->backend_version = oyStringCopy_( src->backend_version, allocateFunc_ );

  dst->getText = src->getText;

  dst->oy_compatibility = src->oy_compatibility;

  return error;
}

/* } Include "CMMinfo.private_custom_definitions.c" */


/** @internal
 *  Function oyCMMinfo_New_
 *  @memberof oyCMMinfo_s_
 *  @brief   allocate a new oyCMMinfo_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMinfo_s_ * oyCMMinfo_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CMM_INFO_S;
  int error = 0, id = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object, "oyCMMinfo_s" );
  oyCMMinfo_s_ * s = 0;

  if(s_obj)
  {
    id = s_obj->id_;
    switch(id) /* give valgrind a glue, which object was created */
    {
      case 1: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 2: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 3: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 4: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 5: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 6: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 7: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 8: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 9: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 10: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 11: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 12: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 13: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 14: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 15: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 16: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 17: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 18: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 19: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 20: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 21: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 22: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 23: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 24: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 25: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 26: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 27: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 28: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 29: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 30: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 31: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 32: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 33: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 34: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 35: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 36: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 37: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 38: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      case 39: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_)); break;
      default: s = (oyCMMinfo_s_*)s_obj->allocateFunc_(sizeof(oyCMMinfo_s_));
    }
  }
  else
  {
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  if(!s)
  {
    if(s_obj)
      oyObject_Release( &s_obj );
    WARNc_S(_("MEM Error."));
    return NULL;
  }

  error = !memset( s, 0, sizeof(oyCMMinfo_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyCMMinfo_Copy;
  s->release = (oyStruct_Release_f) oyCMMinfo_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom CMMinfo constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CMM_INFO_S, (oyPointer)s );
  /* ---- end of custom CMMinfo constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom CMMinfo constructor ----- */
  error += oyCMMinfo_Init__Members( s );
  /* ---- end of custom CMMinfo constructor ------- */
  
  
  
  

  if(!oy_cmminfo_init_)
  {
    oy_cmminfo_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyCMMinfo_StaticMessageFunc_,
                                        oyCMMinfo_StaticFree_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyCMMinfo_Copy__
 *  @memberof oyCMMinfo_s_
 *  @brief   real copy a CMMinfo object
 *
 *  @param[in]     cmminfo                 CMMinfo struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMinfo_s_ * oyCMMinfo_Copy__ ( oyCMMinfo_s_ *cmminfo, oyObject_s object )
{
  oyCMMinfo_s_ *s = 0;
  int error = 0;

  if(!cmminfo || !object)
    return s;

  s = (oyCMMinfo_s_*) oyCMMinfo_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom CMMinfo copy constructor ----- */
    error = oyCMMinfo_Copy__Members( s, cmminfo );
    /* ---- end of custom CMMinfo copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyCMMinfo_Release_( &s );

  return s;
}

/** @internal
 *  Function oyCMMinfo_Copy_
 *  @memberof oyCMMinfo_s_
 *  @brief   copy or reference a CMMinfo object
 *
 *  @param[in]     cmminfo                 CMMinfo struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMinfo_s_ * oyCMMinfo_Copy_ ( oyCMMinfo_s_ *cmminfo, oyObject_s object )
{
  oyCMMinfo_s_ * s = cmminfo;

  if(!cmminfo)
    return 0;

  if(cmminfo && !object)
  {
    if(oy_debug_objects >= 0 && s->oy_)
    {
      const char * t = getenv(OY_DEBUG_OBJECTS);
      int id_ = -1;

      if(t)
        id_ = atoi(t);
      else
        id_ = oy_debug_objects;

      if((id_ >= 0 && s->oy_->id_ == id_) ||
         (t && s && (strstr(oyStructTypeToText(s->type_), t) != 0)) ||
         id_ == 1)
      {
        oyStruct_s ** parents = NULL;
        int n = oyStruct_GetParents( (oyStruct_s*)s, &parents ),
            observer_refs = oyStruct_ObservationCount( (oyStruct_s*)s, 0 ),
            i;
        const char * track_name = oyStructTypeToText(s->type_);
        if(!(track_name && track_name[0]))
          track_name = oyStruct_GetInfo( s, oyNAME_NICK, 0x01 );
        if(s->oy_->id_ == id_)
          for( i = 0; i < s->oy_->ref_ - observer_refs - n; ++i)
            fprintf( stderr, "  " );
        fprintf( stderr, "%s[%d] tracking refs: %d++ observers: %d parents: %d\n",
                 (s->oy_->id_ == id_)?oyjlTermColor(oyjlGREEN, track_name):track_name, s->oy_->id_, s->oy_->ref_, observer_refs, n );
        for(i = 0; i < n; ++i)
        {
          track_name = oyStructTypeToText(parents[i]->type_);
          if(!(track_name && track_name[0]))
            track_name = oyStruct_GetInfo( parents[i], oyNAME_NICK, 0x01 );
          if(s->oy_->id_ == id_)
          {
            int i;
            for( i = 0; i < s->oy_->ref_ - observer_refs - n; ++i)
              fprintf( stderr, "  " );
          }
          fprintf( stderr, "parent[%d]: %s[%d]\n", i,
                   track_name, parents[i]->oy_->id_ );
        }
      }
    }
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyCMMinfo_Copy__( cmminfo, object );

  return s;
}
 
/** @internal
 *  Function oyCMMinfo_Release_
 *  @memberof oyCMMinfo_s_
 *  @brief   release and possibly deallocate a CMMinfo object
 *
 *  @param[in,out] cmminfo                 CMMinfo struct object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/10/23
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyCMMinfo_Release_( oyCMMinfo_s_ **cmminfo )
{
  const char * track_name = NULL;
  int observer_refs = 0, id = 0, refs = 0, parent_refs = 0;
  /* ---- start of common object destructor ----- */
  oyCMMinfo_s_ *s = 0;

  if(!cmminfo || !*cmminfo)
    return 0;

  s = *cmminfo;
  /* static object */
  if(!s->oy_)
    return 0;

  id = s->oy_->id_;
  refs = s->oy_->ref_;

  if(refs <= 0) /* avoid circular or double dereferencing */
    return 0;

  *cmminfo = 0;

  observer_refs = oyStruct_ObservationCount( (oyStruct_s*)s, 0 );

  if(oy_debug_objects >= 0)
  {
    const char * t = getenv(OY_DEBUG_OBJECTS);
    int id_ = -1;

    if(t)
      id_ = atoi(t);
    else
      id_ = oy_debug_objects;

    if((id_ >= 0 && s->oy_->id_ == id_) ||
       (t && (strstr(oyStructTypeToText(s->type_), t) != 0)) ||
       id_ == 1)
    {
      oyStruct_s ** parents = NULL;
      int n = oyStruct_GetParents( (oyStruct_s*)s, &parents );
      {
        int i;
        track_name = oyStructTypeToText(s->type_);
        if(!(track_name && track_name[0]))
          track_name = oyStruct_GetInfo( s, oyNAME_NICK, 0x01 );
        if(s->oy_->id_ == id_)
          for( i = 0; i < s->oy_->ref_ - observer_refs - n; ++i)
            fprintf( stderr, "  " );
        fprintf( stderr, "%s[%d] unref with refs: %d-- observers: %d parents: %d\n",
                 (s->oy_->id_ == id_)?oyjlTermColor(oyjlRED, track_name):track_name, s->oy_->id_, s->oy_->ref_, observer_refs, n );
        for(i = 0; i < n; ++i)
        {
          track_name = oyStructTypeToText(parents[i]->type_);
          if(!(track_name && track_name[0]))
            track_name = oyStruct_GetInfo( parents[i], oyNAME_NICK, 0x01 );
          if(s->oy_->id_ == id_)
          {
            int i;
            for( i = 0; i < s->oy_->ref_ - observer_refs - n; ++i)
              fprintf( stderr, "  " );
          }
          fprintf( stderr, "parent[%d]: %s[%d]\n", i,
                   track_name, parents[i]->oy_->id_ );
        }
      }
    }
  }

  
  if((oyObject_UnRef(s->oy_) - parent_refs - 2*observer_refs) > 0)
    return 0;
  /* ---- end of common object destructor ------- */

  if(oy_debug_objects >= 0)
  {
    const char * t = getenv(OY_DEBUG_OBJECTS);
    int id_ = -1;

    if(t)
      id_ = atoi(t);
    else
      id_ = oy_debug_objects;

    if((id_ >= 0 && s->oy_->id_ == id_) ||
       (t && s && (strstr(oyStructTypeToText(s->type_), t) != 0)) ||
       id_ == 1)
    {
      track_name = oyStructTypeToText(s->type_);
      if(!(track_name && track_name[0]))
        track_name = oyStruct_GetInfo( s, oyNAME_NICK, 0x01 );
      fprintf( stderr, "%s[%d] destruct\n", (s->oy_->id_ == id_)?oyjlTermColor(oyjlRED, track_name):track_name, s->oy_->id_);
    }
  }

  refs = s->oy_->ref_;
  if(refs < 0)
  {
    WARNc2_S( "oyCMMinfo_s[%d]->object can not be untracked with refs: %d\n", id, refs );
    //oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ "refs:%d", OY_DBG_ARGS_, refs);
    return -1; /* issue */
  }

  
  /* ---- start of custom CMMinfo destructor ----- */
  oyCMMinfo_Release__Members( s );
  /* ---- end of custom CMMinfo destructor ------- */
  
  
  
  



  /* remove observer edges */
  oyOptions_Release( &s->oy_->handles_ );

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;
    oyObject_s oy = s->oy_;

    refs = s->oy_->ref_;

    if(track_name)
      fprintf( stderr, "%s[%d] destructing\n", (s->oy_->id_ == oy_debug_objects)?oyjlTermColor(oyjlRED, track_name):track_name, id );

    if(refs > 1)
      fprintf( stderr, "!!!ERROR:%d oyCMMinfo_s[%d]->object can not be untracked with refs: %d\n", __LINE__, id, refs);

    s->oy_ = NULL;
    oyObject_Release( &oy );
    if(track_name)
      fprintf( stderr, "%s[%d] destructed\n", (id == oy_debug_objects)?oyjlTermColor(oyjlRED, track_name):track_name, id );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "CMMinfo.private_methods_definitions.c" { */

/* } Include "CMMinfo.private_methods_definitions.c" */

