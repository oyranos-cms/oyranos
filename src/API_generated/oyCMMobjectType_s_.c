/** @file oyCMMobjectType_s_.c

   [Template file inheritance graph]
   +-> oyCMMobjectType_s_.template.c
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



  
#include "oyCMMobjectType_s.h"
#include "oyCMMobjectType_s_.h"





#include "oyObject_s.h"
#include "oyranos_object_internal.h"
#include "oyranos_module_internal.h"

  


/* Include "CMMobjectType.private_custom_definitions.c" { */
/** Function    oyCMMobjectType_Release__Members
 *  @memberof   oyCMMobjectType_s
 *  @brief      Custom CMMobjectType destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmmobjecttype  the CMMobjectType object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyCMMobjectType_Release__Members( oyCMMobjectType_s_ * cmmobjecttype )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmmobjecttype->member );
   */

  if(cmmobjecttype->oy_->deallocateFunc_)
  {
#if 0
    oyDeAlloc_f deallocateFunc = cmmobjecttype->oy_->deallocateFunc_;
#endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmmobjecttype->member );
     */
  }
}

/** Function    oyCMMobjectType_Init__Members
 *  @memberof   oyCMMobjectType_s
 *  @brief      Custom CMMobjectType constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmmobjecttype  the CMMobjectType object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMobjectType_Init__Members( oyCMMobjectType_s_ * cmmobjecttype OY_UNUSED )
{
  return 0;
}

/** Function    oyCMMobjectType_Copy__Members
 *  @memberof   oyCMMobjectType_s
 *  @brief      Custom CMMobjectType copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyCMMobjectType_s_ input object
 *  @param[out]  dst  the output oyCMMobjectType_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMobjectType_Copy__Members( oyCMMobjectType_s_ * dst, oyCMMobjectType_s_ * src)
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

  return 0;
}

/* } Include "CMMobjectType.private_custom_definitions.c" */



static int oy_cmmobjecttype_init_ = 0;
static const char * oyCMMobjectType_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyCMMobjectType_s_ * s = (oyCMMobjectType_s_*) obj;
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
 *  Function oyCMMobjectType_New_
 *  @memberof oyCMMobjectType_s_
 *  @brief   allocate a new oyCMMobjectType_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMobjectType_s_ * oyCMMobjectType_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CMM_OBJECT_TYPE_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyCMMobjectType_s_ * s = 0;

  if(s_obj)
    s = (oyCMMobjectType_s_*)s_obj->allocateFunc_(sizeof(oyCMMobjectType_s_));
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

  error = !memset( s, 0, sizeof(oyCMMobjectType_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyCMMobjectType_Copy;
  s->release = (oyStruct_Release_f) oyCMMobjectType_Release;

  s->oy_ = s_obj;

  
  /* ---- start of custom CMMobjectType constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CMM_OBJECT_TYPE_S, (oyPointer)s );
  /* ---- end of custom CMMobjectType constructor ------- */
  
  
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  /* ---- start of custom CMMobjectType constructor ----- */
  error += oyCMMobjectType_Init__Members( s );
  /* ---- end of custom CMMobjectType constructor ------- */
  
  
  
  

  if(!oy_cmmobjecttype_init_)
  {
    oy_cmmobjecttype_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyCMMobjectType_StaticMessageFunc_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyCMMobjectType_Copy__
 *  @memberof oyCMMobjectType_s_
 *  @brief   real copy a CMMobjectType object
 *
 *  @param[in]     cmmobjecttype                 CMMobjectType struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMobjectType_s_ * oyCMMobjectType_Copy__ ( oyCMMobjectType_s_ *cmmobjecttype, oyObject_s object )
{
  oyCMMobjectType_s_ *s = 0;
  int error = 0;

  if(!cmmobjecttype || !object)
    return s;

  s = (oyCMMobjectType_s_*) oyCMMobjectType_New( object );
  error = !s;

  if(!error) {
    
    /* ---- start of custom CMMobjectType copy constructor ----- */
    error = oyCMMobjectType_Copy__Members( s, cmmobjecttype );
    /* ---- end of custom CMMobjectType copy constructor ------- */
    
    
    
    
    
    
  }

  if(error)
    oyCMMobjectType_Release_( &s );

  return s;
}

/** @internal
 *  Function oyCMMobjectType_Copy_
 *  @memberof oyCMMobjectType_s_
 *  @brief   copy or reference a CMMobjectType object
 *
 *  @param[in]     cmmobjecttype                 CMMobjectType struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMobjectType_s_ * oyCMMobjectType_Copy_ ( oyCMMobjectType_s_ *cmmobjecttype, oyObject_s object )
{
  oyCMMobjectType_s_ *s = cmmobjecttype;

  if(!cmmobjecttype)
    return 0;

  if(cmmobjecttype && !object)
  {
    s = cmmobjecttype;
    
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

  s = oyCMMobjectType_Copy__( cmmobjecttype, object );

  return s;
}
 
/** @internal
 *  Function oyCMMobjectType_Release_
 *  @memberof oyCMMobjectType_s_
 *  @brief   release and possibly deallocate a CMMobjectType object
 *
 *  @param[in,out] cmmobjecttype                 CMMobjectType struct object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyCMMobjectType_Release_( oyCMMobjectType_s_ **cmmobjecttype )
{
  const char * track_name = NULL;
  /* ---- start of common object destructor ----- */
  oyCMMobjectType_s_ *s = 0;

  if(!cmmobjecttype || !*cmmobjecttype)
    return 0;

  s = *cmmobjecttype;

  *cmmobjecttype = 0;

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

  
  /* ---- start of custom CMMobjectType destructor ----- */
  oyCMMobjectType_Release__Members( s );
  /* ---- end of custom CMMobjectType destructor ------- */
  
  
  
  



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



/* Include "CMMobjectType.private_methods_definitions.c" { */

/* } Include "CMMobjectType.private_methods_definitions.c" */

