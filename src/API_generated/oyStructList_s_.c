/** @file oyStructList_s_.c

   [Template file inheritance graph]
   +-> oyStructList_s_.template.c
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



  
#include "oyStructList_s.h"
#include "oyStructList_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"

#include "oyranos_generic_internal.h"
  

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif


static int oy_structlist_init_ = 0;
static const char * oyStructList_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyStructList_s_ * s = (oyStructList_s_*) obj;
  static char * text = 0;
  static int text_n = 0;
  oyAlloc_f alloc = oyAllocateFunc_;

  /* silently fail */
  if(!s)
   return "";

  if(s->oy_ && s->oy_->allocateFunc_)
    alloc = s->oy_->allocateFunc_;

  if( text == NULL || text_n == 0 )
  {
    text_n = 512;
    text = (char*) alloc( text_n );
    if(text)
      memset( text, 0, text_n );
  }

  if( text == NULL || text_n == 0 )
    return "Memory problem";

  text[0] = '\000';

  if(!(flags & 0x01))
    sprintf(text, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");

  

  
  /* allocate enough space */
  if(text_n < 1000)
  {
    oyDeAlloc_f dealloc = oyDeAllocateFunc_;
    if(s->oy_ && s->oy_->deallocateFunc_)
      dealloc = s->oy_->deallocateFunc_;
    if(text && text_n)
      dealloc( text );
    text_n = 1024;
    text = alloc(text_n);
    if(text)
      text[0] = '\000';
    else
      return "Memory Error";

    if(!(flags & 0x01))
      sprintf(text, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");
  }

  if(type == oyNAME_NICK && (flags & 0x01))
    sprintf( &text[strlen(text)], "%s%d",
             s->n_?"n: ":"", s->n_
           );
  else
  if(type == oyNAME_NAME)
    sprintf( &text[strlen(text)], "%s%s%s%d",
             s->list_name?s->list_name:"", s->list_name?"\n":"",
             s->n_?" n: ":"", s->n_
           );
  else
  if((int)type >= oyNAME_DESCRIPTION)
    sprintf( &text[strlen(text)], "%s%s%s%s%s%d",
             s->list_name?s->list_name:"", s->list_name?"\n":"",
             s->parent_type_?"parent: ":"", s->parent_type_?oyStructTypeToText(s->parent_type_):"",
             " n: ", s->n_
           );


  return text;
}


/* Include "StructList.private_custom_definitions.c" { */
/** @internal
 *  Function    oyStructList_Release__Members
 *  @memberof   oyStructList_s
 *  @brief      Custom StructList destructor
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  structlist  the StructList object
 */
void oyStructList_Release__Members( oyStructList_s_ * structlist )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &structlist->member );
   */

  oyStructList_Clear((oyStructList_s*)structlist);

  if(structlist->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = structlist->oy_->deallocateFunc_;

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( structlist->member );
     */
    if(structlist->ptr_)
      deallocateFunc(structlist->ptr_);

    if(structlist->list_name)
      deallocateFunc(structlist->list_name);

    memset( structlist, 0, sizeof(oyOBJECT_e) );
  }
}

/** @internal
 *  Function    oyStructList_Init__Members
 *  @memberof   oyStructList_s
 *  @brief      Custom StructList constructor 
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  structlist  the StructList object
 */
int oyStructList_Init__Members( oyStructList_s_ * structlist OY_UNUSED )
{
  return 0;
}

/** @internal
 *  Function    oyStructList_Copy__Members
 *  @memberof   oyStructList_s
 *  @brief      Custom StructList copy constructor
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyStructList_s_ input object
 *  @param[out]  dst  the output oyStructList_s_ object
 */
int oyStructList_Copy__Members( oyStructList_s_ * dst, oyStructList_s_ * src)
{
  int error = 0, i;
  oyAlloc_f allocateFunc_ = 0;

  if(!dst || !src)
    return 1;

  allocateFunc_ = dst->oy_->allocateFunc_;

  /* Copy each value of src to dst here */
  dst->parent_type_ = src->parent_type_;
  if(src->list_name)
    dst->list_name = oyStringAppend_(0, src->list_name, allocateFunc_);

  oyObject_Lock( dst->oy_, __FILE__, __LINE__ );

  dst->n_reserved_ = (src->n_ > 10) ? (int)(src->n_ * 1.5) : 10;
  dst->n_ = src->n_;
  dst->ptr_ = oyStruct_Allocate( (oyStruct_s*) dst, sizeof(int*) * dst->n_reserved_ );
  memset( dst->ptr_, 0, sizeof(int*) * dst->n_reserved_ );

  for(i = 0; i < src->n_; ++i)
    if(src->ptr_[i]->copy)
      dst->ptr_[i] = src->ptr_[i]->copy( src->ptr_[i], 0 );

  if(oyStruct_IsObserved( (oyStruct_s*)dst, 0) )
    error = oyStructList_ObserverAdd( (oyStructList_s*)src, 0, 0, 0 );

  oyObject_UnLock( dst->oy_, __FILE__, __LINE__ );

  return error;
}

