/** @file oyCMMapi8_s_.c

   [Template file inheritance graph]
   +-> oyCMMapi8_s_.template.c
   |
   +-> oyCMMapiFilter_s_.template.c
   |
   +-> oyCMMapi_s_.template.c
   |
   +-- Base_s_.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2019 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */



  
#include "oyCMMapi8_s.h"
#include "oyCMMapi8_s_.h"



#include "oyCMMapi_s_.h"
#include "oyCMMapiFilter_s_.h"



#include "oyObject_s.h"
#include "oyranos_object_internal.h"
#include "oyranos_module_internal.h"



#include "oyCMMapiFilter_s_.h"
  

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif


static int oy_cmmapi8_init_ = 0;
static char * oy_cmmapi8_msg_text_ = NULL;
static int oy_cmmapi8_msg_text_n_ = 0;
static const char * oyCMMapi8_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyCMMapi8_s_ * s = (oyCMMapi8_s_*) obj;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if(s->oy_ && s->oy_->allocateFunc_)
    alloc = s->oy_->allocateFunc_;

  if( oy_cmmapi8_msg_text_ == NULL || oy_cmmapi8_msg_text_n_ == 0 )
  {
    oy_cmmapi8_msg_text_n_ = 512;
    oy_cmmapi8_msg_text_ = (char*) alloc( oy_cmmapi8_msg_text_n_ );
    if(oy_cmmapi8_msg_text_)
      memset( oy_cmmapi8_msg_text_, 0, oy_cmmapi8_msg_text_n_ );
  }

  if( oy_cmmapi8_msg_text_ == NULL || oy_cmmapi8_msg_text_n_ == 0 )
    return "Memory problem";

  oy_cmmapi8_msg_text_[0] = '\000';

  if(!(flags & 0x01))
    sprintf(oy_cmmapi8_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  
  

  return oy_cmmapi8_msg_text_;
}

static void oyCMMapi8_StaticFree_           ( void )
{
  if(oy_cmmapi8_init_)
  {
    oy_cmmapi8_init_ = 0;
    if(oy_cmmapi8_msg_text_)
      oyFree_m_(oy_cmmapi8_msg_text_);
    if(oy_debug)
      fprintf(stderr, "%s() freeing static \"%s\" memory\n", "oyCMMapi8_StaticFree_", "oyCMMapi8_s" );
  }
}


/* Include "CMMapi8.private_custom_definitions.c" { */
/** Function    oyCMMapi8_Release__Members
 *  @memberof   oyCMMapi8_s
 *  @brief      Custom CMMapi8 destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmmapi8  the CMMapi8 object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyCMMapi8_Release__Members( oyCMMapi8_s_ * cmmapi8 )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmmapi8->member );
   */

  if(cmmapi8->oy_->deallocateFunc_)
  {
#if 0
    oyDeAlloc_f deallocateFunc = cmmapi8->oy_->deallocateFunc_;
#endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmmapi8->member );
     */
  }
}

/** Function    oyCMMapi8_Init__Members
 *  @memberof   oyCMMapi8_s
 *  @brief      Custom CMMapi8 constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmmapi8  the CMMapi8 object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapi8_Init__Members( oyCMMapi8_s_ * cmmapi8 OY_UNUSED )
{
  return 0;
}

/** Function    oyCMMapi8_Copy__Members
 *  @memberof   oyCMMapi8_s
 *  @brief      Custom CMMapi8 copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyCMMapi8_s_ input object
 *  @param[out]  dst  the output oyCMMapi8_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapi8_Copy__Members( oyCMMapi8_s_ * dst, oyCMMapi8_s_ * src)
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

/* } Include "CMMapi8.private_custom_definitions.c" */


