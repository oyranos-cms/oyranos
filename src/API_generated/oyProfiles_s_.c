/** @file oyProfiles_s_.c

   [Template file inheritance graph]
   +-> oyProfiles_s_.template.c
   |
   +-> BaseList_s_.c
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



  
#include "oyProfiles_s.h"
#include "oyProfiles_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"


  

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif


static int oy_profiles_init_ = 0;
static const char * oyProfiles_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyProfiles_s_ * s = (oyProfiles_s_*) obj;
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

  
  

  return text;
}


/* Include "Profiles.private_custom_definitions.c" { */
/** Function    oyProfiles_Release__Members
 *  @memberof   oyProfiles_s
 *  @brief      Custom Profiles destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  profiles  the Profiles object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyProfiles_Release__Members( oyProfiles_s_ * profiles )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &profiles->member );
   */

  if(profiles->oy_->deallocateFunc_)
  {
#if 0
    oyDeAlloc_f deallocateFunc = profiles->oy_->deallocateFunc_;
#endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( profiles->member );
     */
  }
}

/** Function    oyProfiles_Init__Members
 *  @memberof   oyProfiles_s
 *  @brief      Custom Profiles constructor
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  profiles  the Profiles object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyProfiles_Init__Members( oyProfiles_s_ * profiles OY_UNUSED )
{
  return 0;
}

/** Function    oyProfiles_Copy__Members
 *  @memberof   oyProfiles_s
 *  @brief      Custom Profiles copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyProfiles_s_ input object
 *  @param[out]  dst  the output oyProfiles_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyProfiles_Copy__Members( oyProfiles_s_ * dst, oyProfiles_s_ * src)
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

/* } Include "Profiles.private_custom_definitions.c" */


/** @internal
 *  Function oyProfiles_New_
 *  @memberof oyProfiles_s_
 *  @brief   allocate a new oyProfiles_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyProfiles_s_ * oyProfiles_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_PROFILES_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyProfiles_s_ * s = 0;

  if(s_obj)
    s = (oyProfiles_s_*)s_obj->allocateFunc_(sizeof(oyProfiles_s_));
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

  error = !memset( s, 0, sizeof(oyProfiles_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyProfiles_Copy;
  s->release = (oyStruct_Release_f) oyProfiles_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom Profiles constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_PROFILES_S, (oyPointer)s );
  /* ---- end of custom Profiles constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  s->list_ = oyStructList_Create( s->type_, 0, 0 );


  
  /* ---- start of custom Profiles constructor ----- */
  error += oyProfiles_Init__Members( s );
  /* ---- end of custom Profiles constructor ------- */
  
  
  
  

  if(!oy_profiles_init_)
  {
    oy_profiles_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyProfiles_StaticMessageFunc_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyProfiles_Copy__
 *  @memberof oyProfiles_s_
 *  @brief   real copy a Profiles object
 *
 *  @param[in]     profiles                 Profiles struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyProfiles_s_ * oyProfiles_Copy__ ( oyProfiles_s_ *profiles, oyObject_s object )
{
  oyProfiles_s_ *s = 0;
  int error = 0;

  if(!profiles || !object)
    return s;

  s = (oyProfiles_s_*) oyProfiles_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom Profiles copy constructor ----- */
    error = oyProfiles_Copy__Members( s, profiles );
    /* ---- end of custom Profiles copy constructor ------- */
    
    
    
    
    
    oyStructList_Release( &s->list_ );
    s->list_ = oyStructList_Copy( profiles->list_, s->oy_ );

  }

  if(error)
    oyProfiles_Release_( &s );

  return s;
}

/** @internal
 *  Function oyProfiles_Copy_
 *  @memberof oyProfiles_s_
 *  @brief   copy or reference a Profiles object
 *
 *  @param[in]     profiles                 Profiles struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyProfiles_s_ * oyProfiles_Copy_ ( oyProfiles_s_ *profiles, oyObject_s object )
{
  oyProfiles_s_ *s = profiles;

  if(!profiles)
    return 0;

  if(profiles && !object)
  {
    s = profiles;
    
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

  s = oyProfiles_Copy__( profiles, object );

  return s;
}
 
/** @internal
 *  Function oyProfiles_Release_
 *  @memberof oyProfiles_s_
 *  @brief   release and possibly deallocate a Profiles list
 *
 *  @param[in,out] profiles                 Profiles struct object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/10/03
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyProfiles_Release_( oyProfiles_s_ **profiles )
{
  const char * track_name = NULL;
  int observer_refs = 0, i;
  /* ---- start of common object destructor ----- */
  oyProfiles_s_ *s = 0;

  if(!profiles || !*profiles)
    return 0;

  s = *profiles;
  /* static object */
  if(!s->oy_)
    return 0;

  *profiles = 0;

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

  
  /* ---- start of custom Profiles destructor ----- */
  oyProfiles_Release__Members( s );
  /* ---- end of custom Profiles destructor ------- */
  
  
  
  

  oyStructList_Release( &s->list_ );


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



/* Include "Profiles.private_methods_definitions.c" { */

/* } Include "Profiles.private_methods_definitions.c" */

