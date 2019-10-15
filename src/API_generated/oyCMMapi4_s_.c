/** @file oyCMMapi4_s_.c

   [Template file inheritance graph]
   +-> oyCMMapi4_s_.template.c
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



  
#include "oyCMMapi4_s.h"
#include "oyCMMapi4_s_.h"



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


static int oy_cmmapi4_init_ = 0;
static const char * oyCMMapi4_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyCMMapi4_s_ * s = (oyCMMapi4_s_*) obj;
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
    text_n = 1000;
    text = alloc(text_n);
    if(text)
      text[0] = '\000';
    else
      return "Memory Error";

    if(!(flags & 0x01))
      sprintf(text, "%s%s", oyStructTypeToText( s->type_ ), type != oyNAME_NICK?" ":"");
  }

  if(type == oyNAME_NICK && (flags & 0x01))
  {
    sprintf( &text[strlen(text)], "%s",
             s->registration?s->registration:""
           );
  } else
  if(type == oyNAME_NAME)
    sprintf( &text[strlen(text)], "%s %s",
             s->registration?s->registration:"", s->context_type
           );
  else
  if((int)type >= oyNAME_DESCRIPTION)
    sprintf( &text[strlen(text)], "%s%s%s",
             s->registration?s->registration:"", s->context_type[0]?"\ntype: ":"", s->context_type
           );


  return text;
}


/* Include "CMMapi4.private_custom_definitions.c" { */
/** Function    oyCMMapi4_Release__Members
 *  @memberof   oyCMMapi4_s
 *  @brief      Custom CMMapi4 destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmmapi4  the CMMapi4 object
 *
 *  @version Oyranos: 0.9.5
 *  @since   2013/06/09 (Oyranos: 0.9.5)
 *  @date    2013/06/09
 */
void oyCMMapi4_Release__Members( oyCMMapi4_s_ * cmmapi4 )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmmapi4->member );
   */

  if(cmmapi4 && cmmapi4->oy_ && cmmapi4->oy_->deallocateFunc_)
  {
#   if 0
    oyDeAlloc_f deallocateFunc = cmmapi4->oy_->deallocateFunc_;
#   endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmmapi4->member );
     */

    oyCMMui_Release( (oyCMMui_s**) &cmmapi4->ui );
  }
}

/** Function    oyCMMapi4_Init__Members
 *  @memberof   oyCMMapi4_s
 *  @brief      Custom CMMapi4 constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmmapi4  the CMMapi4 object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapi4_Init__Members( oyCMMapi4_s_ * cmmapi4 OY_UNUSED )
{
  return 0;
}

/** Function    oyCMMapi4_Copy__Members
 *  @memberof   oyCMMapi4_s
 *  @brief      Custom CMMapi4 copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyCMMapi4_s_ input object
 *  @param[out]  dst  the output oyCMMapi4_s_ object
 *
 *  @version Oyranos: 0.9.5
 *  @since   2013/06/09 (Oyranos: 0.9.5)
 *  @date    2013/06/09
 */
int oyCMMapi4_Copy__Members( oyCMMapi4_s_ * dst, oyCMMapi4_s_ * src)
{
  if(!dst || !src)
    return 1;

  /* Copy each value of src to dst here */

  memcpy( dst->context_type, src->context_type, 8 );
  dst->oyCMMFilterNode_ContextToMem = src->oyCMMFilterNode_ContextToMem;
  dst->oyCMMFilterNode_GetText = src->oyCMMFilterNode_GetText;
  dst->ui = (oyCMMui_s_*) oyCMMui_Copy( (oyCMMui_s*)src->ui, src->oy_ );

  return 0;
}

/* } Include "CMMapi4.private_custom_definitions.c" */


