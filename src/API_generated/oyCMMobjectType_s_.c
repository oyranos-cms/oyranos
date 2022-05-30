/** @file oyCMMobjectType_s_.c

   [Template file inheritance graph]
   +-> oyCMMobjectType_s_.template.c
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



  
#include "oyCMMobjectType_s.h"
#include "oyCMMobjectType_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"
#include "oyranos_module_internal.h"

  

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif


static int oy_cmmobjecttype_init_ = 0;
static char * oy_cmmobjecttype_msg_text_ = NULL;
static int oy_cmmobjecttype_msg_text_n_ = 0;
static const char * oyCMMobjectType_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyCMMobjectType_s_ * s = (oyCMMobjectType_s_*) obj;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if( oy_cmmobjecttype_msg_text_ == NULL || oy_cmmobjecttype_msg_text_n_ == 0 )
  {
    oy_cmmobjecttype_msg_text_n_ = 512;
    oy_cmmobjecttype_msg_text_ = (char*) alloc( oy_cmmobjecttype_msg_text_n_ );
    if(oy_cmmobjecttype_msg_text_)
      memset( oy_cmmobjecttype_msg_text_, 0, oy_cmmobjecttype_msg_text_n_ );
  }

  if( oy_cmmobjecttype_msg_text_ == NULL || oy_cmmobjecttype_msg_text_n_ == 0 )
    return "Memory problem";

  oy_cmmobjecttype_msg_text_[0] = '\000';

  if(!(flags & 0x01))
    sprintf(oy_cmmobjecttype_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  
  

  return oy_cmmobjecttype_msg_text_;
}

static void oyCMMobjectType_StaticFree_           ( void )
{
  if(oy_cmmobjecttype_init_)
  {
    oy_cmmobjecttype_init_ = 0;
    if(oy_cmmobjecttype_msg_text_)
      oyFree_m_(oy_cmmobjecttype_msg_text_);
    if(oy_debug)
      fprintf(stderr, "%s() freeing static \"%s\" memory\n", "oyCMMobjectType_StaticFree_", "oyCMMobjectType_s" );
  }
}


/* Include "CMMobjectType.private_custom_definitions.c" { */
/** Function    oyCMMobjectType_Release__Members
 *  @memberof   oyCMMobjectType_s
 *  @brief      Custom CMMobjectType destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmmobjecttype  the CMMobjectType object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyCMMobjectType_Release__Members( oyCMMobjectType_s_ * cmmobjecttype )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmmobjecttype->member );
   */

  if(cmmobjecttype->oy_->deallocateFunc_)
  {
#if 0
    oyDeAlloc_f deallocateFunc = cmmobjecttype->oy_->deallocateFunc_;
#endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmmobjecttype->member );
     */
  }
}

/** Function    oyCMMobjectType_Init__Members
 *  @memberof   oyCMMobjectType_s
 *  @brief      Custom CMMobjectType constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmmobjecttype  the CMMobjectType object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMobjectType_Init__Members( oyCMMobjectType_s_ * cmmobjecttype OY_UNUSED )
{
  return 0;
}

/** Function    oyCMMobjectType_Copy__Members
 *  @memberof   oyCMMobjectType_s
 *  @brief      Custom CMMobjectType copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyCMMobjectType_s_ input object
 *  @param[out]  dst  the output oyCMMobjectType_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMobjectType_Copy__Members( oyCMMobjectType_s_ * dst, oyCMMobjectType_s_ * src)
{
#if 0
  oyAlloc_f allocateFunc_ = 0;
  oyDeAlloc_f deallocateFunc_ = 0;
#endif

  if(!dst || !src)
    return 1;

#if 0
  allocateFunc_ = dst->oy_->allocateFunc_;
  deallocateFunc_ = dst->oy_->deallocateFunc_;
#endif

  /* Copy each value of src to dst here */

  return 0;
}

/* } Include "CMMobjectType.private_custom_definitions.c" */


