/** @file oyCMMapi6_s_.c

   [Template file inheritance graph]
   +-> oyCMMapi6_s_.template.c
   |
   +-> oyCMMapiFilter_s_.template.c
   |
   +-> oyCMMapi_s_.template.c
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



  
#include "oyCMMapi6_s.h"
#include "oyCMMapi6_s_.h"



#include "oyCMMapi_s_.h"
#include "oyCMMapiFilter_s_.h"



#include "oyObject_s.h"
#include "oyranos_object_internal.h"
#include "oyranos_module_internal.h"



#include "oyCMMapiFilter_s_.h"
  


/* Include "CMMapi6.private_custom_definitions.c" { */
/** Function    oyCMMapi6_Release__Members
 *  @memberof   oyCMMapi6_s
 *  @brief      Custom CMMapi6 destructor
 *  @internal
 *
 *  This function will free up all memmory allocated by the
 *  input object. First all object members witch have their
 *  own release method are deallocated. Then the deallocateFunc_
 *  of the oy_ object is used to release the rest of the members
 *  that were allocated with oy_->allocateFunc_.
 *
 *  @param[in]  cmmapi6  the CMMapi6 object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
void oyCMMapi6_Release__Members( oyCMMapi6_s_ * cmmapi6 )
{
  /* Deallocate members here
   * E.g: oyXXX_Release( &cmmapi6->member );
   */

  if(cmmapi6->oy_->deallocateFunc_)
  {
#if 0
    oyDeAlloc_f deallocateFunc = cmmapi6->oy_->deallocateFunc_;
#endif

    /* Deallocate members of basic type here
     * E.g.: deallocateFunc( cmmapi6->member );
     */
  }
}

/** Function    oyCMMapi6_Init__Members
 *  @memberof   oyCMMapi6_s
 *  @brief      Custom CMMapi6 constructor 
 *  @internal
 *
 *  This function will allocate all memmory for the input object.
 *  For the basic member types this is done using the allocateFunc_
 *  of the attatced (oyObject_s)oy_ object.
 *
 *  @param[in]  cmmapi6  the CMMapi6 object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapi6_Init__Members( oyCMMapi6_s_ * cmmapi6 OY_UNUSED )
{
  return 0;
}

/** Function    oyCMMapi6_Copy__Members
 *  @memberof   oyCMMapi6_s
 *  @brief      Custom CMMapi6 copy constructor
 *  @internal
 *
 *  This function makes a copy of all values from the input
 *  to the output object. The destination object and all of its
 *  members should already be allocated.
 *
 *  @param[in]   src  the oyCMMapi6_s_ input object
 *  @param[out]  dst  the output oyCMMapi6_s_ object
 *
 *  @version Oyranos: x.x.x
 *  @since   YYYY/MM/DD (Oyranos: x.x.x)
 *  @date    YYYY/MM/DD
 */
int oyCMMapi6_Copy__Members( oyCMMapi6_s_ * dst, oyCMMapi6_s_ * src)
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

/* } Include "CMMapi6.private_custom_definitions.c" */



