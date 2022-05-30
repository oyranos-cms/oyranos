/** @file oyFilterCore_s_.c

   [Template file inheritance graph]
   +-> oyFilterCore_s_.template.c
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



  
#include "oyFilterCore_s.h"
#include "oyFilterCore_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"




#include "oyranos_module_internal.h"
#include "oyCMMapiFilters_s.h"
#include "oyCMMapi9_s_.h"
  

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif


static int oy_filtercore_init_ = 0;
static char * oy_filtercore_msg_text_ = NULL;
static int oy_filtercore_msg_text_n_ = 0;
static const char * oyFilterCore_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyFilterCore_s_ * s = (oyFilterCore_s_*) obj;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if( oy_filtercore_msg_text_ == NULL || oy_filtercore_msg_text_n_ == 0 )
  {
    oy_filtercore_msg_text_n_ = 512;
    oy_filtercore_msg_text_ = (char*) alloc( oy_filtercore_msg_text_n_ );
    if(oy_filtercore_msg_text_)
      memset( oy_filtercore_msg_text_, 0, oy_filtercore_msg_text_n_ );
  }

  if( oy_filtercore_msg_text_ == NULL || oy_filtercore_msg_text_n_ == 0 )
    return "Memory problem";

  oy_filtercore_msg_text_[0] = '\000';

  if(!(flags & 0x01))
    sprintf(oy_filtercore_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  

  
  if(type == oyNAME_NICK && (flags & 0x01) && s->category_)
  {
    sprintf( &oy_filtercore_msg_text_[strlen(oy_filtercore_msg_text_)], "%s",
             s->category_
           );
  } else
  if(type == oyNAME_NAME && (s->category_ || s->registration_))
    sprintf( &oy_filtercore_msg_text_[strlen(oy_filtercore_msg_text_)], "%s %s",
             s->category_?s->category_:"", s->registration_?s->registration_:""
           );
  else
  if((int)type >= oyNAME_DESCRIPTION && (s->category_ || s->registration_))
    sprintf( &oy_filtercore_msg_text_[strlen(oy_filtercore_msg_text_)], "category: %s\nreg: %s",
             s->category_?s->category_:"", s->registration_?s->registration_:""
           );


  return oy_filtercore_msg_text_;
}

static void oyFilterCore_StaticFree_           ( void )
{
  if(oy_filtercore_init_)
  {
    oy_filtercore_init_ = 0;
    if(oy_filtercore_msg_text_)
      oyFree_m_(oy_filtercore_msg_text_);
    if(oy_debug)
      fprintf(stderr, "%s() freeing static \"%s\" memory\n", "oyFilterCore_StaticFree_", "oyFilterCore_s" );
  }
}


/* Include "FilterCore.private_custom_definitions.c" { */
/** Function    oyFilterCore_Release__Members
 *  @memberof   oyFilterCore_s
 *  @brief      Custom FilterCore destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  filtercore  the FilterCore object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyFilterCore_Release__Members( oyFilterCore_s_ * filtercore )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &filtercore->member );
   */
  oyOptions_Release( &filtercore->options_ );

  if(filtercore->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = filtercore->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( filtercore->member );
     */
    if(filtercore->registration_)
      deallocateFunc( filtercore->registration_ );

    if(filtercore->category_) {
      deallocateFunc( filtercore->category_ );
      filtercore->category_ = 0;
    }

    if(filtercore->api4_ && filtercore->api4_->release)
      filtercore->api4_->release( (oyStruct_s**)&filtercore->api4_ );
  }
}

/** Function    oyFilterCore_Init__Members
 *  @memberof   oyFilterCore_s
 *  @brief      Custom FilterCore constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  filtercore  the FilterCore object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyFilterCore_Init__Members( oyFilterCore_s_ * filtercore OY_UNUSED )
{
  /* No members seem to need initialization for now */
  return 0;
}

/** Function    oyFilterCore_Copy__Members
 *  @memberof   oyFilterCore_s
 *  @brief      Custom FilterCore copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyFilterCore_s_ input object
 *  @param[out]  dst  the output oyFilterCore_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyFilterCore_Copy__Members( oyFilterCore_s_ * dst, oyFilterCore_s_ * src)
{
  int error = 0;
  oyAlloc_f allocateFunc_ = 0;
#if 0
  oyDeAlloc_f deallocateFunc_ = 0;
#endif

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;
#if 0
  deallocateFunc_ = dst->oy_->deallocateFunc_;
#endif

  /* Copy each value of src to dst here */
  dst->registration_ = oyStringCopy_( src->registration_, allocateFunc_ );
  dst->category_ = oyStringCopy_( src->category_, allocateFunc_ );
  dst->options_ = oyOptions_Copy( src->options_, dst->oy_ );
  if(src->api4_ && src->api4_->copy)
    dst->api4_ = (oyCMMapi4_s_*) src->api4_->copy( (oyStruct_s*)src->api4_, NULL );
  else
    dst->api4_ = src->api4_;

  return error;
}

/* } Include "FilterCore.private_custom_definitions.c" */


