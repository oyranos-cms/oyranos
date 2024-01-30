/** @file oyOption_s_.c

   [Template file inheritance graph]
   +-> oyOption_s_.template.c
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



  
#include "oyOption_s.h"
#include "oyOption_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"


  

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif


static int oy_option_init_ = 0;
static char * oy_option_msg_text_ = NULL;
static int oy_option_msg_text_n_ = 0;
static const char * oyOption_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyOption_s_ * s = (oyOption_s_*) obj;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if( oy_option_msg_text_ == NULL || oy_option_msg_text_n_ == 0 )
  {
    oy_option_msg_text_n_ = 512;
    oy_option_msg_text_ = (char*) alloc( oy_option_msg_text_n_ );
    if(oy_option_msg_text_)
      memset( oy_option_msg_text_, 0, oy_option_msg_text_n_ );
  }

  if( oy_option_msg_text_ == NULL || oy_option_msg_text_n_ == 0 )
    return "Memory problem";

  oy_option_msg_text_[0] = '\000';

  if(!(flags & 0x01))
    sprintf(oy_option_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  

  
  {
    const char * t = "";
    oyDeAlloc_f dealloc = oyDeAllocateFunc_;
#define AD alloc,dealloc
    if(!(flags & 0x01))
      oyStringAddPrintf( &oy_option_msg_text_, AD, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");


    if(type == oyNAME_NICK && (flags & 0x01))
    {
      t = oyOption_GetText((oyOption_s*)s,type);
      oyStringAddPrintf( &oy_option_msg_text_, AD, "%s",
             oyNoEmptyString_m_(t)
             );
    } else
    if(type == oyNAME_NAME)
    {
      t = oyOption_GetText((oyOption_s*)s, oyNAME_NICK);
      oyStringAddPrintf( &oy_option_msg_text_, AD, "id:%d %s",
             s->id, oyNoEmptyString_m_(t)
           );
    } else
    if((int)type >= oyNAME_DESCRIPTION)
    {
      t = oyOption_GetText((oyOption_s*)s, oyNAME_NICK);
      oyStringAddPrintf( &oy_option_msg_text_, AD, "%s\nid: %d type: %s source: %d flags: %d",
             oyNoEmptyString_m_(t),
             s->id,
             oyValueTypeText(s->value_type),
             s->source,
             s->flags
           );
    }
  }


  return oy_option_msg_text_;
}

static void oyOption_StaticFree_           ( void )
{
  if(oy_option_init_)
  {
    oy_option_init_ = 0;
    if(oy_option_msg_text_)
      oyFree_m_(oy_option_msg_text_);
    if(oy_debug)
      fprintf(stderr, "%s() freeing static \"%s\" memory\n", "oyOption_StaticFree_", "oyOption_s" );
  }
}


/* Include "Option.private_custom_definitions.c" { */
#include "oyStructList_s.h"

/** @internal
 *  Function    oyOption_Release__Members
 *  @memberof   oyOption_s
 *  @brief      Custom Option destructor
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  option  the Option object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyOption_Release__Members( oyOption_s_ * option )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &option->member );
   */

  option->id = 0;

  if(option->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = option->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( option->member );
     */

    if(option->registration)
      deallocateFunc(option->registration);
    option->registration = NULL;

    oyOption_Clear( (oyOption_s*) option );
  }
}

static int oy_option_id_ = 0;