static int oy_cmmapi6_init_ = 0;
static const char * oyCMMapi6_StaticMessageFunc_ (
                                       oyPointer           obj,
                                       oyNAME_e            type,
                                       int                 flags )
{
  oyCMMapi6_s_ * s = (oyCMMapi6_s_*) obj;
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
    text_n = 128;
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
 *  Function oyCMMapi6_New_
 *  @memberof oyCMMapi6_s_
 *  @brief   allocate a new oyCMMapi6_s_  object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapi6_s_ * oyCMMapi6_New_ ( oyObject_s object )
{
  /* ---- start of common object constructor ----- */
  oyOBJECT_e type = oyOBJECT_CMM_API6_S;
  int error = 0;
  oyObject_s    s_obj = oyObject_NewFrom( object );
  oyCMMapi6_s_ * s = 0;

  if(s_obj)
    s = (oyCMMapi6_s_*)s_obj->allocateFunc_(sizeof(oyCMMapi6_s_));
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

  error = !memset( s, 0, sizeof(oyCMMapi6_s_) );
  if(error)
    WARNc_S( "memset failed" );

  memcpy( s, &type, sizeof(oyOBJECT_e) );
  s->copy = (oyStruct_Copy_f) oyCMMapi6_Copy;
  s->release = (oyStruct_Release_f) oyCMMapi6_Release;

  s->oy_ = s_obj;

  
  
  
  /* ---- start of custom CMMapi constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CMM_API_S, (oyPointer)s );
  /* ---- end of custom CMMapi constructor ------- */
  /* ---- start of custom CMMapiFilter constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CMM_API_FILTER_S, (oyPointer)s );
  /* ---- end of custom CMMapiFilter constructor ------- */
  /* ---- start of custom CMMapi6 constructor ----- */
  error += !oyObject_SetParent( s_obj, oyOBJECT_CMM_API6_S, (oyPointer)s );
  /* ---- end of custom CMMapi6 constructor ------- */
  
  
  /* ---- end of common object constructor ------- */
  if(error)
    WARNc_S( "oyObject_SetParent failed" );


  
  

  
  
  
  /* ---- start of custom CMMapi constructor ----- */
  error += oyCMMapi_Init__Members( (oyCMMapi_s_*)s );
  /* ---- end of custom CMMapi constructor ------- */
  /* ---- start of custom CMMapiFilter constructor ----- */
  error += oyCMMapiFilter_Init__Members( (oyCMMapiFilter_s_*)s );
  /* ---- end of custom CMMapiFilter constructor ------- */
  /* ---- start of custom CMMapi6 constructor ----- */
  error += oyCMMapi6_Init__Members( s );
  /* ---- end of custom CMMapi6 constructor ------- */
  
  

  if(!oy_cmmapi6_init_)
  {
    oy_cmmapi6_init_ = 1;
    oyStruct_RegisterStaticMessageFunc( type,
                                        oyCMMapi6_StaticMessageFunc_ );
  }

  if(error)
    WARNc1_S("%d", error);

  if(oy_debug_objects >= 0)
    oyObject_GetId( s->oy_ );

  return s;
}

/** @internal
 *  Function oyCMMapi6_Copy__
 *  @memberof oyCMMapi6_s_
 *  @brief   real copy a CMMapi6 object
 *
 *  @param[in]     cmmapi6                 CMMapi6 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapi6_s_ * oyCMMapi6_Copy__ ( oyCMMapi6_s_ *cmmapi6, oyObject_s object )
{
  oyCMMapi6_s_ *s = 0;
  int error = 0;

  if(!cmmapi6 || !object)
    return s;

  s = (oyCMMapi6_s_*) oyCMMapi6_New( object );
  error = !s;

  if(!error) {
    
    
    
    /* ---- start of custom CMMapi copy constructor ----- */
    error = oyCMMapi_Copy__Members( (oyCMMapi_s_*)s, (oyCMMapi_s_*)cmmapi6 );
    /* ---- end of custom CMMapi copy constructor ------- */
    /* ---- start of custom CMMapiFilter copy constructor ----- */
    error = oyCMMapiFilter_Copy__Members( (oyCMMapiFilter_s_*)s, (oyCMMapiFilter_s_*)cmmapi6 );
    /* ---- end of custom CMMapiFilter copy constructor ------- */
    /* ---- start of custom CMMapi6 copy constructor ----- */
    error = oyCMMapi6_Copy__Members( s, cmmapi6 );
    /* ---- end of custom CMMapi6 copy constructor ------- */
    
    
    
    
  }

  if(error)
    oyCMMapi6_Release_( &s );

  return s;
}

/** @internal
 *  Function oyCMMapi6_Copy_
 *  @memberof oyCMMapi6_s_
 *  @brief   copy or reference a CMMapi6 object
 *
 *  @param[in]     cmmapi6                 CMMapi6 struct object
 *  @param         object              the optional object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
oyCMMapi6_s_ * oyCMMapi6_Copy_ ( oyCMMapi6_s_ *cmmapi6, oyObject_s object )
{
  oyCMMapi6_s_ *s = cmmapi6;

  if(!cmmapi6)
    return 0;

  if(cmmapi6 && !object)
  {
    s = cmmapi6;
    
    oyObject_Copy( s->oy_ );
    return s;
  }

  s = oyCMMapi6_Copy__( cmmapi6, object );

  return s;
}
 
/** @internal
 *  Function oyCMMapi6_Release_
 *  @memberof oyCMMapi6_s_
 *  @brief   release and possibly deallocate a CMMapi6 object
 *
 *  @param[in,out] cmmapi6                 CMMapi6 struct object
 *
 *  @version Oyranos: 
 *  @since   2010/04/26 (Oyranos: 0.1.10)
 *  @date    2010/04/26
 */
int oyCMMapi6_Release_( oyCMMapi6_s_ **cmmapi6 )
{
  const char * track_name = NULL;
  /* ---- start of common object destructor ----- */
  oyCMMapi6_s_ *s = 0;

  if(!cmmapi6 || !*cmmapi6)
    return 0;

  s = *cmmapi6;

  *cmmapi6 = 0;

  if(oyObject_UnRef(s->oy_))
    return 0;
  /* ---- end of common object destructor ------- */

  if(oy_debug_objects >= 0)
  {
    const char * t = getenv(OY_DEBUG_OBJECTS);
    int id_ = -1;

    if(t)
      id_ = atoi(t);

    if((id_ >= 0 && s->oy_->id_ == id_) ||
       (t && s && strstr(oyStructTypeToText(s->type_), t) != 0) ||
       id_ == 1)
    {
      track_name = oyStructTypeToText(s->type_);
      fprintf( stderr, "%s[%d] untracking\n", track_name, s->oy_->id_);
    }
  }

  
  
  
  /* ---- start of custom CMMapi destructor ----- */
  oyCMMapi_Release__Members( (oyCMMapi_s_*)s );
  /* ---- end of custom CMMapi destructor ------- */
  /* ---- start of custom CMMapiFilter destructor ----- */
  oyCMMapiFilter_Release__Members( (oyCMMapiFilter_s_*)s );
  /* ---- end of custom CMMapiFilter destructor ------- */
  /* ---- start of custom CMMapi6 destructor ----- */
  oyCMMapi6_Release__Members( s );
  /* ---- end of custom CMMapi6 destructor ------- */
  
  



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



/* Include "CMMapi6.private_methods_definitions.c" { */

/* } Include "CMMapi6.private_methods_definitions.c" */

