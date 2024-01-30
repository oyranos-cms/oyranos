/** @file oyOptions_s_.c

   [Template file inheritance graph]
   +-> oyOptions_s_.template.c
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



  
#include "oyOptions_s.h"
#include "oyOptions_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"


  

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif


static int oy_options_init_ = 0;
static char * oy_options_msg_text_ = NULL;
static int oy_options_msg_text_n_ = 0;
static const char * oyOptions_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyOptions_s_ * s = (oyOptions_s_*) obj;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if( oy_options_msg_text_ == NULL || oy_options_msg_text_n_ == 0 )
  {
    oy_options_msg_text_n_ = 512;
    oy_options_msg_text_ = (char*) alloc( oy_options_msg_text_n_ );
    if(oy_options_msg_text_)
      memset( oy_options_msg_text_, 0, oy_options_msg_text_n_ );
  }

  if( oy_options_msg_text_ == NULL || oy_options_msg_text_n_ == 0 )
    return "Memory problem";

  oy_options_msg_text_[0] = '\000';

  if(!(flags & 0x01))
    sprintf(oy_options_msg_text_, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  
  

  return oy_options_msg_text_;
}

static void oyOptions_StaticFree_           ( void )
{
  if(oy_options_init_)
  {
    oy_options_init_ = 0;
    if(oy_options_msg_text_)
      oyFree_m_(oy_options_msg_text_);
    if(oy_debug)
      fprintf(stderr, "%s() freeing static \"%s\" memory\n", "oyOptions_StaticFree_", "oyOptions_s" );
  }
}


/* Include "Options.private_custom_definitions.c" { */
/** Function    oyOptions_Release__Members
 *  @memberof   oyOptions_s
 *  @brief      Custom Options destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  options  the Options object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyOptions_Release__Members( oyOptions_s_ * options )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &options->member );
   */

  if(options->oy_->deallocateFunc_)
  {
#if 0
    /* not used */
    oyDeAlloc_f deallocateFunc = options->oy_->deallocateFunc_;
#endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( options->member );
     */
  }
}

/** Function    oyOptions_Init__Members
 *  @memberof   oyOptions_s
 *  @brief      Custom Options constructor
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  options  the Options object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyOptions_Init__Members( oyOptions_s_ * options OY_UNUSED )
{
  return 0;
}

/** Function    oyOptions_Copy__Members
 *  @memberof   oyOptions_s
 *  @brief      Custom Options copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyOptions_s_ input object
 *  @param[out]  dst  the output oyOptions_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyOptions_Copy__Members( oyOptions_s_ * dst, oyOptions_s_ * src)
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

/* } Include "Options.private_custom_definitions.c" */