/* } Include "StructList.private_custom_definitions.c" */


/** @internal
 *  Function oyStructList_New_
 *  @memberof oyStructList_s_
 *  @brief   allocate a new oyStructList_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyStructList_s_ * oyStructList_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_STRUCT_LIST_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyStructList_s_ * s = 0;

  if(s_obj)
    s = (oyStructList_s_*)s_obj->allocateFunc_(sizeof(oyStructList_s_));
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

  error = !memset( s, 0, sizeof(oyStructList_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyStructList_Copy;
  s->release = (oyStruct_Release_f) oyStructList_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom StructList constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_STRUCT_LIST_S, (oyPointer)s );
  /* ---- end of custom StructList constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom StructList constructor ----- */
  error += oyStructList_Init__Members( s );
  /* ---- end of custom StructList constructor ------- */
  
  
  
  

  if(!oy_structlist_init_)
  {
    oy_structlist_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyStructList_StaticMessageFunc_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyStructList_Copy__
 *  @memberof oyStructList_s_
 *  @brief   real copy a StructList object
 *
 *  @param[in]     structlist                 StructList struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyStructList_s_ * oyStructList_Copy__ ( oyStructList_s_ *structlist, oyObject_s object )
{
  oyStructList_s_ *s = 0;
  int error = 0;

  if(!structlist || !object)
    return s;

  s = (oyStructList_s_*) oyStructList_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom StructList copy constructor ----- */
    error = oyStructList_Copy__Members( s, structlist );
    /* ---- end of custom StructList copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyStructList_Release_( &s );

  return s;
}

/** @internal
 *  Function oyStructList_Copy_
 *  @memberof oyStructList_s_
 *  @brief   copy or reference a StructList object
 *
 *  @param[in]     structlist                 StructList struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyStructList_s_ * oyStructList_Copy_ ( oyStructList_s_ *structlist, oyObject_s object )
{
  oyStructList_s_ *s = structlist;

  if(!structlist)
    return 0;

  if(structlist && !object)
  {
    s = structlist;
    
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
        if(n != s->oy_->ref_)
        {
          int i;
          const char * track_name = oyStructTypeToText(s->type_);
#ifdef HAVE_BACKTRACE
          int j, nptrs;
          void *buffer[BT_BUF_SIZE];
          char **strings;

          nptrs = backtrace(buffer, BT_BUF_SIZE);

          /* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
             would produce similar output to the following: */

          strings = backtrace_symbols(buffer, nptrs);
          if( strings == NULL )
          {
            perror("backtrace_symbols");
          } else
          {
            int start = nptrs-1;
            do { --start; } while( start >= 0 && (strstr(strings[start], "(main+") == NULL) );
            fprintf(stderr, "\n");
            for(j = start; j >= 0; j--)
            {
              if(oy_debug)
                fprintf(stderr, "%s\n", strings[j]);
              else
              {
                char * t = NULL, * txt = NULL;
                const char * line = strings[j],
                           * tmp = strchr( line, '(' );
                if(tmp) t = oyStringCopy( &tmp[1], NULL );
                else t = oyStringCopy( line, NULL );
                txt = strchr( t, '+' );
                if(txt) txt[0] = '\000';
                if(j > 0 && (strstr(strings[j-1], t) != NULL) )
                  oyFree_m_(t);
                if(t)
                {
                  if(j==0)
                    fprintf(stderr, "%s() ", t);
                  else
                    fprintf(stderr, "%s()->", t);
                  oyFree_m_(t);
                }
              }
            }
            free(strings);
          }
#endif
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

  s = oyStructList_Copy__( structlist, object );

  return s;
}
 
/** @internal
 *  Function oyStructList_Release_
 *  @memberof oyStructList_s_
 *  @brief   release and possibly deallocate a StructList object
 *
 *  @param[in,out] structlist                 StructList struct object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/10/03
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyStructList_Release_( oyStructList_s_ **structlist )
{
  const char * track_name = NULL;
  int observer_refs = 0, i;
  /* ---- start of common object destructor ----- */
  oyStructList_s_ *s = 0;

  if(!structlist || !*structlist)
    return 0;

  s = *structlist;
  /* static object */
  if(!s->oy_)
    return 0;

  *structlist = 0;

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
      if(n != s->oy_->ref_)
      {
        int i;
#ifdef HAVE_BACKTRACE
          int j, nptrs;
          void *buffer[BT_BUF_SIZE];
          char **strings;

          nptrs = backtrace(buffer, BT_BUF_SIZE);

          /* The call backtrace_symbols_fd(buffer, nptrs, STDOUT_FILENO)
             would produce similar output to the following: */

          strings = backtrace_symbols(buffer, nptrs);
          if( strings == NULL )
          {
            perror("backtrace_symbols");
          } else
          {
            int start = nptrs-1;
            do { --start; } while( start >= 0 && (strstr(strings[start], "(main+") == NULL) );
            fprintf(stderr, "\n");
            for(j = start; j >= 0; j--)
            {
              if(oy_debug)
                fprintf(stderr, "%s\n", strings[j]);
              else
              {
                char * t = NULL, * txt = NULL;
                const char * line = strings[j],
                           * tmp = strchr( line, '(' );
                if(tmp) t = oyStringCopy( &tmp[1], NULL );
                else t = oyStringCopy( line, NULL );
                txt = strchr( t, '+' );
                if(txt) txt[0] = '\000';
                if(j > 0 && (strstr(strings[j-1], t) != NULL) )
                  oyFree_m_(t);
                if(t)
                {
                  if(j==0)
                    fprintf(stderr, "%s() ", t);
                  else
                    fprintf(stderr, "%s()->", t);
                  oyFree_m_(t);
                }
              }
            }
            free(strings);
          }
#endif
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

  
  if((oyObject_UnRef(s->oy_) - observer_refs) > 0)
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

  
  /* ---- start of custom StructList destructor ----- */
  oyStructList_Release__Members( s );
  /* ---- end of custom StructList destructor ------- */
  
  
  
  



  /* model and observer reference each other. So release the object two times.
   * The models and and observers are released later inside the
   * oyObject_s::handles. */
  for(i = 0; i < observer_refs; ++i)
  {
    oyObject_UnRef(s->oy_);
    oyObject_UnRef(s->oy_);
  }

  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;
    int id = s->oy_->id_;
    int refs = s->oy_->ref_;

    if(refs > 1)
      fprintf( stderr, "!!!ERROR: node[%d]->object can not be untracked with refs: %d\n", id, refs);

    oyObject_Release( &s->oy_ );
    if(track_name)
      fprintf( stderr, "%s[%d] destructed\n", track_name, id );

    deallocateFunc( s );
  }

  return 0;
}