/** @internal
 *  Function oyCMMobjectType_New_
 *  @memberof oyCMMobjectType_s_
 *  @brief   allocate a new oyCMMobjectType_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMobjectType_s_ * oyCMMobjectType_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CMM_OBJECT_TYPE_S;
  int error = 0, id = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object, "oyCMMobjectType_s" );
  oyCMMobjectType_s_ * s = 0;

  if(s_obj)
  {
    id = s_obj->id_;
    switch(id) /* give valgrind a glue, which object was created */
    {
      case 1: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 2: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 3: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 4: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 5: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 6: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 7: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 8: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 9: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 10: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 11: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 12: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 13: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 14: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 15: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 16: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 17: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 18: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 19: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 20: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 21: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 22: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 23: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 24: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 25: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 26: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 27: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 28: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 29: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 30: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 31: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 32: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 33: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 34: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 35: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 36: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 37: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 38: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      case 39: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_)); break;
      default: s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_));
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

  error = !memset( s, 0, sizeof(oyCMMobjectType_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyCMMobjectType_Copy;
  s->release = (oyStruct_Release_f) oyCMMobjectType_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom CMMobjectType constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CMM_OBJECT_TYPE_S, (oyPointer)s );
  /* ---- end of custom CMMobjectType constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom CMMobjectType constructor ----- */
  error += oyCMMobjectType_Init__Members( s );
  /* ---- end of custom CMMobjectType constructor ------- */
  
  
  
  

  if(!oy_cmmobjecttype_init_)
  {
    oy_cmmobjecttype_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyCMMobjectType_StaticMessageFunc_,
                                        oyCMMobjectType_StaticFree_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyCMMobjectType_Copy__
 *  @memberof oyCMMobjectType_s_
 *  @brief   real copy a CMMobjectType object
 *
 *  @param[in]     cmmobjecttype                 CMMobjectType struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMobjectType_s_ * oyCMMobjectType_Copy__ ( oyCMMobjectType_s_ *cmmobjecttype, oyObject_s object )
{
  oyCMMobjectType_s_ *s = 0;
  int error = 0;

  if(!cmmobjecttype || !object)
    return s;

  s = (oyCMMobjectType_s_*) oyCMMobjectType_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom CMMobjectType copy constructor ----- */
    error = oyCMMobjectType_Copy__Members( s, cmmobjecttype );
    /* ---- end of custom CMMobjectType copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyCMMobjectType_Release_( &s );

  return s;
}

/** @internal
 *  Function oyCMMobjectType_Copy_
 *  @memberof oyCMMobjectType_s_
 *  @brief   copy or reference a CMMobjectType object
 *
 *  @param[in]     cmmobjecttype                 CMMobjectType struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMobjectType_s_ * oyCMMobjectType_Copy_ ( oyCMMobjectType_s_ *cmmobjecttype, oyObject_s object )
{
  oyCMMobjectType_s_ * s = cmmobjecttype;

  if(!cmmobjecttype)
    return 0;

  if(cmmobjecttype && !object)
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

  s = oyCMMobjectType_Copy__( cmmobjecttype, object );

  return s;
}
 
/** @internal
 *  Function oyCMMobjectType_Release_
 *  @memberof oyCMMobjectType_s_
 *  @brief   release and possibly deallocate a CMMobjectType object
 *
 *  @param[in,out] cmmobjecttype                 CMMobjectType struct object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/10/23
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyCMMobjectType_Release_( oyCMMobjectType_s_ **cmmobjecttype )
{
  const char * track_name = NULL;
  int observer_refs = 0, id = 0, refs = 0, parent_refs = 0;
  /* ---- start of common object destructor ----- */
  oyCMMobjectType_s_ *s = 0;

  if(!cmmobjecttype || !*cmmobjecttype)
    return 0;

  s = *cmmobjecttype;
  /* static object */
  if(!s->oy_)
    return 0;

  id = s->oy_->id_;
  refs = s->oy_->ref_;

  if(refs <= 0) /* avoid circular or double dereferencing */
    return 0;

  *cmmobjecttype = 0;

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
    WARNc2_S( "oyCMMobjectType_s[%d]->object can not be untracked with refs: %d\n", id, refs );
    //oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ "refs:%d", OY_DBG_ARGS_, refs);
    return -1; /* issue */
  }

  
  /* ---- start of custom CMMobjectType destructor ----- */
  oyCMMobjectType_Release__Members( s );
  /* ---- end of custom CMMobjectType destructor ------- */
  
  
  
  



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
      fprintf( stderr, "!!!ERROR:%d oyCMMobjectType_s[%d]->object can not be untracked with refs: %d\n", __LINE__, id, refs);

    s->oy_ = NULL;
    oyObject_Release( &oy );
    if(track_name)
      fprintf( stderr, "%s[%d] destructed\n", (id == oy_debug_objects)?oyjlTermColor(oyjlRED, track_name):track_name, id );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "CMMobjectType.private_methods_definitions.c" { */

/* } Include "CMMobjectType.private_methods_definitions.c" */

