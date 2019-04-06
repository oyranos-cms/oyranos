/** @file oyConfig_s_.c

   [Template file inheritance graph]
   +-> oyConfig_s_.template.c
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



  
#include "oyConfig_s.h"
#include "oyConfig_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"


  

#ifdef HAVE_BACKTRACE
#include <execinfo.h>
#define BT_BUF_SIZE 100
#endif


static int oy_config_init_ = 0;
static const char * oyConfig_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyConfig_s_ * s = (oyConfig_s_*) obj;
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

  

  
  if(type == oyNAME_NICK && (flags & 0x01))
  {
    sprintf( &text[strlen(text)], "%d",
             s->id
           );
  } else
  if(type == oyNAME_NAME)
    sprintf( &text[strlen(text)], "%d %s",
             s->id, s->registration
           );
  else
  if((int)type >= oyNAME_DESCRIPTION)
    sprintf( &text[strlen(text)], "%s id: %d",
             s->registration, s->id
           );


  return text;
}


/* Include "Config.private_custom_definitions.c" { */
/** @internal
 +  Function    oyConfig_Release__Members
 *  @memberof   oyConfig_s
 *  @brief      Custom Config destructor
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  config  the Config object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyConfig_Release__Members( oyConfig_s_ * config )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &config->member );
   */

  oyOptions_Release( &config->db );
  oyOptions_Release( &config->backend_core );
  oyOptions_Release( &config->data );

  if(config->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = config->oy_->deallocateFunc_;

    if(deallocateFunc)
    { 
      int i = 0;
      if(config->rank_map)
      {
        while(config->rank_map[i].key)
          deallocateFunc( config->rank_map[i++].key );
        deallocateFunc( config->rank_map );
      }
      if(config->registration)
        deallocateFunc( config->registration ); 
    }
  }
}

/** @internal
 +  Function    oyConfig_Init__Members
 *  @memberof   oyConfig_s
 *  @brief      Custom Config constructor 
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  config  the Config object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyConfig_Init__Members( oyConfig_s_ * config )
{
  config->db = oyOptions_New( 0 );
  if(config->db)
    oyObject_SetName( config->db->oy_, "db", oyNAME_NICK );
  config->backend_core = oyOptions_New( 0 );
  if(config->backend_core)
    oyObject_SetName( config->backend_core->oy_, "backend_core", oyNAME_NICK );
  config->data = oyOptions_New( 0 );
  if(config->data)
    oyObject_SetName( config->data->oy_, "data", oyNAME_NICK );

  return 0;
}

/** @internal
 +  Function    oyConfig_Copy__Members
 *  @memberof   oyConfig_s
 *  @brief      Custom Config copy constructor
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyConfig_s_ input object
 *  @param[out]  dst  the output oyConfig_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyConfig_Copy__Members( oyConfig_s_ * dst, oyConfig_s_ * src)
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
  dst->db = oyOptions_Copy( src->db, dst->oy_ );
  dst->backend_core = oyOptions_Copy( src->backend_core, dst->oy_ );
  dst->data = oyOptions_Copy( src->data, dst->oy_ );
  error = !memcpy( dst->version, src->version, 3*sizeof(int) );

  dst->rank_map = oyRankMapCopy( src->rank_map, allocateFunc_ );

  return error;
}

/* } Include "Config.private_custom_definitions.c" */


/** @internal
 *  Function oyConfig_New_
 *  @memberof oyConfig_s_
 *  @brief   allocate a new oyConfig_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyConfig_s_ * oyConfig_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CONFIG_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyConfig_s_ * s = 0;

  if(s_obj)
    s = (oyConfig_s_*)s_obj->allocateFunc_(sizeof(oyConfig_s_));
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

  error = !memset( s, 0, sizeof(oyConfig_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyConfig_Copy;
  s->release = (oyStruct_Release_f) oyConfig_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom Config constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CONFIG_S, (oyPointer)s );
  /* ---- end of custom Config constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom Config constructor ----- */
  error += oyConfig_Init__Members( s );
  /* ---- end of custom Config constructor ------- */
  
  
  
  

  if(!oy_config_init_)
  {
    oy_config_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyConfig_StaticMessageFunc_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyConfig_Copy__
 *  @memberof oyConfig_s_
 *  @brief   real copy a Config object
 *
 *  @param[in]     config                 Config struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyConfig_s_ * oyConfig_Copy__ ( oyConfig_s_ *config, oyObject_s object )
{
  oyConfig_s_ *s = 0;
  int error = 0;

  if(!config || !object)
    return s;

  s = (oyConfig_s_*) oyConfig_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom Config copy constructor ----- */
    error = oyConfig_Copy__Members( s, config );
    /* ---- end of custom Config copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyConfig_Release_( &s );

  return s;
}

/** @internal
 *  Function oyConfig_Copy_
 *  @memberof oyConfig_s_
 *  @brief   copy or reference a Config object
 *
 *  @param[in]     config                 Config struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyConfig_s_ * oyConfig_Copy_ ( oyConfig_s_ *config, oyObject_s object )
{
  oyConfig_s_ *s = config;

  if(!config)
    return 0;

  if(config && !object)
  {
    s = config;
    
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

  s = oyConfig_Copy__( config, object );

  return s;
}
 
/** @internal
 *  Function oyConfig_Release_
 *  @memberof oyConfig_s_
 *  @brief   release and possibly deallocate a Config object
 *
 *  @param[in,out] config                 Config struct object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/10/03
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyConfig_Release_( oyConfig_s_ **config )
{
  const char * track_name = NULL;
  int observer_refs = 0, i;
  /* ---- start of common object destructor ----- */
  oyConfig_s_ *s = 0;

  if(!config || !*config)
    return 0;

  s = *config;
  /* static object */
  if(!s->oy_)
    return 0;

  *config = 0;

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

  
  /* ---- start of custom Config destructor ----- */
  oyConfig_Release__Members( s );
  /* ---- end of custom Config destructor ------- */
  
  
  
  



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



/* Include "Config.private_methods_definitions.c" { */

/* } Include "Config.private_methods_definitions.c" */