/** @internal
 *  Function oyCMMapi8_New_
 *  @memberof oyCMMapi8_s_
 *  @brief   allocate a new oyCMMapi8_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapi8_s_ * oyCMMapi8_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CMM_API8_S;
  int error = 0, id = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyCMMapi8_s_ * s = 0;

  if(s_obj)
  {
    id = s_obj->id_;
    switch(id) /* give valgrind a glue, which object was created */
    {
      case 1: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 2: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 3: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 4: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 5: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 6: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 7: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 8: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 9: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 10: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 11: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 12: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 13: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 14: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 15: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 16: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 17: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 18: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 19: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 20: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 21: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 22: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 23: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 24: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 25: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 26: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 27: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 28: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 29: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 30: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 31: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 32: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 33: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 34: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 35: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 36: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 37: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 38: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      case 39: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_)); break;
      default: s = (oyCMMapi8_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi8_s_));
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

  error = !memset( s, 0, sizeof(oyCMMapi8_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyCMMapi8_Copy;
  s->release = (oyStruct_Release_f) oyCMMapi8_Release;

  s->oy_ = s_obj;

  
  
  
  /* ---- start of custom CMMapi constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CMM_API_S, (oyPointer)s );
  /* ---- end of custom CMMapi constructor ------- */
  /* ---- start of custom CMMapiFilter constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CMM_API_FILTER_S, (oyPointer)s );
  /* ---- end of custom CMMapiFilter constructor ------- */
  /* ---- start of custom CMMapi8 constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CMM_API8_S, (oyPointer)s );
  /* ---- end of custom CMMapi8 constructor ------- */
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  
  
  /* ---- start of custom CMMapi constructor ----- */
  error += oyCMMapi_Init__Members( (oyCMMapi_s_*)s );
  /* ---- end of custom CMMapi constructor ------- */
  /* ---- start of custom CMMapiFilter constructor ----- */
  error += oyCMMapiFilter_Init__Members( (oyCMMapiFilter_s_*)s );
  /* ---- end of custom CMMapiFilter constructor ------- */
  /* ---- start of custom CMMapi8 constructor ----- */
  error += oyCMMapi8_Init__Members( s );
  /* ---- end of custom CMMapi8 constructor ------- */
  
  

  if(!oy_cmmapi8_init_)
  {
    oy_cmmapi8_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyCMMapi8_StaticMessageFunc_,
                                        oyCMMapi8_StaticFree_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyCMMapi8_Copy__
 *  @memberof oyCMMapi8_s_
 *  @brief   real copy a CMMapi8 object
 *
 *  @param[in]     cmmapi8                 CMMapi8 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapi8_s_ * oyCMMapi8_Copy__ ( oyCMMapi8_s_ *cmmapi8, oyObject_s object )
{
  oyCMMapi8_s_ *s = 0;
  int error = 0;

  if(!cmmapi8 || !object)
    return s;

  s = (oyCMMapi8_s_*) oyCMMapi8_New( object );
  error = !s;

  if(!error) {
    
    
    
    /* ---- start of custom CMMapi copy constructor ----- */
    error = oyCMMapi_Copy__Members( (oyCMMapi_s_*)s, (oyCMMapi_s_*)cmmapi8 );
    /* ---- end of custom CMMapi copy constructor ------- */
    /* ---- start of custom CMMapiFilter copy constructor ----- */
    error = oyCMMapiFilter_Copy__Members( (oyCMMapiFilter_s_*)s, (oyCMMapiFilter_s_*)cmmapi8 );
    /* ---- end of custom CMMapiFilter copy constructor ------- */
    /* ---- start of custom CMMapi8 copy constructor ----- */
    error = oyCMMapi8_Copy__Members( s, cmmapi8 );
    /* ---- end of custom CMMapi8 copy constructor ------- */
    
    
    
    
  }

  if(error)
    oyCMMapi8_Release_( &s );

  return s;
}

/** @internal
 *  Function oyCMMapi8_Copy_
 *  @memberof oyCMMapi8_s_
 *  @brief   copy or reference a CMMapi8 object
 *
 *  @param[in]     cmmapi8                 CMMapi8 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapi8_s_ * oyCMMapi8_Copy_ ( oyCMMapi8_s_ *cmmapi8, oyObject_s object )
{
  oyCMMapi8_s_ *s = cmmapi8;

  if(!cmmapi8)
    return 0;

  if(cmmapi8 && !object)
  {
    s = cmmapi8;
    
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
        int n = oyStruct_GetParents( (oyStruct_s*)s, &parents );
        {
          int i;
          const char * track_name = oyStructTypeToText(s->type_);
          OY_BACKTRACE_PRINT
          fprintf( stderr, "%s[%d] tracking refs: %d parents: %d\n",
                   track_name, s->oy_->id_, s->oy_->ref_, n );
          for(i = 0; i < n; ++i)
          {
            track_name = oyStructTypeToText(parents[i]->type_);
            fprintf( stderr, "parent[%d]: %s[%d]\n", i,
                     track_name, parents[i]->oy_->id_ );
          }
        }
      }
    }
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyCMMapi8_Copy__( cmmapi8, object );

  return s;
}
 
/** @internal
 *  Function oyCMMapi8_Release_
 *  @memberof oyCMMapi8_s_
 *  @brief   release and possibly deallocate a CMMapi8 object
 *
 *  @param[in,out] cmmapi8                 CMMapi8 struct object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/10/23
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyCMMapi8_Release_( oyCMMapi8_s_ **cmmapi8 )
{
  const char * track_name = NULL;
  int observer_refs = 0, i, id = 0, refs = 0;
  /* ---- start of common object destructor ----- */
  oyCMMapi8_s_ *s = 0;

  if(!cmmapi8 || !*cmmapi8)
    return 0;

  s = *cmmapi8;
  /* static object */
  if(!s->oy_)
    return 0;

  id = s->oy_->id_;
  refs = s->oy_->ref_;

  *cmmapi8 = 0;

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
        OY_BACKTRACE_PRINT
        track_name = oyStructTypeToText(s->type_);
        fprintf( stderr, "%s[%d] unref with refs: %d observers: %d parents: %d\n",
                 track_name, s->oy_->id_, s->oy_->ref_, observer_refs, n );
        for(i = 0; i < n; ++i)
        {
          track_name = oyStructTypeToText(parents[i]->type_);
          fprintf( stderr, "parent[%d]: %s[%d]\n", i,
                   track_name, parents[i]->oy_->id_ );
        }
      }
    }
  }

  
  if((oyObject_UnRef(s->oy_) - observer_refs*2) > 0)
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
      fprintf( stderr, "%s[%d] destruct\n", track_name, s->oy_->id_);
    }
  }

  /* model and observer reference each other. So release the object two times.
   * The models and and observers are released later inside the
   * oyObject_s::handles. */
  for(i = 0; i < observer_refs; ++i)
  {
    //oyObject_UnRef(s->oy_);
    oyObject_UnRef(s->oy_);
  }

  refs = s->oy_->ref_;
  if(refs < 0)
  {
    WARNc2_S( "node[%d]->object can not be untracked with refs: %d\n", id, refs );
    //oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ "refs:%d", OY_DBG_ARGS_, refs);
    return -1; /* issue */
  }

  
  
  
  /* ---- start of custom CMMapi destructor ----- */
  oyCMMapi_Release__Members( (oyCMMapi_s_*)s );
  /* ---- end of custom CMMapi destructor ------- */
  /* ---- start of custom CMMapiFilter destructor ----- */
  oyCMMapiFilter_Release__Members( (oyCMMapiFilter_s_*)s );
  /* ---- end of custom CMMapiFilter destructor ------- */
  /* ---- start of custom CMMapi8 destructor ----- */
  oyCMMapi8_Release__Members( s );
  /* ---- end of custom CMMapi8 destructor ------- */
  
  



  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;
    oyObject_s oy = s->oy_;

    refs = s->oy_->ref_;

    if(track_name)
      fprintf( stderr, "%s[%d] destructing\n", track_name, id );

    if(refs > 1)
      fprintf( stderr, "!!!ERROR:%d node[%d]->object can not be untracked with refs: %d\n", __LINE__, id, refs);

    for(i = 1; i < observer_refs; ++i) /* oyObject_Release(oy) will dereference one more time, so preserve here one ref for oyObject_Release(oy) */
      oyObject_UnRef(oy);

    s->oy_ = NULL;
    oyObject_Release( &oy );
    if(track_name)
      fprintf( stderr, "%s[%d] destructed\n", track_name, id );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "CMMapi8.private_methods_definitions.c" { */

/* } Include "CMMapi8.private_methods_definitions.c" */