/** @internal
 *  Function oyCMMapi4_New_
 *  @memberof oyCMMapi4_s_
 *  @brief   allocate a new oyCMMapi4_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapi4_s_ * oyCMMapi4_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CMM_API4_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyCMMapi4_s_ * s = 0;

  if(s_obj)
    s = (oyCMMapi4_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi4_s_));
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

  error = !memset( s, 0, sizeof(oyCMMapi4_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyCMMapi4_Copy;
  s->release = (oyStruct_Release_f) oyCMMapi4_Release;

  s->oy_ = s_obj;

  
  
  
  /* ---- start of custom CMMapi constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CMM_API_S, (oyPointer)s );
  /* ---- end of custom CMMapi constructor ------- */
  /* ---- start of custom CMMapiFilter constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CMM_API_FILTER_S, (oyPointer)s );
  /* ---- end of custom CMMapiFilter constructor ------- */
  /* ---- start of custom CMMapi4 constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CMM_API4_S, (oyPointer)s );
  /* ---- end of custom CMMapi4 constructor ------- */
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  
  
  /* ---- start of custom CMMapi constructor ----- */
  error += oyCMMapi_Init__Members( (oyCMMapi_s_*)s );
  /* ---- end of custom CMMapi constructor ------- */
  /* ---- start of custom CMMapiFilter constructor ----- */
  error += oyCMMapiFilter_Init__Members( (oyCMMapiFilter_s_*)s );
  /* ---- end of custom CMMapiFilter constructor ------- */
  /* ---- start of custom CMMapi4 constructor ----- */
  error += oyCMMapi4_Init__Members( s );
  /* ---- end of custom CMMapi4 constructor ------- */
  
  

  if(!oy_cmmapi4_init_)
  {
    oy_cmmapi4_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyCMMapi4_StaticMessageFunc_,
                                        &oy_cmmapi4_init_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyCMMapi4_Copy__
 *  @memberof oyCMMapi4_s_
 *  @brief   real copy a CMMapi4 object
 *
 *  @param[in]     cmmapi4                 CMMapi4 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapi4_s_ * oyCMMapi4_Copy__ ( oyCMMapi4_s_ *cmmapi4, oyObject_s object )
{
  oyCMMapi4_s_ *s = 0;
  int error = 0;

  if(!cmmapi4 || !object)
    return s;

  s = (oyCMMapi4_s_*) oyCMMapi4_New( object );
  error = !s;

  if(!error) {
    
    
    
    /* ---- start of custom CMMapi copy constructor ----- */
    error = oyCMMapi_Copy__Members( (oyCMMapi_s_*)s, (oyCMMapi_s_*)cmmapi4 );
    /* ---- end of custom CMMapi copy constructor ------- */
    /* ---- start of custom CMMapiFilter copy constructor ----- */
    error = oyCMMapiFilter_Copy__Members( (oyCMMapiFilter_s_*)s, (oyCMMapiFilter_s_*)cmmapi4 );
    /* ---- end of custom CMMapiFilter copy constructor ------- */
    /* ---- start of custom CMMapi4 copy constructor ----- */
    error = oyCMMapi4_Copy__Members( s, cmmapi4 );
    /* ---- end of custom CMMapi4 copy constructor ------- */
    
    
    
    
  }

  if(error)
    oyCMMapi4_Release_( &s );

  return s;
}

/** @internal
 *  Function oyCMMapi4_Copy_
 *  @memberof oyCMMapi4_s_
 *  @brief   copy or reference a CMMapi4 object
 *
 *  @param[in]     cmmapi4                 CMMapi4 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapi4_s_ * oyCMMapi4_Copy_ ( oyCMMapi4_s_ *cmmapi4, oyObject_s object )
{
  oyCMMapi4_s_ *s = cmmapi4;

  if(!cmmapi4)
    return 0;

  if(cmmapi4 && !object)
  {
    s = cmmapi4;
    
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

  s = oyCMMapi4_Copy__( cmmapi4, object );

  return s;
}
 
/** @internal
 *  Function oyCMMapi4_Release_
 *  @memberof oyCMMapi4_s_
 *  @brief   release and possibly deallocate a CMMapi4 object
 *
 *  @param[in,out] cmmapi4                 CMMapi4 struct object
 *
 *  @version Oyranos: 0.9.7
 *  @date    2018/10/03
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 */
int oyCMMapi4_Release_( oyCMMapi4_s_ **cmmapi4 )
{
  const char * track_name = NULL;
  int observer_refs = 0, i;
  /* ---- start of common object destructor ----- */
  oyCMMapi4_s_ *s = 0;

  if(!cmmapi4 || !*cmmapi4)
    return 0;

  s = *cmmapi4;
  /* static object */
  if(!s->oy_)
    return 0;

  *cmmapi4 = 0;

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

  
  {
    uint32_t ui_p = s->ui->parent ? 1 : 0;
    int r OY_UNUSED = oyObject_UnRef(s->oy_);

    /* references from members has to be substracted
     * from this objects ref count */
    if(oyObject_GetRefCount( s->oy_ ) > (int)(ui_p + observer_refs))
      return 0;
  }

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

  
  
  
  /* ---- start of custom CMMapi destructor ----- */
  oyCMMapi_Release__Members( (oyCMMapi_s_*)s );
  /* ---- end of custom CMMapi destructor ------- */
  /* ---- start of custom CMMapiFilter destructor ----- */
  oyCMMapiFilter_Release__Members( (oyCMMapiFilter_s_*)s );
  /* ---- end of custom CMMapiFilter destructor ------- */
  /* ---- start of custom CMMapi4 destructor ----- */
  oyCMMapi4_Release__Members( s );
  /* ---- end of custom CMMapi4 destructor ------- */
  
  



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



/* Include "CMMapi4.private_methods_definitions.c" { */
/** @internal
 *  Function oyCMMapi4_SelectFilter_
 *  @brief   filter the desired api
 *  @memberof oyCMMapi_s_
 *
 *  @version Oyranos: 0.1.9
 *  @since   2008/12/16 (Oyranos: 0.1.9)
 *  @date    2008/12/16
 */
oyOBJECT_e   oyCMMapi4_SelectFilter_ ( oyCMMapi_s_       * api,
                                       oyPointer           data )
{
  oyOBJECT_e type = oyOBJECT_NONE,
             searched = oyOBJECT_CMM_API4_S;
  int error = !data || !api;
  oyRegistrationData_s * reg_filter;
  oyCMMapi4_s_ * cmm_api = (oyCMMapi4_s_ *) api;
  int found = 0;

  if(error <= 0)
    reg_filter = (oyRegistrationData_s*) data;

  if(error <= 0 &&
     api->type_ == searched &&
     reg_filter->type == searched)
  {
    if(reg_filter->registration)
    {
      if(oyFilterRegistrationMatch( cmm_api->registration,
                                    reg_filter->registration, api->type_ ))
        found = 1;
    } else
      found = 1;

    if( found )
      type = api->type_;
  }

  return type;
}

/* } Include "CMMapi4.private_methods_definitions.c" */