/* Include "StructList.private_methods_definitions.c" { */
/** Function  oyStructList_Get_
 *  @memberof oyStructList_s
 *  @brief    oyStructList_s pointer access
 *  @internal
 *
 *  non thread save
 *
 *  @since Oyranos: version 0.1.8
 *  @date  21 november 2007 (API 0.1.8)
 */
oyStruct_s *     oyStructList_Get_   ( oyStructList_s_   * list,
                                       int                 pos )
{
  int n = 0;
  oyStructList_s_ * s = list;
  int error = !s;
  oyStruct_s * obj = 0;

  if(error <= 0)
    if(s->type_ != oyOBJECT_STRUCT_LIST_S)
      error = 1;

  if(error <= 0)
    n = s->n_;

  if(error <= 0)
    if(pos >= 0 && n > pos && s->ptr_[pos])
      obj = s->ptr_[pos];

  return obj;
}

/** Function  oyStructList_ReferenceAt_
 *  @memberof oyStructList_s
 *  @brief    oyStructList_s pointer referencing
 *  @internal
 *
 *  @since Oyranos: version 0.1.8
 *  @date  23 november 2007 (API 0.1.8)
 */
int              oyStructList_ReferenceAt_(oyStructList_s_ * list,
                                           int               pos )
{
  int n = 0;
  int error = 0;
  oyStructList_s_ * s = list;
  oyStruct_s * p = 0;

  if(s)
    n = s->n_;
  else
    error = 1;

  if(error <= 0)
  if(s->type_ != oyOBJECT_STRUCT_LIST_S)
    error = 1;

  if(error <= 0)
  if(pos >= 0 && n > pos && s->ptr_[pos])
  {
    p = s->ptr_[pos];
    error = !(p && p->copy);

    if(error <= 0)
    {
      p = p->copy( p, 0 );
      if(oy_debug_objects >= 0)
        oyObjectDebugMessage_( p->oy_, __func__, oyStructTypeToText(p->type_) );
    }
  }

  return !p;
}

/** Function  oyStructList_GetType_
 *  @memberof oyStructList_s
 *  @brief    oyStructList_s pointer access
 *  @internal
 *
 *  non thread save
 *
 *  @since Oyranos: version 0.1.8
 *  @date  1 january 2008 (API 0.1.8)
 */
oyStruct_s *     oyStructList_GetType_(oyStructList_s_   * list,
                                       int                 pos,
                                       oyOBJECT_e          type )
{
  oyStruct_s * obj = oyStructList_Get_( list, pos );

  if(obj && obj->type_ != type)
    obj = 0;
  return obj;
}

/* } Include "StructList.private_methods_definitions.c" */

