/** @file oyCMMui_s_.c

   [Template file inheritance graph]
   +-> oyCMMui_s_.template.c
   |
   +-- Base_s_.c

 *  Oyranos is an open source Color Management System
 *
 *  @par Copyright:
 *            2004-2018 (C) Kai-Uwe Behrmann
 *
 *  @author   Kai-Uwe Behrmann <ku.b@gmx.de>
 *  @par License:
 *            new BSD - see: http://www.opensource.org/licenses/BSD-3-Clause
 */



  
#include "oyCMMui_s.h"
#include "oyCMMui_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"
#include "oyranos_module_internal.h"

  


/* Include "CMMui.private_custom_definitions.c" { */
/** Function    oyCMMui_Release__Members
 *  @memberof   oyCMMui_s
 *  @brief      Custom CMMui destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmmui  the CMMui object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyCMMui_Release__Members( oyCMMui_s_ * cmmui )
{
  /* Deallocate members here
   */
  if(cmmui->parent && cmmui->parent->release)
    cmmui->parent->release( (oyStruct_s**) &cmmui->parent );
  cmmui->parent = NULL;

  if(cmmui->oy_->deallocateFunc_)
  {
#if 0
    oyDeAlloc_f deallocateFunc = cmmui->oy_->deallocateFunc_;
#endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmmui->member );
     */
  }
}

/** Function    oyCMMui_Init__Members
 *  @memberof   oyCMMui_s
 *  @brief      Custom CMMui constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmmui  the CMMui object
 *
 *  @version Oyranos: 0.9.7
 *  @since   2017/10/26 (Oyranos: 0.9.7)
 *  @date    2017/10/26
 */
int oyCMMui_Init__Members( oyCMMui_s_ * cmmui )
{
  /* set basic versions, as is checked during module load */
  cmmui->module_api[0] = cmmui->version[0] = OYRANOS_VERSION_A;
  cmmui->module_api[1] = cmmui->version[1] = OYRANOS_VERSION_B;
  cmmui->module_api[2] = cmmui->version[2] = OYRANOS_VERSION_C;

  return 0;
}

/** Function    oyCMMui_Copy__Members
 *  @memberof   oyCMMui_s
 *  @brief      Custom CMMui copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyCMMui_s_ input object
 *  @param[out]  dst  the output oyCMMui_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMui_Copy__Members( oyCMMui_s_ * dst, oyCMMui_s_ * src)
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
  if(src->parent && src->parent->copy)
  {
    dst->parent = (oyCMMapiFilter_s*) src->parent->copy( (oyStruct_s*) src->parent, src->oy_ );
    if(oy_debug_objects >= 0 && dst->parent)
        oyObjectDebugMessage_( dst->parent->oy_, __func__,
                               oyStructTypeToText(dst->parent->type_) );
  } else
    dst->parent = src->parent;

  return 0;
}

/* } Include "CMMui.private_custom_definitions.c" */



