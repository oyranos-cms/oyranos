/** @file oyCMMapi5_s_.c

   [Template file inheritance graph]
   +-> oyCMMapi5_s_.template.c
   |
   +-> oyCMMapi_s_.template.c
   |
   +-- Base_s_.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2020 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */



  
#include "oyCMMapi5_s.h"
#include "oyCMMapi5_s_.h"


#include "oyCMMapi_s_.h"




#include "oyObject_s.h"
#include "oyranos_object_internal.h"
#include "oyranos_module_internal.h"



#include "oyCMMapiFilter_s_.h"
  

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif


static int oy_cmmapi5_init_ = 0;
static char * oy_cmmapi5_msg_text_ = NULL;
static int oy_cmmapi5_msg_text_n_ = 0;
static const char * oyCMMapi5_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyCMMapi5_s_ * s = (oyCMMapi5_s_*) obj;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if( oy_cmmapi5_msg_text_ == NULL || oy_cmmapi5_msg_text_n_ == 0 )
  {
    oy_cmmapi5_msg_text_n_ = 512;
    oy_cmmapi5_msg_text_ = (char*) alloc( oy_cmmapi5_msg_text_n_ );
    if(oy_cmmapi5_msg_text_)
      memset( oy_cmmapi5_msg_text_, 0, oy_cmmapi5_msg_text_n_ );
  }

  if( oy_cmmapi5_msg_text_ == NULL || oy_cmmapi5_msg_text_n_ == 0 )
    return "Memory problem";

  oy_cmmapi5_msg_text_[0] = '\000';

  if(!(flags & 0x01))
    sprintf(oy_cmmapi5_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  
  

  return oy_cmmapi5_msg_text_;
}

static void oyCMMapi5_StaticFree_           ( void )
{
  if(oy_cmmapi5_init_)
  {
    oy_cmmapi5_init_ = 0;
    if(oy_cmmapi5_msg_text_)
      oyFree_m_(oy_cmmapi5_msg_text_);
    if(oy_debug)
      fprintf(stderr, "%s() freeing static \"%s\" memory\n", "oyCMMapi5_StaticFree_", "oyCMMapi5_s" );
  }
}


/* Include "CMMapi5.private_custom_definitions.c" { */
/** Function    oyCMMapi5_Release__Members
 *  @memberof   oyCMMapi5_s
 *  @brief      Custom CMMapi5 destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmmapi5  the CMMapi5 object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyCMMapi5_Release__Members( oyCMMapi5_s_ * cmmapi5 )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmmapi5->member );
   */

  if(cmmapi5->oy_->deallocateFunc_)
  {
#if 0
    oyDeAlloc_f deallocateFunc = cmmapi5->oy_->deallocateFunc_;
#endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmmapi5->member );
     */
  }
}

/** Function    oyCMMapi5_Init__Members
 *  @memberof   oyCMMapi5_s
 *  @brief      Custom CMMapi5 constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmmapi5  the CMMapi5 object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapi5_Init__Members( oyCMMapi5_s_ * cmmapi5 OY_UNUSED )
{
  return 0;
}

/** Function    oyCMMapi5_Copy__Members
 *  @memberof   oyCMMapi5_s
 *  @brief      Custom CMMapi5 copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyCMMapi5_s_ input object
 *  @param[out]  dst  the output oyCMMapi5_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapi5_Copy__Members( oyCMMapi5_s_ * dst, oyCMMapi5_s_ * src)
{
  if(!dst || !src)
    return 1;

  /* Copy each value of src to dst here */

  return 0;
}

/* } Include "CMMapi5.private_custom_definitions.c" */


