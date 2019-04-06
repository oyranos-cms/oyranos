/** @file oyFilterCore_s_.c

   [Template file inheritance graph]
   +-> oyFilterCore_s_.template.c
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
static const char * oyFilterCore_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyFilterCore_s_ * s = (oyFilterCore_s_*) obj;
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

  

  
  if(type == oyNAME_NICK && (flags & 0x01) && s->category_)
  {
    sprintf( &text[strlen(text)], "%s",
             s->category_
           );
  } else
  if(type == oyNAME_NAME && (s->category_ || s->registration_))
    sprintf( &text[strlen(text)], "%s %s",
             s->category_?s->category_:"", s->registration_?s->registration_:""
           );
  else
  if((int)type >= oyNAME_DESCRIPTION && (s->category_ || s->registration_))
    sprintf( &text[strlen(text)], "category: %s\nreg: %s",
             s->category_?s->category_:"", s->registration_?s->registration_:""
           );


  return text;
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
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyFilterCore_s_ * s = 0;

  if(s_obj)
    s = (oyFilterCore_s_*)s_obj->allocateFunc_(sizeof(oyFilterCore_s_));
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
                                        oyFilterCore_StaticMessageFunc_ );
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
  oyFilterCore_s_ *s = filtercore;

  if(!filtercore)
    return 0;

  if(filtercore && !object)
  {
    s = filtercore;
    
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
 *  @date    2018/10/03
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyFilterCore_Release_( oyFilterCore_s_ **filtercore )
{
  const char * track_name = NULL;
  int observer_refs = 0, i;
  /* ---- start of common object destructor ----- */
  oyFilterCore_s_ *s = 0;

  if(!filtercore || !*filtercore)
    return 0;

  s = *filtercore;
  /* static object */
  if(!s->oy_)
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

  
  /* ---- start of custom FilterCore destructor ----- */
  oyFilterCore_Release__Members( s );
  /* ---- end of custom FilterCore destructor ------- */
  
  
  
  



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