/** @internal
 *  Function    oyOption_Init__Members
 *  @memberof   oyOption_s
 *  @brief      Custom Option constructor 
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  option  the Option object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyOption_Init__Members( oyOption_s_ * option )
{
  option->id = oy_option_id_++;
  option->registration = NULL;
  option->value = NULL;

  return 0;
}

/** @internal
 *  Function    oyOption_Copy__Members
 *  @memberof   oyOption_s
 *  @brief      Custom Option copy constructor
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyOption_s_ input object
 *  @param[out]  dst  the output oyOption_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyOption_Copy__Members( oyOption_s_ * dst, oyOption_s_ * src)
{
  oyAlloc_f allocateFunc_ = 0;
  oyDeAlloc_f deallocateFunc_ = 0;
  int error = 0;

  if(!dst || !src || dst == src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;
  deallocateFunc_ = dst->oy_->deallocateFunc_;

  /* Copy each value of src to dst here */

  if(! (dst->value_type == src->value_type &&
        oyValueEqual( dst->value, src->value, dst->value_type, -1 )) )
  {
    /* oyOption_Clear does normally signal emitting; block that. */
    oyStruct_DisableSignalSend( (oyStruct_s*)dst );
    error = oyOption_Clear( (oyOption_s*)dst );
    oyStruct_EnableSignalSend( (oyStruct_s*)dst );

    dst->value_type = src->value_type;
    dst->value = allocateFunc_(sizeof(oyValue_u));
    memset(dst->value, 0, sizeof(oyValue_u));
    oyValueCopy( dst->value, src->value, dst->value_type,
                 allocateFunc_, deallocateFunc_ );
  }

  if(dst->registration)
    deallocateFunc_(dst->registration);
  dst->registration = oyStringCopy_( src->registration, allocateFunc_ );
  dst->source = src->source;
  dst->flags = src->flags;
  oyStruct_ObserverSignal( (oyStruct_s*)dst, oySIGNAL_DATA_CHANGED, 0 );

  return error;
}

/* } Include "Option.private_custom_definitions.c" */