/** @internal
 *  Function oyOptions_New_
 *  @memberof oyOptions_s_
 *  @brief   allocate a new oyOptions_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyOptions_s_ * oyOptions_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_OPTIONS_S;
  int error = 0, id = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object, "oyOptions_s" );
  oyOptions_s_ * s = 0;

  if(s_obj)
  {
    id = s_obj->id_;
    switch(id) /* give valgrind a glue, which object was created */
    {
      case 1: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 2: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 3: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 4: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 5: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 6: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 7: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 8: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 9: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 10: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 11: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 12: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 13: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 14: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 15: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 16: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 17: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 18: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 19: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 20: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 21: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 22: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 23: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 24: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 25: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 26: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 27: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 28: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 29: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 30: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 31: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 32: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 33: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 34: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 35: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 36: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 37: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 38: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      case 39: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_)); break;
      default: s = (oyOptions_s_*)s_obj->allocateFunc_(sizeof(oyOptions_s_));
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

  error = !memset( s, 0, sizeof(oyOptions_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyOptions_Copy;
  s->release = (oyStruct_Release_f) oyOptions_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom Options constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_OPTIONS_S, (oyPointer)s );
  /* ---- end of custom Options constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  s->list_ = oyStructList_Create( s->type_, 0, 0 );


  
  /* ---- start of custom Options constructor ----- */
  error += oyOptions_Init__Members( s );
  /* ---- end of custom Options constructor ------- */
  
  
  
  

  if(!oy_options_init_)
  {
    oy_options_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyOptions_StaticMessageFunc_,
                                        oyOptions_StaticFree_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyOptions_Copy__
 *  @memberof oyOptions_s_
 *  @brief   real copy a Options object
 *
 *  @param[in]     options                 Options struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyOptions_s_ * oyOptions_Copy__ ( oyOptions_s_ *options, oyObject_s object )
{
  oyOptions_s_ *s = 0;
  int error = 0;

  if(!options || !object)
    return s;

  s = (oyOptions_s_*) oyOptions_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom Options copy constructor ----- */
    error = oyOptions_Copy__Members( s, options );
    /* ---- end of custom Options copy constructor ------- */
    
    
    
    
    
    oyStructList_Release( &s->list_ );
    s->list_ = oyStructList_Copy( options->list_, s->oy_ );

  }

  if(error)
    oyOptions_Release_( &s );

  return s;
}

/** @internal
 *  Function oyOptions_Copy_
 *  @memberof oyOptions_s_
 *  @brief   copy or reference a Options object
 *
 *  @param[in]     options                 Options struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyOptions_s_ * oyOptions_Copy_ ( oyOptions_s_ *options, oyObject_s object )
{
  oyOptions_s_ * s = options;

  if(!options)
    return 0;

  if(options && !object)
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

  s = oyOptions_Copy__( options, object );

  return s;
}
 
/** @internal
 *  Function oyOptions_Release_
 *  @memberof oyOptions_s_
 *  @brief   release and possibly deallocate a Options list
 *
 *  @param[in,out] options                 Options struct object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2019/10/23
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyOptions_Release_( oyOptions_s_ **options )
{
  const char * track_name = NULL;
  int observer_refs = 0, id = 0, refs = 0, parent_refs = 0;
  /* ---- start of common object destructor ----- */
  oyOptions_s_ *s = 0;

  if(!options || !*options)
    return 0;

  s = *options;
  /* static object */
  if(!s->oy_)
    return 0;

  id = s->oy_->id_;
  refs = s->oy_->ref_;

  if(refs <= 0) /* avoid circular or double dereferencing */
    return 0;

  *options = 0;

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
        fprintf( stderr, OY_DBG_FORMAT_ "%s[%d] unref with refs: %d-- observers: %d parents: %d\n", OY_DBG_ARGS_,
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
    WARNc2_S( "oyOptions_s[%d]->object can not be untracked with refs: %d\n", id, refs );
    //oyMessageFunc_p( oyMSG_WARN,0,OY_DBG_FORMAT_ "refs:%d", OY_DBG_ARGS_, refs);
    return -1; /* issue */
  }

  
  /* ---- start of custom Options destructor ----- */
  oyOptions_Release__Members( s );
  /* ---- end of custom Options destructor ------- */
  
  
  
  

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
      fprintf( stderr, "!!!ERROR:%d oyOptions_s[%d]->object can not be untracked with refs: %d\n", __LINE__, id, refs);

    s->oy_ = NULL;
    oyObject_Release( &oy );
    if(track_name)
      fprintf( stderr, "%s[%d] destructed\n", (id == oy_debug_objects)?oyjlTermColor(oyjlRED, track_name):track_name, id );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "Options.private_methods_definitions.c" { */
#ifdef HAVE_LIBXML2
/**
 *  @internal
 *  Function oyOptions_ParseXML_
 *  @memberof oyOptions_s
 *  @brief   deserialise a text file to oyOptions_s data
 *
 *  This function is parsing libxml2 structures.
 *
 *  @version Oyranos: 0.1.10
 *  @since   2008/11/17 (Oyranos: 0.1.9)
 *  @date    2009/09/01
 */
void           oyOptions_ParseXML_   ( oyOptions_s_      * s,
                                       char            *** texts,
                                       int               * texts_n,
                                       xmlDocPtr           doc,
                                       xmlNodePtr          cur )
{
  oyOption_s * o = 0;
  char * tmp = 0;
  int i;
  xmlChar *key = 0;

  while (cur != NULL)
  {
    if(cur->type == XML_ELEMENT_NODE)
      oyStringListAddStaticString( texts, texts_n, (const char*)cur->name,
                                    oyAllocateFunc_, oyDeAllocateFunc_ );

    if(cur->xmlChildrenNode)
    {
      oyOptions_ParseXML_( s, texts, texts_n, doc, cur->xmlChildrenNode );
      *texts_n -= 1;
      oyDeAllocateFunc_( (*texts)[*texts_n] );
    }

    if(cur->type == XML_TEXT_NODE && !cur->children &&
       cur->content && cur->content[0] &&
       cur->content[0] != '\n')
    {
      for( i = 0; i < *texts_n; ++i )
      {
        if(i)
          STRING_ADD( tmp, "/" );
        STRING_ADD( tmp, (*texts)[i] );
      }

      if(tmp)
        o = (oyOption_s*)oyOption_FromRegistration( tmp, 0 );

      if(!o)
        goto clean_stage;

      key = xmlNodeListGetString(doc, cur, 1);
      oyOption_SetFromString( o, (char*)key, 0 );
      xmlFree(key);

      oyOption_SetSource( o, oyOPTIONSOURCE_DATA );

      oyOptions_MoveIn( (oyOptions_s*)s, &o, -1 );

      clean_stage:
      if(tmp)
        oyFree_m_( tmp );
    }
    cur = cur->next;
  }
}
#endif

/* } Include "Options.private_methods_definitions.c" */