static int oy_cmmui_init_ = 0;
static const char * oyCMMui_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyCMMui_s_ * s = (oyCMMui_s_*) obj;
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
/** @internal
 *  Function oyCMMui_New_
 *  @memberof oyCMMui_s_
 *  @brief   allocate a new oyCMMui_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMui_s_ * oyCMMui_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CMM_UI_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyCMMui_s_ * s = 0;

  if(s_obj)
    s = (oyCMMui_s_*)s_obj->allocateFunc_(sizeof(oyCMMui_s_));
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

  error = !memset( s, 0, sizeof(oyCMMui_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyCMMui_Copy;
  s->release = (oyStruct_Release_f) oyCMMui_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom CMMui constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CMM_UI_S, (oyPointer)s );
  /* ---- end of custom CMMui constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom CMMui constructor ----- */
  error += oyCMMui_Init__Members( s );
  /* ---- end of custom CMMui constructor ------- */
  
  
  
  

  if(!oy_cmmui_init_)
  {
    oy_cmmui_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyCMMui_StaticMessageFunc_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyCMMui_Copy__
 *  @memberof oyCMMui_s_
 *  @brief   real copy a CMMui object
 *
 *  @param[in]     cmmui                 CMMui struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMui_s_ * oyCMMui_Copy__ ( oyCMMui_s_ *cmmui, oyObject_s object )
{
  oyCMMui_s_ *s = 0;
  int error = 0;

  if(!cmmui || !object)
    return s;

  s = (oyCMMui_s_*) oyCMMui_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom CMMui copy constructor ----- */
    error = oyCMMui_Copy__Members( s, cmmui );
    /* ---- end of custom CMMui copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyCMMui_Release_( &s );

  return s;
}

/** @internal
 *  Function oyCMMui_Copy_
 *  @memberof oyCMMui_s_
 *  @brief   copy or reference a CMMui object
 *
 *  @param[in]     cmmui                 CMMui struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMui_s_ * oyCMMui_Copy_ ( oyCMMui_s_ *cmmui, oyObject_s object )
{
  oyCMMui_s_ *s = cmmui;

  if(!cmmui)
    return 0;

  if(cmmui && !object)
  {
    s = cmmui;
    
    if(oy_debug_objects >= 0 && s->oy_)
    {
      const char * t = getenv(OY_DEBUG_OBJECTS);
      int id_ = -1;

      if(t)
        id_ = atoi(t);
      else
        id_ = oy_debug_objects;

      if((id_ >= 0 && s->oy_->id_ == id_) ||
         (t && s && strstr(oyStructTypeToText(s->type_), t) != 0) ||
         id_ == 1)
      {
        oyStruct_s ** parents = NULL;
        int n = oyStruct_GetParents( (oyStruct_s*)s, &parents );
        if(n != s->oy_->ref_)
        {
          int i;
          const char * track_name = oyStructTypeToText(s->type_);
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

  s = oyCMMui_Copy__( cmmui, object );

  return s;
}
 
/** @internal
 *  Function oyCMMui_Release_
 *  @memberof oyCMMui_s_
 *  @brief   release and possibly deallocate a CMMui object
 *
 *  @param[in,out] cmmui                 CMMui struct object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyCMMui_Release_( oyCMMui_s_ **cmmui )
{
  const char * track_name = NULL;
  /* ---- start of common object destructor ----- */
  oyCMMui_s_ *s = 0;

  if(!cmmui || !*cmmui)
    return 0;

  s = *cmmui;

  *cmmui = 0;

  if(oy_debug_objects >= 0 && s->oy_)
  {
    const char * t = getenv(OY_DEBUG_OBJECTS);
    int id_ = -1;

    if(t)
      id_ = atoi(t);
    else
      id_ = oy_debug_objects;

    if((id_ >= 0 && s->oy_->id_ == id_) ||
       (t && s && strstr(oyStructTypeToText(s->type_), t) != 0) ||
       id_ == 1)
    {
      oyStruct_s ** parents = NULL;
      int n = oyStruct_GetParents( (oyStruct_s*)s, &parents );
      if(n != s->oy_->ref_)
      {
        int i;
        track_name = oyStructTypeToText(s->type_);
        fprintf( stderr, "%s[%d] untracking refs: %d parents: %d\n",
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


  if(oyObject_UnRef(s->oy_))
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
       (t && s && strstr(oyStructTypeToText(s->type_), t) != 0) ||
       id_ == 1)
    {
      track_name = oyStructTypeToText(s->type_);
      fprintf( stderr, "%s[%d] untracking\n", track_name, s->oy_->id_);
    }
  }

  
  /* ---- start of custom CMMui destructor ----- */
  oyCMMui_Release__Members( s );
  /* ---- end of custom CMMui destructor ------- */
  
  
  
  



  if(s->oy_->deallocateFunc_)
  {
    oyDeAlloc_f deallocateFunc = s->oy_->deallocateFunc_;
    int id = s->oy_->id_;

    oyObject_Release( &s->oy_ );
    if(track_name)
      fprintf( stderr, "%s[%d] untracked\n", track_name, id);

    deallocateFunc( s );
  }

  return 0;
}



/* Include "CMMui.private_methods_definitions.c" { */

/* } Include "CMMui.private_methods_definitions.c" */