/** @internal
 *  Function oyFilterCore_New_
 *  @memberof oyFilterCore_s_
 *  @brief   allocate a new oyFilterCore_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyFilterCore_s_ * oyFilterCore_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_FILTER_CORE_S;
  int error = 0, id = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object, "oyFilterCore_s" );
  oyFilterCore_s_ * s = 0;

  if(s_obj)
  {
    id = s_obj->id_;
    switch(id) /* give valgrind a glue, which object was created */
    {
      case 1: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 2: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 3: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 4: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 5: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 6: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 7: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 8: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 9: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 10: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 11: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 12: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 13: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 14: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 15: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 16: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 17: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 18: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 19: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 20: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 21: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 22: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 23: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 24: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 25: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 26: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 27: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 28: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 29: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 30: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 31: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 32: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 33: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 34: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 35: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 36: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 37: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 38: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      case 39: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_)); break;
      default: s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_));
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

  error = !memset( s, 0, sizeof(oyFilterCore_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyFilterCore_Copy;
  s->release = (oyStruct_Release_f) oyFilterCore_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom FilterCore constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_FILTER_CORE_S, (oyPointer)s );
  /* ---- end of custom FilterCore constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom FilterCore constructor ----- */
  error += oyFilterCore_Init__Members( s );
  /* ---- end of custom FilterCore constructor ------- */
  
  
  
  

  if(!oy_filtercore_init_)
  {
    oy_filtercore_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyFilterCore_StaticMessageFunc_,
                                        oyFilterCore_StaticFree_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyFilterCore_Copy__
 *  @memberof oyFilterCore_s_
 *  @brief   real copy a FilterCore object
 *
 *  @param[in]     filtercore                 FilterCore struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyFilterCore_s_ * oyFilterCore_Copy__ ( oyFilterCore_s_ *filtercore, oyObject_s object )
{
  oyFilterCore_s_ *s = 0;
  int error = 0;

  if(!filtercore || !object)
    return s;

  s = (oyFilterCore_s_*) oyFilterCore_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom FilterCore copy constructor ----- */
    error = oyFilterCore_Copy__Members( s, filtercore );
    /* ---- end of custom FilterCore copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyFilterCore_Release_( &s );

  return s;
}

/** @internal
 *  Function oyFilterCore_Copy_
 *  @memberof oyFilterCore_s_
 *  @brief   copy or reference a FilterCore object
 *
 *  @param[in]     filtercore                 FilterCore struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyFilterCore_s_ * oyFilterCore_Copy_ ( oyFilterCore_s_ *filtercore, oyObject_s object )
{
  oyFilterCore_s_ * s = filtercore;

  if(!filtercore)
    return 0;

  if(filtercore && !object)
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

  s = oyFilterCore_Copy__( filtercore, object );

  return s;
}
 
/** @internal
 *  Function oyFilterCore_Release_
 *  @memberof oyFilterCore_s_
 *  @brief   release and possibly deallocate a FilterCore object
 *
 *  @param[in,out] filtercore                 FilterCore struct object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/10/23
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyFilterCore_Release_( oyFilterCore_s_ **filtercore )
{
  const char * track_name = NULL;
  int observer_refs = 0, id = 0, refs = 0, parent_refs = 0;
  /* ---- start of common object destructor ----- */
  oyFilterCore_s_ *s = 0;

  if(!filtercore || !*filtercore)
    return 0;

  s = *filtercore;
  /* static object */
  if(!s->oy_)
    return 0;

  id = s->oy_->id_;
  refs = s->oy_->ref_;

  if(refs <= 0) /* avoid circular or double dereferencing */
    return 0;

  *filtercore = 0;

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
    WARNc2_S( "oyFilterCore_s[%d]->object can not be untracked with refs: %d\n", id, refs );
    //oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ "refs:%d", OY_DBG_ARGS_, refs);
    return -1; /* issue */
  }

  
  /* ---- start of custom FilterCore destructor ----- */
  oyFilterCore_Release__Members( s );
  /* ---- end of custom FilterCore destructor ------- */
  
  
  
  



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
      fprintf( stderr, "!!!ERROR:%d oyFilterCore_s[%d]->object can not be untracked with refs: %d\n", __LINE__, id, refs);

    s->oy_ = NULL;
    oyObject_Release( &oy );
    if(track_name)
      fprintf( stderr, "%s[%d] destructed\n", (id == oy_debug_objects)?oyjlTermColor(oyjlRED, track_name):track_name, id );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "FilterCore.private_methods_definitions.c" { */
/** Function  oyFilterCore_SetCMMapi4_
 *  @memberof oyFilterCore_s
 *  @brief    Lookup and initialise a new filter object
 *  @internal
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/11/27 (Oyranos: 0.1.9)
 *  @date    2009/02/28
 */
int          oyFilterCore_SetCMMapi4_( oyFilterCore_s_   * s,
                                       oyCMMapi4_s_      * cmm_api4 )
{
  int error = !s;
  oyAlloc_f allocateFunc_ = 0;
#if 0
  static const char * lang = 0;
  int update = 1;
#endif

  if(error <= 0)
    allocateFunc_ = s->oy_->allocateFunc_;

  if(error <= 0)
    error = !(cmm_api4 && cmm_api4->type_ == oyOBJECT_CMM_API4_S);

  if(error <= 0)
  {
    s->registration_ = oyStringCopy_( cmm_api4->registration,
                                      allocateFunc_);

    s->category_ = oyStringCopy_( cmm_api4->ui->category, allocateFunc_ );

    /* we lock here as cmm_api4->oyCMMuiGet might not be thread save */
    {
#if 0
      if(!lang)
        lang = oyLanguage();

      oyObject_Lock( s->oy_, __FILE__, __LINE__ );
      if(lang &&
         oyStrcmp_( oyNoEmptyName_m_(oyLanguage()), lang ) == 0)
        update = 0;

      oyObject_UnLock( s->oy_, __FILE__, __LINE__ );
#endif
    }

    s->api4_ = cmm_api4;
  }

  if(error && s)
    oyFilterCore_Release_( &s );

  return error;
}

/* } Include "FilterCore.private_methods_definitions.c" */

