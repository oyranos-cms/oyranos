/** @file oyCMMapis_s_.c

   [Template file inheritance graph]
   +-> oyCMMapis_s_.template.c
   |
   +-> BaseList_s_.c
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



  
#include "oyCMMapis_s.h"
#include "oyCMMapis_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"
#include "oyranos_module_internal.h"

  

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif


static int oy_cmmapis_init_ = 0;
static char * oy_cmmapis_msg_text_ = NULL;
static int oy_cmmapis_msg_text_n_ = 0;
static const char * oyCMMapis_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyCMMapis_s_ * s = (oyCMMapis_s_*) obj;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if( oy_cmmapis_msg_text_ == NULL || oy_cmmapis_msg_text_n_ == 0 )
  {
    oy_cmmapis_msg_text_n_ = 512;
    oy_cmmapis_msg_text_ = (char*) alloc( oy_cmmapis_msg_text_n_ );
    if(oy_cmmapis_msg_text_)
      memset( oy_cmmapis_msg_text_, 0, oy_cmmapis_msg_text_n_ );
  }

  if( oy_cmmapis_msg_text_ == NULL || oy_cmmapis_msg_text_n_ == 0 )
    return "Memory problem";

  oy_cmmapis_msg_text_[0] = '\000';

  if(!(flags & 0x01))
    sprintf(oy_cmmapis_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  
  

  return oy_cmmapis_msg_text_;
}

static void oyCMMapis_StaticFree_           ( void )
{
  if(oy_cmmapis_init_)
  {
    oy_cmmapis_init_ = 0;
    if(oy_cmmapis_msg_text_)
      oyFree_m_(oy_cmmapis_msg_text_);
    if(oy_debug)
      fprintf(stderr, "%s() freeing static \"%s\" memory\n", "oyCMMapis_StaticFree_", "oyCMMapis_s" );
  }
}


/* Include "CMMapis.private_custom_definitions.c" { */
/** Function    oyCMMapis_Release__Members
 *  @memberof   oyCMMapis_s
 *  @brief      Custom CMMapis destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmmapis  the CMMapis object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyCMMapis_Release__Members( oyCMMapis_s_ * cmmapis )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmmapis->member );
   */

  if(cmmapis->oy_->deallocateFunc_)
  {
#if 0
    oyDeAlloc_f deallocateFunc = cmmapis->oy_->deallocateFunc_;
#endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmmapis->member );
     */
  }
}

/** Function    oyCMMapis_Init__Members
 *  @memberof   oyCMMapis_s
 *  @brief      Custom CMMapis constructor
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmmapis  the CMMapis object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapis_Init__Members( oyCMMapis_s_ * cmmapis OY_UNUSED )
{
  return 0;
}

/** Function    oyCMMapis_Copy__Members
 *  @memberof   oyCMMapis_s
 *  @brief      Custom CMMapis copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyCMMapis_s_ input object
 *  @param[out]  dst  the output oyCMMapis_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapis_Copy__Members( oyCMMapis_s_ * dst, oyCMMapis_s_ * src)
{
  int error = 0;
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

  return error;
}

/* } Include "CMMapis.private_custom_definitions.c" */