/** @internal
 *  Function oyCMMapi5_New_
 *  @memberof oyCMMapi5_s_
 *  @brief   allocate a new oyCMMapi5_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapi5_s_ * oyCMMapi5_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CMM_API5_S;
  int error = 0, id = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object, "oyCMMapi5_s" );
  oyCMMapi5_s_ * s = 0;

  if(s_obj)
  {
    id = s_obj->id_;
    switch(id) /* give valgrind a glue, which object was created */
    {
      case 1: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 2: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 3: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 4: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 5: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 6: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 7: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 8: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 9: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 10: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 11: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 12: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 13: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 14: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 15: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 16: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 17: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 18: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 19: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 20: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 21: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 22: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 23: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 24: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 25: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 26: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 27: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 28: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 29: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 30: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 31: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 32: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 33: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 34: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 35: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 36: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 37: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 38: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      case 39: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_)); break;
      default: s = (oyCMMapi5_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi5_s_));
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

  error = !memset( s, 0, sizeof(oyCMMapi5_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyCMMapi5_Copy;
  s->release = (oyStruct_Release_f) oyCMMapi5_Release;

  s->oy_ = s_obj;

  
  
  /* ---- start of custom CMMapi constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CMM_API_S, (oyPointer)s );
  /* ---- end of custom CMMapi constructor ------- */
  /* ---- start of custom CMMapi5 constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CMM_API5_S, (oyPointer)s );
  /* ---- end of custom CMMapi5 constructor ------- */
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  
  /* ---- start of custom CMMapi constructor ----- */
  error += oyCMMapi_Init__Members( (oyCMMapi_s_*)s );
  /* ---- end of custom CMMapi constructor ------- */
  /* ---- start of custom CMMapi5 constructor ----- */
  error += oyCMMapi5_Init__Members( s );
  /* ---- end of custom CMMapi5 constructor ------- */
  
  
  

  if(!oy_cmmapi5_init_)
  {
    oy_cmmapi5_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyCMMapi5_StaticMessageFunc_,
                                        oyCMMapi5_StaticFree_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyCMMapi5_Copy__
 *  @memberof oyCMMapi5_s_
 *  @brief   real copy a CMMapi5 object
 *
 *  @param[in]     cmmapi5                 CMMapi5 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapi5_s_ * oyCMMapi5_Copy__ ( oyCMMapi5_s_ *cmmapi5, oyObject_s object )
{
  oyCMMapi5_s_ *s = 0;
  int error = 0;

  if(!cmmapi5 || !object)
    return s;

  s = (oyCMMapi5_s_*) oyCMMapi5_New( object );
  error = !s;

  if(!error) {
    
    
    /* ---- start of custom CMMapi copy constructor ----- */
    error = oyCMMapi_Copy__Members( (oyCMMapi_s_*)s, (oyCMMapi_s_*)cmmapi5 );
    /* ---- end of custom CMMapi copy constructor ------- */
    /* ---- start of custom CMMapi5 copy constructor ----- */
    error = oyCMMapi5_Copy__Members( s, cmmapi5 );
    /* ---- end of custom CMMapi5 copy constructor ------- */
    
    
    
    
    
  }

  if(error)
    oyCMMapi5_Release_( &s );

  return s;
}

/** @internal
 *  Function oyCMMapi5_Copy_
 *  @memberof oyCMMapi5_s_
 *  @brief   copy or reference a CMMapi5 object
 *
 *  @param[in]     cmmapi5                 CMMapi5 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapi5_s_ * oyCMMapi5_Copy_ ( oyCMMapi5_s_ *cmmapi5, oyObject_s object )
{
  oyCMMapi5_s_ * s = cmmapi5;

  if(!cmmapi5)
    return 0;

  if(cmmapi5 && !object)
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
        int n = oyStruct_GetParents( (oyStruct_s*)s, &parents );
        {
          int i;
          const char * track_name = oyStructTypeToText(s->type_);
          fprintf( stderr, "%s[%d] tracking refs: %d parents: %d\n",
                   (s->oy_->id_ == id_)?oyjlTermColor(oyjlGREEN, track_name):track_name, s->oy_->id_, s->oy_->ref_, n );
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

  s = oyCMMapi5_Copy__( cmmapi5, object );

  return s;
}
 
/** @internal
 *  Function oyCMMapi5_Release_
 *  @memberof oyCMMapi5_s_
 *  @brief   release and possibly deallocate a CMMapi5 object
 *
 *  @param[in,out] cmmapi5                 CMMapi5 struct object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/10/23
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyCMMapi5_Release_( oyCMMapi5_s_ **cmmapi5 )
{
  const char * track_name = NULL;
  int observer_refs = 0, id = 0, refs = 0, parent_refs = 0;
  /* ---- start of common object destructor ----- */
  oyCMMapi5_s_ *s = 0;

  if(!cmmapi5 || !*cmmapi5)
    return 0;

  s = *cmmapi5;
  /* static object */
  if(!s->oy_)
    return 0;

  id = s->oy_->id_;
  refs = s->oy_->ref_;

  if(refs <= 0) /* avoid circular or double dereferencing */
    return 0;

  *cmmapi5 = 0;

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
        fprintf( stderr, "%s[%d] unref with refs: %d observers: %d parents: %d\n",
                 (s->oy_->id_ == id_)?oyjlTermColor(oyjlRED, track_name):track_name, s->oy_->id_, s->oy_->ref_, observer_refs, n );
        for(i = 0; i < n; ++i)
        {
          track_name = oyStructTypeToText(parents[i]->type_);
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
      fprintf( stderr, "%s[%d] destruct\n", (s->oy_->id_ == id_)?oyjlTermColor(oyjlRED, track_name):track_name, s->oy_->id_);
    }
  }

  refs = s->oy_->ref_;
  if(refs < 0)
  {
    WARNc2_S( "oyCMMapi5_s[%d]->object can not be untracked with refs: %d\n", id, refs );
    //oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ "refs:%d", OY_DBG_ARGS_, refs);
    return -1; /* issue */
  }

  
  
  /* ---- start of custom CMMapi destructor ----- */
  oyCMMapi_Release__Members( (oyCMMapi_s_*)s );
  /* ---- end of custom CMMapi destructor ------- */
  /* ---- start of custom CMMapi5 destructor ----- */
  oyCMMapi5_Release__Members( s );
  /* ---- end of custom CMMapi5 destructor ------- */
  
  
  



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
      fprintf( stderr, "!!!ERROR:%d oyCMMapi5_s[%d]->object can not be untracked with refs: %d\n", __LINE__, id, refs);

    s->oy_ = NULL;
    oyObject_Release( &oy );
    if(track_name)
      fprintf( stderr, "%s[%d] destructed\n", (id == oy_debug_objects)?oyjlTermColor(oyjlRED, track_name):track_name, id );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "CMMapi5.private_methods_definitions.c" { */

/* } Include "CMMapi5.private_methods_definitions.c" */

