/** @file oyCMMapiFilters_s_.c

   [Template file inheritance graph]
   +-> oyCMMapiFilters_s_.template.c
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



  
#include "oyCMMapiFilters_s.h"
#include "oyCMMapiFilters_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"
#include "oyranos_module_internal.h"

  

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif


static int oy_cmmapifilters_init_ = 0;
static char * oy_cmmapifilters_msg_text_ = NULL;
static int oy_cmmapifilters_msg_text_n_ = 0;
static const char * oyCMMapiFilters_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyCMMapiFilters_s_ * s = (oyCMMapiFilters_s_*) obj;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if( oy_cmmapifilters_msg_text_ == NULL || oy_cmmapifilters_msg_text_n_ == 0 )
  {
    oy_cmmapifilters_msg_text_n_ = 512;
    oy_cmmapifilters_msg_text_ = (char*) alloc( oy_cmmapifilters_msg_text_n_ );
    if(oy_cmmapifilters_msg_text_)
      memset( oy_cmmapifilters_msg_text_, 0, oy_cmmapifilters_msg_text_n_ );
  }

  if( oy_cmmapifilters_msg_text_ == NULL || oy_cmmapifilters_msg_text_n_ == 0 )
    return "Memory problem";

  oy_cmmapifilters_msg_text_[0] = '\000';

  if(!(flags & 0x01))
    sprintf(oy_cmmapifilters_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  
  

  return oy_cmmapifilters_msg_text_;
}

static void oyCMMapiFilters_StaticFree_           ( void )
{
  if(oy_cmmapifilters_init_)
  {
    oy_cmmapifilters_init_ = 0;
    if(oy_cmmapifilters_msg_text_)
      oyFree_m_(oy_cmmapifilters_msg_text_);
    if(oy_debug)
      fprintf(stderr, "%s() freeing static \"%s\" memory\n", "oyCMMapiFilters_StaticFree_", "oyCMMapiFilters_s" );
  }
}


/* Include "CMMapiFilters.private_custom_definitions.c" { */
/** Function    oyCMMapiFilters_Release__Members
 *  @memberof   oyCMMapiFilters_s
 *  @brief      Custom CMMapiFilters destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmmapifilters  the CMMapiFilters object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyCMMapiFilters_Release__Members( oyCMMapiFilters_s_ * cmmapifilters )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmmapifilters->member );
   */

  if(cmmapifilters->oy_->deallocateFunc_)
  {
#if 0
    oyDeAlloc_f deallocateFunc = cmmapifilters->oy_->deallocateFunc_;
#endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmmapifilters->member );
     */
  }
}

/** Function    oyCMMapiFilters_Init__Members
 *  @memberof   oyCMMapiFilters_s
 *  @brief      Custom CMMapiFilters constructor
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmmapifilters  the CMMapiFilters object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapiFilters_Init__Members( oyCMMapiFilters_s_ * cmmapifilters OY_UNUSED )
{
  return 0;
}

/** Function    oyCMMapiFilters_Copy__Members
 *  @memberof   oyCMMapiFilters_s
 *  @brief      Custom CMMapiFilters copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyCMMapiFilters_s_ input object
 *  @param[out]  dst  the output oyCMMapiFilters_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapiFilters_Copy__Members( oyCMMapiFilters_s_ * dst, oyCMMapiFilters_s_ * src)
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

/* } Include "CMMapiFilters.private_custom_definitions.c" */