/** @internal
 *  Function oyCMMapis_New_
 *  @memberof oyCMMapis_s_
 *  @brief   allocate a new oyCMMapis_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapis_s_ * oyCMMapis_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CMM_APIS_S;
  int error = 0, id = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object, "oyCMMapis_s" );
  oyCMMapis_s_ * s = 0;

  if(s_obj)
  {
    id = s_obj->id_;
    switch(id) /* give valgrind a glue, which object was created */
    {
      case 1: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 2: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 3: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 4: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 5: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 6: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 7: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 8: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 9: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 10: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 11: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 12: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 13: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 14: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 15: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 16: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 17: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 18: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 19: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 20: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 21: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 22: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 23: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 24: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 25: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 26: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 27: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 28: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 29: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 30: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 31: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 32: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 33: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 34: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 35: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 36: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 37: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 38: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      case 39: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_)); break;
      default: s = (oyCMMapis_s_*)s_obj->allocateFunc_(sizeof(oyCMMapis_s_));
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

  error = !memset( s, 0, sizeof(oyCMMapis_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyCMMapis_Copy;
  s->release = (oyStruct_Release_f) oyCMMapis_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom CMMapis constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CMM_APIS_S, (oyPointer)s );
  /* ---- end of custom CMMapis constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  s->list_ = oyStructList_Create( s->type_, 0, 0 );


  
  /* ---- start of custom CMMapis constructor ----- */
  error += oyCMMapis_Init__Members( s );
  /* ---- end of custom CMMapis constructor ------- */
  
  
  
  

  if(!oy_cmmapis_init_)
  {
    oy_cmmapis_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyCMMapis_StaticMessageFunc_,
                                        oyCMMapis_StaticFree_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyCMMapis_Copy__
 *  @memberof oyCMMapis_s_
 *  @brief   real copy a CMMapis object
 *
 *  @param[in]     cmmapis                 CMMapis struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapis_s_ * oyCMMapis_Copy__ ( oyCMMapis_s_ *cmmapis, oyObject_s object )
{
  oyCMMapis_s_ *s = 0;
  int error = 0;

  if(!cmmapis || !object)
    return s;

  s = (oyCMMapis_s_*) oyCMMapis_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom CMMapis copy constructor ----- */
    error = oyCMMapis_Copy__Members( s, cmmapis );
    /* ---- end of custom CMMapis copy constructor ------- */
    
    
    
    
    
    oyStructList_Release( &s->list_ );
    s->list_ = oyStructList_Copy( cmmapis->list_, s->oy_ );

  }

  if(error)
    oyCMMapis_Release_( &s );

  return s;
}

/** @internal
 *  Function oyCMMapis_Copy_
 *  @memberof oyCMMapis_s_
 *  @brief   copy or reference a CMMapis object
 *
 *  @param[in]     cmmapis                 CMMapis struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapis_s_ * oyCMMapis_Copy_ ( oyCMMapis_s_ *cmmapis, oyObject_s object )
{
  oyCMMapis_s_ * s = cmmapis;

  if(!cmmapis)
    return 0;

  if(cmmapis && !object)
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

  s = oyCMMapis_Copy__( cmmapis, object );

  return s;
}
 
/** @internal
 *  Function oyCMMapis_Release_
 *  @memberof oyCMMapis_s_
 *  @brief   release and possibly deallocate a CMMapis list
 *
 *  @param[in,out] cmmapis                 CMMapis struct object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/10/23
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyCMMapis_Release_( oyCMMapis_s_ **cmmapis )
{
  const char * track_name = NULL;
  int observer_refs = 0, id = 0, refs = 0, parent_refs = 0;
  /* ---- start of common object destructor ----- */
  oyCMMapis_s_ *s = 0;

  if(!cmmapis || !*cmmapis)
    return 0;

  s = *cmmapis;
  /* static object */
  if(!s->oy_)
    return 0;

  id = s->oy_->id_;
  refs = s->oy_->ref_;

  if(refs <= 0) /* avoid circular or double dereferencing */
    return 0;

  *cmmapis = 0;

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
    WARNc2_S( "oyCMMapis_s[%d]->object can not be untracked with refs: %d\n", id, refs );
    //oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ "refs:%d", OY_DBG_ARGS_, refs);
    return -1; /* issue */
  }

  
  /* ---- start of custom CMMapis destructor ----- */
  oyCMMapis_Release__Members( s );
  /* ---- end of custom CMMapis destructor ------- */
  
  
  
  

  oyStructList_Release( &s->list_ );


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
      fprintf( stderr, "!!!ERROR:%d oyCMMapis_s[%d]->object can not be untracked with refs: %d\n", __LINE__, id, refs);

    s->oy_ = NULL;
    oyObject_Release( &oy );
    if(track_name)
      fprintf( stderr, "%s[%d] destructed\n", (id == oy_debug_objects)?oyjlTermColor(oyjlRED, track_name):track_name, id );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "CMMapis.private_methods_definitions.c" { */

/* } Include "CMMapis.private_methods_definitions.c" */