/** @internal
 *  Function oyOption_New_
 *  @memberof oyOption_s_
 *  @brief   allocate a new oyOption_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyOption_s_ * oyOption_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_OPTION_S;
  int error = 0, id = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object, "oyOption_s" );
  oyOption_s_ * s = 0;

  if(s_obj)
  {
    id = s_obj->id_;
    switch(id) /* give valgrind a glue, which object was created */
    {
      case 1: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 2: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 3: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 4: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 5: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 6: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 7: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 8: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 9: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 10: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 11: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 12: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 13: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 14: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 15: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 16: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 17: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 18: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 19: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 20: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 21: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 22: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 23: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 24: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 25: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 26: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 27: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 28: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 29: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 30: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 31: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 32: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 33: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 34: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 35: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 36: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 37: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 38: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      case 39: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_)); break;
      default: s = (oyOption_s_*)s_obj->allocateFunc_(sizeof(oyOption_s_));
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

  error = !memset( s, 0, sizeof(oyOption_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyOption_Copy;
  s->release = (oyStruct_Release_f) oyOption_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom Option constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_OPTION_S, (oyPointer)s );
  /* ---- end of custom Option constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom Option constructor ----- */
  error += oyOption_Init__Members( s );
  /* ---- end of custom Option constructor ------- */
  
  
  
  

  if(!oy_option_init_)
  {
    oy_option_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyOption_StaticMessageFunc_,
                                        oyOption_StaticFree_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyOption_Copy__
 *  @memberof oyOption_s_
 *  @brief   real copy a Option object
 *
 *  @param[in]     option                 Option struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyOption_s_ * oyOption_Copy__ ( oyOption_s_ *option, oyObject_s object )
{
  oyOption_s_ *s = 0;
  int error = 0;

  if(!option || !object)
    return s;

  s = (oyOption_s_*) oyOption_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom Option copy constructor ----- */
    error = oyOption_Copy__Members( s, option );
    /* ---- end of custom Option copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyOption_Release_( &s );

  return s;
}

/** @internal
 *  Function oyOption_Copy_
 *  @memberof oyOption_s_
 *  @brief   copy or reference a Option object
 *
 *  @param[in]     option                 Option struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyOption_s_ * oyOption_Copy_ ( oyOption_s_ *option, oyObject_s object )
{
  oyOption_s_ * s = option;

  if(!option)
    return 0;

  if(option && !object)
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
          fprintf( stderr, "\\ parent[%d]: %s[%d]\n", i,
                   track_name, parents[i]->oy_->id_ );
        }
      }
    }
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyOption_Copy__( option, object );

  return s;
}
 
/** @internal
 *  Function oyOption_Release_
 *  @memberof oyOption_s_
 *  @brief   release and possibly deallocate a Option object
 *
 *  @param[in,out] option                 Option struct object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/10/23
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyOption_Release_( oyOption_s_ **option )
{
  const char * track_name = NULL;
  int observer_refs = 0, id = 0, refs = 0, parent_refs = 0;
  /* ---- start of common object destructor ----- */
  oyOption_s_ *s = 0;

  if(!option || !*option)
    return 0;

  s = *option;
  /* static object */
  if(!s->oy_)
    return 0;

  id = s->oy_->id_;
  refs = s->oy_->ref_;

  if(refs <= 0) /* avoid circular or double dereferencing */
    return 0;

  *option = 0;

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
                 (s->oy_->id_ == id_)?oyjlTermColorF(oyjlRED, "%s", track_name):track_name, s->oy_->id_, s->oy_->ref_, observer_refs, n );
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
          fprintf( stderr, "\\ parent[%d]: %s[%d]\n", i,
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
    WARNc2_S( "oyOption_s[%d]->object can not be untracked with refs: %d\n", id, refs );
    //oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ "refs:%d", OY_DBG_ARGS_, refs);
    return -1; /* issue */
  }

  
  /* ---- start of custom Option destructor ----- */
  oyOption_Release__Members( s );
  /* ---- end of custom Option destructor ------- */
  
  
  
  



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
      fprintf( stderr, "!!!ERROR:%d oyOption_s[%d]->object can not be untracked with refs: %d\n", __LINE__, id, refs);

    s->oy_ = NULL;
    oyObject_Release( &oy );
    if(track_name)
      fprintf( stderr, "%s[%d] destructed\n", (id == oy_debug_objects)?oyjlTermColor(oyjlRED, track_name):track_name, id );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "Option.private_methods_definitions.c" { */
#include <locale.h>  /* setlocale */

#include "oyStructList_s_.h" /* not nice, but direct access is probably faster */
#include "oyranos_i18n.h"

/**
 *  @internal
 *  Function oyOption_Match_
 *  @memberof oyOption_s
 *  @brief   two option key name matches
 *
 *  A registration name match is not required.
 *
 *  @version Oyranos: 0.1.8
 *  @since   2008/06/28 (Oyranos: 0.1.8)
 *  @date    2008/06/28
 */
int            oyOption_Match_       ( oyOption_s_        * option_a,
                                       oyOption_s_        * option_b )
{
  int erg = 0;

  if( option_a && option_b )
  {
    char * a =  oyFilterRegistrationToText( option_a->registration,
                                            oyFILTER_REG_TYPE, 0 );
    char * b =  oyFilterRegistrationToText( option_b->registration,
                                            oyFILTER_REG_TYPE, 0 );

    if( oyStrcmp_( a, b ) == 0 )
      erg = 1;

    oyDeAllocateFunc_(a);
    oyDeAllocateFunc_(b);
  }

  return erg;
}

/**
 *  @internal
 *  Function oyOption_UpdateFlags_
 *  @memberof oyOption_s
 *  @brief   set the ::flags member
 *
 *  @date    2015/05/13
 *  @version Oyranos: 0.9.6
 *  @since   2008/11/27 (Oyranos: 0.1.9)
 */
void         oyOption_UpdateFlags_   ( oyOption_s_        * o )
{
  const char * tmp = 0;

  if(o && o->registration && oyStrrchr_( o->registration, '/' ))
  {
      tmp = oyStrrchr_( o->registration, '/' );
      if(oyStrstr_( tmp, "front" ))
         o->flags |= oyOPTIONATTRIBUTE_FRONT;
      if(oyStrstr_( tmp, "advanced" ))
         o->flags |= oyOPTIONATTRIBUTE_ADVANCED;
      tmp = 0;

      o->flags |= oyOPTIONATTRIBUTE_EDIT;
  }
}

/**
 *  @internal
 *  Function oyOption_GetId_
 *  @memberof oyOption_s
 *  @brief   get the identification number of a option
 *
 *  @version Oyranos: 0.1.8
 */
int            oyOption_GetId_       ( oyOption_s_       * obj )
{
  if(obj)
    return obj->id;

  return -1;
}

/**
 *  @internal
 *  Function oyOption_SetFromString_
 *  @memberof oyOption_s
 *  @brief   set a option value from a string
 *
 *  @param         obj                 the option
 *  @param         text                the text to set
 *  @param         flags               possible is OY_STRING_LIST
 *  @return                            0 - success, 1 - error
 *
 *  @version Oyranos: 0.1.9
 */
int            oyOption_SetFromString_(oyOption_s_       * obj,
                                       const char        * text,
                                       uint32_t            flags )
{
  int error = !obj;
  char ** list = 0;
  int n = 0;

  if(error <= 0)
  {
    /* ignore the special case of assigning the same string twice. */
    if(obj->value && obj->value_type == oyVAL_STRING &&
       obj->value->string == text)
      return error;

    if(obj->value)
    {
      oyDeAlloc_f deallocateFunc = obj->oy_->deallocateFunc_;

      if( text )
      {
        int j = 0;
        if( obj->value_type == oyVAL_STRING && obj->value->string )
        {
          if(oyStrcmp_(text, obj->value->string) == 0)
            return error;
        }
        if( obj->value_type == oyVAL_STRING_LIST && obj->value->string_list )
        while(obj->value->string_list[j])
        {
          const char * value = obj->value->string_list[j];

          if(value && oyStrcmp_(value, text))
            return error;
          ++j;
        }

      }
      oyValueRelease( &obj->value, obj->value_type, deallocateFunc );
    }

    obj->value = obj->oy_->allocateFunc_(sizeof(oyValue_u));
    memset( obj->value, 0, sizeof(oyValue_u) );

    if(oyToStringList_m(flags))
    {
      /** Split for flags & OY_STRING_LIST at newline. */
      list = oyStringSplit_( text, '\n', &n, obj->oy_->allocateFunc_ );
      obj->value->string_list = list; list = 0;
      obj->value_type = oyVAL_STRING_LIST;
    } else
    {
      if(text)
        obj->value->string = oyStringCopy_( text, obj->oy_->allocateFunc_ );
      obj->value_type = oyVAL_STRING;
    }
    obj->flags |= oyOPTIONATTRIBUTE_EDIT;
    oyStruct_ObserverSignal( (oyStruct_s*)obj, oySIGNAL_DATA_CHANGED, 0 );
  }

  return error;
}

/**
 *  @internal
 *  Function oyOption_GetValueText_
 *  @memberof oyOption_s
 *  @brief   get value as a text dump
 *
 *  @param         obj                 the option
 *  @param         allocateFunc        user allocator
 *  @return                            the text
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/12/05 (Oyranos: 0.1.9)
 *  @date    2009/08/17
 */
char *         oyOption_GetValueText_( oyOption_s_       * obj,
                                       oyAlloc_f           allocateFunc )
{
  int error = !obj;
  char * erg = 0;
  oyValue_u * v = 0;
  oyStructList_s * oy_struct_list = 0;
  char * text = 0;
  char * save_locale = 0;

  if(error <= 0)
    v = obj->value;

  if(!allocateFunc)
    allocateFunc = oyAllocateFunc_;

  error = !v;

  save_locale = oyStringCopy_( setlocale(LC_NUMERIC, 0 ), oyAllocateFunc_ );
  setlocale(LC_NUMERIC, "C");

  if(error <= 0)
  {
    int n = 1, i = 0;
    char * tmp = oyAllocateFunc_(1024);
    const char * ct = 0;

    switch(obj->value_type)
    {
    case oyVAL_INT_LIST:    n = v->int32_list[0]; break;
    case oyVAL_DOUBLE_LIST: n = (int)v->dbl_list[0]; break;
    case oyVAL_STRING_LIST: n = 0; while( v->string_list[n] ) ++n; break;
    case oyVAL_INT:
    case oyVAL_DOUBLE:
    case oyVAL_STRING:
    case oyVAL_STRUCT:
         n = 1; break;
    case oyVAL_NONE:
    case oyVAL_MAX:
         n = 0; break;
    }

    if(obj->value_type == oyVAL_STRUCT)
    {
      oy_struct_list = (oyStructList_s*) v->oy_struct;
      if(oy_struct_list)
      {
        if(oy_struct_list->type_ == oyOBJECT_STRUCT_LIST_S)
          n = oyStructList_Count( oy_struct_list );
      } else
        WARNc2_S( "missed \"oy_struct\" member of \"%s\" [%d]",
                  obj->registration, oyObject_GetId(obj->oy_) );
    }

    for(i = 0; i < n; ++i)
    {
      if(obj->value_type == oyVAL_INT)
        oySprintf_(tmp, "%d", v->int32);
      if(obj->value_type == oyVAL_DOUBLE)
        oySprintf_(tmp, "%g", v->dbl);
      if(obj->value_type == oyVAL_INT_LIST)
        oySprintf_(tmp, "%d", v->int32_list[i+1]);
      if(obj->value_type == oyVAL_DOUBLE_LIST)
        oySprintf_(tmp, "%g", v->dbl_list[i+1]);

      if((obj->value_type == oyVAL_INT_LIST ||
          obj->value_type == oyVAL_DOUBLE_LIST) && i)
        STRING_ADD( text, "," );

      switch(obj->value_type)
      {
      case oyVAL_INT:
      case oyVAL_DOUBLE:
      case oyVAL_INT_LIST:
      case oyVAL_DOUBLE_LIST: STRING_ADD( text, tmp ); break;
      case oyVAL_STRING:      STRING_ADD( text, v->string ); break;
      case oyVAL_STRING_LIST: STRING_ADD( text, v->string_list[i] ); break;
      case oyVAL_STRUCT:
      case oyVAL_NONE:
      case oyVAL_MAX:         break;
      }
      if(obj->value_type == oyVAL_STRUCT)
      {
        oyStruct_s * oy_struct = 0;

        if(oy_struct_list && oy_struct_list->type_ == oyOBJECT_STRUCT_LIST_S)
          oy_struct = oyStructList_Get_( ( oyStructList_s_*)oy_struct_list, i );
        else if(v->oy_struct)
          oy_struct = v->oy_struct;

        if(oy_struct)
        {
          ct = 0;
          /* get explicite name */
          if(oy_struct->oy_)
            ct = oyObject_GetName( oy_struct->oy_, oyNAME_NICK );
          if(!ct)
          /* fall back to oyCMMapi9_s object type lookup */
            ct = oyStruct_GetText( oy_struct, oyNAME_NICK, 0 );
          if(ct)
            STRING_ADD( text, ct );
          if(!ct)
          /* fall back to plain struct type name, if known */
            STRING_ADD ( text, oyStructTypeToText(oy_struct->type_) );
        }
      }
    }

    if(text)
      erg = oyStringCopy_( text, allocateFunc );

    oyFree_m_( tmp );
    if(!text)
    {
      DBG_NUM2_S( "missed value in \"%s\" [%d]", obj->registration,
                    oyObject_GetId(obj->oy_) );
    } else
      oyFree_m_( text );
  }
  setlocale(LC_NUMERIC, save_locale);
  oyFree_m_( save_locale );

  return erg;
}

/**
 *  @internal
 *  Function oyOption_SetFromInt_
 *  @memberof oyOption_s
 *  @brief   set a integer
 *
 *  @param[in,out] obj                 the option
 *  @param         integer             the value
 *  @param         pos                 position in a list
 *  @param         flags               unused
 *  @return                            0 - success, 1 - error
 *
 *  @version Oyranos: 0.1.10
 *  @since   2009/05/04 (Oyranos: 0.1.10)
 *  @date    2009/05/04
 */
int            oyOption_SetFromInt_  ( oyOption_s_       * obj,
                                       int32_t             integer,
                                       int                 pos,
                                       uint32_t            flags OY_UNUSED )
{
  int error = !obj;
  oyOption_s_ * s = obj;

  if(!obj)
    return error;

  if(error <= 0)
  {
    if(s->value && 0 /*flags & OY_CLEAR*/)
    {
      oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;

      if(s->value_type == oyVAL_INT && obj->value)
        if(s->value->int32 == integer)
          return error;

      if(s->value_type == oyVAL_INT_LIST && s->value)
        if(0 <= pos && pos < s->value->int32_list[0] &&
           s->value->int32_list[1 + pos] == integer)
          return error;

      oyValueRelease( &s->value, s->value_type, deallocateFunc );
    }

    if(!s->value)
    {
      oyStruct_AllocHelper_m_( s->value, oyValue_u, 1,
                               s, error = 1 );
      if(pos == 0 &&
         s->value_type != oyVAL_INT_LIST)
        s->value_type = oyVAL_INT;
      else
        s->value_type = oyVAL_INT_LIST;
    }

    if(!error && pos > 0 &&
       (s->value_type != oyVAL_INT_LIST ||
        (s->value_type == oyVAL_INT_LIST &&
         (!s->value->int32_list || pos >= s->value->int32_list[0]))))
    {
      int32_t * old_list = 0,
                old_int = 0;

      if(s->value_type == oyVAL_INT_LIST)
        old_list = s->value->int32_list;
      if(s->value_type == oyVAL_INT)
        old_int = s->value->int32;

      s->value->int32_list = 0;
      oyStruct_AllocHelper_m_( s->value->int32_list, int32_t, pos + 2,
                               s, error = 1 );

      if(!error && old_list)
      {
        memcpy( s->value->int32_list, old_list,
                (old_list[0] + 1) * sizeof(uint32_t) );
        s->oy_->deallocateFunc_( old_list ); old_list = 0;
      }

      if(!error && old_int)
        s->value->int32_list[1] = old_int;

      s->value_type = oyVAL_INT_LIST;
      s->value->int32_list[0] = pos + 1;
    }

    if(s->value_type == oyVAL_INT)
      s->value->int32 = integer;
    else
      s->value->int32_list[pos+1] = integer;

    s->flags |= oyOPTIONATTRIBUTE_EDIT;
    oyStruct_ObserverSignal( (oyStruct_s*)obj, oySIGNAL_DATA_CHANGED, 0 );
  }

  return error;
}

/**
 *  @internal
 *  Function oyOption_MoveInStruct_
 *  @memberof oyOption_s
 *  @brief   value filled by a oyStruct_s object
 *
 *  @param         option              the option
 *  @param         s                   the Oyranos style object
 *  @return                            error
 *
 *  @version Oyranos: 0.9.5
 *  @date    2013/08/13
 *  @since   2009/01/28 (Oyranos: 0.1.10)
 */
int            oyOption_MoveInStruct_( oyOption_s_       * option,
                                       oyStruct_s       ** s )
{
  int error = !option;

  if(error <= 0)
  {
    if(option->value)
    {
      oyDeAlloc_f deallocateFunc = option->oy_->deallocateFunc_;

      oyValueRelease( &option->value, option->value_type, deallocateFunc );

      option->value_type = 0;
    }

    oyStruct_AllocHelper_m_( option->value, oyValue_u, 1, option, error = 1 );
  }

  if(error <= 0)
  {
    option->value->oy_struct = *s;
    *s = 0;
    option->value_type = oyVAL_STRUCT;
  }

  return error;
}

/* } Include "Option.private_methods_definitions.c" */