/** @internal
 *  Function oyCMMapiFilters_New_
 *  @memberof oyCMMapiFilters_s_
 *  @brief   allocate a new oyCMMapiFilters_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapiFilters_s_ * oyCMMapiFilters_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CMM_API_FILTERS_S;
  int error = 0, id = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object, "oyCMMapiFilters_s" );
  oyCMMapiFilters_s_ * s = 0;

  if(s_obj)
  {
    id = s_obj->id_;
    switch(id) /* give valgrind a glue, which object was created */
    {
      case 1: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 2: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 3: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 4: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 5: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 6: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 7: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 8: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 9: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 10: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 11: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 12: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 13: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 14: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 15: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 16: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 17: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 18: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 19: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 20: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 21: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 22: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 23: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 24: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 25: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 26: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 27: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 28: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 29: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 30: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 31: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 32: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 33: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 34: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 35: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 36: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 37: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 38: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      case 39: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_)); break;
      default: s = (oyCMMapiFilters_s_*)s_obj->allocateFunc_(sizeof(oyCMMapiFilters_s_));
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

  error = !memset( s, 0, sizeof(oyCMMapiFilters_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyCMMapiFilters_Copy;
  s->release = (oyStruct_Release_f) oyCMMapiFilters_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom CMMapiFilters constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CMM_API_FILTERS_S, (oyPointer)s );
  /* ---- end of custom CMMapiFilters constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  s->list_ = oyStructList_Create( s->type_, 0, 0 );


  
  /* ---- start of custom CMMapiFilters constructor ----- */
  error += oyCMMapiFilters_Init__Members( s );
  /* ---- end of custom CMMapiFilters constructor ------- */
  
  
  
  

  if(!oy_cmmapifilters_init_)
  {
    oy_cmmapifilters_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyCMMapiFilters_StaticMessageFunc_,
                                        oyCMMapiFilters_StaticFree_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyCMMapiFilters_Copy__
 *  @memberof oyCMMapiFilters_s_
 *  @brief   real copy a CMMapiFilters object
 *
 *  @param[in]     cmmapifilters                 CMMapiFilters struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapiFilters_s_ * oyCMMapiFilters_Copy__ ( oyCMMapiFilters_s_ *cmmapifilters, oyObject_s object )
{
  oyCMMapiFilters_s_ *s = 0;
  int error = 0;

  if(!cmmapifilters || !object)
    return s;

  s = (oyCMMapiFilters_s_*) oyCMMapiFilters_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom CMMapiFilters copy constructor ----- */
    error = oyCMMapiFilters_Copy__Members( s, cmmapifilters );
    /* ---- end of custom CMMapiFilters copy constructor ------- */
    
    
    
    
    
    oyStructList_Release( &s->list_ );
    s->list_ = oyStructList_Copy( cmmapifilters->list_, s->oy_ );

  }

  if(error)
    oyCMMapiFilters_Release_( &s );

  return s;
}

/** @internal
 *  Function oyCMMapiFilters_Copy_
 *  @memberof oyCMMapiFilters_s_
 *  @brief   copy or reference a CMMapiFilters object
 *
 *  @param[in]     cmmapifilters                 CMMapiFilters struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapiFilters_s_ * oyCMMapiFilters_Copy_ ( oyCMMapiFilters_s_ *cmmapifilters, oyObject_s object )
{
  oyCMMapiFilters_s_ * s = cmmapifilters;

  if(!cmmapifilters)
    return 0;

  if(cmmapifilters && !object)
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

  s = oyCMMapiFilters_Copy__( cmmapifilters, object );

  return s;
}
 
/** @internal
 *  Function oyCMMapiFilters_Release_
 *  @memberof oyCMMapiFilters_s_
 *  @brief   release and possibly deallocate a CMMapiFilters list
 *
 *  @param[in,out] cmmapifilters                 CMMapiFilters struct object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/10/23
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyCMMapiFilters_Release_( oyCMMapiFilters_s_ **cmmapifilters )
{
  const char * track_name = NULL;
  int observer_refs = 0, id = 0, refs = 0, parent_refs = 0;
  /* ---- start of common object destructor ----- */
  oyCMMapiFilters_s_ *s = 0;

  if(!cmmapifilters || !*cmmapifilters)
    return 0;

  s = *cmmapifilters;
  /* static object */
  if(!s->oy_)
    return 0;

  id = s->oy_->id_;
  refs = s->oy_->ref_;

  if(refs <= 0) /* avoid circular or double dereferencing */
    return 0;

  *cmmapifilters = 0;

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
    WARNc2_S( "oyCMMapiFilters_s[%d]->object can not be untracked with refs: %d\n", id, refs );
    //oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ "refs:%d", OY_DBG_ARGS_, refs);
    return -1; /* issue */
  }

  
  /* ---- start of custom CMMapiFilters destructor ----- */
  oyCMMapiFilters_Release__Members( s );
  /* ---- end of custom CMMapiFilters destructor ------- */
  
  
  
  

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
      fprintf( stderr, "!!!ERROR:%d oyCMMapiFilters_s[%d]->object can not be untracked with refs: %d\n", __LINE__, id, refs);

    s->oy_ = NULL;
    oyObject_Release( &oy );
    if(track_name)
      fprintf( stderr, "%s[%d] destructed\n", (id == oy_debug_objects)?oyjlTermColor(oyjlRED, track_name):track_name, id );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "CMMapiFilters.private_methods_definitions.c" { */

/* } Include "CMMapiFilters.private_methods_definitions.c" */

